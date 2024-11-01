#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh

echo "AIT build..."
$DCALL image pull ghcr.io/sphinxdefense/gsw-ait:main
$DCALL image pull ghcr.io/sphinxdefense/ttc-command:main
echo ""
