#!/bin/bash -i
#
# Convenience script for NOS3 development
# Cleans up local gem files, removes the plugin from OpenC3, and wipes database volumes
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh
export GSW="openc3-openc3-operator-1"

echo "-> Stopping OpenC3 containers and wiping persistent volumes..."
# 1. Graceful compose teardown (removes containers, networks, and volumes)
if [ -f "$OPENC3_DIR/compose.yaml" ]; then
    docker compose -f "$OPENC3_DIR/compose.yaml" down -v 2>/dev/null
fi

# 2. Aggressive fallback wipe (just in case 'compose down' missed anything)
# This stops remaining openc3 containers, removes them, and forcibly deletes the volumes
docker stop $(docker ps -q -f name=openc3) 2>/dev/null
docker rm $(docker ps -aq -f name=openc3) 2>/dev/null
docker volume ls -q | grep "openc3" | xargs -r docker volume rm 2>/dev/null

echo "-> Removing local gem files..."
if [ -d "$OPENC3_DIR/openc3-cosmos-nos3" ]; then
    cd "$OPENC3_DIR/openc3-cosmos-nos3"
    rm -f openc3-cosmos-nos3-*.gem 2>/dev/null
    cd ..
fi

echo "-> Wiping generated build directories..."
cd "$OPENC3_DIR" 2>/dev/null
rm -rf build openc3-cosmos-nos3 2>/dev/null
rm -rf plugins/* 2>/dev/null
rm -rf openc3-data/* 2>/dev/null