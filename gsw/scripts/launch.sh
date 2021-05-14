#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
SIM_BIN=$BASE_DIR/sims/build/bin
SIMS=$(cd $SIM_BIN; ls nos3*simulator)
#SIM_TABS=$(for i in $SIMS; do printf " --tab --title=$i -e \"$SIM_BIN/$i\" "; done)

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#echo "FSW directory    = " $FSW_BIN
#echo "Sim directory    = " $SIM_BIN
#echo "Sim list         = " $SIMS
#echo "Sim tabs         = " $SIM_TABS
#exit

echo "COSMOS Ground Station..."
cd $BASE_DIR/gsw/cosmos
bundle install # just in case... sometimes the first run of cosmos fails on the nokogiri gem
ruby Launcher -c nos3_launcher.txt -- system nos3_system.txt &

echo "Simulators..."
cd $SIM_BIN
gnome-terminal \
--tab --title="NOS Engine Standalone Server" -e "/usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json" \
--tab --title="NOS Time Driver" -e $SIM_BIN/nos-time-driver \
--tab --title="Simulator Terminal" -e $SIM_BIN/nos3-simulator-terminal \
--tab -t 'Sample Simulator' -e "$SIM_BIN/nos3-sample-simulator" \
--tab -t 'Battery Simulator' -e "$SIM_BIN/nos3-battery-simulator --config $SIM_BIN/batteries.json" \
--tab -t 'CAM Simulator' -e "$SIM_BIN/nos3-cam-simulator"  \
--tab -t 'EPS Simulator' -e "$SIM_BIN/nos3-eps-simulator --iconic true --config $SIM_BIN/eps.json" \
--tab -t 'GPS Simulator' -e "$SIM_BIN/nos3-gps-simulator" \
--tab -t 'RW Simulator' -e "$SIM_BIN/nos3-generic-reactionwheel-simulator" \
--tab --title="truth42sim" -e "$SIM_BIN/nos3-single-simulator truth42sim"
#$SIM_TABS > /dev/null

echo "42..."
cd /opt/nos3/42/
rm -rf NOS3InOut
cp -r $BASE_DIR/sims/cfg/InOut /opt/nos3/42/NOS3InOut
gnome-terminal --title="42 Dynamic Simulator" -e "/opt/nos3/42/42 NOS3InOut" \

echo "Flight Software..."
cd $FSW_BIN
gnome-terminal --title="NOS3 Flight Software" -- $FSW_BIN/core-cpu1 &
