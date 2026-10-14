#!/bin/bash
set -Ee -o pipefail

TEST_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" && pwd )
TEST_PATH=${SYSTEM_TEST_FILE_PATH:-/scripts/gsw/system_test.rb}
# Pin the actual container: a replacement must not silently inherit this test.
test_container=$(timeout 10 docker inspect --format '{{.Id}}' cosmos-openc3-operator-1)
[[ "$test_container" =~ ^[a-f0-9]{64}$ ]] || { echo 'Invalid COSMOS container identity' >&2; exit 1; }
test_identities=$(mktemp)
trap 'rm -f -- "$test_identities"' EXIT
python3 "$TEST_SCRIPT_DIR/ci_readiness.py" wait --container "$test_container" \
    --identity-file "$test_identities" --listen 7777 --timeout "${NOS3_CI_TIMEOUT_SECONDS:-180}"
# Bound the remote Ruby process as well as the Docker client; no interactive TTY.
timeout --signal=TERM --kill-after=5 200 docker exec "$test_container" \
    timeout --signal=TERM --kill-after=5 190 ruby "$TEST_PATH"
python3 "$TEST_SCRIPT_DIR/ci_readiness.py" wait --container "$test_container" \
    --identity-file "$test_identities" --listen 7777 --timeout 10
