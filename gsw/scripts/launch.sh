#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
SIM_BIN=$BASE_DIR/sims/build/bin
SIMS=$(cd $SIM_BIN; ls nos3*simulator)
#SIM_TABS=$(for i in $SIMS; do printf " --tab --title=$i --command=\"$SIM_BIN/$i\" "; done)

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#echo "FSW directory    = " $FSW_BIN
#echo "Sim directory    = " $SIM_BIN
#echo "Sim list         = " $SIMS
#echo "Sim tabs         = " $SIM_TABS
#exit


echo "42..."
cd /opt/nos3/42/
rm -rf NOS3InOut
cp -r $BASE_DIR/sims/cfg/InOut /opt/nos3/42/NOS3InOut
gnome-terminal \
--tab --title="42 Dynamic Simulator" --command="/opt/nos3/42/42 NOS3InOut"

echo "Simulators..."
cd $SIM_BIN
gnome-terminal \
--tab --title="NOS Engine Standalone Server" --command="/usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json" \
--tab --title="NOS Time Driver" --command=$SIM_BIN/nos-time-driver \
--tab --title="Simulator Terminal" --command=$SIM_BIN/nos3-simulator-terminal \
--tab -t 'Sample Simulator' --command="$SIM_BIN/nos3-sample-simulator" \
--tab -t 'CAM Simulator' --command="$SIM_BIN/nos3-cam-simulator"  \
--tab -t 'GPS Simulator' --command="$SIM_BIN/nos3-gps-simulator" \
--tab -t 'RW Simulator' --command="$SIM_BIN/nos3-generic-reactionwheel-simulator" \
--tab --title="truth42sim" --command="$SIM_BIN/nos3-single-simulator truth42sim"
#$SIM_TABS > /dev/null

echo "COSMOS Ground Station..."
cd $BASE_DIR/gsw/cosmos
export MISSION_NAME=$(echo "NOS3")
export PROCESSOR_ENDIANNESS=$(echo "LITTLE_ENDIAN")
ruby Launcher -c nos3_launcher.txt -- system nos3_system.txt &

sleep 5

echo "Flight Software..."
cd $FSW_BIN
gnome-terminal --title="NOS3 Flight Software" --command="$FSW_BIN/core-cpu1 -R PO" &
