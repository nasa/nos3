#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

# Note the first argument passed is expected to be the BASE_DIR of the NOS3 repository
source $1/scripts/env.sh

# Check that local NOS3 directory exists
if [ ! -d $USER_NOS3_DIR ]; then
    echo ""
    echo "    Need to run make prep first!"
    echo ""
    exit 1
fi

# Check that configure build directory exists
if [ ! -d $USER_NOS3_BUILD_DIR/cfg ]; then
    echo ""
    echo "    Need to run make config first!"
    echo ""
    exit 1
fi

echo "Make data folders..."
# FSW Side
mkdir $FSW_DIR/data 2> /dev/null
mkdir $FSW_DIR/data/cam 2> /dev/null
mkdir $FSW_DIR/data/evs 2> /dev/null
mkdir $FSW_DIR/data/hk 2> /dev/null
mkdir $FSW_DIR/data/inst 2> /dev/null
# GSW Side
mkdir /tmp/nos3 2> /dev/null
mkdir /tmp/nos3/data 2> /dev/null
mkdir /tmp/nos3/data/cam 2> /dev/null
mkdir /tmp/nos3/data/evs 2> /dev/null
mkdir /tmp/nos3/data/hk 2> /dev/null
mkdir /tmp/nos3/data/inst 2> /dev/null
mkdir /tmp/nos3/uplink 2> /dev/null
cp $BASE_DIR/fsw/build/exe/cpu1/cf/cfe_es_startup.scr /tmp/nos3/uplink/tmp0.so 2> /dev/null
cp $BASE_DIR/fsw/build/exe/cpu1/cf/sample.so /tmp/nos3/uplink/tmp1.so 2> /dev/null

echo "Create ground networks..."
$DNETWORK create \
    --driver=bridge \
    --subnet=192.168.41.0/24 \
    --gateway=192.168.41.1 \
    nos3_core
echo ""

echo "Launch GSW..."
echo ""
source $USER_NOS3_BUILD_DIR/cfg/gsw_launch.sh $BASE_DIR
echo ""

echo "Create NOS interfaces..."
export GND_CFG_FILE="-f nos3-simulator.xml"
gnome-terminal --tab --title="NOS Terminal"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos_terminal"        --network=nos3_core -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE stdio-terminal
gnome-terminal --tab --title="NOS UDP Terminal"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos_udp_terminal"    --network=nos3_core -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE udp-terminal
echo ""

# Note only currently working with a single spacecraft
export SATNUM=1

