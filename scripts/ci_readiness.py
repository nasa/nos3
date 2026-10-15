#!/usr/bin/env python3
"""Bounded, passive startup checks for the standalone CI launcher.

Never connect to 42 or radio sockets: they accept real mission clients, not
health probes. This checks startup only; it is not a background supervisor.
"""
import argparse
import ipaddress
import json
import os
import re
import shlex
import stat
import subprocess
import sys
import tempfile
import time
from pathlib import Path


class NotReady(RuntimeError):
    pass


def sockets(text):
    result = []
    for line in text.splitlines():
        fields = line.split()
        if not fields:
            continue
        if fields[0] == "sl":
            if (len(fields) < 4 or fields[1] != "local_address" or
                    fields[2] not in ("rem_address", "remote_address") or fields[3] != "st"):
                raise ValueError("Malformed /proc socket table header")
            continue
        endpoint = r"(?:[0-9A-Fa-f]{8}|[0-9A-Fa-f]{32}):[0-9A-Fa-f]{4}"
        if (len(fields) < 4 or not re.fullmatch(r"[0-9]+:", fields[0]) or
                not re.fullmatch(endpoint, fields[1]) or
                not re.fullmatch(endpoint, fields[2]) or
                not re.fullmatch(r"[0-9A-Fa-f]{2}", fields[3])):
            raise ValueError("Malformed /proc socket table")
        result.append((int(fields[1].rsplit(":", 1)[1], 16),
                       int(fields[2].rsplit(":", 1)[1], 16), fields[3].upper()))
    return result


def headless_config(path):
    path = Path(path)
    original = path.read_bytes()
    # Normalize only for matching; preserve every original byte except TRUE.
    pattern = rb"(?m)^(TRUE|FALSE)([ \t]+![ \t]+Graphics Front End\?[^\r\n]*)(?=\r?$)"
    changed, count = re.subn(pattern, rb"FALSE\2", original)
    if count != 1:
        raise ValueError("Expected exactly one 42 Graphics Front End setting")
    path.write_bytes(changed)


