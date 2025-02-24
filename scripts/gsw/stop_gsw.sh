#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

echo "Stop gsw..."

# OpenC3
cd $OPENC3_DIR
$OPENC3_PATH stop

# COSMOS
$DCALL ps --filter ancestor="ballaerospace/cosmos:4.5.0" -aq | xargs $DCALL stop > /dev/null 2>&1 &
