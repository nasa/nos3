#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh

sleep 5
cd $BASE_DIR/fsw/fprime/fprime-nos3/logs
echo "$PWD"

cd  "$(\ls -1dt ./*/ | head -n 1)"
echo "$PWD"
sleep 15
tail -f SampleSimDeployment.log