#
# Spacecraft Loop
#
for (( i=1; i<=$SATNUM; i++ ))
do
    export SC_NUM="sc_"$i
    export SC_NETNAME="nos3_"$SC_NUM
    export SC_CFG_FILE="-f nos3-simulator.xml" #"-f sc_"$i"_nos3_simulator.xml"

    # Debugging
    #echo "Spacecraft number        = " $SC_NUM
    #echo "Spacecraft network       = " $SC_NETNAME
    #echo "Spacecraft configuration = " $SC_CFG_FILE
    
    echo $SC_NUM " - Create spacecraft network..."
    $DNETWORK create $SC_NETNAME 2> /dev/null
    echo ""

    # echo $SC_NUM " - Connect COSMOS to spacecraft network..."
    # $DNETWORK connect $SC_NETNAME cosmos_openc3-operator_1 --alias cosmos
    # echo ""

    echo $SC_NUM " - 42..."
    rm -rf $USER_NOS3_DIR/42/NOS3InOut
    cp -r $USER_NOS3_BUILD_DIR/cfg/InOut $USER_NOS3_DIR/42/NOS3InOut
    xhost +local:*
    gnome-terminal --tab --title=$SC_NUM" - 42" -- $DFLAGS -e DISPLAY=$DISPLAY -v $USER_NOS3_DIR:$USER_NOS3_DIR -v /tmp/.X11-unix:/tmp/.X11-unix:ro --name $SC_NUM"_fortytwo" -h fortytwo --network=$SC_NETNAME -w $USER_NOS3_DIR/42 -t $DBOX $USER_NOS3_DIR/42/42 NOS3InOut
    echo ""

    echo $SC_NUM " - Flight Software..."
    cd $FSW_DIR
    gnome-terminal --window-with-profile=KeepOpen --title="FPrime" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_fprime" --network=$SC_NETNAME -h nos_fsw -w $BASE_DIR $DBOX $SCRIPT_DIR/fsw/start_fprime.sh
    echo ""

    # Debugging
    # Replace `--tab` with `--window-with-profile=KeepOpen` once you've created this gnome-terminal profile manually

    echo $SC_NUM " - CryptoLib..."
    gnome-terminal --tab --title=$SC_NUM" - CryptoLib" -- $DFLAGS -v $USER_NOS3_BUILD_DIR:$USER_NOS3_BUILD_DIR --name $SC_NUM"_cryptolib"  --network=$SC_NETNAME --network-alias=cryptolib -w $USER_NOS3_BUILD_DIR/gsw $DBOX ./support/standalone
    echo ""

    echo $SC_NUM " - Simulators..."
    cd $SIM_BIN
    gnome-terminal --tab --title=$SC_NUM" - NOS Engine Server" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_nos_engine_server"  -h nos_engine_server --network=$SC_NETNAME -w $SIM_BIN $DBOX /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
    gnome-terminal --tab --title=$SC_NUM" - 42 Truth Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_truth42sim"          -h truth42sim --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE  truth42sim
    
    $DNETWORK connect $SC_NETNAME nos_terminal
    $DNETWORK connect $SC_NETNAME nos_udp_terminal

    # Component simulators
    gnome-terminal --tab --title=$SC_NUM" - CAM Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_cam_sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE camsim
    gnome-terminal --tab --title=$SC_NUM" - CSS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_css_sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_css_sim
    gnome-terminal --tab --title=$SC_NUM" - EPS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_eps_sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_eps_sim
    gnome-terminal --tab --title=$SC_NUM" - FSS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_fss_sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_fss_sim
    gnome-terminal --tab --title=$SC_NUM" - GPS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_gps_sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE gps
    gnome-terminal --tab --title=$SC_NUM" - IMU Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_imu_sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_imu_sim
    gnome-terminal --tab --title=$SC_NUM" - MAG Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_mag_sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_mag_sim
    gnome-terminal --tab --title=$SC_NUM" - RW 0 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_rw_sim0"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim0
    gnome-terminal --tab --title=$SC_NUM" - RW 1 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_rw_sim1"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim1
    gnome-terminal --tab --title=$SC_NUM" - RW 2 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_rw_sim2"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim2
    gnome-terminal --tab --title=$SC_NUM" - Radio Sim"    -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_radio_sim"    -h radio_sim --network=$SC_NETNAME --network-alias=radio_sim -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_radio_sim
    gnome-terminal --tab --title=$SC_NUM" - Sample Sim"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_sample_sim"   --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE sample_sim
    gnome-terminal --tab --title=$SC_NUM" - StarTrk Sim"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_startrk_sim"  --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_star_tracker_sim
    gnome-terminal --tab --title=$SC_NUM" - Thruster Sim" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_thruster_sim" --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_thruster_sim
    gnome-terminal --tab --title=$SC_NUM" - Torquer Sim"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_torquer_sim"  --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic_torquer_sim
    echo ""
done

echo "NOS Time Driver..."
sleep 8
gnome-terminal --tab --title="NOS Time Driver"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name nos_time_driver --network=nos3_core -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE time
sleep 1
for (( i=1; i<=$SATNUM; i++ ))
do
    export SC_NUM="sc_"$i
    export SC_NETNAME="nos3_"$SC_NUM
    export TIMENAME=$SC_NUM"_nos_time_driver"
    $DNETWORK connect --alias nos_time_driver $SC_NETNAME nos_time_driver
done
echo ""

    
sleep 1

urlIP=$(docker container inspect sc_1_fprime | grep -i IPAddress | grep -oE "\b([0-9]{1,3}\.){3}[0-9]{1,3}\b")

sleep 10

pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    firefox ${urlIP}:5000 & 
fi

echo "Docker launch script completed!"
