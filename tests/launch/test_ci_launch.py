#!/usr/bin/env python3
"""Execute the CI launcher with deterministic Docker/process dependency fixtures.

No Docker daemon, network, VM, X server, or application binary is used. The
production Bash launcher and its helpers are copied into a disposable fixture;
only its literal /tmp/nos3 scratch path is redirected into that fixture. The
environment script is replaced with fixture paths. Configuration copy/edit
operations and shell error handling execute normally, rather than being mocked.
"""

import json
import hashlib
import os
from pathlib import Path
import re
import shlex
import runpy
import shutil
import subprocess
import sys
import tempfile
import time
import unittest


ROOT = Path(__file__).resolve().parents[2]
BASH = os.environ.get("NOS3_TEST_BASH") or shutil.which("bash")
if os.name == "nt" and "NOS3_TEST_BASH" not in os.environ:
    candidate = Path("C:/Program Files/Git/bin/bash.exe")
    if candidate.is_file():
        BASH = str(candidate)


def fake_command(kind, args):
    """A stateful command fixture; every invocation is retained as JSON."""
    log = Path(os.environ["NOS3_FAKE_LOG"])
    with log.open("a", encoding="utf-8") as stream:
        stream.write(json.dumps({"kind": kind, "args": args}) + "\n")
    if kind == "xhost":
        return 0 if os.environ.get("DISPLAY") else 1
    if kind == "sleep":
        time.sleep(0.025)
        return 0
    state_path = Path(os.environ["NOS3_FAKE_STATE"])
    state = json.loads(state_path.read_text()) if state_path.exists() else {}
    fail = os.environ.get("NOS3_FAKE_FAIL", "")
    if not args:
        return 2
    command = args[0]
    if command in ("network", "logs", "compose", "image"):
        return 0
    if command in ("run", "create"):
        name = next((args[i + 1] for i, arg in enumerate(args[:-1]) if arg == "--name"), "probe")
        if fail == "run:" + name:
            return 1
        state[name] = {"running": command == "run", "args": args, "id": hashlib.sha256(name.encode()).hexdigest()}
        state_path.write_text(json.dumps(state))
        print(state[name]["id"])
        return 0
    if command == "start":
        name = next((key for key, value in state.items() if args[-1] in (key, value["id"])), args[-1])
        if name not in state:
            return 1
        state[name]["running"] = True
        state_path.write_text(json.dumps(state))
        print(state[name]["id"])
        return 0
    if command == "inspect":
        if "--format" not in args and "-f" not in args:
            objects = []
            for reference in args[1:]:
                name = next((key for key, value in state.items() if reference in (key, value["id"])), reference)
                if name not in state or fail == "inspect:" + name:
                    return 1
                late_exit = fail == "exit-after-release:" + name and any(value.get("released") for value in state.values())
                running = state[name]["running"] and fail != "exit:" + name and not late_exit
                objects.append({
                    "Id": state[name]["id"], "Name": "/" + name,
                    "RestartCount": 0,
                    "State": {"Running": running, "Status": "running" if running else "exited", "ExitCode": 0 if running else 23, "StartedAt": "2026-01-01T00:00:00Z"},
                    "NetworkSettings": {"Networks": {"nos3-sc01": {"IPAddress": "192.168.42.10"}, "nos3-core": {"IPAddress": "192.168.41.10"}}},
                })
            print(json.dumps(objects))
            return 0
        name = next((key for key, value in state.items() if args[-1] in (key, value["id"])), args[-1])
        if name not in state or fail == "inspect:" + name:
            return 1
        running = fail != "exit:" + name
        fmt = " ".join(args[1:-1])
        if ".State.Running" in fmt:
            print("true" if running else "false")
        elif ".State.Status" in fmt:
            print("running" if running else "exited")
        elif ".State.ExitCode" in fmt:
            print("0" if running else "23")
        else:
            print(json.dumps([{
                "Id": state[name]["id"], "Name": "/" + name,
                "RestartCount": 0,
                "State": {"Running": running, "Status": "running" if running else "exited", "ExitCode": 0 if running else 23},
                "NetworkSettings": {"Networks": {"nos3-sc01": {"IPAddress": "192.168.42.10"}, "nos3-core": {"IPAddress": "192.168.41.10"}}},
            }]))
        return 0
    if command == "exec":
        index = 1
        while index < len(args) and args[index].startswith("-"):
            index += 1
        if index >= len(args):
            return 2
        name = next((key for key, value in state.items() if args[index] in (key, value["id"])), args[index])
        body = " ".join(args[index + 1:])
        if name not in state or fail == "exit:" + name:
            return 1
        if fail == "exec:" + name or (fail and fail.startswith("probe:") and fail[6:] in body):
            return 1
        if "getent" in body:
            if fail == "dns:" + args[-1]:
                return 1
            if "radio-sim" in body and not any("--network-alias=radio-sim" in value["args"] for value in state.values()):
                return 1
            print("192.168.42.10 STREAM radio-sim\n192.168.42.10 DGRAM\n192.168.42.10 RAW")
        elif "touch /tmp/nos3-ci-start-fsw" in body:
            state[name]["released"] = True
            state_path.write_text(json.dumps(state))
        elif "/proc/net/" in body:
            print("  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode")
            # Every expected NOS/42/control port is independently removable.
            ports = (7777, 8090, 12000, 12001, 8010, 8011, 5010, 6010, 4277, 4278, 4377, 4378, 4477, 4478,
                     4279, 4280, 4245, 4227, 4234, 9999, 4284, 4281, 4282, 4283, 4286)
            row = 0
            for port in ports:
                if fail == "listen:" + str(port):
                    continue
                for status, remote_port in (("0A", 0), ("01", 12001), ("01", port)):
                    print(f"{row:4d}: 00000000:{port:04X} 0A2AA8C0:{remote_port:04X} {status} 00000000:00000000 00:00000000 00000000 0 0 12345")
                    row += 1
        return 0
    print("Unsupported fake Docker operation: " + repr(args), file=sys.stderr)
    return 2


