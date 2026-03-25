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
touch $FSW_DIR/data/dummy.txt
echo "1234567890" > $FSW_DIR/data/dummy.txt
truncate -s 1M $FSW_DIR/data/dummy.txt
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
    nos3-core
echo ""

echo "Launch GSW..."
echo ""
source $BASE_DIR/cfg/build/gsw_launch.sh

echo "Create NOS interfaces..."
export GND_CFG_FILE="-f nos3-simulator.xml"
gnome-terminal --tab --title="NOS Terminal"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos-terminal"        --network nos3-core -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE stdio-terminal
gnome-terminal --tab --title="NOS UDP Terminal"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos-udp-terminal"    --network nos3-core -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE udp-terminal
gnome-terminal --tab --title="NOS CmdBus Bridge"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos-sim-bridge"     --network nos3-core -w $SIM_BIN $DBOX ./nos3-sim-cmdbus-bridge $GND_CFG_FILE

echo ""

echo "sc01 - Create spacecraft network..."
$DNETWORK create "nos3-sc01" --subnet=192.168.1.0/24
echo "sc02 - Create spacecraft network..."
$DNETWORK create "nos3-sc02" --subnet=192.168.2.0/24
echo "sc03 - Create spacecraft network..."
$DNETWORK create "nos3-sc03" --subnet=192.168.3.0/24
echo "42..."
rm -rf $USER_NOS3_DIR/42/NOS3InOut
cp -r $BASE_DIR/cfg/build/InOut $USER_NOS3_DIR/42/NOS3InOut
xhost +local:*
gnome-terminal --tab --title="42" -- $DFLAGS -e DISPLAY=$DISPLAY -v $USER_NOS3_DIR:$USER_NOS3_DIR -v /tmp/.X11-unix:/tmp/.X11-unix:ro --name "fortytwo" -h fortytwo --network nos3-core --network nos3-sc01 --network nos3-sc02 --network nos3-sc03 -w $USER_NOS3_DIR/42 -t $DBOX $USER_NOS3_DIR/42/42 NOS3InOut
echo ""

# Note only currently working with a single spacecraft
export SATNUM=3

#
# Spacecraft Loop
#
# for (( i=1; i<=$SATNUM; i++ ))
for (( i=$SATNUM; i>0; i-- ))
do
    export SC_NUM="sc0"$i
    export SC_NETNAME="nos3-"$SC_NUM
    export SC_CFG_FILE="-f sc-"$i"-nos3-simulator.xml"

    # Debugging
    echo "Spacecraft number        = " $SC_NUM
    echo "Spacecraft network       = " $SC_NETNAME
    echo "Spacecraft configuration = " $SC_CFG_FILE
    
    # echo $SC_NUM " - Create spacecraft network..."
    # $DNETWORK create $SC_NETNAME 2> /dev/null
    # echo ""

    echo $SC_NUM " - Connect GSW " "${GSW:-cosmos-openc3-operator-1}" " to spacecraft network..."
    $DNETWORK connect  $SC_NETNAME "${GSW:-cosmos-openc3-operator-1}" --alias cosmos --alias active-gs --ip 192.168.$i.100
    echo ""

    echo $SC_NUM " - OnAIR..."
    gnome-terminal --tab --title=$SC_NUM" - OnAIR" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"-onair" --network $SC_NETNAME -w $FSW_DIR -t $DBOX $SCRIPT_DIR/fsw/onair_launch.sh
    echo ""

    echo $SC_NUM " - Flight Software..."
    cd $FSW_DIR
    # Debugging
    # Replace `--tab` with `--window-with-profile=KeepOpen` once you've created this gnome-terminal profile manually
    gnome-terminal --title=$SC_NUM" - NOS3 Flight Software" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"-nos-fsw" -h nos-fsw --network $SC_NETNAME -w $FSW_DIR --sysctl fs.mqueue.msg_max=10000 --ulimit rtprio=99 --cap-add=sys_nice $DBOX $SCRIPT_DIR/fsw/fsw_respawn.sh &
    #gnome-terminal --window-with-profile=KeepOpen --title=$SC_NUM" - NOS3 Flight Software" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"-nos-fsw" -h nos-fsw --network $SC_NETNAME -w $FSW_DIR --sysctl fs.mqueue.msg_max=10000 --ulimit rtprio=99 --cap-add=sys_nice $DBOX $FSW_DIR/core-cpu1 -R PO &
    echo ""

    echo $SC_NUM " - Simulators..."
    cd $SIM_BIN
    gnome-terminal --tab --title=$SC_NUM" - NOS Engine Server" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-nos-engine-server"  -h nos-engine-server --network $SC_NETNAME -w $SIM_BIN $DBOX /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
    gnome-terminal --tab --title=$SC_NUM" - 42 Truth Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-truth42sim"          -h truth42sim --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE  truth42sim
    
    $DNETWORK connect $SC_NETNAME nos-terminal
    $DNETWORK connect $SC_NETNAME nos-udp-terminal
    $DNETWORK connect $SC_NETNAME nos-sim-bridge

    # Component simulators
    gnome-terminal --tab --title=$SC_NUM" - CAM Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-cam-sim"      --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE camsim
    gnome-terminal --tab --title=$SC_NUM" - CSS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-css-sim"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-css-sim
    gnome-terminal --tab --title=$SC_NUM" - EPS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-eps-sim"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-eps-sim
    gnome-terminal --tab --title=$SC_NUM" - FSS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-fss-sim"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-fss-sim
    gnome-terminal --tab --title=$SC_NUM" - GPS Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-gps-sim"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE gps
    gnome-terminal --tab --title=$SC_NUM" - IMU Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-imu-sim"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-imu-sim
    gnome-terminal --tab --title=$SC_NUM" - MAG Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-mag-sim"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-mag-sim
    gnome-terminal --tab --title=$SC_NUM" - RW 0 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-rw-sim0"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim0
    gnome-terminal --tab --title=$SC_NUM" - RW 1 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-rw-sim1"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim1
    gnome-terminal --tab --title=$SC_NUM" - RW 2 Sim"     -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-rw-sim2"      -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim2
    # docker create --rm -it -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro -u 1001:999 -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-radio-sim"    --network $SC_NETNAME --network-alias radio-sim -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-radio-sim
    gnome-terminal --tab --title=$SC_NUM" - Radio Sim"    -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-radio-sim"    --network $SC_NETNAME --network-alias radio-sim -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-radio-sim
    
    if [[ $i -lt $SATNUM ]]; then
        # SC_NUM="sc0"$i
        # j=$(($i+1))
        # SC_PREVNET="nos3-sc0"$j
        # SC_NETNAME="nos3-"$SC_NUM
        # RADNAME=$SC_NUM"-radio-sim"
        # sleep 1
        # docker network connect --alias "next-radio" $SC_PREVNET $RADNAME
        j=$(($i+1))
        NEXT_RADNAME="sc0"$j"-radio-sim"
        echo "Connecting $NEXT_RADNAME to network $SC_NETNAME as 'next-radio'"
        docker network connect --alias "next-radio" $SC_NETNAME $NEXT_RADNAME
    fi

