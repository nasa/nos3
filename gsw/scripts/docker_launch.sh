#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://docs.docker.com/engine/install/ubuntu/
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
SIM_DIR=$BASE_DIR/sims/build
SIM_BIN=$SIM_DIR/bin
SIMS=$(cd $SIM_BIN; ls nos3*simulator)

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#echo "FSW directory    = " $FSW_BIN
#echo "Sim directory    = " $SIM_BIN
#echo "Sim list         = " $SIMS
#exit

#echo "Make /tmp folders..."
#mkdir /tmp/data 2> /dev/null
#mkdir /tmp/data/hk 2> /dev/null
#mkdir /tmp/uplink 2> /dev/null

echo "42..."
cd /opt/nos3/42/
rm -rf NOS3InOut
cp -r $BASE_DIR/sims/cfg/InOut /opt/nos3/42/NOS3InOut
xhost +local:*
docker run -d -e DISPLAY=$DISPLAY -v /opt/nos3/42/NOS3InOut:/opt/nos3/42/NOS3InOut -v /tmp/.X11-unix:/tmp/.X11-unix:ro --network=host -w /opt/nos3/42 -t nos3 /opt/nos3/42/42 NOS3InOut

echo "Simulators..."
#cd $SIM_BIN
docker run --rm -d -v $SIM_DIR:$SIM_DIR --network=host -w $SIM_BIN -t nos3 /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
docker run --rm -d -v $SIM_DIR:$SIM_DIR --network=host -w $SIM_BIN nos3 $SIM_BIN/nos-time-driver
docker run --rm -d -v $SIM_DIR:$SIM_DIR --network=host -w $SIM_BIN nos3 $SIM_BIN/nos3-simulator-terminal
docker run --rm -d -v $SIM_DIR:$SIM_DIR --network=host -w $SIM_BIN nos3 $SIM_BIN/nos3-cam-simulator
docker run --rm -d -v $SIM_DIR:$SIM_DIR --network=host -w $SIM_BIN nos3 $SIM_BIN/nos3-generic-reactionwheel-simulator
docker run --rm -d -v $SIM_DIR:$SIM_DIR --network=host -w $SIM_BIN nos3 $SIM_BIN/nos3-gps-simulator
docker run --rm -d -v $SIM_DIR:$SIM_DIR --network=host -w $SIM_BIN nos3 $SIM_BIN/nos3-sample-simulator
docker run --rm -d -v $SIM_DIR:$SIM_DIR --network=host -w $SIM_BIN nos3 $SIM_BIN/nos3-single-simulator truth42sim

echo "COSMOS Ground Station..."
cd $BASE_DIR/gsw/cosmos
export MISSION_NAME=$(echo "NOS3")
export PROCESSOR_ENDIANNESS=$(echo "LITTLE_ENDIAN")
docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:ro -e QT_X11_NO_MITSHM=1 \
    -v /home/nos3/Desktop/github-nos3/gsw/cosmos:/cosmos/cosmos \
    -v /home/nos3/Desktop/github-nos3/components/:/COMPONENTS -w /cosmos/cosmos -d --network=host \
    ballaerospace/cosmos /bin/bash -c 'ruby Launcher -c nos3_launcher.txt --system nos3_system.txt && true' # true is necessary to avoid setpgrp error

sleep 5

echo "Flight Software..."
cd $FSW_BIN
gnome-terminal --title="NOS3 Flight Software" -- docker run --rm -it -v $FSW_BIN:$FSW_BIN --network=host -w $FSW_BIN --sysctl fs.mqueue.msg_max=500 nos3 $FSW_BIN/core-cpu1 -R PO &
