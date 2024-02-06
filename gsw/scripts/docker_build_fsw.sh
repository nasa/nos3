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

mkdir -p $BASE_DIR/fsw/build
$DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR --name "nos_build_fsw" -w $BASE_DIR ivvitc/nos3 make -j$NUM_CPUS build-fsw