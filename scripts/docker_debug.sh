#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

mkdir -p $BASE_DIR/fsw/build
# $DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -w $BASE_DIR --name "nos3_debug" $DBOX bash
# $DFLAGS_CPUS -it -v $BASE_DIR:$BASE_DIR --network nos3_sc_1 -p 5000:5000 -p 50000:50000 -p 5001:5001 -v $USERDIR:$USERDIR -w $BASE_DIR --name "nos3_debug" $DBOX bash
$DFLAGS_CPUS -it -v $BASE_DIR:$BASE_DIR  -p 5000:5000 -p 50000:50000 -p 5001:5001 -v $USERDIR:$USERDIR -w $BASE_DIR --name "nos3_debug" $DBOX bash
# $DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR --network host -v $USERDIR:$USERDIR -w $BASE_DIR --name "nos3_debug" $DBOX bash
# $DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR --expose 5000 -v $USERDIR:$USERDIR -w $BASE_DIR --name "nos3_debug" $DBOX bash
# $DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -p 5000:5000 -v $USERDIR:$USERDIR -w $BASE_DIR --name "nos3_debug" $DBOX bash