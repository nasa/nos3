"""Isolated regressions for nasa/nos3#890; no Docker or network access."""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


SOURCE = Path(__file__).resolve().parents[2] / "scripts" / "cfg" / "prepare.sh"
BASH = os.environ.get("NOS3_TEST_BASH")
if not BASH and os.name == "nt":
    candidate = Path("C:/Program Files/Git/bin/bash.exe")
    if candidate.is_file():
        BASH = str(candidate)
if not BASH:
    BASH = shutil.which("bash")


ENVIRONMENT = r'''#!/bin/bash
FIXTURE_ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
printf 'source\n' >> "$FIXTURE_ROOT/calls"
BASE_DIR="$FIXTURE_ROOT"
USER_NOS3_DIR="$FIXTURE_ROOT/user state"
USER_YAMCS_PATH="$USER_NOS3_DIR/.m2"
USER_FPRIME_PATH="$FIXTURE_ROOT/fprime state"
DCALL=docker
DFLAGS_CPUS='docker run --rm --cpus=2'
DBOX=fixture/nos3:test
# This is deliberate: the real env.sh also changes SCRIPT_DIR.
SCRIPT_DIR="$FIXTURE_ROOT/scripts"
if [ "$FAIL_STAGE" = environment ]; then return 23; fi
if [ "$FAIL_STAGE" = missing-variable ]; then unset DBOX; fi
mkdir() {
    if [ "$FAIL_STAGE" = mkdir ]; then return 31; fi
    command mkdir "$@"
}
git() {
    printf 'git\n' >> "$FIXTURE_ROOT/calls"
    if [ "$FAIL_STAGE" = clone ]; then return 32; fi
    if [ "$FAIL_STAGE" = clone-no-checkout ]; then return 0; fi
    if [ "$1" != clone ]; then return 90; fi
    command mkdir -p -- "${@: -1}/.git"
}
docker() {
    printf 'docker %s\n' "$1" >> "$FIXTURE_ROOT/calls"
    printf 'DOCKER_ARG=%s\n' "$@" >> "$FIXTURE_ROOT/arguments"
    if [ "$1" = image ] && [ "$FAIL_STAGE" = pull ]; then return 33; fi
    if [ "$1" = run ] && [ "$FAIL_STAGE" = build ]; then return 34; fi
    return 0
}
pip3() {
    printf 'pip3\n' >> "$FIXTURE_ROOT/calls"
    if [ "$FAIL_STAGE" = optional ]; then return 35; fi
    return 0
}
python3() {
    printf 'igniter\n' >> "$FIXTURE_ROOT/calls"
    printf 'GUI_ARG=%s\n' "$@" >> "$FIXTURE_ROOT/arguments"
    return 0
}
sleep() { return 0; }
'''


