#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://docs.docker.com/engine/install/ubuntu/
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
export FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
export SIM_DIR=$BASE_DIR/sims/build
export SIM_BIN=$SIM_DIR/bin
export SIMS=$(cd $SIM_BIN; ls nos3*simulator)

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

echo "COSMOS Ground Station..."
cd $BASE_DIR/gsw/cosmos
export MISSION_NAME=$(echo "NOS3")
export PROCESSOR_ENDIANNESS=$(echo "LITTLE_ENDIAN")
#docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:ro -e QT_X11_NO_MITSHM=1 \
#    -v /home/nos3/Desktop/github-nos3/gsw/cosmos:/cosmos/cosmos \
#    -v /home/nos3/Desktop/github-nos3/components/:/COMPONENTS -w /cosmos/cosmos -d --network=host \
#    ballaerospace/cosmos /bin/bash -c 'ruby Launcher -c nos3_launcher.txt --system nos3_system.txt && true' # true is necessary to avoid setpgrp error

cd $SCRIPT_DIR
docker compose up -d
