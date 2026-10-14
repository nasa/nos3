#!/bin/bash
# Standalone CI startup. The normal graphical make-launch path is unchanged.
set -Ee -o pipefail

GSW=cosmos
while [[ $# -gt 0 ]]; do
    case "$1" in
        --use-yamcs) GSW=yamcs ;;
        --use-cosmos-gui) GSW=cosmos-gui ;;
        --use-cosmos) GSW=cosmos ;;
        *) echo "Usage: $0 [--use-cosmos | --use-cosmos-gui | --use-yamcs]" >&2; exit 1 ;;
    esac
    shift
done

CI_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" && pwd )
[[ -r "$CI_SCRIPT_DIR/env.sh" ]] || { echo 'Missing scripts/env.sh' >&2; exit 1; }
# env.sh performs its own Docker Compose fallback; do not inherit errexit there.
set +e
source "$CI_SCRIPT_DIR/env.sh"
env_status=$?
set -e
[[ "$env_status" == 0 ]] || { echo 'Could not load scripts/env.sh' >&2; exit 1; }
readonly CI_SCRIPT_DIR
CI_TIMEOUT=${NOS3_CI_TIMEOUT_SECONDS:-180}
[[ "$CI_TIMEOUT" =~ ^[1-9][0-9]*$ ]] && (( CI_TIMEOUT <= 900 )) || {
    echo 'NOS3_CI_TIMEOUT_SECONDS must be between 1 and 900' >&2; exit 1;
}
for required in BASE_DIR USER_NOS3_DIR FSW_DIR GSW_DIR SIM_DIR SIM_BIN DCALL DBOX; do
    [[ -n "${!required}" ]] || { echo "Missing environment value: $required" >&2; exit 1; }
done
for program in python3 timeout; do command -v "$program" >/dev/null; done
[[ -d "$USER_NOS3_DIR/42" && -d "$BASE_DIR/cfg/build/InOut" ]] || {
    echo 'Run make prep and make config first' >&2; exit 1;
}
if [[ "$GSW" == cosmos-gui ]]; then
    [[ -n "${DISPLAY:-}" ]] || { echo 'Graphical mode requires DISPLAY' >&2; exit 1; }
    timeout 10 xhost +local: >/dev/null
fi

declare -a WATCH_ARGS=()
CI_IDENTITY_FILE=$(mktemp)
trap 'rm -f -- "$CI_IDENTITY_FILE"' EXIT
docker_cmd() { timeout --signal=TERM --kill-after=5 "$CI_TIMEOUT" $DCALL "$@"; }
failed() {
    local code=$?
    trap - ERR
    echo "ERROR: CI startup failed (exit $code). No readiness success is claimed." >&2
    echo 'No mission-wide cleanup was performed; inspect remaining containers/logs before make stop.' >&2
    return "$code"
}
trap failed ERR
track_container() {
    [[ "$NEW_CONTAINER" =~ ^[a-f0-9]{64}$ ]] || { echo 'Invalid Docker container ID' >&2; return 1; }
    WATCH_ARGS+=(--watch "$NEW_CONTAINER")
}
start_container() {
    NEW_CONTAINER=$(docker_cmd run --pull=never "$@")
    track_container
}
wait_ready() {
    local target=$1
    shift
    python3 "$CI_SCRIPT_DIR/ci_readiness.py" wait --docker "$DCALL" \
        --container "$target" --timeout "$CI_TIMEOUT" --identity-file "$CI_IDENTITY_FILE" "${WATCH_ARGS[@]}" "$@"
}

# Never pull an image implicitly in the middle of the dependency sequence.
docker_cmd image inspect "$DBOX" >/dev/null
if [[ "$GSW" != yamcs ]]; then docker_cmd image inspect ballaerospace/cosmos:4.5.0 >/dev/null; fi
mkdir -p "$FSW_DIR"/data/{cam,evs,hk,inst} /tmp/nos3/data/{cam,evs,hk,inst} /tmp/nos3/uplink
cp "$FSW_DIR/cf/cfe_es_startup.scr" /tmp/nos3/uplink/tmp0.so 2>/dev/null || true
cp "$FSW_DIR/cf/sample.so" /tmp/nos3/uplink/tmp1.so 2>/dev/null || true

