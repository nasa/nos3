"""Execute the real Ruby system test against deterministic COSMOS 4 API fakes.

These are regression tests for the test logic, not evidence of a live mission.
Set RUBY to a Ruby executable when it is not available on PATH.
"""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
RUBY = os.environ.get("RUBY") or shutil.which("ruby")
BASH = os.environ.get("NOS3_TEST_BASH") or shutil.which("bash")
FAKE = r'''
module Process
  def self.clock_gettime(*_args)
    @test_clock = (@test_clock || 0.0) + 0.5
  end
end
module Kernel
  def sleep(_seconds); end
end
module Cosmos
  module Script
    def scenario; ENV.fetch('SCENARIO'); end
    def counters
      @counters ||= Hash.new(0)
    end
    def snapshots
      @snapshots ||= Hash.new(0)
    end
    def get_target_list
      raise 'COSMOS API unavailable' if scenario == 'api_failure'
      scenario == 'missing_radio' ? ['SAMPLE'] : ['SAMPLE', 'SAMPLE_RADIO', 'SIM_42_TRUTH']
    end
    def get_interface_targets(interface)
      return ['SAMPLE'] if scenario == 'wrong_mapping'
      interface == 'DEBUG' ? ['SAMPLE'] : ['SAMPLE_RADIO']
    end
    def get_tlm_cnt(target, _packet)
      @after_snapshot ||= {}
      if @after_snapshot.delete(target)
        counters[target] += 1 if scenario == 'torn_snapshot'
        return counters[target]
      end
      return 0 if scenario == 'receive_reset' && @command_target == target
      return counters[target] if scenario == 'stale_packets'
      return counters[target] if scenario == 'post_command_stale' && @command_target == target
      counters[target] += 1
    end
    def get_tlm_packet(target, _packet, type)
      raise 'Expected raw snapshot' unless type == :RAW
      @after_snapshot[target] = true
      snapshots[target] += 1
      if target == 'SIM_42_TRUTH'
        second = snapshots[target].to_f
        second = 0.0 if scenario == 'frozen_truth'
        second = 30.0 - second if scenario == 'backwards_truth'
        minute = 12
        if scenario == 'minute_rollover'
          minute = snapshots[target] == 1 ? 12 : 13
          second = snapshots[target] == 1 ? 59.9 : 0.1
        end
        second = Float::NAN if scenario == 'invalid_truth'
        return [['YEAR', 2026, nil], ['DOY', 287, nil],
                ['UTC_HOUR', 14, nil], ['UTC_MINUTE', minute, nil],
                ['UTC_SECOND', second, nil]]
      end
      count = @command_count ||= (scenario == 'command_rollover' ? 255 : 9)
      error = scenario == 'nonzero_baseline_errors' ? 3 : 0
      device_error = scenario == 'nonzero_baseline_errors' ? 4 : 0
      if @command_target == target
        error += 1 if scenario == 'command_error'
        device_error += 1 if scenario == 'device_error'
        device_error += 1 if scenario == 'late_device_error' && snapshots[target] >= 3
      end
      count = '9' if scenario == 'invalid_command_count'
      result = [['CMD_COUNT', count, nil], ['CMD_ERR_COUNT', error, nil],
                ['DEVICE_ERR_COUNT', device_error, nil]]
      result.pop if scenario == 'missing_field'
      result
    end
    def cmd(command)
      target, packet = command.split
      raise 'Unexpected command' unless packet == 'SAMPLE_NOOP_CC'
      puts "FAKE_SENT #{target}"
      @command_target = target
      return if scenario == 'command_dropped' || (scenario == 'radio_dropped' && target == 'SAMPLE_RADIO')
      step = scenario == 'unexpected_command' ? 2 : 1
      @command_count = (@command_count + step) % 256
    end
  end
end
'''