@unittest.skipUnless(BASH, "Bash is required for the preparation regression tests")
class PreparationTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="nos3 prepare tests ")
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.script = self.root / "scripts" / "cfg" / "prepare.sh"
        self.script.parent.mkdir(parents=True)
        shutil.copyfile(SOURCE, self.script)
        self.environment = self.root / "scripts" / "env.sh"
        self.environment.write_text(ENVIRONMENT, encoding="utf-8", newline="\n")
        # A mistaken second include must not accidentally succeed in this fixture.
        (self.script.parent / "env.sh").write_text("return 91\n", encoding="utf-8")

    def run_prepare(self, fail_stage=""):
        environment = dict(os.environ, FAIL_STAGE=fail_stage)
        environment.pop("BASH_ENV", None)
        environment.pop("ENV", None)
        return subprocess.run(
            [BASH, "--noprofile", "--norc", self.script.as_posix()],
            cwd=self.root,
            env=environment,
            capture_output=True,
            text=True,
            timeout=15,
        )

    def calls(self):
        path = self.root / "calls"
        return path.read_text(encoding="utf-8").splitlines() if path.exists() else []

    def assert_required_failure(self, result):
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("ERROR: NOS3 preparation failed:", result.stderr)
        self.assertNotIn("NOS3 required preparations complete!", result.stdout)
        self.assertNotIn("NOS3 prep script complete:", result.stdout)
        self.assertNotIn("pip3", self.calls())
        self.assertNotIn("igniter", self.calls())

    def test_missing_environment_fails_before_work(self):
        self.environment.unlink()
        result = self.run_prepare()
        self.assert_required_failure(result)
        self.assertIn("missing or unreadable", result.stderr)
        self.assertEqual(self.calls(), [])
        self.assertFalse((self.root / "user state").exists())

    def test_environment_directory_is_rejected(self):
        self.environment.unlink()
        self.environment.mkdir()
        self.assert_required_failure(self.run_prepare())
        self.assertEqual(self.calls(), [])

    @unittest.skipIf(os.name == "nt", "POSIX permissions are required")
    def test_unreadable_environment_fails_before_work(self):
        self.environment.chmod(0)
        self.addCleanup(self.environment.chmod, 0o600)
        if os.access(self.environment, os.R_OK):
            self.skipTest("The test user can read mode-000 files")
        self.assert_required_failure(self.run_prepare())
        self.assertEqual(self.calls(), [])

    def test_failed_environment_is_not_ignored(self):
        self.assert_required_failure(self.run_prepare("environment"))
        self.assertEqual(self.calls(), ["source"])

    def test_missing_required_environment_variable_is_rejected(self):
        result = self.run_prepare("missing-variable")
        self.assert_required_failure(result)
        self.assertIn("did not define DBOX", result.stderr)
        self.assertEqual(self.calls(), ["source"])

    def test_required_directory_failure_stops_before_pull(self):
        self.assert_required_failure(self.run_prepare("mkdir"))
        self.assertEqual(self.calls(), ["source"])

    def test_image_pull_failure_stops_before_clone(self):
        self.assert_required_failure(self.run_prepare("pull"))
        self.assertEqual(self.calls(), ["source", "docker image"])

    def test_clone_failure_stops_before_build(self):
        self.assert_required_failure(self.run_prepare("clone"))
        self.assertEqual(self.calls(), ["source", "docker image", "git"])

    def test_missing_checkout_stops_before_build(self):
        self.assert_required_failure(self.run_prepare("clone-no-checkout"))
        self.assertNotIn("docker run", self.calls())

    def test_build_failure_is_not_reported_as_success(self):
        self.assert_required_failure(self.run_prepare("build"))
        self.assertEqual(self.calls(), ["source", "docker image", "git", "docker run"])

    def test_success_sources_once_quotes_paths_and_keeps_gui(self):
        result = self.run_prepare()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(self.calls(), ["source", "docker image", "git", "docker run", "pip3", "igniter"])
        self.assertIn("NOS3 required preparations complete!", result.stdout)
        self.assertIn("NOS3 prep script complete:", result.stdout)
        arguments = (self.root / "arguments").read_text(encoding="utf-8").splitlines()
        workdir = arguments[arguments.index("DOCKER_ARG=-w") + 1]
        self.assertTrue(workdir.endswith("/user state/42"), arguments)
        self.assertIn("nos3 prepare tests ", workdir)
        self.assertTrue(any(line.startswith("GUI_ARG=") and line.endswith("/cfg/gui/cfg_gui_main.py") for line in arguments))
        self.assertTrue((self.root / "user state" / ".m2").is_dir())
        self.assertTrue((self.root / "fprime state").is_dir())

    def test_optional_failure_does_not_fail_preparation(self):
        result = self.run_prepare("optional")
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("optional Igniter dependencies could not be installed", result.stderr)
        self.assertIn("NOS3 prep script complete:", result.stdout)
        self.assertIn("igniter", self.calls())

    def test_rerun_reuses_existing_checkout_and_rebuilds(self):
        for _ in range(2):
            result = self.run_prepare()
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(self.calls().count("source"), 2)
        self.assertEqual(self.calls().count("git"), 1)
        self.assertEqual(self.calls().count("docker run"), 2)


if __name__ == "__main__":
    unittest.main()
