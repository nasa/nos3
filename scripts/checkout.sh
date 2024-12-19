#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[1]}" )/scripts" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

export SC_NUM="sc_1"
export SC_NETNAME="nos3_"$SC_NUM
export SC_CFG_FILE="-f nos3-simulator.xml" #"-f sc_"$i"_nos3_simulator.xml"

echo "Create spacecraft network..."
$DNETWORK create $SC_NETNAME 2> /dev/null
echo ""

# Debugging
# Replace `--tab` with `--window-with-profile=KeepOpen` once you've created this gnome-terminal profile manually

echo "NOS Core..."
gnome-terminal --tab --title="NOS Engine Server" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_nos_engine_server"  -h nos_engine_server --network=$SC_NETNAME -w $SIM_BIN $DBOX /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
gnome-terminal --tab --title="NOS Time Driver"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name nos_time_driver --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE time
gnome-terminal --tab --title="NOS Terminal"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos_terminal"        --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE stdio-terminal

echo " Checkout..."
# Rename for your simulator under test to allow checkout, uncomment if already exists

# Example manual build for sample checkout:
#   make debug
#   cd ./components/sample/fsw/standalone
#   mkdir build
#   cd build
#   cmake .. -DTGTNAME=cpu1
#   make
#   exit

##
## Arducam
##
#gnome-terminal --tab --title="Arducam Sim"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_cam_sim"   --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE camsim
#gnome-terminal --title="Arducam Checkout"   -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_arducam_checkout"   --network=$SC_NETNAME -w $BASE_DIR $DBOX ./components/arducam/fsw/standalone/build/arducam_checkout

##
## Sample
##
#gnome-terminal --tab --title="Sample Sim"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_sample_sim"   --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE sample_sim
#gnome-terminal --title="Sample Checkout"   -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_sample_checkout"   --network=$SC_NETNAME -w $BASE_DIR $DBOX ./components/sample/fsw/standalone/build/sample_checkout

#novatel_oem615
gnome-terminal --tab --title="gps"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_gps"   --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE gps
gnome-terminal --title="novatel_oem615_checkout"   -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_novatel_oem615_checkout"   --network=$SC_NETNAME -w $BASE_DIR $DBOX ./components/novatel_oem615/fsw/standalone/build/novatel_oem615_checkout


# sleep 1
# urlIP=$(docker container inspect sc_1_sample_checkout | grep -i IPAddress | grep -oE "\b([0-9]{1,3}\.){3}[0-9]{1,3}\b")
# sleep 10
# firefox ${urlIP}:5000

echo ""