def run_fixture_helper(args):
    """Windows subprocess requires an explicit interpreter for shell fixtures."""
    with Path(os.environ["NOS3_FAKE_LOG"]).open("a", encoding="utf-8") as stream:
        stream.write(json.dumps({"kind": "helper", "args": args}) + "\n")
    if os.name == "nt":
        original_run = subprocess.run

        def run(command, *positional, **keywords):
            if isinstance(command, (list, tuple)) and command and command[0] == "docker":
                command = [sys.executable, "-B", str(Path(__file__).resolve()), "--fake-docker", *command[1:]]
            return original_run(command, *positional, **keywords)

        subprocess.run = run
    script = args[0]
    if os.name == "nt":
        args = [(value[1] + ":" + value[2:]) if len(value) > 3 and value[0] == "/" and value[2] == "/" else value for value in args]
        script = args[0]
    sys.argv = args
    runpy.run_path(script, run_name="__main__")


@unittest.skipUnless(BASH, "Bash is required for actual launcher regression execution")
class LaunchTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix="nos3-launch-", dir=os.environ.get("NOS3_TEST_TMPDIR"))
        self.addCleanup(self.directory.cleanup)
        self.path = Path(self.directory.name)
        self.scripts = self.path / "scripts"
        shutil.copytree(ROOT / "scripts", self.scripts)
        self.source = self.path / "cfg/build/InOut/Inp_Sim.txt"
        self.source.parent.mkdir(parents=True)
        shutil.copyfile(ROOT / "cfg/InOut/Inp_Sim.txt", self.source)
        self.original = self.source.read_bytes()
        self.runtime = self.path / "user/42/NOS3InOut/Inp_Sim.txt"
        (self.path / "user/42").mkdir(parents=True)
        for relative in ("fsw/build/exe/cpu1", "gsw/cosmos", "gsw/build", "sims/build/bin", "bin", "tmp"):
            (self.path / relative).mkdir(parents=True)
        self.log_path = self.path / "commands.jsonl"
        fixture_path = self.shell_path(self.path)
        # The real launcher historically uses this fixed host scratch directory.
        # Redirect that data path only; do not change launch order or predicates.
        launcher = self.scripts / "ci_launch.sh"
        launcher.write_text(re.sub(r"/tmp/nos3(?=[/:\s\"']|$)", fixture_path + "/tmp/nos3", launcher.read_text()), newline="\n")
        (self.scripts / "env.sh").write_text(
            "#!/bin/bash\n"
            + "BASE_DIR=" + shlex.quote(fixture_path) + "\n"
            + 'SCRIPT_DIR="$BASE_DIR/scripts"\n'
            + 'USER_NOS3_DIR="$BASE_DIR/user"\n'
            + 'FSW_DIR="$BASE_DIR/fsw/build/exe/cpu1"\n'
            + 'GSW_DIR="$BASE_DIR/gsw/cosmos"\n'
            + 'SIM_DIR="$BASE_DIR/sims/build"\n'
            + 'SIM_BIN="$SIM_DIR/bin"\n'
            + 'COMPONENT_DIR="$BASE_DIR/components"\n'
            + 'DCALL=docker\nDNETWORK="docker network"\nDFLAGS="docker run"\n'
            + 'DBOX=nos3-test-image\nCRYPTO_TX_RADIO_PORT=8010\nCRYPTO_RX_RADIO_PORT=8011\n',
            newline="\n",
        )
        executable = shlex.quote(self.shell_path(Path(sys.executable)))
        driver = shlex.quote(Path(__file__).resolve().as_posix())
        for kind in ("docker", "xhost", "sleep", "python3"):
            wrapper = self.path / "bin" / kind
            wrapper.write_text("#!/bin/bash\nexec " + executable + " " + driver + " --fake-" + kind + ' "$@"\n', newline="\n")
            wrapper.chmod(0o755)
        self.environment = os.environ.copy()
        self.environment.update({
            "PATH": str(self.path / "bin") + os.pathsep + os.environ.get("PATH", ""),
            "NOS3_FAKE_LOG": str(self.log_path),
            "NOS3_FAKE_STATE": str(self.path / "state.json"),
            "TMPDIR": fixture_path + "/tmp",
            "NOS3_CI_TIMEOUT_SECONDS": "5",
            "MSYS_NO_PATHCONV": "1",
            "MSYS2_ARG_CONV_EXCL": "*",
        })
        self.environment.pop("DISPLAY", None)

    @staticmethod
    def shell_path(path):
        value = path.as_posix()
        if os.name == "nt":
            return "/" + value[0].lower() + value[2:]
        return value

    def launch(self, *args, fail="", display=None):
        self.environment["NOS3_FAKE_FAIL"] = fail
        if display is not None:
            self.environment["DISPLAY"] = display
        self.started = time.monotonic()
        self.result = subprocess.run(
            [BASH, "--noprofile", "--norc", self.shell_path(self.scripts / "ci_launch.sh"), *args],
            env=self.environment, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            text=True, timeout=180, cwd=self.path,
        )
        self.elapsed = time.monotonic() - self.started
        self.calls = [json.loads(line) for line in self.log_path.read_text().splitlines()] if self.log_path.exists() else []
        candidates = list((self.path / "user/42").glob("NOS3InOut-ci.*/Inp_Sim.txt"))
        if len(candidates) == 1:
            self.runtime = candidates[0]
        return self.result

    def runs(self):
        return [(i, call["args"]) for i, call in enumerate(self.calls) if call["kind"] == "docker" and call["args"][0] == "run"]

    def run_named(self, name):
        matches = [(i, args) for i, args in self.runs() if "--name" in args and args[args.index("--name") + 1] == name]
        self.assertEqual(len(matches), 1, (name, self.result.stdout))
        return matches[0]

    def assert_failed_without_success(self):
        self.assertNotEqual(self.result.returncode, 0, self.result.stdout)
        self.assertNotIn("headless launch script completed", self.result.stdout)
        self.assertNotIn("CI infrastructure startup checks passed", self.result.stdout)
        self.assertLess(self.elapsed, 180)

    def test_headless_changes_runtime_copy_only_and_has_no_host_x_dependency(self):
        self.launch()
        self.assertEqual(self.result.returncode, 0, self.result.stdout)
        self.assertEqual(self.source.read_bytes(), self.original)
        graphics = [line for line in self.runtime.read_text().splitlines() if "Graphics Front End?" in line]
        self.assertEqual(len(graphics), 1)
        self.assertEqual(graphics[0].split()[0], "FALSE")
        self.assertFalse(any(call["kind"] == "xhost" for call in self.calls))
        for _, args in self.runs():
            self.assertFalse(any("/tmp/.X11-unix" in arg or arg.startswith("DISPLAY=") for arg in args), args)

    def test_gui_preserves_graphics_and_host_display(self):
        self.launch("--use-cosmos-gui", display=":72")
        self.assertEqual(self.result.returncode, 0, self.result.stdout)
        self.assertEqual(self.source.read_bytes(), self.original)
        self.assertEqual(self.runtime.read_bytes(), self.original)
        _, args = self.run_named("sc01-fortytwo")
        self.assertIn("DISPLAY=:72", args)
        self.assertTrue(any("/tmp/.X11-unix" in arg for arg in args))

    def test_radio_alias_and_dependency_checks_precede_crypto(self):
        self.launch()
        self.assertEqual(self.result.returncode, 0, self.result.stdout)
        radio_index, radio = self.run_named("sc01-generic-radio-sim")
        self.assertIn("--network-alias=radio-sim", radio)
        crypto_index, _ = self.run_named("sc01-cryptolib")
        self.assertLess(radio_index, crypto_index)
        probes = [" ".join(call["args"]) for call in self.calls[radio_index + 1:crypto_index]]
        self.assertTrue(any("getent" in probe and "radio-sim" in probe for probe in probes), probes)
        self.assertTrue(any("8010" in probe or "1F4A" in probe.upper() for probe in probes), probes)

    def test_crypto_is_ready_before_fsw_core_release(self):
        self.launch()
        self.assertEqual(self.result.returncode, 0, self.result.stdout)
        crypto_index, _ = self.run_named("sc01-cryptolib")
        _, flight = self.run_named("sc01_nos_fsw")
        self.assertTrue(any("nos3-ci-start-fsw" in arg and "core-cpu1" in arg for arg in flight))
        releases = [index for index, call in enumerate(self.calls) if call["kind"] == "docker" and "touch /tmp/nos3-ci-start-fsw" in " ".join(call["args"])]
        self.assertEqual(len(releases), 1, self.result.stdout)
        self.assertLess(crypto_index, releases[0])
        probes = [" ".join(call["args"]) for call in self.calls[crypto_index + 1:releases[0]]]
        self.assertTrue(any("8011" in probe or "1F4B" in probe.upper() for probe in probes), probes)

    def test_early_42_exit_fails_without_completion(self):
        self.launch(fail="exit:sc01-fortytwo")
        self.assert_failed_without_success()
        self.run_named("sc01-fortytwo")
        self.assertIn("exited during startup", self.result.stdout)

    def test_radio_dns_failure_is_bounded_and_never_starts_crypto(self):
        self.launch(fail="dns:radio-sim")
        self.assert_failed_without_success()
        self.run_named("sc01-generic-radio-sim")
        self.assertIn("DNS", self.result.stdout)
        self.assertFalse(any("sc01-cryptolib" in args for _, args in self.runs()))

    def test_radio_listener_failure_is_bounded_and_never_starts_crypto(self):
        self.launch(fail="listen:8010")
        self.assert_failed_without_success()
        self.run_named("sc01-generic-radio-sim")
        self.assertIn("8010", self.result.stdout)
        self.assertFalse(any("sc01-cryptolib" in args for _, args in self.runs()))

    def test_crypto_exit_prevents_fsw_core_release(self):
        self.launch(fail="exit:sc01-cryptolib")
        self.assert_failed_without_success()
        self.run_named("sc01-cryptolib")
        self.assertIn("exited during startup", self.result.stdout)
        self.assertFalse(any("touch /tmp/nos3-ci-start-fsw" in " ".join(call["args"]) for call in self.calls))

    def test_other_dependency_exit_during_final_fsw_wait_fails(self):
        self.launch(fail="exit-after-release:sc01-generic-radio-sim")
        self.assert_failed_without_success()
        self.assertTrue(any("touch /tmp/nos3-ci-start-fsw" in " ".join(call["args"]) for call in self.calls), self.result.stdout)
        self.assertIn("exited during startup", self.result.stdout)

    def test_container_creation_failure_never_claims_success(self):
        self.launch(fail="run:sc01-fortytwo")
        self.assert_failed_without_success()

    def test_unknown_option_starts_nothing(self):
        self.launch("--unknown-test-option")
        self.assert_failed_without_success()
        self.assertEqual(self.calls, [])


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--fake-python3":
        run_fixture_helper(sys.argv[2:])
        raise SystemExit(0)
    if len(sys.argv) > 1 and sys.argv[1].startswith("--fake-"):
        raise SystemExit(fake_command(sys.argv[1][7:], sys.argv[2:]))
    unittest.main()