@unittest.skipUnless(RUBY, "Ruby is required to execute COSMOS system-test regressions")
class SystemTestRegression(unittest.TestCase):
    def run_scenario(self, scenario):
        with tempfile.TemporaryDirectory(prefix="nos3-ruby-system-test-") as folder:
            directory = Path(folder)
            (directory / "cosmos").mkdir()
            (directory / "cosmos.rb").write_text(FAKE, encoding="utf-8")
            (directory / "cosmos" / "script.rb").write_text("", encoding="utf-8")
            environment = dict(os.environ, SCENARIO=scenario)
            return subprocess.run(
                [RUBY, "-I", str(directory), str(ROOT / "scripts/gsw/system_test.rb")],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                env=environment,
                timeout=10,
                check=False,
            )

    def expect_failure(self, scenario, message):
        result = self.run_scenario(scenario)
        self.assertNotEqual(result.returncode, 0, result.stdout)
        self.assertIn(message, result.stdout)
        self.assertNotIn("TEST COMPLETED SUCCESSFULLY", result.stdout)
        return result.stdout

    def test_real_script_exercises_debug_and_radio(self):
        result = self.run_scenario("success")
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertEqual(result.stdout.count("FAKE_SENT SAMPLE\n"), 1)
        self.assertEqual(result.stdout.count("FAKE_SENT SAMPLE_RADIO\n"), 1)
        self.assertIn("PASS: Fresh 42 truth", result.stdout)
        self.assertIn("TEST COMPLETED SUCCESSFULLY", result.stdout)

    def test_command_count_wraparound_is_valid(self):
        result = self.run_scenario("command_rollover")
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("CMD_COUNT 255 -> 0", result.stdout)

    def test_truth_minute_wraparound_is_valid(self):
        result = self.run_scenario("minute_rollover")
        self.assertEqual(result.returncode, 0, result.stdout)

    def test_existing_errors_are_not_cleared_or_hidden(self):
        result = self.run_scenario("nonzero_baseline_errors")
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("error=3 device_error=4", result.stdout)

    def test_radio_failure_cannot_pass_on_debug_path(self):
        output = self.expect_failure("radio_dropped", "Timed out waiting for SAMPLE_RADIO CMD_COUNT")
        self.assertIn("PASS: SAMPLE fresh telemetry", output)
        self.assertIn("FAKE_SENT SAMPLE_RADIO", output)

    def test_no_radio_target_cannot_silently_skip(self):
        self.expect_failure("missing_radio", "Required target SAMPLE_RADIO is not configured")

    def test_radio_mapping_cannot_use_debug(self):
        self.expect_failure("wrong_mapping", "SAMPLE_RADIO must use RADIO")

    def test_stale_truth_cannot_send_command(self):
        output = self.expect_failure("stale_packets", "Timed out waiting for fresh SIM_42_TRUTH")
        self.assertNotIn("FAKE_SENT", output)

    def test_frozen_truth_values_fail_despite_arriving_packets(self):
        self.expect_failure("frozen_truth", "Timed out waiting for advancing 42 simulated time")

    def test_backwards_truth_fails(self):
        self.expect_failure("backwards_truth", "42 simulated time went backwards")

    def test_invalid_truth_fails(self):
        self.expect_failure("invalid_truth", "Invalid 42 truth time")

    def test_torn_snapshots_cannot_pass(self):
        self.expect_failure("torn_snapshot", "Timed out waiting for fresh SIM_42_TRUTH")

    def test_dropped_command_fails_despite_fresh_housekeeping(self):
        self.expect_failure("command_dropped", "Timed out waiting for SAMPLE CMD_COUNT")

    def test_cached_post_command_packet_cannot_pass(self):
        self.expect_failure("post_command_stale", "Timed out waiting for SAMPLE CMD_COUNT")

    def test_receive_counter_reset_fails(self):
        self.expect_failure("receive_reset", "Telemetry receive counter reset for SAMPLE")

    def test_more_than_one_command_increment_fails(self):
        self.expect_failure("unexpected_command", "unexpected command count")

    def test_command_error_fails(self):
        self.expect_failure("command_error", "error counters changed")

    def test_device_error_fails(self):
        self.expect_failure("device_error", "error counters changed")

    def test_later_error_cannot_pass_immediate_cached_check(self):
        self.expect_failure("late_device_error", "command result did not remain stable")

    def test_invalid_command_count_fails(self):
        self.expect_failure("invalid_command_count", "Invalid SAMPLE CMD_COUNT")

    def test_missing_field_fails(self):
        self.expect_failure("missing_field", "DEVICE_ERR_COUNT")

    def test_api_failure_fails(self):
        self.expect_failure("api_failure", "COSMOS API unavailable")


WRAPPER_ENV = r'''
record() { printf '%s\0' "$@" >> "$WRAPPER_LOG"; printf '\n' >> "$WRAPPER_LOG"; }
timeout() {
    record timeout "$@"
    while [[ "$1" == --* ]]; do shift; done
    [[ "$1" =~ ^[0-9]+$ ]] || return 91
    shift
    "$@"
}
docker() {
    record docker "$@"
    if [[ "$1" == inspect ]]; then
        [[ "$WRAPPER_MODE" != inspect_failure ]] || return 27
        if [[ "$WRAPPER_MODE" == invalid_id ]]; then printf 'not-an-id\n'; else printf '%s\n' "$WRAPPER_ID"; fi
    elif [[ "$1" == exec ]]; then
        [[ "$WRAPPER_MODE" != exec_failure ]] || return 23
    else
        return 92
    fi
}
mktemp() { command mktemp "$WRAPPER_DIR/identity.XXXXXX"; }
probe_count=0
python3() {
    record probe "$@"
    probe_count=$((probe_count + 1))
    local previous='' argument
    for argument in "$@"; do
        if [[ "$previous" == --identity-file ]]; then [[ -f "$argument" ]] || return 93; fi
        previous="$argument"
    done
    [[ "$WRAPPER_MODE" != pre_failure || "$probe_count" != 1 ]] || return 31
    [[ "$WRAPPER_MODE" != post_failure || "$probe_count" != 2 ]] || return 32
}
export -f record timeout docker mktemp python3
'''


