#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
SIM_BIN=$BASE_DIR/sims/build/bin
SIMS=$(cd $SIM_BIN; ls nos3*simulator)


echo "Simulators..."
cd $SIM_BIN
gnome-terminal --tab --title="NOS Engine Server" -- /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
gnome-terminal --tab --title="NOS Time Driver" -- $SIM_BIN/nos3-single-simulator time
gnome-terminal --tab --title="NOS Terminal" -- $SIM_BIN/nos3-single-simulator terminal
