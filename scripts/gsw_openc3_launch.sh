#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#exit

#echo "Make /tmp folders..."
#mkdir /tmp/data 2> /dev/null
#mkdir /tmp/data/hk 2> /dev/null
#mkdir /tmp/uplink 2> /dev/null

echo "OpenC3 launch..."
#cd $BASE_DIR/gsw/cosmos
#export MISSION_NAME=$(echo "NOS3")
#export PROCESSOR_ENDIANNESS=$(echo "LITTLE_ENDIAN")
#ruby Launcher -c nos3_launcher.txt --system nos3_system.txt &
firefox localhost:2900 &
