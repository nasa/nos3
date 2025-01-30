#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

export SC_NUM="sc_"1
export SC_NETNAME="nos3_"$SC_NUM

mkdir -p $BASE_DIR/fsw/build
# $DFLAGS_CPUS -v $USER_FPRIME_PATH:$USER_FPRIME_PATH -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -v $MVN_DIR:$MVN_DIR -p 8090:8090 -p 5012:5012  -w $BASE_DIR --sysctl fs.mqueue.msg_max=10000 --ulimit rtprio=99 --cap-add=sys_nice --name "nos3_debug" $DBOX bash

# -p 8091:8090 -p 5000:5000 -p 50050:50050 -p 50000:50000 -p 0.0.0.0:8091:8090 -p 5020:5012 -p 5021:5013
$DFLAGS_CPUS -v $USER_FPRIME_PATH:$USER_FPRIME_PATH -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -v $MVN_DIR:$MVN_DIR  --network=$SC_NETNAME -w $BASE_DIR --sysctl fs.mqueue.msg_max=10000 --ulimit rtprio=99 --cap-add=sys_nice --name "nos3_debug" $DBOX bash