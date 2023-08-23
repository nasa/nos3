#!/bin/bash
#
# Convenience script for NOS3 development
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
echo "Script directory = " $SCRIPT_DIR
echo "Base directory   = " $BASE_DIR
echo "DFLAGS           = " $DFLAGS
echo "FSW directory    = " $FSW_DIR
echo "GSW directory    = " $GSW_DIR
echo "Sim directory    = " $SIM_BIN
echo "Sim list         = " $SIMS
echo "Docker flags     = " $DFLAGS
echo "Docker create    = " $DCREATE
echo "Docker network   = " $DNETWORK
