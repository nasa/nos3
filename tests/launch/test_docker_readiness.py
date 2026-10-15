"""Opt-in checks against real Docker, not a full NOS3 mission qualification.

Set NOS3_TEST_DOCKER_IMAGE to an approved, already-cached Linux image containing
python3, cat, and getent. Without that opt-in this module makes no Docker calls.
Only uniquely named/labeled fixture containers and an internal fixture network
are created. Images are never pulled; no host ports or host paths are mounted.
"""

import json
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys
import tempfile
import unittest
import uuid


IMAGE = os.environ.get("NOS3_TEST_DOCKER_IMAGE")
DOCKER = shlex.split(os.environ.get("NOS3_TEST_DOCKER", "docker"))
HELPER = Path(__file__).resolve().parents[2] / "scripts" / "ci_readiness.py"
LABEL = "org.nasa.nos3.readiness-fixture"
SERVER = r'''
import socket
import time
listener = socket.socket()
listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
listener.bind(("0.0.0.0", 8010))
listener.listen(1)
print("FIXTURE_LISTENING", flush=True)
connection, address = listener.accept()
listener.close()
print("FIXTURE_ACCEPTED_ONCE", flush=True)
time.sleep(120)
'''
CONNECT = 'import socket,time; connection=socket.create_connection(("fixture-radio",8010),10); time.sleep(120)'


