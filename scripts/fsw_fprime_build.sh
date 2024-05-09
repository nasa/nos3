#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[1]}" )/scripts" && pwd )
echo "SCRIPT DIR: $SCRIPT_DIR"
source $SCRIPT_DIR/env.sh

$DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR --name "fprime_build" -w $BASE_DIR $DBOX ./scripts/build_fprime.sh
