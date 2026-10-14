"""Passive CI startup regressions; Docker subprocesses are replaced by fixtures."""

import argparse
import contextlib
import copy
import importlib.util
import io
import json
from pathlib import Path
import subprocess
import tempfile
import time
import unittest
from unittest.mock import patch


HELPER = Path(__file__).resolve().parents[2] / "scripts" / "ci_readiness.py"
SPEC = importlib.util.spec_from_file_location("nos3_ci_readiness", HELPER)
READINESS = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(READINESS)

HEADER = "  sl  local_address rem_address st tx_queue rx_queue tr tm->when retrnsmt uid timeout inode\n"


def row(local, remote=0, state="0A", ipv6=False):
    address = "0" * (32 if ipv6 else 8)
    return "  0: {}:{:04X} {}:{:04X} {} 00000000:00000000 00:00000000 00000000 0 0 1\n".format(
        address, local, address, remote, state)


def container(name="target", identity="a", health=None):
    obj = {
        "Id": identity * 64,
        "Name": "/" + name,
        "RestartCount": 0,
        "State": {"Running": True, "Status": "running", "StartedAt": "2026-10-14T00:00:00Z", "ExitCode": 0},
        "NetworkSettings": {"Networks": {"nos3-sc01": {"IPAddress": "172.20.0.3"}}},
    }
    if health is not None:
        obj["State"]["Health"] = {"Status": health}
    return obj


def arguments(**updates):
    values = dict(container="target", docker="docker", timeout=3, watch=[], dns=None,
                  dns_from=None, network=None, listen=[], accept=[], peer=[], local_peer=[], udp=[])
    values.update(updates)
    return argparse.Namespace(**values)


class DockerFixture:
    """Only substitutes subprocess.run; parser, checks, and clock remain real."""

    def __init__(self):
        self.objects = {"target": container(), "engine": container("engine", "b")}
        self.tcp = HEADER
        self.udp = HEADER
        self.dns = "172.20.0.3 STREAM radio\n172.20.0.3 DGRAM radio\n172.20.0.3 RAW radio\n"
        self.calls = []
        self.inspect_count = 0
        self.mutate_inspect = None
        self.raw_inspect = None
        self.fail = None
        self.delay = 0

    def run(self, argv, **kwargs):
        self.calls.append((argv, kwargs))
        if self.delay:
            time.sleep(self.delay)
        if self.fail:
            return subprocess.CompletedProcess(argv, 1, "", "fixture Docker failure")
        command = argv[1:]
        if command[0] == "inspect":
            self.inspect_count += 1
            if self.mutate_inspect:
                self.mutate_inspect(self.inspect_count, self.objects)
            if self.raw_inspect is not None:
                output = self.raw_inspect
            else:
                output = json.dumps([self.objects[name] for name in command[1:]])
        elif command[0] == "exec" and command[2:] == ["cat", "/proc/net/tcp", "/proc/net/tcp6"]:
            output = self.tcp
        elif command[0] == "exec" and command[2:] == ["cat", "/proc/net/udp", "/proc/net/udp6"]:
            output = self.udp
        elif command[0] == "exec" and command[2:4] == ["getent", "ahostsv4"]:
            output = self.dns
        else:
            raise AssertionError("Unexpected Docker operation: {!r}".format(argv))
        return subprocess.CompletedProcess(argv, 0, output, "")


class SocketParserTests(unittest.TestCase):
    def test_ipv4_ipv6_headers_and_hex_ports(self):
        text = HEADER + row(8010, 12001, "01") + HEADER.replace("rem_address", "remote_address") + row(4277, state="0a", ipv6=True)
        self.assertEqual(READINESS.sockets(text), [(8010, 12001, "01"), (4277, 0, "0A")])

    def test_empty_tables(self):
        self.assertEqual(READINESS.sockets(HEADER + "\n" + HEADER), [])

    def test_malformed_tables_are_rejected(self):
        examples = [
            "sl unrelated header\n", "garbage\n", "0: 00000000:1F4A\n",
            "x: 00000000:1F4A 00000000:0000 0A\n",
            "0: 00000000:1F4A 00000000 0A\n",
            "0: INVALID:1F4A 00000000:0000 0A\n",
            "0: 00000000:10000 00000000:0000 0A\n",
            "0: 00000000:ZZZZ 00000000:0000 0A\n",
            "0: 00000000:1F4A 00000000:0000 LISTEN\n",
        ]
        for example in examples:
            with self.subTest(example=example), self.assertRaises(ValueError):
                READINESS.sockets(example)


