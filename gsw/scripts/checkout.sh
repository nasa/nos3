#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

export SC_NUM="sc_1"
export SC_NETNAME="nos3_"$SC_NUM
export SC_CFG_FILE="-f nos3-simulator.xml" #"-f sc_"$i"_nos3_simulator.xml"

echo "Create spacecraft network..."
$DNETWORK create $SC_NETNAME 2> /dev/null
echo ""

# Debugging
# Replace `--tab` with `--window-with-profile=KeepOpen` once you've created this gnome-terminal profile manually

#echo "42..."
#cd /opt/nos3/42/
#rm -rf NOS3InOut
#cp -r $BASE_DIR/sims/cfg/InOut /opt/nos3/42/NOS3InOut
#xhost +local:*
#gnome-terminal --window-with-profile=KeepOpen --title="42" -- $DFLAGS -e DISPLAY=$DISPLAY -v /opt/nos3/42/NOS3InOut:/opt/nos3/42/NOS3InOut -v /tmp/.X11-unix:/tmp/.X11-unix:ro --name $SC_NUM"_fortytwo" -h fortytwo --network=$SC_NETNAME -w /opt/nos3/42 -t ivvitc/nos3 /opt/nos3/42/42 NOS3InOut
#echo ""

echo "NOS Core..."
gnome-terminal --tab --title="NOS Engine Server" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_nos_engine_server"  -h nos_engine_server --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
gnome-terminal --tab --title="NOS Time Driver"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name nos_time_driver --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE time
gnome-terminal --tab --title="NOS Terminal"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos_terminal"        --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE stdio-terminal

echo "Checkout..."
# Rename for your simulator under test to allow checkout
gnome-terminal --tab --title="Sample Sim"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_sample_sim"   --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE sample_sim

# Example manual build for sample checkout:
#   cd ./components/sample/support
#   mkdir build
#   cd build
#   cmake .. -DTGTNAME=cpu1
#   make

# Rename for your checkout under test to allow checkout
gnome-terminal --title="Sample Sim"   -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_sample_checkout"   --network=$SC_NETNAME -w $BASE_DIR ivvitc/nos3 ./components/sample/support/build/sample_checkout

echo ""
