#!/bin/bash -i
set -e

# Default GSW is cosmos
GSW="cosmos"

# Parse GSW selection
while [[ $# -gt 0 ]]; do
  case "$1" in
    --use-yamcs)
      GSW="yamcs"
      shift
      ;;
    --use-cosmos-gui)
      GSW="cosmos-gui"
      shift
      ;;      
    --use-cosmos)
      GSW="cosmos"
      shift
      ;;
    *)
      echo "Unknown option: $1"
      echo "Usage: $0 [--use-cosmos | --use-cosmos-gui | --use-yamcs]"
      exit 1
      ;;
  esac
done

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source "$SCRIPT_DIR/env.sh"

if [ ! -d $USER_NOS3_DIR ]; then
    echo ""
    echo "    Need to run make prep first!"
    echo ""
    exit 1
fi

if [ ! -d $BASE_DIR/cfg/build ]; then
    echo ""
    echo "    Need to run make config first!"
    echo ""
    exit 1
fi

mkdir -p $FSW_DIR/data/{cam,evs,hk,inst}
mkdir -p /tmp/nos3/data/{cam,evs,hk,inst} /tmp/nos3/uplink
cp $BASE_DIR/fsw/build/exe/cpu1/cf/cfe_es_startup.scr /tmp/nos3/uplink/tmp0.so 2>/dev/null || true
cp $BASE_DIR/fsw/build/exe/cpu1/cf/sample.so /tmp/nos3/uplink/tmp1.so 2>/dev/null || true

$DNETWORK rm nos3_core 2>/dev/null || true
$DNETWORK create \
    --driver=bridge \
    --subnet=192.168.41.0/24 \
    --gateway=192.168.41.1 \
    nos3_core

echo "Launch GSW..."

if [ "$GSW" == "cosmos" ]; then
  echo "Launching COSMOS..."
  $DCALL run -d --name cosmos_openc3-operator_1 \
      --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
      -v "$GSW_DIR/config:/cosmos/config:ro" \
      -v "$GSW_DIR:/cosmos" \
      -v "$BASE_DIR/scripts:/scripts:ro" \
      -v /tmp/nos3:/tmp/nos3 \
      --network=nos3_core \
      -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
      -e DISPLAY=$DISPLAY \
      -e QT_X11_NO_MITSHM=1 \
      -e PROCESSOR_ENDIANNESS="LITTLE_ENDIAN" \
      -w /cosmos/tools \
      ballaerospace/cosmos:4.5.0 tail -f /dev/null

  sleep 5
  $DCALL exec cosmos_openc3-operator_1 bash -c "apt update && apt install -y xvfb"
  $DCALL exec -d cosmos_openc3-operator_1 bash -c "xvfb-run ruby CmdTlmServer /cosmos/config/tools/cmd_tlm_server/cmd_tlm_server.txt"

elif [ "$GSW" == "cosmos-gui" ]; then
    $DFLAGS -v $BASE_DIR:$BASE_DIR -dit -v /tmp/nos3:/tmp/nos3 -v /tmp/.X11-unix:/tmp/.X11-unix:ro -e DISPLAY=$DISPLAY -e QT_X11_NO_MITSHM=1 -e PROCESSOR_ENDIANNESS="LITTLE_ENDIAN" -w $GSW_DIR --name cosmos_openc3-operator_1 --network=nos3_core ballaerospace/cosmos:4.5.0

    echo ""
    echo "Please quickly click the COSMOS Ok button to launch"
    echo "Afterwards click the top left COSMOS button in the NOS3 Launcher"
    sleep 20
    echo ""
    echo "If you haven't fully started COSMOS by now, you're too late ... start over"
    echo ""

elif [ "$GSW" == "yamcs" ]; then
  echo "Launching YAMCS..."
  YAMCS_CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
  YAMCS_SCRIPT_DIR=$YAMCS_CFG_BUILD_DIR
  source $YAMCS_SCRIPT_DIR/env.sh

  # rm -rf $USER_NOS3_DIR/yamcs 2> /dev/null
  cp -r $BASE_DIR/gsw/yamcs $USER_NOS3_DIR/
  echo "Directories Created"

  $DCALL run -dit \
      --name cosmos_openc3-operator_1 \
      --hostname cosmos \
      --network=nos3_core \
      --network-alias=cosmos \
      -p 8090:8090 -p 5012:5012 \
      -e COMPONENT_DIR=$COMPONENT_DIR \
      -v $BASE_DIR:$BASE_DIR \
      -v $USER_NOS3_DIR:$USER_NOS3_DIR \
      -w $USER_NOS3_DIR/yamcs \
      $DBOX \
      mvn -Dmaven.repo.local=$USER_NOS3_DIR/.m2/repository -DCOMPONENT_DIR=$COMPONENT_DIR yamcs:run

  if ! pidof firefox > /dev/null; then
      echo "Opening Firefox to localhost:8090..."
      sleep 30 && firefox localhost:8090 &
  fi
fi

