#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh

# Make flight software configuration directory
mkdir -p $USER_NOS3_BUILD_DIR/cfg

# Copy baseline configurations into build directory
cp -r $BASE_DIR/cfg/InOut $USER_NOS3_BUILD_DIR/cfg/
cp -r $BASE_DIR/cfg/nos3_defs $USER_NOS3_BUILD_DIR/cfg/
cp -r $BASE_DIR/cfg/sims $USER_NOS3_BUILD_DIR/cfg/

# Configure flight software
python3 $SCRIPT_DIR/cfg/configure.py
