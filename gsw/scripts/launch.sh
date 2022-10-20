#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
SIM_BIN=$BASE_DIR/sims/build/bin
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
gnome-terminal --tab --title="42 Dynamic Simulator" -- /opt/nos3/42/42 NOS3InOut

echo "Simulators..."
cd $SIM_BIN
gnome-terminal --tab --title="NOS Engine Server" -- /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
gnome-terminal --tab --title="NOS Time Driver" -- $SIM_BIN/nos-time-driver
gnome-terminal --tab --title="NOS Terminal" -- $SIM_BIN/nos3-simulator-terminal
gnome-terminal --tab --title='CAM Sim' -- $SIM_BIN/nos3-cam-simulator
gnome-terminal --tab --title='RW Sim' -- $SIM_BIN/nos3-generic-reactionwheel-simulator
gnome-terminal --tab --title='GPS Sim' -- $SIM_BIN/nos3-gps-simulator
gnome-terminal --tab --title='Sample Sim' -- $SIM_BIN/nos3-sample-simulator
gnome-terminal --tab --title="42 Truth Sim" -- $SIM_BIN/nos3-single-simulator truth42sim

echo "COSMOS Ground Station..."
cd $BASE_DIR/gsw/cosmos
export MISSION_NAME=$(echo "NOS3")
export PROCESSOR_ENDIANNESS=$(echo "LITTLE_ENDIAN")
ruby Launcher -c nos3_launcher.txt --system nos3_system.txt &

sleep 5

echo "Flight Software..."
cd $FSW_BIN
gnome-terminal --title="NOS3 Flight Software" -- $FSW_BIN/core-cpu1 -R PO &
