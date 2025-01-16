#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh

echo "Cleaning up any COSMOS files..."
yes | rm $BASE_DIR/gsw/cosmos/Gemfile 2> /dev/null
yes | rm $BASE_DIR/gsw/cosmos/Gemfile.lock 2> /dev/null
yes | rm -r $BASE_DIR/gsw/cosmos/COMPONENTS 2> /dev/null
yes | rm -r $BASE_DIR/gsw/cosmos/outputs 2> /dev/null

echo "Cleaning up Minicom log..."
yes | rm $BASE_DIR/minicom.cap 2> /dev/null

echo "Cleaning up local user directory..."
if docker ps -a --format "{{.Names}}" | grep -q "^${DBOX}$"; then
    rm -f "${USER_NOS3_DIR}"
fi
rm -rf $USER_NOS3_DIR/*
rm -rf $USER_FPRIME_PATH

echo "Removing NOS Based containers..."
yes 2> /dev/null | $DCALL images --format "{{.Repository}}:{{.Tag}}" | grep '^ivvitc' | xargs -r docker rmi  

echo "Removing NOS Based container networks..."
yes | $DNETWORK ls --format "{{.Name}}" | grep '^nos3_' | xargs -r docker network rm 2> /dev/null

yes | $DCALL swarm leave --force 2> /dev/null

exit 0