class Probe:
    def __init__(self, args):
        self.args = args
        self.deadline = time.monotonic() + args.timeout
        self.docker = shlex.split(args.docker)
        self.identities = {}
        self.identity_path = Path(args.identity_file) if getattr(args, "identity_file", None) else None
        if self.identity_path is not None:
            self.identity_bytes, self.identity_stat = self.read_identity_file()
            saved = json.loads(self.identity_bytes) if self.identity_bytes else {}
            if not isinstance(saved, dict):
                raise ValueError("Malformed launch identity file")
            for name, identity in saved.items():
                if (not isinstance(name, str) or not name or not isinstance(identity, list) or
                        len(identity) != 2 or not isinstance(identity[0], str) or
                        not re.fullmatch(r"[0-9a-f]{64}", identity[0]) or
                        not isinstance(identity[1], str) or not identity[1]):
                    raise ValueError("Malformed launch identity entry")
                self.identities[name] = tuple(identity)

    def read_identity_file(self):
        metadata = self.identity_path.lstat()
        if (not stat.S_ISREG(metadata.st_mode) or
                (hasattr(os, "getuid") and metadata.st_uid != os.getuid())):
            raise ValueError("Launch identity file must be a caller-owned regular file")
        with os.fdopen(os.open(self.identity_path, os.O_RDONLY | getattr(os, "O_NOFOLLOW", 0)), "rb") as stream:
            opened = os.fstat(stream.fileno())
            if (opened.st_dev, opened.st_ino) != (metadata.st_dev, metadata.st_ino):
                raise ValueError("Launch identity file changed while opening")
            content = stream.read(1024 * 1024 + 1)
        if len(content) > 1024 * 1024:
            raise ValueError("Launch identity file is too large")
        return content, (metadata.st_dev, metadata.st_ino)

    def persist_identities(self):
        if self.identity_path is None:
            return
        content, identity = self.read_identity_file()
        if content != self.identity_bytes or identity != self.identity_stat:
            raise RuntimeError("Launch identity file changed during readiness check")
        updated = (json.dumps(self.identities, sort_keys=True) + "\n").encode("utf-8")
        descriptor, temporary = tempfile.mkstemp(prefix=".nos3-identities-", dir=self.identity_path.parent)
        try:
            with os.fdopen(descriptor, "wb") as stream:
                stream.write(updated)
                stream.flush()
                os.fsync(stream.fileno())
            os.replace(temporary, self.identity_path)
        finally:
            if os.path.exists(temporary):
                os.unlink(temporary)
        self.identity_bytes, self.identity_stat = self.read_identity_file()

    def command(self, *args):
        remaining = self.deadline - time.monotonic()
        if remaining <= 0:
            raise TimeoutError("Startup deadline expired")
        result = subprocess.run(self.docker + list(args), capture_output=True,
                                text=True, timeout=min(10, remaining))
        if time.monotonic() >= self.deadline:
            raise TimeoutError("Startup deadline expired during Docker command")
        if result.returncode:
            raise RuntimeError("Docker command failed: {}: {}".format(
                " ".join(args[:3]), result.stderr.strip()))
        return result.stdout

    def inspect(self):
        names = list(dict.fromkeys([self.args.container] + self.args.watch +
                                  ([self.args.dns_from] if self.args.dns_from else [])))
        objects = json.loads(self.command("inspect", *names))
        if not isinstance(objects, list) or len(objects) != len(names):
            raise RuntimeError("Incomplete Docker inspect response")
        for name, obj in zip(names, objects):
            if not isinstance(obj, dict) or not isinstance(obj.get("State"), dict):
                raise RuntimeError("Malformed Docker inspect response for {}".format(name))
            state = obj["State"]
            identity_id = obj.get("Id")
            actual_name = obj.get("Name")
            if (not isinstance(identity_id, str) or not re.fullmatch(r"[0-9a-f]{64}", identity_id) or
                    not isinstance(actual_name, str) or not actual_name or
                    not isinstance(state.get("StartedAt"), str) or not state["StartedAt"] or
                    type(state.get("Running")) is not bool or
                    type(obj.get("RestartCount")) is not int or obj["RestartCount"] < 0):
                raise RuntimeError("Malformed Docker identity or state for {}".format(name))
            if name != identity_id and name.lstrip("/") != actual_name.lstrip("/"):
                raise RuntimeError("Docker inspect returned the wrong container for {}".format(name))
            if not state["Running"] or state["Status"] != "running":
                raise RuntimeError("{} exited during startup: {} (exit {})".format(
                    name, state["Status"], state.get("ExitCode", "unknown")))
            if obj["RestartCount"] != 0:
                raise RuntimeError("{} restarted during startup".format(name))
            identity = (obj["Id"], state["StartedAt"])
            if name in self.identities and self.identities[name] != identity:
                raise RuntimeError("{} changed identity during startup".format(name))
            self.identities[name] = identity
            health_info = state.get("Health")
            if health_info is not None and (not isinstance(health_info, dict) or
                    health_info.get("Status") not in ("none", "starting", "healthy", "unhealthy")):
                raise RuntimeError("Malformed Docker health state for {}".format(name))
            health = health_info.get("Status") if health_info else None
            if health == "unhealthy":
                raise RuntimeError("{} is unhealthy".format(name))
            if health == "starting":
                raise NotReady("{} health check is starting".format(name))
        return objects[0]

    def sample(self):
        obj = self.inspect()
        target = obj["Id"]
        if self.args.listen or self.args.accept or self.args.peer or self.args.local_peer:
            rows = sockets(self.command("exec", target, "cat", "/proc/net/tcp", "/proc/net/tcp6"))
            for port in self.args.listen:
                if not any(local == port and state == "0A" for local, _, state in rows):
                    raise NotReady("TCP listener {}".format(port))
            for port in self.args.accept:
                if not any(local == port and state in ("0A", "01") for local, _, state in rows):
                    raise NotReady("TCP listener or accepted session {}".format(port))
            for port in self.args.peer:
                if not any(remote == port and state == "01" for _, remote, state in rows):
                    raise NotReady("established TCP peer {}".format(port))
            for port in self.args.local_peer:
                if not any(local == port and state == "01" for local, _, state in rows):
                    raise NotReady("accepted TCP session {}".format(port))
        if self.args.udp:
            rows = sockets(self.command("exec", target, "cat", "/proc/net/udp", "/proc/net/udp6"))
            for port in self.args.udp:
                if not any(local == port and state in ("07", "01") for local, _, state in rows):
                    raise NotReady("UDP binding {}".format(port))
        if self.args.dns:
            expected = obj["NetworkSettings"]["Networks"][self.args.network]["IPAddress"]
            ipaddress.IPv4Address(expected)
            # The already-running engine shares the spacecraft network. No
            # throwaway container or TCP connection is required for discovery.
            source = self.identities[self.args.dns_from][0]
            try:
                output = self.command("exec", source, "getent", "ahostsv4", self.args.dns)
            except RuntimeError as exc:
                raise NotReady("DNS resolution: {}".format(exc)) from exc
            addresses = {line.split()[0] for line in output.splitlines() if line.split()}
            if addresses != {expected}:
                raise NotReady("DNS {} must resolve only to {}".format(self.args.dns, expected))
        # Do not accept a probe from a process that exited while being checked.
        self.inspect()
        self.persist_identities()

    def wait(self):
        stable = 0
        reason = "no readiness observation"
        while time.monotonic() < self.deadline:
            try:
                self.sample()
                if time.monotonic() >= self.deadline:
                    raise TimeoutError("Startup deadline expired before readiness confirmation")
                stable += 1
                if stable >= 2:
                    print("READY: {}".format(self.args.container))
                    return
            except NotReady as exc:
                stable = 0
                reason = str(exc)
            except (TimeoutError, subprocess.TimeoutExpired) as exc:
                raise TimeoutError("{} not ready: {}; probe deadline expired: {}".format(
                    self.args.container, reason, exc)) from exc
            time.sleep(min(0.25, max(0, self.deadline - time.monotonic())))
        raise TimeoutError("{} not ready: {}".format(self.args.container, reason))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    modes = parser.add_subparsers(dest="mode", required=True)
    config = modes.add_parser("headless-config")
    config.add_argument("path")
    wait = modes.add_parser("wait")
    wait.add_argument("--container", required=True)
    wait.add_argument("--docker", default="docker")
    wait.add_argument("--timeout", type=int, default=180)
    wait.add_argument("--identity-file", help="Caller-created file retaining container identities across launch stages")
    wait.add_argument("--watch", action="append", default=[])
    for flag in ("listen", "accept", "peer", "local-peer", "udp"):
        wait.add_argument("--" + flag, action="append", type=int, default=[])
    wait.add_argument("--dns")
    wait.add_argument("--dns-from")
    wait.add_argument("--network")
    args = parser.parse_args()
    try:
        if args.mode == "headless-config":
            headless_config(args.path)
        else:
            if not 1 <= args.timeout <= 900:
                raise ValueError("timeout must be between 1 and 900 seconds")
            if args.dns and not (args.dns_from and args.network):
                raise ValueError("DNS checks require --dns-from and --network")
            for values in (args.listen, args.accept, args.peer, args.local_peer, args.udp):
                if any(not 1 <= port <= 65535 for port in values):
                    raise ValueError("invalid port")
            Probe(args).wait()
    except (RuntimeError, ValueError, OSError, KeyError, TypeError,
            subprocess.TimeoutExpired) as exc:
        print("ERROR: {}".format(exc), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
