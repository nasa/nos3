#!/bin/bash
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR` && pwd)
FSW_DIR=$BASE_DIR/fsw/build/exe/cpu1
GSW_BIN=$BASE_DIR/gsw/cosmos/build/openc3-cosmos-nos3
GSW_DIR=$BASE_DIR/gsw/cosmos
SIM_DIR=$BASE_DIR/sims/build
SIM_BIN=$SIM_DIR/bin

if [ -d $SIM_DIR/bin ]; then
    SIMS=$(ls $SIM_BIN/nos3*simulator) 
fi 

DATE=$(date "+%Y%m%d%H%M")
NUM_CPUS="$( nproc )"

USER_NOS3_DIR=$(cd ~/ && pwd)/.nos3
OPENC3_DIR=$USER_NOS3_DIR/cosmos
OPENC3_PATH=$OPENC3_DIR/openc3.sh

###
### Notes: 
###   Podman and/or Docker on RHEL not yet supported
###
#if [ -f "/etc/redhat-release" ]; then
#    DCALL="docker"
#    DFLAGS="docker run --rm -it -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro -u $(id -u $(stat -c '%U' $SCRIPT_DIR/env.sh)):$(getent group $(stat -c '%G' $SCRIPT_DIR/env.sh) | cut -d: -f3)"
#    DFLAGS_CPUS="$DFLAGS --cpus=$NUM_CPUS"
#    DCREATE="docker create --rm -it"
#    DNETWORK="docker network"
#else
    DCALL="docker"
    DFLAGS="docker run --rm -it -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro -u $(id -u $(stat -c '%U' $SCRIPT_DIR/env.sh)):$(getent group $(stat -c '%G' $SCRIPT_DIR/env.sh) | cut -d: -f3)"
    DFLAGS_CPUS="$DFLAGS --cpus=$NUM_CPUS"
    DCREATE="docker create --rm -it"
    DNETWORK="docker network"
#fi

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
#echo "Local user .nos3 = " $USER_NOS3_DIR
#echo "OpenC3 directory = " $OPENC3_DIR
#echo "OpenC3 path      = " $OPENC3_PATH
