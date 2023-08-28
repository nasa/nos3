#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

killall -q -r -INT core-cpu*

sleep 5

echo "Flight Software..."
cd $FSW_BIN
gnome-terminal --title="NOS3 Flight Software" -- $FSW_BIN/core-cpu1 -R PO &
# Note: Can keep open if desired after a new gnome-profile is manually created
#gnome-terminal --window-with-profile=KeepOpen --title="NOS3 Flight Software" -- $FSW_BIN/core-cpu1 -R PO &
