#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

mkdir -p $BASE_DIR/fsw/build
$DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -w $BASE_DIR --name "nos3_debug" $DBOX bash
