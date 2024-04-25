#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

# Make flight software configuration directory
mkdir -p $BASE_DIR/cfg/build

# Copy baseline configurations into build directory
cp -r $BASE_DIR/cfg/InOut $BASE_DIR/cfg/build/
cp -r $BASE_DIR/cfg/nos3_defs $BASE_DIR/cfg/build/
cp -r $BASE_DIR/cfg/sims $BASE_DIR/cfg/build/

# Configure flight software
python3 $SCRIPT_DIR/configure.py
