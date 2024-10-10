#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

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

echo "Create networks..."
$DNETWORK create \
    --driver=overlay \
    --subnet=192.168.41.0/24 \
    --gateway=192.168.41.1 \
    --attachable \
    nos3_core


echo "Launch GSW..."
$BASE_DIR/cfg/build/gsw_launch.sh
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

    $DNETWORK create \
        --driver=overlay \
        --attachable \
        $SC_NETNAME

    # Debugging
    #echo "Spacecraft number        = " $SC_NUM
    #echo "Spacecraft network       = " $SC_NETNAME
    #echo "Spacecraft configuration = " $SC_CFG_FILE
    
    echo $SC_NUM " - Connect COSMOS to spacecraft network..."
    $DNETWORK connect $SC_NETNAME cosmos_openc3-operator_1 --alias cosmos
    echo ""

    echo ""
done

echo "Docker launch script completed!"
