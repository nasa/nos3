#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="cosmos-openc3-operator-1"

echo "COSMOS build..."
$DCALL image pull ballaerospace/cosmos:4.5.0
mkdir $GSW_DIR/COMPONENTS 2> /dev/null
rm -r $GSW_DIR/COMPONENTS/* 2> /dev/null
cp -r $GSW_DIR/config/targets/SIM_CMDBUS_BRIDGE $GSW_DIR/COMPONENTS/
for i in $(find $BASE_DIR/components/ -name "gsw" -type d)
do
    #echo "$i"
    cp -r $i/* $GSW_DIR/COMPONENTS/
    cp $i/*.txt $GSW_DIR/COMPONENTS/SIM_CMDBUS_BRIDGE/cmd_tlm/ 2> /dev/null
done
echo ""