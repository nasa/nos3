#!/bin/bash -i
#
# Convenience script for NOS3 development
#

# Note the first argument passed is expected to be the BASE_DIR of the NOS3 repository
source $1/scripts/env.sh

echo "AIT build..."
$DCALL image pull ghcr.io/sphinxdefense/gsw-ait:main
$DCALL image pull ghcr.io/sphinxdefense/ttc-command:main
echo ""
