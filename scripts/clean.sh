#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

$DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_BUILD_DIR:$USER_NOS3_BUILD_DIR --name "nos_clean" -w $BASE_DIR $DBOX rm -rf $USER_NOS3_BUILD_DIR/*
