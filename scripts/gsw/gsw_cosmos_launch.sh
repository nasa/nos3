#!/bin/bash -i
#
# Convenience script for NOS3 development
#

# Note the first argument passed is expected to be the BASE_DIR of the NOS3 repository
source $1/scripts/env.sh

export GSW="cosmos_openc3-operator_1"

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#exit

#echo "Make /tmp folders..."
#mkdir /tmp/data 2> /dev/null
#mkdir /tmp/data/hk 2> /dev/null
#mkdir /tmp/uplink 2> /dev/null

echo "COSMOS launch..."
gnome-terminal --tab --title="Cosmos" -- $DFLAGS -v $BASE_DIR:$BASE_DIR -v /tmp/nos3:/tmp/nos3 -v /tmp/.X11-unix:/tmp/.X11-unix:ro -e DISPLAY=$DISPLAY -e QT_X11_NO_MITSHM=1 -w $GSW_DIR --name cosmos_openc3-operator_1 --network=nos3_core ballaerospace/cosmos:4.5.0
