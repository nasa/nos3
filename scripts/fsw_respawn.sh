#!/bin/bash
#
# Script to start FSW and restart it if it dies/is killed
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

#echo "fsw_respawn.sh script"

cd $FSW_DIR

while [ 1 ]
do
    pidof core-cpu1 > /dev/null
    if [ $? -eq 1 ]
    then
        sleep 5
        pidof core-cpu1 > /dev/null
        if [ $? -eq 1 ]
        then
            $FSW_DIR/core-cpu1 -R PO & 
        fi
    fi
    sleep 1
done