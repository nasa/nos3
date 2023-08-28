#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

echo "Simulators..."
cd $SIM_BIN
gnome-terminal --tab --title="NOS Engine Server"  -- /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
gnome-terminal --tab --title="NOS Time Driver"    -- $SIM_BIN/nos3-single-simulator time
gnome-terminal --tab --title="NOS STDIO Terminal" -- $SIM_BIN/nos3-single-simulator stdio-terminal

# Rename for your simulator under test to allow checkout
gnome-terminal --tab --title='Sample Sim'         -- $SIM_BIN/nos3-single-simulator sample_sim

# It is assumed you'll build and launch the checkout manually
