#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh

echo "COSMOS build..."
mkdir $GSW_DIR/COMPONENTS 2> /dev/null
rm -r $GSW_DIR/COMPONENTS/* 2> /dev/null
for i in $(find $BASE_DIR/components/ -name "gsw" -type d)
do
    #echo "$i"
    cp -r $i/* $GSW_DIR/COMPONENTS/
done
