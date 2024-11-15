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

echo "OpenC3 launch..."
pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    firefox localhost:2900 &
fi
