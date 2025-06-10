#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="cosmos-openc3-operator-1"

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#exit

#echo "Make /tmp folders..."
#mkdir /tmp/data 2> /dev/null
#mkdir /tmp/data/hk 2> /dev/null
#mkdir /tmp/uplink 2> /dev/null

echo "COSMOS launch..."
gnome-terminal --tab --title="Cosmos" -- $DFLAGS -v $BASE_DIR:$BASE_DIR -v /tmp/nos3:/tmp/nos3 -v /tmp/.X11-unix:/tmp/.X11-unix:ro -e DISPLAY=$DISPLAY -e QT_X11_NO_MITSHM=1 -e PROCESSOR_ENDIANNESS="LITTLE_ENDIAN" -w $GSW_DIR --name cosmos-openc3-operator-1 --network=nos3-core ballaerospace/cosmos:4.5.0