#    gnome-terminal --tab --title=$SC_NUM" - Radio Sim"    -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-radio-sim"    --network nos3-core -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-radio-sim
    gnome-terminal --tab --title=$SC_NUM" - Sample Sim"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-sample-sim"   -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE sample-sim
    gnome-terminal --tab --title=$SC_NUM" - StarTrk Sim"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-startrk-sim"  -v /dev/shm:/dev/shm --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-star-tracker-sim
    gnome-terminal --tab --title=$SC_NUM" - Thruster Sim" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-thruster-sim" --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-thruster-sim
    gnome-terminal --tab --title=$SC_NUM" - Torquer Sim"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-torquer-sim"  -h trq-sim --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE generic-torquer-sim
    
    # gnome-terminal --tab --title=$SC_NUM" - Blackboard Sim"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"-blackboard-sim" -v /dev/shm:/dev/shm -h blackboard-sim --network $SC_NETNAME -w $SIM_BIN $DBOX ./nos3-single-simulator $SC_CFG_FILE blackboard-sim
    # cp cfg/InOut/Inp_IPC.shmem.txt cfg/InOut/Inp_IPC.txt
    # cp cfg/sims/nos3-simulator.shmem.xml cfg/sims/nos3-simulator.xml

    echo ""

    echo $SC_NUM " - CryptoLib..."
    gnome-terminal --tab --title=$SC_NUM" - CryptoLib GSW" -- $DFLAGS -v $BASE_DIR:$BASE_DIR --name $SC_NUM"-cryptolib-gsw"  -h cryptolib --network $SC_NETNAME --network-alias=cryptolib -w $BASE_DIR/gsw/build $DBOX ./support/standalone
    echo ""

    sleep 3
done
# docker network connect --alias "next-radio" "nos3-sc01" sc03-radio-sim
echo "Closing the ring: connecting sc01-radio-sim to nos3-sc03 as 'next-radio'"
docker network connect --alias "next-radio" "nos3-sc03" sc01-radio-sim
# sleep 8
# docker network connect --alias "next-radio" "nos3-sc0"$SATNUM sc01-radio-sim
# #docker network connect --alias "radio-sim" "nos3-sc01" sc01-radio-sim
# for (( i=2; i<=$SATNUM; i++))
# do
#     export SC_NUM="sc0"$i
#     export j=$((i-1))
#     export SC_PREVNET="nos3-sc0"$j
#     export SC_NETNAME="nos3-"$SC_NUM
#     export RADNAME=$SC_NUM"-radio-sim"
# #    docker network connect --alias "radio-sim" $SC_NETNAME $RADNAME
#     docker network connect --alias "next-radio" $SC_PREVNET $RADNAME
# done

# for (( i=1; i<=$SATNUM; i++ ))
# do
#     export SC_NUM="sc0"$i
#     gnome-terminal --tab --title=$SC_NUM" - Radio Sim" -- docker start -i $SC_NUM"-radio-sim"
# done

echo "NOS Time Driver..."
sleep 6
echo EDITED11: gnome-terminal --tab --title="NOS Time Driver"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name nos-time-driver --network nos3-core --network nos3-sc01 --network nos3-sc02 --network nos3-sc03 -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE time
gnome-terminal --tab --title="NOS Time Driver"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name nos-time-driver --network nos3-core --network nos3-sc01 --network nos3-sc02 --network nos3-sc03 -w $SIM_BIN $DBOX ./nos3-single-simulator $GND_CFG_FILE time
sleep 1
# for (( i=1; i<=$SATNUM; i++ ))
# do
#     export SC_NUM="sc0"$i
#     export SC_NETNAME="nos3-"$SC_NUM
#     export TIMENAME=$SC_NUM"-nos-time-driver"
#     $DNETWORK connect --alias nos-time-driver $SC_NETNAME nos-time-driver
# done


echo ""

echo "Docker launch script completed!"
