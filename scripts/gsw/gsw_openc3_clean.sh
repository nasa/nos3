#!/bin/bash -i
#
# Convenience script for NOS3 development
# Cleans up local gem files and removes the plugin from OpenC3
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh

echo "================================================================="
echo " Cleaning OpenC3 NOS3 Plugin Environment..."
echo "================================================================="

# 1. API Cleanup: Query OpenC3 for installed nos3 plugins and delete them
echo "-> Querying running OpenC3 instance for installed NOS3 plugins..."

# Fetch the list of plugins, extract the names matching our plugin, and get unique entries
PLUGINS=$(curl -s http://localhost:2900/openc3-api/plugins | grep -o '"name":"openc3-cosmos-nos3[^"]*"' | cut -d'"' -f4 | sort -u)

if [ -z "$PLUGINS" ]; then
    echo "-> No NOS3 plugins currently found in the OpenC3 API (or OpenC3 is not running)."
else
    for PLUGIN in $PLUGINS; do
        echo "-> Deleting plugin $PLUGIN from OpenC3 API..."
        HTTP_STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE "http://localhost:2900/openc3-api/plugins/$PLUGIN")
        echo "   (HTTP Status: $HTTP_STATUS)"
    done
fi

# 2. Local File Cleanup
if [ -d "$OPENC3_DIR/openc3-cosmos-nos3" ]; then
    echo "-> Removing local gem files..."
    cd "$OPENC3_DIR/openc3-cosmos-nos3"
    rm -f openc3-cosmos-nos3-1.0.*.gem 2>/dev/null
    cd ..
fi

echo "-> Wiping generated build directories..."
cd "$OPENC3_DIR"
rm -rf build openc3-cosmos-nos3

echo "================================================================="
echo " Cleanup complete! You are ready for a fresh build."
echo "================================================================="
echo ""