#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh

cd $OPENC3_DIR/openc3-cosmos-nos3
rm -f openc3-cosmos-nos3-1.0.*.gem 2>/dev/null