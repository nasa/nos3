#!/bin/bash -i
#
# Convenience script for NOS3 development
# https://docs.docker.com/engine/install/ubuntu/
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_DIR=$BASE_DIR/fsw/build/exe/cpu1
GSW_DIR=$BASE_DIR/gsw/cosmos
SIM_DIR=$BASE_DIR/sims/build
SIM_BIN=$SIM_DIR/bin
SIMS=$(cd $SIM_BIN; ls nos3*simulator)

if [ -f "/etc/redhat-release" ]; then
    DFLAGS="sudo docker run --rm --group-add keep-groups -it"
    DCREATE="sudo docker create --rm -it"
    DNETWORK="sudo docker network"
else
    DFLAGS="docker run --rm -it"
    DCREATE="docker create --rm -it"
    DNETWORK="docker network"
fi

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#echo "DFLAGS           = " $DFLAGS
#echo "FSW directory    = " $FSW_DIR
#echo "GSW directory    = " $GSW_DIR
#echo "Sim directory    = " $SIM_BIN
#echo "Sim list         = " $SIMS
#echo "Docker flags     = " $DFLAGS
#echo "Docker create    = " $DCREATE
#echo "Docker network   = " $DNETWORK
#exit


echo "Make data folders..."
# FSW Side
mkdir $FSW_DIR/data 2> /dev/null
mkdir $FSW_DIR/data/cam 2> /dev/null
mkdir $FSW_DIR/data/evs 2> /dev/null
mkdir $FSW_DIR/data/hk 2> /dev/null
mkdir $FSW_DIR/data/inst 2> /dev/null
# GSW Side
mkdir /tmp/data 2> /dev/null
mkdir /tmp/data/cam 2> /dev/null
mkdir /tmp/data/evs 2> /dev/null
mkdir /tmp/data/hk 2> /dev/null
mkdir /tmp/data/inst 2> /dev/null
mkdir /tmp/uplink 2> /dev/null
cp $BASE_DIR/fsw/build/exe/cpu1/cf/cfe_es_startup.scr /tmp/uplink/tmp0.so 2> /dev/null
cp $BASE_DIR/fsw/build/exe/cpu1/cf/sample.so /tmp/uplink/tmp1.so 2> /dev/null
# 42
cd /opt/nos3/42/
rm -rf NOS3InOut
cp -r $BASE_DIR/sims/cfg/InOut /opt/nos3/42/NOS3InOut


echo "Create ground networks..."
$DNETWORK create \
    --driver=bridge \
    --subnet=192.168.41.0/24 \
    --gateway=192.168.41.1 \
    NOS3_GC
echo ""


echo "Create NOS interfaces..."
export GND_CFG_FILE="-f nos3-simulator.xml"
gnome-terminal --tab --title="NOS Terminal"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos_terminal"        --network=NOS3_GC -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $GND_CFG_FILE stdio-terminal
gnome-terminal --tab --title="NOS UDP Terminal"  -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name "nos_udp_terminal"    --network=NOS3_GC -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $GND_CFG_FILE udp-terminal
echo ""


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
    $DNETWORK create $SC_NETNAME
    echo ""

    echo $SC_NUM " - 42..."
    cd /opt/nos3/42/
    xhost +local:*
    gnome-terminal --tab --title=$SC_NUM" - 42" -- $DFLAGS -e DISPLAY=$DISPLAY -v /opt/nos3/42/NOS3InOut:/opt/nos3/42/NOS3InOut -v /tmp/.X11-unix:/tmp/.X11-unix:ro --name $SC_NUM"_fortytwo" -h fortytwo --network=$SC_NETNAME -w /opt/nos3/42 -t ivvitc/nos3 /opt/nos3/42/42 NOS3InOut
    echo ""

    echo $SC_NUM " - Flight Software..."
    cd $FSW_DIR
    gnome-terminal --title=$SC_NUM" - NOS3 Flight Software" -- $DFLAGS -v $FSW_DIR:$FSW_DIR --name $SC_NUM"_nos_fsw" -h nos_fsw --network=$SC_NETNAME -w $FSW_DIR --sysctl fs.mqueue.msg_max=10000 --cap-add sys_nice ivvitc/nos3 ./core-cpu1 -R PO &
    docker network connect openc3-cosmos-network nos_fsw
    echo ""

    # Debugging
    # Replace `--tab` with `--window-with-profile=KeepOpen` once you've created this gnome-terminal profile manually

    echo $SC_NUM " - Simulators..."
    cd $SIM_BIN
    gnome-terminal --tab --title=$SC_NUM" - NOS Engine Server" -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_nos3_engine_server"  -h nos_engine_server --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 /usr/bin/nos_engine_server_standalone -f $SIM_BIN/nos_engine_server_config.json
    gnome-terminal --tab --title=$SC_NUM" - 42 Truth Sim"      -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_truth42sim"          -h truth42sim --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE  truth42sim
    gnome-terminal --tab --title=$SC_NUM" - CAM Sim"           -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_cam_sim"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE camsim
    gnome-terminal --tab --title=$SC_NUM" - CSS Sim"           -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_css_sim"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic_css_sim
    gnome-terminal --tab --title=$SC_NUM" - EPS Sim"           -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_eps_sim"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic_eps_sim
    gnome-terminal --tab --title=$SC_NUM" - FSS Sim"           -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_fss_sim"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic-fss-sim
    gnome-terminal --tab --title=$SC_NUM" - IMU Sim"           -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_imu_sim"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic_imu_sim
    gnome-terminal --tab --title=$SC_NUM" - GPS Sim"           -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_gps_sim"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE gps
    gnome-terminal --tab --title=$SC_NUM" - RW 0 Sim"          -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_rw_sim0"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim0
    gnome-terminal --tab --title=$SC_NUM" - RW 1 Sim"          -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_rw_sim1"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim1
    gnome-terminal --tab --title=$SC_NUM" - RW 2 Sim"          -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_rw_sim2"             --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic-reactionwheel-sim2
    gnome-terminal --tab --title=$SC_NUM" - Radio Sim"         -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_radio_sim"           -h radio_sim --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic_radio_sim
    $DNETWORK connect openc3-cosmos-network radio_sim
    gnome-terminal --tab --title=$SC_NUM" - Sample Sim"        -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_sample_sim"          --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE sample-sim
    gnome-terminal --tab --title=$SC_NUM" - Torquer Sim"       -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name $SC_NUM"_torquer_sim"         --network=$SC_NETNAME -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $SC_CFG_FILE generic_torquer_sim
    $DNETWORK connect $SC_NETNAME nos_terminal
    $DNETWORK connect $SC_NETNAME nos_udp_terminal
    echo ""
done


echo "NOS Time Driver..."
sleep 5
gnome-terminal --tab --title="NOS Time Driver"   -- $DFLAGS -v $SIM_DIR:$SIM_DIR --name nos_time_driver --network=NOS3_GC -w $SIM_BIN ivvitc/nos3 ./nos3-single-simulator $GND_CFG_FILE time
sleep 1
for (( i=1; i<=$SATNUM; i++ ))
do
    export SC_NUM="sc_"$i
    export SC_NETNAME="nos3_"$SC_NUM
    export TIMENAME=$SC_NUM"_nos_time_driver"
    $DNETWORK connect --alias nos_time_driver $SC_NETNAME nos_time_driver
done
echo ""


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
echo ""

echo "Docker launch script completed!"
