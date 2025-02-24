#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

mkdir -p $BASE_DIR/fsw/build
$DFLAGS_CPUS -v $USER_FPRIME_PATH:$USER_FPRIME_PATH -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -v $MVN_DIR:$MVN_DIR -p 8090:8090 -p 5012:5012  -w $BASE_DIR --sysctl fs.mqueue.msg_max=10000 --ulimit rtprio=99 --cap-add=sys_nice -e LD_LIBRARY_PATH="/usr/local/lib" --name "nos3_debug" $DBOX bash
