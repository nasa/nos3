#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh

echo "YAMCS build..."
$DCALL image pull maven:3.9.9-eclipse-temurin-17
rm -rf $USER_NOS3_DIR/yamcs 2> /dev/null
cp -r $BASE_DIR/gsw/yamcs $USER_NOS3_DIR/
echo ""
