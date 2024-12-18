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
echo ""

##
## HOW TO CHECKOUT
##
# Rename for your simulator under test in this file below to allow checkout, uncomment if already exists
# Change configuration for your selected component (if using 42 for data)
#   Modify ./cfg/nos3-mission.xml to use `<sc-1-cfg>sc-minimal-config.xml</sc-1-cfg>` 
#   Modify ./cfg/sc_minimal-config.xml to enable your specific component to test
# Manually build checkout: (sample shown as reference)
#   make debug
#   cd ./components/sample/fsw/standalone
#   mkdir build
#   cd build
#   cmake .. -DTGTNAME=cpu1
#   make
#   exit
# Run `make clean; make; make checkout`
#   Double check everything running as expected
#   Run tests as needed
# Run `make stop` and repeat until feature complete
echo "Checkout..."

##
## Arducam
##
#gnome-terminal --tab --title="Arducam Sim"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_cam_sim"   --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE camsim
#gnome-terminal --title="Arducam Checkout"   -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_arducam_checkout"   --network=$SC_NETNAME -w $BASE_DIR $DBOX ./components/arducam/fsw/standalone/build/arducam_checkout

##
## Fine Sun Sensor (FSS)
##
rm -rf $USER_NOS3_DIR/42/NOS3InOut
cp -r $BASE_DIR/cfg/build/InOut $USER_NOS3_DIR/42/NOS3InOut
xhost +local:*
gnome-terminal --tab --title=$SC_NUM" - 42" -- $DFLAGS -e DISPLAY=$DISPLAY -v $USER_NOS3_DIR:$USER_NOS3_DIR -v /tmp/.X11-unix:/tmp/.X11-unix:ro --name $SC_NUM"_fortytwo" -h fortytwo --network=$SC_NETNAME -w $USER_NOS3_DIR/42 -t $DBOX $USER_NOS3_DIR/42/42 NOS3InOut
echo ""
gnome-terminal --tab --title=$SC_NUM" - FSS Sim" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_fss_sim" --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_fss_sim
gnome-terminal --title="FSS Checkout" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_fss_checkout" --network=$SC_NETNAME -w $BASE_DIR $DBOX ./components/generic_fss/fsw/standalone/build/generic_fss_checkout

##
## Sample
##
#gnome-terminal --tab --title="Sample Sim" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_sample_sim" --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE sample_sim
#gnome-terminal --title="Sample Checkout" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_sample_checkout" --network=$SC_NETNAME -w $BASE_DIR $DBOX ./components/sample/fsw/standalone/build/sample_checkout

# sleep 1
# urlIP=$(docker container inspect sc_1_sample_checkout | grep -i IPAddress | grep -oE "\b([0-9]{1,3}\.){3}[0-9]{1,3}\b")
# sleep 10
# firefox ${urlIP}:5000

echo ""