@unittest.skipUnless(
    BASH and (os.name != "nt" or os.environ.get("NOS3_TEST_BASH")),
    "Bash is required; on Windows set NOS3_TEST_BASH to Git Bash",
)
class SystemTestWrapperRegression(unittest.TestCase):
    """Run actual Bash control flow; real Docker readiness has separate tests."""

    def run_wrapper(self, mode):
        with tempfile.TemporaryDirectory(prefix="nos3-system-test-wrapper-") as folder:
            directory = Path(folder)
            environment_script = directory / "environment.sh"
            environment_script.write_text(WRAPPER_ENV, encoding="utf-8")
            log = directory / "calls.log"
            environment = dict(
                os.environ,
                BASH_ENV=environment_script.as_posix(),
                WRAPPER_DIR=directory.as_posix(),
                WRAPPER_LOG=log.as_posix(),
                WRAPPER_ID="a" * 64,
                WRAPPER_MODE=mode,
                SYSTEM_TEST_FILE_PATH="/scripts/custom path/system_test.rb",
            )
            result = subprocess.run(
                [BASH, str(ROOT / "scripts/system_tests.sh")],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                env=environment,
                timeout=10,
                check=False,
            )
            calls = [
                [field.decode("utf-8") for field in row.split(b"\0")[:-1]]
                for row in log.read_bytes().splitlines()
            ] if log.exists() else []
            self.assertEqual(list(directory.glob("identity.*")), [], "Temporary identity file was not cleaned up")
            return result, calls

    def test_pin_precedes_both_probes_and_exec_uses_same_identity(self):
        result, calls = self.run_wrapper("success")
        self.assertEqual(result.returncode, 0, result.stdout)
        relevant = [call for call in calls if call[0] in ("docker", "probe")]
        self.assertEqual([call[:2] if call[0] == "docker" else [call[0], Path(call[1]).name]
                          for call in relevant], [
            ["docker", "inspect"],
            ["probe", "ci_readiness.py"],
            ["docker", "exec"],
            ["probe", "ci_readiness.py"],
        ])
        probes = [call for call in calls if call[0] == "probe"]
        self.assertEqual(probes[0][probes[0].index("--container") + 1], "a" * 64)
        self.assertEqual(probes[1][probes[1].index("--container") + 1], "a" * 64)
        self.assertEqual(probes[0][probes[0].index("--identity-file") + 1],
                         probes[1][probes[1].index("--identity-file") + 1])
        execution = relevant[2]
        self.assertEqual(execution[2:], ["a" * 64, "timeout", "--signal=TERM", "--kill-after=5",
                                         "190", "ruby", "/scripts/custom path/system_test.rb"])
        self.assertNotIn("-it", execution)
        self.assertNotIn("-i", execution)
        self.assertNotIn("-t", execution)
        self.assertIn(["timeout", "--signal=TERM", "--kill-after=5", "200"] + execution, calls)

    def test_remote_failure_propagates_without_post_check(self):
        result, calls = self.run_wrapper("exec_failure")
        self.assertEqual(result.returncode, 23, result.stdout)
        self.assertEqual(sum(call[0] == "probe" for call in calls), 1)

    def test_pre_readiness_failure_prevents_exec(self):
        result, calls = self.run_wrapper("pre_failure")
        self.assertEqual(result.returncode, 31, result.stdout)
        self.assertFalse(any(call[:2] == ["docker", "exec"] for call in calls))

    def test_invalid_id_prevents_probe_and_exec(self):
        result, calls = self.run_wrapper("invalid_id")
        self.assertNotEqual(result.returncode, 0, result.stdout)
        self.assertIn("Invalid COSMOS container identity", result.stdout)
        self.assertFalse(any(call[0] == "probe" or call[:2] == ["docker", "exec"] for call in calls))

    def test_post_readiness_failure_propagates(self):
        result, calls = self.run_wrapper("post_failure")
        self.assertEqual(result.returncode, 32, result.stdout)
        self.assertEqual(sum(call[0] == "probe" for call in calls), 2)

    def test_inspect_failure_prevents_probe_and_exec(self):
        result, calls = self.run_wrapper("inspect_failure")
        self.assertEqual(result.returncode, 27, result.stdout)
        self.assertFalse(any(call[0] == "probe" or call[:2] == ["docker", "exec"] for call in calls))


if __name__ == "__main__":
    unittest.main()
