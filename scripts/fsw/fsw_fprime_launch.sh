#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

# Note this is copied to ./cfg/build as part of `make config`
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../../scripts/env.sh

# Check that local NOS3 directory exists
if [ ! -d $USER_NOS3_DIR ]; then
    echo ""
    echo "    Need to run make prep first!"
    echo ""
    exit 1
fi

# Check that configure build directory exists
if [ ! -d $BASE_DIR/cfg/build ]; then
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
# touch $FSW_DIR/data/dummy.txt
# echo "1234567890" > $FSW_DIR/data/dummy.txt
# truncate -s 1M $FSW_DIR/data/dummy.txt
# GSW Side
mkdir /tmp/nos3 2> /dev/null
mkdir /tmp/nos3/data 2> /dev/null
mkdir /tmp/nos3/data/cam 2> /dev/null
mkdir /tmp/nos3/data/evs 2> /dev/null
mkdir /tmp/nos3/data/hk 2> /dev/null
mkdir /tmp/nos3/data/inst 2> /dev/null
mkdir /tmp/nos3/uplink 2> /dev/null
# cp $BASE_DIR/fsw/build/exe/cpu1/cf/cfe_es_startup.scr /tmp/nos3/uplink/tmp0.so 2> /dev/null
# cp $BASE_DIR/fsw/build/exe/cpu1/cf/sample.so /tmp/nos3/uplink/tmp1.so 2> /dev/null

echo "Create ground networks..."
$DNETWORK create \
    --driver=bridge \
    --subnet=192.168.41.0/24 \
    --gateway=192.168.41.1 \
    nos3-core
echo ""

echo "Launch GSW..."
echo ""
source $BASE_DIR/cfg/build/gsw_launch.sh


echo "Create NOS interfaces..."
export GND_CFG_FILE="-f nos3-simulator.xml"
gnome-terminal --tab --title="NOS Terminal"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos-terminal"        --network=nos3-core -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE stdio-terminal
gnome-terminal --tab --title="NOS UDP Terminal"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos-udp-terminal"    --network=nos3-core -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE udp-terminal
gnome-terminal --tab --title="NOS CmdBus Bridge"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos-sim-bridge"      --network=nos3-core -w $SIM_BIN $DBOX ./nos3-sim-cmdbus-bridge $GND_CFG_FILE
echo ""

# Note only currently working with a single spacecraft
export SATNUM=1

