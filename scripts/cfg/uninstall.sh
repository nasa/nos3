#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh

echo "Cleaning up all COSMOS files..."
yes | rm $BASE_DIR/gsw/cosmos/Gemfile 2> /dev/null
yes | rm $BASE_DIR/gsw/cosmos/Gemfile.lock 2> /dev/null
yes | rm -r $BASE_DIR/gsw/cosmos/COMPONENTS 2> /dev/null
yes | rm -r $BASE_DIR/gsw/cosmos/outputs 2> /dev/null

echo "Cleaning up Minicom log..."
yes | rm $BASE_DIR/minicom.cap 2> /dev/null

echo "Cleaning up CryptoLib build..."
yes | rm $BASE_DIR/minicom.cap 2> /dev/null

echo "Cleaning up local user directory..."
$DFLAGS -v $USER_NOS3_DIR:$USER_NOS3_DIR $DBOX rm -rf $USER_NOS3_DIR
rm -rf $USER_NOS3_DIR/*

yes | rm -rf $USER_NOS3_DIR/.m2 2> /dev/null
yes | rm -rf $USER_NOS3_DIR 2> /dev/null

echo "Removing containers..."
$DCALL system prune -f 2> /dev/null

echo "Removing container networks..."
yes | docker network prune -f 2> /dev/null
yes | docker swarm leave --force 2> /dev/null

exit 0
