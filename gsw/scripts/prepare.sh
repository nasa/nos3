#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

# COSMOS Installation
cd $GSW_DIR
./openc3.sh run

# NOS3 Container
$DCALL image pull ivvitc/nos3
