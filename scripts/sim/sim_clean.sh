#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

# Check that NOS3 build directory exists
if [ -d $USER_NOS3_BUILD_DIR/sim ]; then
    # Cleanup all build files
    $DFLAGS -v $USER_NOS3_BUILD_DIR:$USER_NOS3_BUILD_DIR --name "nos_clean" -w $BASE_DIR $DBOX rm -rf $USER_NOS3_BUILD_DIR/sim/*
fi
