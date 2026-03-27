#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh

cd $OPENC3_DIR/openc3-cosmos-nos3

# --- FIX: Also remove the plugin from the running OpenC3 instance ---
echo "Attempting to remove plugin from running OpenC3 instance..."
curl -s -X DELETE http://localhost:2900/openc3-api/plugins/openc3-cosmos-nos3 > /dev/null

echo "Removing local gem files..."
rm -f openc3-cosmos-nos3-1.0.*.gem 2>/dev/null