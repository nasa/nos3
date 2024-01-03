#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

echo "Make data folders..."
# FSW Side
mkdir $FSW_BIN/data 2> /dev/null
mkdir $FSW_BIN/data/cam 2> /dev/null
mkdir $FSW_BIN/data/evs 2> /dev/null
mkdir $FSW_BIN/data/hk 2> /dev/null
mkdir $FSW_BIN/data/inst 2> /dev/null
# GSW Side
mkdir /tmp/data 2> /dev/null
mkdir /tmp/data/cam 2> /dev/null
mkdir /tmp/data/evs 2> /dev/null
mkdir /tmp/data/hk 2> /dev/null
mkdir /tmp/data/inst 2> /dev/null
mkdir /tmp/uplink 2> /dev/null
cp $BASE_DIR/fsw/build/exe/cpu1/cf/cfe_es_startup.scr /tmp/uplink/tmp0.so 2> /dev/null
cp $BASE_DIR/fsw/build/exe/cpu1/cf/sample.so /tmp/uplink/tmp1.so 2> /dev/null

echo "42..."
cd /opt/nos3/42/
rm -rf NOS3InOut
cp -r $BASE_DIR/cfg/InOut /opt/nos3/42/NOS3InOut
gnome-terminal --tab --title="42 Dynamic Simulator" -- /opt/nos3/42/42 NOS3InOut

echo "Simulators..."
cd $SIM_BIN
gnome-terminal --tab --title="NOS Engine Server"  -- /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
gnome-terminal --tab --title="NOS STDIO Terminal" -- $SIM_BIN/nos3-single-simulator stdio-terminal
gnome-terminal --tab --title="NOS UDP Terminal"   -- $SIM_BIN/nos3-single-simulator udp-terminal
gnome-terminal --tab --title="42 Truth Sim"       -- $SIM_BIN/nos3-single-simulator truth42sim
gnome-terminal --tab --title='CAM Sim'            -- $SIM_BIN/nos3-single-simulator camsim
gnome-terminal --tab --title='CSS Sim'            -- $SIM_BIN/nos3-single-simulator generic_css_sim
gnome-terminal --tab --title='EPS Sim'            -- $SIM_BIN/nos3-single-simulator generic_eps_sim
gnome-terminal --tab --title="FSS Sim"            -- $SIM_BIN/nos3-single-simulator generic_fss_sim
gnome-terminal --tab --title='GPS Sim'            -- $SIM_BIN/nos3-single-simulator gps
gnome-terminal --tab --title='IMU Sim'            -- $SIM_BIN/nos3-single-simulator generic_imu_sim
gnome-terminal --tab --title='MAG Sim'            -- $SIM_BIN/nos3-single-simulator generic_mag_sim
gnome-terminal --tab --title='Radio Sim'          -- $SIM_BIN/nos3-single-simulator generic_radio_sim
gnome-terminal --tab --title='RW 0 Sim'           -- $SIM_BIN/nos3-single-simulator generic-reactionwheel-sim0
gnome-terminal --tab --title='RW 1 Sim'           -- $SIM_BIN/nos3-single-simulator generic-reactionwheel-sim1
gnome-terminal --tab --title='RW 2 Sim'           -- $SIM_BIN/nos3-single-simulator generic-reactionwheel-sim2
gnome-terminal --tab --title='Sample Sim'         -- $SIM_BIN/nos3-single-simulator sample_sim
gnome-terminal --tab --title='Torquer Sim'        -- $SIM_BIN/nos3-single-simulator generic_torquer_sim
gnome-terminal --tab --title="NOS Time Driver"    -- $SIM_BIN/nos3-single-simulator time

#echo "CryptoLib..."
#mkdir $BASE_DIR/components/cryptolib/build/
#cd $BASE_DIR/components/cryptolib/build/
#export CFLAGS="-m32"
#cmake .. -DSUPPORT=1 && make -j2
#gnome-terminal --tab --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/support/standalone
## Note: Can keep open if desired after a new gnome-profile is manually created
##cmake .. -DDEBUG=1 && make -j2
##gnome-terminal --window-with-profile=KeepOpen --title="CryptoLib" -- $BASE_DIR/components/cryptolib/build/bin/standalone

echo "COSMOS Ground Station..."
#cd $BASE_DIR/gsw/cosmos
#export MISSION_NAME=$(echo "NOS3")
#export PROCESSOR_ENDIANNESS=$(echo "LITTLE_ENDIAN")
#ruby Launcher -c nos3_launcher.txt --system nos3_system.txt &
pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    firefox localhost:2900 &
fi

sleep 5

echo "Flight Software..."
cd $FSW_BIN
$SCRIPT_DIR/fsw_respawn.sh $FSW_BIN &
# Note: Can keep open if desired after a new gnome-profile is manually created
#gnome-terminal --window-with-profile=KeepOpen --title="NOS3 Flight Software" -- $FSW_BIN/core-cpu1 -R PO &
