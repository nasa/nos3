#!/bin/bash
#
# Script to start FSW and restart it if it dies/is killed
#

FSW_BIN=$1

#echo "fsw_respawn.sh script"

while [ 1 ]
do
    pidof core-cpu1 > /dev/null
    if [ $? -eq 1 ]
    then
        sleep 5
        pidof core-cpu1 > /dev/null
        if [ $? -eq 1 ]
        then
            gnome-terminal --title="NOS3 Flight Software" -- $FSW_BIN/core-cpu1 -R PO
        fi
    fi
    sleep 1
done