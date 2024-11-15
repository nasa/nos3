#!/bin/bash -i
#
# Convenience script for NOS3 development
#

# Note the first argument passed is expected to be the BASE_DIR of the NOS3 repository
source $1/scripts/env.sh

echo "COSMOS build..."
$DCALL image pull ballaerospace/cosmos:4.5.0
mkdir $GSW_DIR/COMPONENTS 2> /dev/null
rm -r $GSW_DIR/COMPONENTS/* 2> /dev/null
for i in $(find $BASE_DIR/components/ -name "gsw" -type d)
do
    #echo "$i"
    cp -r $i/* $GSW_DIR/COMPONENTS/
done
echo ""
