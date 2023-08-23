#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)

echo "Cleaning up all COSMOS files..."
yes | rm $BASE_DIR/gsw/cosmos/Gemfile 2> /dev/null
yes | rm $BASE_DIR/gsw/cosmos/Gemfile.lock 2> /dev/null
yes | rm -r $BASE_DIR/gsw/cosmos/COMPONENTS 2> /dev/null
yes | rm -r $BASE_DIR/gsw/cosmos/outputs 2> /dev/null

echo "Cleaning up Minicom log..."
yes | rm $BASE_DIR/minicom.cap 2> /dev/null

echo "Cleaning up CryptoLib build..."
yes | rm $BASE_DIR/minicom.cap 2> /dev/null

exit 0