### Connections
$DCALL run -dit --name nos_terminal --network=nos3_core \
    -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" $DBOX \
    ./nos3-single-simulator -f nos3-simulator.xml stdio-terminal

$DCALL run -dit --name nos_udp_terminal --network=nos3_core \
    -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" $DBOX \
    ./nos3-single-simulator -f nos3-simulator.xml udp-terminal

$DCALL run -dit --name nos_sim_bridge --network=nos3_core \
    -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" $DBOX \
    ./nos3-sim-cmdbus-bridge -f nos3-simulator.xml

CFG_FILE="-f nos3-simulator.xml"

$DCALL run -dit --name nos_time_driver --network=nos3_core \
    --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
    -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" $DBOX \
    ./nos3-single-simulator -f nos3-simulator.xml time

SATNUM=1
for (( i=1; i<=$SATNUM; i++ )); do
    SC_NUM="sc_$i"
    SC_NET="nos3_"$SC_NUM
    CFG_FILE="-f nos3-simulator.xml"

    $DNETWORK rm $SC_NET 2>/dev/null || true
    $DNETWORK create $SC_NET

    echo "$SC_NUM - Create spacecraft network..."
    echo "$SC_NUM - Connect GSW to spacecraft network..."
    $DNETWORK connect $SC_NET cosmos_openc3-operator_1 --alias cosmos --alias active-gs

    echo "$SC_NUM - 42..."
    rm -rf $USER_NOS3_DIR/42/NOS3InOut
    cp -r $BASE_DIR/cfg/build/InOut $USER_NOS3_DIR/42/NOS3InOut
    xhost +local:*
    $DCALL run -d --name ${SC_NUM}_fortytwo -h fortytwo --network=$SC_NET \
        --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
        -e DISPLAY=$DISPLAY -v "$USER_NOS3_DIR:$USER_NOS3_DIR" \
        -v /tmp/.X11-unix:/tmp/.X11-unix:ro -w "$USER_NOS3_DIR/42" $DBOX $USER_NOS3_DIR/42/42 NOS3InOut

    echo "$SC_NUM - Flight Software..."
    $DCALL run -dit --name ${SC_NUM}_nos_fsw -h nos_fsw --network=$SC_NET \
        -v "$BASE_DIR:$BASE_DIR" -v "$FSW_DIR:$FSW_DIR" -v "$SCRIPT_DIR:$SCRIPT_DIR" \
        -e USER=$(whoami) -e LD_LIBRARY_PATH=$FSW_DIR:/usr/lib:/usr/local/lib \
        -w $FSW_DIR --sysctl fs.mqueue.msg_max=10000 --ulimit rtprio=99 --cap-add=sys_nice \
        $DBOX bash -c "exec ./core-cpu1 -R PO"

    echo "$SC_NUM - CryptoLib..."
    $DCALL run -d --name ${SC_NUM}_cryptolib --network=$SC_NET \
        --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
        --network-alias=cryptolib \
        -v "$BASE_DIR:$BASE_DIR" -w "$BASE_DIR/gsw/build" $DBOX ./support/standalone

    echo "$SC_NUM - Simulators..."
    echo "$SC_NUM - NOS Engine Server..."
    $DCALL run -dit --name ${SC_NUM}_nos_engine_server -h nos_engine_server --network=$SC_NET \
        --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
        -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" $DBOX \
        /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json

    $DCALL run -dit --name ${SC_NUM}_truth42sim --network=$SC_NET \
        -h truth42sim --log-driver json-file --log-opt max-size=5m --log-opt max-file=3 \
        -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" $DBOX \
        ./nos3-single-simulator $CFG_FILE truth42sim

    for sim in \
        camsim generic_css_sim generic_eps_sim generic_fss_sim \
        gps generic_imu_sim generic_mag_sim \
        generic-reactionwheel-sim0 generic-reactionwheel-sim1 \
        generic-reactionwheel-sim2 generic_radio_sim sample_sim \
        generic_star_tracker_sim generic_thruster_sim generic_torquer_sim; do

        if [[ "$sim" == "generic_radio_sim" ]]; then
            $DCALL run -d --name ${SC_NUM}_${sim} --network=$SC_NET \
                -h radio_sim --network-alias=radio_sim \
                -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" $DBOX \
                ./nos3-single-simulator $CFG_FILE $sim
        else
            $DCALL run -d --name ${SC_NUM}_${sim} --network=$SC_NET \
                -v "$SIM_DIR:$SIM_DIR" -w "$SIM_BIN" $DBOX \
                ./nos3-single-simulator $CFG_FILE $sim
        fi
    done

    $DNETWORK connect --alias nos_time_driver $SC_NET nos_time_driver

    echo "Connecting ground simulators to spacecraft network..."
    $DNETWORK connect $SC_NET nos_terminal
    $DNETWORK connect $SC_NET nos_udp_terminal
    $DNETWORK connect $SC_NET nos_sim_bridge
done

echo "Docker headless launch script completed!"
