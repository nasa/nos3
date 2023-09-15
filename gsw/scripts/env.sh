#!/bin/bash
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_DIR=$BASE_DIR/fsw/build/exe/cpu1
GSW_BIN=$BASE_DIR/gsw/cosmos/build/openc3-cosmos-nos3
GSW_DIR=$BASE_DIR/gsw/cosmos
SIM_DIR=$BASE_DIR/sims/build
SIM_BIN=$SIM_DIR/bin

if [ -d $SIM_DIR/bin ]; then
    SIMS=$(ls $SIM_BIN/nos3*simulator) 
fi 

if [ -f "/etc/redhat-release" ]; then
    DCALL="sudo docker"
    DFLAGS="sudo docker run --rm --group-add keep-groups -it"
    DCREATE="sudo docker create --rm -it"
    DNETWORK="sudo docker network"
else
    DCALL="docker"
    DFLAGS="docker run --rm -it -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro -u $(id -u):$(id -g)"
    DCREATE="docker create --rm -it"
    DNETWORK="docker network"
fi

DATE=$(date "+%Y%m%d%H%M")
OPENC3_PATH="/opt/nos3/cosmos/openc3.sh"

NUM_CPUS="$( nproc )"

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#echo "DFLAGS           = " $DFLAGS
#echo "FSW directory    = " $FSW_DIR
#echo "GSW bin          = " $GSW_BIN
#echo "GSW directory    = " $GSW_DIR
#echo "Sim directory    = " $SIM_BIN
#echo "Sim list         = " $SIMS
#echo "Docker flags     = " $DFLAGS
#echo "Docker create    = " $DCREATE
#echo "Docker network   = " $DNETWORK
#echo "Date             = " $DATE
#echo "OpenC3 path      = " $OPENC3_PATH