SC_NUM=sc01
SC_NET=nos3-sc01
docker_cmd network rm nos3-core 2>/dev/null || true
docker_cmd network create --driver=bridge --subnet=192.168.41.0/24 --gateway=192.168.41.1 nos3-core
docker_cmd network rm "$SC_NET" 2>/dev/null || true
docker_cmd network create "$SC_NET"

echo 'Launch ground software...'
if [[ "$GSW" == cosmos ]]; then
    start_container -d --name cosmos-openc3-operator-1 --network=nos3-core \
        --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
        -v "$GSW_DIR/config:/cosmos/config:ro" -v "$GSW_DIR:/cosmos" \
        -v "$BASE_DIR/scripts:/scripts:ro" -v /tmp/nos3:/tmp/nos3 \
        -e QT_X11_NO_MITSHM=1 -e PROCESSOR_ENDIANNESS=LITTLE_ENDIAN \
        -w /cosmos/tools --entrypoint /bin/bash ballaerospace/cosmos:4.5.0 -c \
        'set -e; if ! command -v xvfb-run >/dev/null; then apt-get update; DEBIAN_FRONTEND=noninteractive apt-get install -y xvfb; fi; exec xvfb-run -a ruby CmdTlmServer /cosmos/config/tools/cmd_tlm_server/cmd_tlm_server.txt'
elif [[ "$GSW" == cosmos-gui ]]; then
    # Preserve the stock GUI entrypoint and host X display in this explicit mode.
    NEW_CONTAINER=$(timeout --signal=TERM --kill-after=5 "$CI_TIMEOUT" $DFLAGS \
        -v "$BASE_DIR:$BASE_DIR" -dit -v /tmp/nos3:/tmp/nos3 \
        -v /tmp/.X11-unix:/tmp/.X11-unix:ro -e DISPLAY="$DISPLAY" \
        -e QT_X11_NO_MITSHM=1 -e PROCESSOR_ENDIANNESS=LITTLE_ENDIAN \
        -v "$BASE_DIR/scripts:/scripts:ro" -w "$GSW_DIR" \
        --name cosmos-openc3-operator-1 --network=nos3-core ballaerospace/cosmos:4.5.0)
    track_container
    echo "Open the COSMOS command/telemetry server. Waiting up to $CI_TIMEOUT seconds for its API."
else
    cp -r "$BASE_DIR/gsw/yamcs" "$USER_NOS3_DIR/"
    start_container -dit --name cosmos-openc3-operator-1 --hostname cosmos \
        --network=nos3-core --network-alias=cosmos -p 8090:8090 -p 5012:5012 \
        -e COMPONENT_DIR="$COMPONENT_DIR" -v "$BASE_DIR:$BASE_DIR" \
        -v "$USER_NOS3_DIR:$USER_NOS3_DIR" -w "$USER_NOS3_DIR/yamcs" "$DBOX" \
        mvn "-Dmaven.repo.local=$USER_NOS3_DIR/.m2/repository" "-DCOMPONENT_DIR=$COMPONENT_DIR" yamcs:run
fi
GSW_CONTAINER=$NEW_CONTAINER
docker_cmd network connect --alias cosmos --alias active-gs "$SC_NET" "$GSW_CONTAINER"
if [[ "$GSW" == yamcs ]]; then
    wait_ready "$GSW_CONTAINER" --listen 8090
    echo 'YAMCS is listening at http://localhost:8090 (no browser is required by CI).'
else
    wait_ready "$GSW_CONTAINER" --listen 7777
fi

echo "$SC_NUM - NOS Engine Server..."
start_container -dit --name "$SC_NUM-nos-engine-server" -h nos-engine-server \
    --network="$SC_NET" --network-alias=nos-engine-server \
    --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
    -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" "$DBOX" \
    /usr/bin/nos_engine_server_standalone -f "$SIM_BIN/nos_engine_server_config.json"