@unittest.skipUnless(IMAGE, "Set NOS3_TEST_DOCKER_IMAGE to opt into isolated real-Docker tests")
class RealDockerReadinessTests(unittest.TestCase):
    def setUp(self):
        self.token = uuid.uuid4().hex
        self.prefix = "nos3-readiness-" + self.token
        self.intents = []
        self.ids = {}
        self.addCleanup(self.cleanup_fixtures)
        self.temporary = tempfile.TemporaryDirectory(prefix=self.prefix + "-")
        self.addCleanup(self.temporary.cleanup)
        self.identities = Path(self.temporary.name) / "identities.json"
        self.identities.write_bytes(b"")
        self.identities.chmod(0o600)
        # Resolve the image before creating anything; --pull=never is also used
        # for every container to protect against an absent/changed image tag.
        self.docker("image", "inspect", IMAGE)
        self.network_name = self.prefix + "-net"
        self.network = self.create_owned(
            "network", self.network_name, "network", "create", "--internal",
            "--label", LABEL + "=" + self.token, self.network_name)
        self.server_name = self.prefix + "-server"
        self.peer_name = self.prefix + "-peer"
        self.server = self.create_container(self.server_name, SERVER, "fixture-radio")
        self.peer = self.create_container(self.peer_name, "import time; time.sleep(120)")
        self.docker("start", self.server, self.peer)

    def docker(self, *arguments, check=True):
        result = subprocess.run(DOCKER + list(arguments), capture_output=True, text=True, timeout=30)
        if check and result.returncode:
            self.fail("Fixture Docker command failed: {}\n{}".format(" ".join(arguments[:3]), result.stderr))
        return result

    def create_owned(self, kind, name, *arguments):
        # Record the unique creation intent first so a partially completed
        # create command can be discovered, ownership-checked, and cleaned up.
        self.intents.append((kind, name))
        identity = self.docker(*arguments).stdout.strip()
        self.assertRegex(identity, r"^[0-9a-f]{64}$")
        self.ids[(kind, name)] = identity
        return identity

    def create_container(self, name, program, alias=None):
        options = ["create", "--pull=never", "--name", name, "--label", LABEL + "=" + self.token,
                   "--network", self.network, "--read-only", "--cap-drop=ALL", "--security-opt", "no-new-privileges",
                   "--memory", "128m", "--cpus", "0.25", "--pids-limit", "32", "--entrypoint", "python3"]
        if alias:
            options.extend(["--network-alias", alias])
        return self.create_owned("container", name, *options, IMAGE, "-B", "-u", "-c", program)

    def helper(self, name, *arguments, timeout=10, identities=True):
        command = [sys.executable, "-B", str(HELPER), "wait", "--docker", shlex.join(DOCKER),
                   "--container", name, "--timeout", str(timeout)]
        if identities:
            command.extend(["--identity-file", str(self.identities)])
        return subprocess.run(command + list(arguments), capture_output=True, text=True, timeout=timeout + 15)

    def assert_ready(self, result):
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("READY:", result.stdout)

    def assert_not_ready(self, result):
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertNotIn("READY:", result.stdout)
        self.assertIn("ERROR:", result.stderr)

    def inspect(self, identity):
        return json.loads(self.docker("inspect", identity).stdout)[0]

    def test_real_passive_sockets_dns_exit_and_cross_stage_restart(self):
        with self.subTest(stage="passive listener and exact Docker DNS"):
            result = self.helper(self.server_name, "--listen", "8010", "--dns", "fixture-radio",
                                 "--dns-from", self.peer_name, "--network", self.network_name)
            self.assert_ready(result)
            logs = self.docker("logs", self.server).stdout
            self.assertIn("FIXTURE_LISTENING", logs)
            self.assertNotIn("FIXTURE_ACCEPTED_ONCE", logs, "A passive probe consumed the one-client socket")

        with self.subTest(stage="real established session in both directions"):
            self.docker("exec", "--detach", self.peer, "python3", "-B", "-c", CONNECT)
            self.assert_ready(self.helper(self.server_name, "--accept", "8010", "--local-peer", "8010", "--watch", self.peer_name))
            self.assert_ready(self.helper(self.peer_name, "--peer", "8010", "--watch", self.server_name))
            self.assertEqual(self.docker("logs", self.server).stdout.count("FIXTURE_ACCEPTED_ONCE"), 1)

        with self.subTest(stage="missing listener fails within bounded helper deadline"):
            result = self.helper(self.server_name, "--listen", "8011", timeout=1)
            self.assert_not_ready(result)

        with self.subTest(stage="exact container exit is rejected"):
            self.docker("stop", "--time", "1", self.peer)
            self.assertFalse(self.inspect(self.peer)["State"]["Running"])
            result = self.helper(self.peer_name)
            self.assert_not_ready(result)
            self.assertIn("exited during startup", result.stderr)

        with self.subTest(stage="manual restart rejected across helper invocations"):
            before = self.inspect(self.server)
            self.docker("restart", "--time", "1", self.server)
            after = self.inspect(self.server)
            self.assertEqual(before["Id"], after["Id"])
            self.assertNotEqual(before["State"]["StartedAt"], after["State"]["StartedAt"])
            result = self.helper(self.server_name, "--listen", "8010")
            self.assert_not_ready(result)
            self.assertRegex(result.stderr, r"changed identity|restarted during startup")

    def cleanup_fixtures(self):
        errors = []
        # Reverse creation order removes the two fixture containers first.
        # Resolve and validate the recorded unique name/label even after a
        # timed-out create; removal itself always uses the exact full ID.
        for kind, name in reversed(self.intents):
            try:
                result = self.docker(kind, "inspect", name, check=False)
                if result.returncode:
                    if "No such" in result.stderr or "not found" in result.stderr:
                        continue
                    raise RuntimeError(result.stderr)
                objects = json.loads(result.stdout)
                if not isinstance(objects, list) or len(objects) != 1:
                    raise RuntimeError("Unexpected fixture inspect response")
                obj = objects[0]
                labels = (obj["Config"].get("Labels") if kind == "container" else obj.get("Labels")) or {}
                identity = obj.get("Id", "")
                if (not re.fullmatch(r"[0-9a-f]{64}", identity) or labels.get(LABEL) != self.token or
                        obj.get("Name", "").lstrip("/") != name or
                        self.ids.get((kind, name), identity) != identity):
                    raise RuntimeError("Fixture ownership mismatch; refusing removal of " + name)
                if kind == "container":
                    self.docker("container", "rm", "--force", identity)
                else:
                    self.docker("network", "rm", identity)
            except (OSError, ValueError, RuntimeError, AssertionError, KeyError, TypeError, subprocess.TimeoutExpired) as exc:
                errors.append("{}: {}".format(name, exc))
        if errors:
            raise AssertionError("Fixture cleanup incomplete: " + "; ".join(errors))


if __name__ == "__main__":
    unittest.main()