#
# Spacecraft Loop
#
for (( i=1; i<=$SATNUM; i++ ))
do
    export SC_NUM="sc0"$i
    export SC_NETNAME="nos3-"$SC_NUM
    export SC_CFG_FILE="-f nos3-simulator.xml" #"-f sc0"$i"_nos3_simulator.xml"

    # Debugging
    #echo "Spacecraft number        = " $SC_NUM
    #echo "Spacecraft network       = " $SC_NETNAME
    #echo "Spacecraft configuration = " $SC_CFG_FILE
    
    echo $SC_NUM " - Create spacecraft network..."
    $DNETWORK create $SC_NETNAME 2> /dev/null
    echo ""

    echo $SC_NUM " - Connect COSMOS to spacecraft network..."
    $DNETWORK connect $SC_NETNAME cosmos-openc3-operator-1 --alias cosmos
    echo ""

    echo $SC_NUM " - 42..."
    rm -rf $USER_NOS3_DIR/42/NOS3InOut
    cp -r $BASE_DIR/cfg/build/InOut $USER_NOS3_DIR/42/NOS3InOut
    xhost +local:*
    gnome-terminal --tab --title=$SC_NUM" - 42" -- $DFLAGS -e DISPLAY=$DISPLAY -v $USER_NOS3_DIR:$USER_NOS3_DIR -v /tmp/.X11-unix:/tmp/.X11-unix:ro --name $SC_NUM"-fortytwo" -h fortytwo --network=$SC_NETNAME -w $USER_NOS3_DIR/42 -t $DBOX $USER_NOS3_DIR/42/42 NOS3InOut
    echo ""

    # Debugging
    # Replace `--tab` with `--window-with-profile=KeepOpen` once you've created this gnome-terminal profile manually
    

    echo $SC_NUM " - Simulators..."
    cd $SIM_BIN
    gnome-terminal --tab --title=$SC_NUM" - NOS Engine Server" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-nos-engine-server"  -h nos-engine-server --network=$SC_NETNAME -w $SIM_BIN $DBOX /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
    gnome-terminal --tab --title=$SC_NUM" - 42 Truth Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-truth42sim"          -h truth42sim --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE  truth42sim
    
    $DNETWORK connect $SC_NETNAME nos-terminal
    $DNETWORK connect $SC_NETNAME nos-udp-terminal
    $DNETWORK connect $SC_NETNAME nos-sim-bridge

    # Component simulators
    gnome-terminal --tab --title=$SC_NUM" - CAM Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-cam-sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE camsim
    gnome-terminal --tab --title=$SC_NUM" - CSS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-css-sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-css-sim
    gnome-terminal --tab --title=$SC_NUM" - EPS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-eps-sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-eps-sim
    gnome-terminal --tab --title=$SC_NUM" - FSS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-fss-sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-fss-sim
    gnome-terminal --tab --title=$SC_NUM" - GPS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-gps-sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE gps
    gnome-terminal --tab --title=$SC_NUM" - IMU Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-imu-sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-imu-sim
    gnome-terminal --tab --title=$SC_NUM" - MAG Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-mag-sim"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-mag-sim
    gnome-terminal --tab --title=$SC_NUM" - RW 0 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-rw-sim0"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim0
    gnome-terminal --tab --title=$SC_NUM" - RW 1 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-rw-sim1"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim1
    gnome-terminal --tab --title=$SC_NUM" - RW 2 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-rw-sim2"      --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim2
    gnome-terminal --tab --title=$SC_NUM" - Radio Sim"    -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-radio-sim"    -h radio-sim --network=$SC_NETNAME --network-alias=radio-sim -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-radio-sim
    gnome-terminal --tab --title=$SC_NUM" - Sample Sim"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-sample-sim"   --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE sample-sim
    gnome-terminal --tab --title=$SC_NUM" - StarTrk Sim"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-startrk-sim"  --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-star-tracker-sim
    gnome-terminal --tab --title=$SC_NUM" - Thruster Sim" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-thruster-sim" --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-thruster-sim
    gnome-terminal --tab --title=$SC_NUM" - Torquer Sim"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-torquer-sim"  -h trq-sim --network=$SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-torquer-sim
    echo ""

    echo $SC_NUM " - CryptoLib..."
    gnome-terminal --tab --title=$SC_NUM" - CryptoLib GSW" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"_cryptolib_gsw"  --network=$SC_NETNAME --network-alias=cryptolib -w $BASE_DIR/gsw/build $DBOX ./support/standalone
    echo ""

    echo $SC_NUM " - Flight Software..."
    # cd $FSW_DIR
    gnome-terminal --window-with-profile=KeepOpen --title="FPrime" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"-fprime" --network=$SC_NETNAME -h nos-fsw -w $BASE_DIR --sysctl fs.mqueue.msg_max=10000 --ulimit rtprio=105 --cap-add=sys_nice $DBOX $SCRIPT_DIR/fsw/start_fprime.sh
    echo ""
done

echo "NOS Time Driver..."
sleep 8
gnome-terminal --tab --title="NOS Time Driver"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name nos-time-driver --network=nos3-core -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE time
sleep 1
for (( i=1; i<=$SATNUM; i++ ))
do
    export SC_NUM="sc0"$i
    export SC_NETNAME="nos3-"$SC_NUM
    export TIMENAME=$SC_NUM"-nos-time-driver"
    $DNETWORK connect --alias nos-time-driver $SC_NETNAME nos-time-driver
done
echo ""

    
sleep 1

urlIP=$(docker container inspect sc01-fprime | grep -i IPAddress | grep -oE "\b([0-9]{1,3}\.){3}[0-9]{1,3}\b")

sleep 10

pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    firefox ${urlIP}:5000 & 
fi

echo "Docker launch script completed!"
