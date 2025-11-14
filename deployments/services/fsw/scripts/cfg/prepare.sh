#!/bin/bash -i
#
# Convenience script for NOS3 development
#
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh
echo ""

echo "Create local user directory... ${USER_NOS3_DIR}"
mkdir -p $USER_NOS3_DIR
mkdir -p $USER_NOS3_DIR/42
echo ""

echo "Preparing Shared Folders for Fprime... ${$USER_FPRIME_PATH}"
mkdir -p $USER_FPRIME_PATH
echo ""