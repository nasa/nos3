#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)

echo "Cleaning up all COSMOS files..."
yes | rm $BASE_DIR/gsw/cosmos/Gemfile 2> /dev/null
yes | rm $BASE_DIR/gsw/cosmos/Gemfile.lock 2> /dev/null
yes | rm $BASE_DIR/gsw/cosmos/outputs/logs/20* 2> /dev/null
yes | rm $BASE_DIR/gsw/cosmos/outputs/tmp/marshal_* 2> /dev/null
yes | rm $BASE_DIR/minicom.cap 2> /dev/null

exit 0