ENGINE_CONTAINER=$NEW_CONTAINER
wait_ready "$ENGINE_CONTAINER" --listen 12000 --listen 12001

# These clients need both networks before their process starts, not afterwards.
for client in time stdio-terminal udp-terminal sim-bridge; do
    case "$client" in
        time) name=nos-time-driver; args=(./nos3-single-simulator -f nos3-simulator.xml time) ;;
        stdio-terminal) name=nos-terminal; args=(./nos3-single-simulator -f nos3-simulator.xml stdio-terminal) ;;
        udp-terminal) name=nos-udp-terminal; args=(./nos3-single-simulator -f nos3-simulator.xml udp-terminal) ;;
        sim-bridge) name=nos-sim-bridge; args=(./nos3-sim-cmdbus-bridge -f nos3-simulator.xml) ;;
    esac
    NEW_CONTAINER=$(docker_cmd create -it --name "$name" --network=nos3-core \
        -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" "$DBOX" "${args[@]}")
    docker_cmd network connect --alias "$name" "$SC_NET" "$NEW_CONTAINER"
    docker_cmd start "$NEW_CONTAINER" >/dev/null
    track_container
    wait_ready "$NEW_CONTAINER" --peer 12001
done

echo "$SC_NUM - 42..."
# A new private input/output directory avoids overwriting graphical inputs or
# retained output from previous runs. Only this CI copy disables rendering.
CI_42_DIR=$(mktemp -d "$USER_NOS3_DIR/42/NOS3InOut-ci.XXXXXX")
cp -R "$BASE_DIR/cfg/build/InOut/." "$CI_42_DIR/"
display_args=()
if [[ "$GSW" == cosmos-gui ]]; then
    display_args=(-e "DISPLAY=$DISPLAY" -v /tmp/.X11-unix:/tmp/.X11-unix:ro)
else
    python3 "$CI_SCRIPT_DIR/ci_readiness.py" headless-config "$CI_42_DIR/Inp_Sim.txt"
fi
echo "42 runtime inputs and outputs: $CI_42_DIR"
start_container -d --name "$SC_NUM-fortytwo" -h fortytwo --network="$SC_NET" --network-alias=fortytwo \
    --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
    "${display_args[@]}" -v "$USER_NOS3_DIR:$USER_NOS3_DIR" -w "$USER_NOS3_DIR/42" \
    "$DBOX" "$USER_NOS3_DIR/42/42" "${CI_42_DIR##*/}"
FORTYTWO_CONTAINER=$NEW_CONTAINER

# Stock Inp_IPC.txt accepts these sockets sequentially. Never probe by connect().
for entry in generic-reactionwheel-sim0:4278:4277 generic-reactionwheel-sim1:4378:4377 \
    generic-reactionwheel-sim2:4478:4477 generic-torquer-sim:4279 generic-thruster-sim:4280 \
    gps:4245 generic-css-sim:4227 generic-mag-sim:4234 truth42sim:9999 \
    generic-fss-sim:4284 generic-imu-sim:4281 generic-star-tracker-sim:4282 \
    generic-eps-sim:4283 generic-radio-sim:4286; do
    IFS=: read -r sim first second <<< "$entry"
    wait_ready "$FORTYTWO_CONTAINER" --listen "$first"
    sim_name="${SC_NUM}_${sim}"
    alias_args=()
    if [[ "$sim" == generic-radio-sim ]]; then
        # Radio resolves nos-fsw once during initialization, before cFS can
        # safely start. Keep its network endpoint alive behind a bounded gate;
        # a merely created/stopped container does not provide Docker DNS.
        start_container -dit --name "${SC_NUM}_nos_fsw" -h nos-fsw --network="$SC_NET" --network-alias=nos-fsw \
            -v "$BASE_DIR:$BASE_DIR" -v "$FSW_DIR:$FSW_DIR" -v "$SCRIPT_DIR:$SCRIPT_DIR" \
            -e USER="$(whoami)" -e LD_LIBRARY_PATH="$FSW_DIR:/usr/lib:/usr/local/lib" \
            -e NOS3_CI_START_TIMEOUT="$CI_TIMEOUT" \
            -w "$FSW_DIR" --sysctl fs.mqueue.msg_max=10000 --ulimit rtprio=99 --cap-add=sys_nice \
            "$DBOX" bash -c 'set -e; timeout --signal=TERM --kill-after=5 "$NOS3_CI_START_TIMEOUT" bash -c '\''until test -f /tmp/nos3-ci-start-fsw; do sleep 0.25; done'\''; exec ./core-cpu1 -R PO'
        FSW_CONTAINER=$NEW_CONTAINER
        wait_ready "$FSW_CONTAINER" --dns nos-fsw --dns-from "$ENGINE_CONTAINER" --network "$SC_NET"
        sim_name="${SC_NUM}-${sim}"
        alias_args=(-h radio-sim --network-alias=radio-sim)
    elif [[ "$sim" == truth42sim ]]; then
        sim_name="${SC_NUM}-${sim}"
        alias_args=(-h truth42sim --network-alias=truth42sim)
    fi
    start_container -d --name "$sim_name" --network="$SC_NET" "${alias_args[@]}" \
        -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" "$DBOX" \
        ./nos3-single-simulator -f nos3-simulator.xml "$sim"
    if [[ "$sim" == generic-radio-sim ]]; then
        RADIO_CONTAINER=$NEW_CONTAINER
        # Start CryptoLib immediately after the radio listener and DNS are ready;
        # the radio's opposite-direction discovery retry window is finite.
        wait_ready "$RADIO_CONTAINER" --listen 8010 --dns radio-sim --dns-from "$ENGINE_CONTAINER" --network "$SC_NET"
        start_container -dit --name "$SC_NUM-cryptolib" --network="$SC_NET" --network-alias=cryptolib \
            --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
            -e STANDALONE_TCP=1 -v "$BASE_DIR:$BASE_DIR" -w "$BASE_DIR/gsw/build" "$DBOX" ./support/standalone
        CRYPTO_CONTAINER=$NEW_CONTAINER
        wait_ready "$CRYPTO_CONTAINER" --accept 8011 --peer 8010 --udp 6010
        wait_ready "$RADIO_CONTAINER" --peer 8011 --peer 4286 --peer 12001
    else
        peer_args=(--peer "$first" --peer 12001)
        [[ -z "$second" ]] || peer_args+=(--peer "$second")
        wait_ready "$NEW_CONTAINER" "${peer_args[@]}"
    fi
done

for sim in camsim sample-sim; do
    start_container -d --name "${SC_NUM}_${sim}" --network="$SC_NET" \
        -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" "$DBOX" ./nos3-single-simulator -f nos3-simulator.xml "$sim"
    wait_ready "$NEW_CONTAINER" --peer 12001
done

echo "$SC_NUM - Flight Software..."
docker_cmd exec "$FSW_CONTAINER" touch /tmp/nos3-ci-start-fsw
wait_ready "$FSW_CONTAINER" --udp 5010 --peer 12000
wait_ready "$CRYPTO_CONTAINER" --peer 8010 --local-peer 8011 --udp 6010
wait_ready "$FORTYTWO_CONTAINER" --local-peer 4278 --local-peer 4277 --local-peer 4378 --local-peer 4377 \
    --local-peer 4478 --local-peer 4477 --local-peer 4279 --local-peer 4280 --local-peer 4245 \
    --local-peer 4227 --local-peer 4234 --local-peer 9999 --local-peer 4284 --local-peer 4281 \
    --local-peer 4282 --local-peer 4283 --local-peer 4286
echo 'CI infrastructure startup checks passed. Command/telemetry validation is a separate system-test step.'
echo 'For COSMOS run scripts/system_tests.sh; make ci-launch runs it automatically.'