class ProbeTests(unittest.TestCase):
    def setUp(self):
        self.docker = DockerFixture()
        self.mock = patch.object(READINESS.subprocess, "run", self.docker.run)
        self.mock.start()
        self.addCleanup(self.mock.stop)

    def test_listener_requires_listen_not_established_or_close_wait(self):
        for state in ("01", "08", "06"):
            with self.subTest(state=state):
                self.docker.tcp = HEADER + row(8010, state=state)
                with self.assertRaises(READINESS.NotReady):
                    READINESS.Probe(arguments(listen=[8010])).sample()
        self.docker.tcp = HEADER + row(8010)
        READINESS.Probe(arguments(listen=[8010])).sample()

    def test_accept_allows_listening_or_already_accepted_session(self):
        for state in ("0A", "01"):
            with self.subTest(state=state):
                self.docker.tcp = HEADER + row(4277, state=state)
                READINESS.Probe(arguments(accept=[4277])).sample()
        self.docker.tcp = HEADER + row(4277, state="08")
        with self.assertRaises(READINESS.NotReady):
            READINESS.Probe(arguments(accept=[4277])).sample()

    def test_remote_peer_requires_established_remote_port(self):
        for text in (row(12001, state="01"), row(50000, 12001, "08"), row(50000, 12001, "0A")):
            with self.subTest(text=text):
                self.docker.tcp = HEADER + text
                with self.assertRaises(READINESS.NotReady):
                    READINESS.Probe(arguments(peer=[12001])).sample()
        self.docker.tcp = HEADER + row(50000, 12001, "01")
        READINESS.Probe(arguments(peer=[12001])).sample()

    def test_local_peer_requires_established_local_port(self):
        for text in (row(8010), row(50000, 8010, "01"), row(8010, 50000, "08")):
            with self.subTest(text=text):
                self.docker.tcp = HEADER + text
                with self.assertRaises(READINESS.NotReady):
                    READINESS.Probe(arguments(local_peer=[8010])).sample()
        self.docker.tcp = HEADER + row(8010, 50000, "01")
        READINESS.Probe(arguments(local_peer=[8010])).sample()

    def test_all_requested_ports_are_required(self):
        self.docker.tcp = HEADER + row(4277)
        with self.assertRaises(READINESS.NotReady):
            READINESS.Probe(arguments(listen=[4277, 4278])).sample()

    def test_udp_binding_accepts_unconnected_or_connected(self):
        for state in ("07", "01"):
            with self.subTest(state=state):
                self.docker.udp = HEADER + row(6010, state=state, ipv6=True)
                READINESS.Probe(arguments(udp=[6010])).sample()
        self.docker.udp = HEADER + row(6010, state="08")
        with self.assertRaises(READINESS.NotReady):
            READINESS.Probe(arguments(udp=[6010])).sample()

    def test_dns_exact_target_address_and_source_identity(self):
        READINESS.Probe(arguments(dns="radio", dns_from="engine", network="nos3-sc01")).sample()
        self.assertIn(["docker", "exec", "b" * 64, "getent", "ahostsv4", "radio"], [call[0] for call in self.docker.calls])

    def test_dns_empty_wrong_or_extra_address_is_not_ready(self):
        for output in ("", "172.20.0.9 STREAM radio\n", "172.20.0.3 STREAM radio\n172.20.0.9 STREAM radio\n"):
            with self.subTest(output=output):
                self.docker.dns = output
                with self.assertRaises(READINESS.NotReady):
                    READINESS.Probe(arguments(dns="radio", dns_from="engine", network="nos3-sc01")).sample()

    def test_invalid_expected_dns_address_is_rejected(self):
        self.docker.objects["target"]["NetworkSettings"]["Networks"]["nos3-sc01"]["IPAddress"] = "invalid"
        with self.assertRaises(ValueError):
            READINESS.Probe(arguments(dns="radio", dns_from="engine", network="nos3-sc01")).sample()

    def test_container_exit_and_restart_fail(self):
        original = copy.deepcopy(self.docker.objects)
        for mutation in (lambda o: o["State"].update(Running=False, Status="exited"), lambda o: o.update(RestartCount=1)):
            self.docker.objects = copy.deepcopy(original)
            mutation(self.docker.objects["target"])
            with self.assertRaises(RuntimeError):
                READINESS.Probe(arguments()).sample()

    def test_watched_dependency_exit_fails(self):
        self.docker.objects["engine"]["State"].update(Running=False, Status="exited")
        with self.assertRaisesRegex(RuntimeError, "engine exited"):
            READINESS.Probe(arguments(watch=["engine"])).sample()

    def test_exit_during_sample_fails_final_inspect(self):
        def exit_on_second(count, objects):
            if count == 2:
                objects["target"]["State"].update(Running=False, Status="exited")
        self.docker.mutate_inspect = exit_on_second
        with self.assertRaisesRegex(RuntimeError, "exited"):
            READINESS.Probe(arguments()).sample()

    def test_identity_and_start_time_change_fail(self):
        for field in ("Id", "StartedAt"):
            with self.subTest(field=field):
                self.docker.objects["target"] = container()
                self.docker.inspect_count = 0
                def replace_on_second(count, objects):
                    if count == 2:
                        if field == "Id":
                            objects["target"]["Id"] = "c" * 64
                        else:
                            objects["target"]["State"]["StartedAt"] = "2026-10-14T01:00:00Z"
                self.docker.mutate_inspect = replace_on_second
                with self.assertRaisesRegex(RuntimeError, "changed identity"):
                    READINESS.Probe(arguments()).sample()

    def test_inspect_rejects_malformed_json_shape_and_identity(self):
        for raw in ("broken", "null", "{}", "[]", "[null]", "[{}]", "[{}, {}]"):
            with self.subTest(raw=raw):
                self.docker.raw_inspect = raw
                with self.assertRaises((RuntimeError, ValueError)):
                    READINESS.Probe(arguments()).sample()
        self.docker.raw_inspect = None
        mutations = [
            lambda o: o.update(Id=""), lambda o: o.update(Id="a" * 12),
            lambda o: o.update(Name="/unrelated"), lambda o: o.update(RestartCount=False),
            lambda o: o.update(RestartCount="0"), lambda o: o["State"].update(StartedAt=""),
            lambda o: o["State"].update(Running="true"),
            lambda o: o["State"].update(Health="healthy"),
            lambda o: o["State"].update(Health={"Status": "unknown"}),
        ]
        for mutation in mutations:
            self.docker.objects["target"] = container()
            mutation(self.docker.objects["target"])
            with self.assertRaises(RuntimeError):
                READINESS.Probe(arguments()).sample()

    def test_full_container_id_is_accepted_and_must_match(self):
        self.docker.objects["a" * 64] = container()
        READINESS.Probe(arguments(container="a" * 64)).sample()
        self.docker.objects["a" * 64]["Id"] = "b" * 64
        with self.assertRaisesRegex(RuntimeError, "wrong container"):
            READINESS.Probe(arguments(container="a" * 64)).sample()

    def test_health_starting_is_not_ready_unhealthy_fails(self):
        self.docker.objects["target"] = container(health="starting")
        with self.assertRaises(READINESS.NotReady):
            READINESS.Probe(arguments()).sample()
        self.docker.objects["target"] = container(health="unhealthy")
        with self.assertRaisesRegex(RuntimeError, "unhealthy"):
            READINESS.Probe(arguments()).sample()
        self.docker.objects["target"] = container(health="healthy")
        READINESS.Probe(arguments()).sample()

    def test_docker_failure_and_timeout_fail_closed(self):
        self.docker.fail = True
        with self.assertRaisesRegex(RuntimeError, "Docker command failed"):
            READINESS.Probe(arguments()).sample()
        with patch.object(READINESS.subprocess, "run", side_effect=subprocess.TimeoutExpired("docker", 1)):
            with self.assertRaises(subprocess.TimeoutExpired):
                READINESS.Probe(arguments()).sample()

    def test_expired_deadline_does_not_invoke_docker(self):
        with self.assertRaises(TimeoutError):
            READINESS.Probe(arguments(timeout=0)).command("inspect", "target")
        self.assertEqual(self.docker.calls, [])

    def test_command_returning_after_deadline_is_rejected(self):
        self.docker.delay = 0.04
        with self.assertRaises(TimeoutError):
            READINESS.Probe(arguments(timeout=0.02)).command("inspect", "target")
        self.assertLessEqual(self.docker.calls[0][1]["timeout"], 0.02)

    def test_wait_requires_two_successful_samples(self):
        output = io.StringIO()
        with contextlib.redirect_stdout(output):
            READINESS.Probe(arguments()).wait()
        self.assertEqual(self.docker.inspect_count, 4)
        self.assertEqual(output.getvalue(), "READY: target\n")
        for argv, kwargs in self.docker.calls:
            self.assertEqual(argv[1], "inspect")
            self.assertTrue(kwargs["capture_output"])
            self.assertTrue(kwargs["text"])
            self.assertGreater(kwargs["timeout"], 0)

    def test_not_ready_observation_resets_consecutive_samples(self):
        def health_transition(count, objects):
            objects["target"]["State"]["Health"] = {"Status": "starting" if count == 3 else "healthy"}
        self.docker.mutate_inspect = health_transition
        with contextlib.redirect_stdout(io.StringIO()):
            READINESS.Probe(arguments()).wait()
        self.assertEqual(self.docker.inspect_count, 7)

    def test_wait_timeout_has_no_ready_output(self):
        output = io.StringIO()
        with contextlib.redirect_stdout(output), self.assertRaises(TimeoutError):
            READINESS.Probe(arguments(timeout=0.04, listen=[8010])).wait()
        self.assertNotIn("READY:", output.getvalue())

    def test_last_pending_reason_survives_rpc_deadline(self):
        for error in (TimeoutError("Docker deadline"), subprocess.TimeoutExpired(["docker", "inspect"], 1)):
            with self.subTest(error=type(error).__name__):
                with patch.object(READINESS.Probe, "sample", side_effect=[
                        READINESS.NotReady("DNS radio-sim"), error]), patch.object(READINESS.time, "sleep"):
                    with self.assertRaisesRegex(TimeoutError, "DNS radio-sim.*probe deadline expired"):
                        READINESS.Probe(arguments()).wait()


class HeadlessConfigTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="nos3-headless-config-")
        self.addCleanup(self.temporary.cleanup)
        self.path = Path(self.temporary.name) / "Inp_Sim.txt"

    def test_lf_crlf_and_unterminated_setting_preserve_other_bytes(self):
        for newline in (b"\n", b"\r\n", b""):
            with self.subTest(newline=newline):
                original = b"Unrelated TRUE \xff\r\nTRUE   ! Graphics Front End? trailing comment" + newline
                self.path.write_bytes(original)
                READINESS.headless_config(self.path)
                self.assertEqual(self.path.read_bytes(), original.replace(b"TRUE   !", b"FALSE   !"))

    def test_false_is_idempotent(self):
        original = b"FALSE\t!\tGraphics Front End?\nTRUE ! unrelated\n"
        self.path.write_bytes(original)
        READINESS.headless_config(self.path)
        READINESS.headless_config(self.path)
        self.assertEqual(self.path.read_bytes(), original)

    def test_missing_or_duplicate_setting_leaves_file_unchanged(self):
        for original in (b"TRUE ! unrelated\n", b"TRUE ! Graphics Front End?\nFALSE ! Graphics Front End?\n"):
            with self.subTest(original=original):
                self.path.write_bytes(original)
                with self.assertRaises(ValueError):
                    READINESS.headless_config(self.path)
                self.assertEqual(self.path.read_bytes(), original)

    def test_missing_file_is_not_created(self):
        with self.assertRaises(OSError):
            READINESS.headless_config(self.path)
        self.assertFalse(self.path.exists())


class LaunchIdentityTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="nos3-launch-identities-")
        self.addCleanup(self.temporary.cleanup)
        self.path = Path(self.temporary.name) / "identities.json"
        self.path.write_bytes(b"")
        self.docker = DockerFixture()
        self.mock = patch.object(READINESS.subprocess, "run", self.docker.run)
        self.mock.start()
        self.addCleanup(self.mock.stop)

    def probe(self, **options):
        return READINESS.Probe(arguments(identity_file=str(self.path), **options))

    def test_empty_file_initializes_and_second_probe_preserves_identities(self):
        self.probe().sample()
        first = json.loads(self.path.read_text(encoding="utf-8"))
        self.assertEqual(first, {"target": ["a" * 64, "2026-10-14T00:00:00Z"]})
        self.probe(watch=["engine"]).sample()
        second = json.loads(self.path.read_text(encoding="utf-8"))
        self.assertEqual(second["target"], first["target"])
        self.assertEqual(second["engine"][0], "b" * 64)
        self.assertEqual(list(self.path.parent.glob(".nos3-identities-*")), [])

    def test_started_at_change_between_probes_is_rejected(self):
        self.probe().sample()
        before = self.path.read_bytes()
        self.docker.objects["target"]["State"]["StartedAt"] = "2026-10-14T02:00:00Z"
        with self.assertRaisesRegex(RuntimeError, "changed identity"):
            self.probe().sample()
        self.assertEqual(self.path.read_bytes(), before)

    def test_container_replacement_between_probes_is_rejected(self):
        self.probe().sample()
        self.docker.objects["target"]["Id"] = "c" * 64
        with self.assertRaisesRegex(RuntimeError, "changed identity"):
            self.probe().sample()

    def test_prior_dependency_replacement_is_rejected_at_next_stage(self):
        self.probe().sample()
        self.docker.objects["target"]["Id"] = "c" * 64
        with self.assertRaisesRegex(RuntimeError, "target changed identity"):
            self.probe(container="engine", watch=["target"]).sample()

    def test_unsuccessful_observation_does_not_persist(self):
        with self.assertRaises(READINESS.NotReady):
            self.probe(listen=[8010]).sample()
        self.assertEqual(self.path.read_bytes(), b"")

    def test_malformed_saved_state_is_rejected_without_docker(self):
        for content in (b"invalid", b"[]", b"null", b'{"target": ["short", "date"]}',
                        b'{"target": null}', b'{"target": ["a", "date", "extra"]}'):
            with self.subTest(content=content):
                self.path.write_bytes(content)
                with self.assertRaises(ValueError):
                    self.probe()
                self.assertEqual(self.path.read_bytes(), content)
        self.assertEqual(self.docker.calls, [])

    def test_missing_file_is_not_created(self):
        self.path.unlink()
        with self.assertRaises(OSError):
            self.probe()
        self.assertFalse(self.path.exists())

    def test_directory_is_rejected(self):
        self.path.unlink()
        self.path.mkdir()
        with self.assertRaisesRegex(ValueError, "regular file"):
            self.probe()

    def test_symlink_is_rejected(self):
        target = self.path.parent / "actual.json"
        target.write_bytes(b"")
        self.path.unlink()
        try:
            self.path.symlink_to(target)
        except OSError:
            self.skipTest("Creating symlinks is not permitted for this test user")
        with self.assertRaisesRegex(ValueError, "regular file"):
            self.probe()
        self.assertEqual(target.read_bytes(), b"")

    def test_changed_file_is_not_overwritten(self):
        probe = self.probe()
        self.path.write_bytes(b"{}\n")
        with self.assertRaisesRegex(RuntimeError, "file changed"):
            probe.sample()
        self.assertEqual(self.path.read_bytes(), b"{}\n")

    def test_oversize_file_is_rejected(self):
        self.path.write_bytes(b" " * (1024 * 1024 + 1))
        with self.assertRaisesRegex(ValueError, "too large"):
            self.probe()


if __name__ == "__main__":
    unittest.main()
