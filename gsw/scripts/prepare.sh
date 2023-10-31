#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

echo "Create local user directory..."
mkdir $USER_NOS3_DIR 2> /dev/null
echo "  "$USER_NOS3_DIR
mkdir $USER_NOS3_DIR/42 2> /dev/null

echo "Clone openc3-cosmos into local user directory..."
cd $USER_NOS3_DIR
git clone https://github.com/nasa-itc/openc3-cosmos.git --depth 1 -b nos3-main
git reset --hard

echo "Prepare openc3-cosmos containers..."
cd $OPENC3_DIR
$OPENC3_PATH start

echo "Prepare nos3 docker container..."
$DCALL image pull ivvitc/nos3

echo "Set a password in openc3-cosmos via firefox..."
sleep 5
firefox localhost:2900 &
