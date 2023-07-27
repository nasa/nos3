#!/bin/bash
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
GSW_BIN=$BASE_DIR/gsw/cosmos/build/openc3-cosmos-nos3
DATE=$(date "+%Y%m%d%H%M")

# Start by changing to a known location
cd $SCRIPT_DIR/../cosmos

# Delete any previous run info
rm -rf build

# Start generating the plugin
mkdir build
cd build
/opt/nos3/cosmos/openc3.sh cliroot generate plugin nos3

# Copy targets
mkdir openc3-cosmos-nos3/targets
cd openc3-cosmos-nos3/targets
targets=""
for i in $(find ../../../../../components -name target.txt) 
do 
    j=$(dirname $i)
    cp -r $j .
    targets="$targets $(basename $j)"
done
for i in $(find ../../../config/targets -name target.txt) 
do 
    j=$(dirname $i)
    cp -r $j .
    k=$(basename $j)
    targets="$targets $(basename $j)"
done
for i in $(find . -name *.txt)
do 
    sed -i -e 's/<%= CosmosCfsConfig::PROCESSOR_ENDIAN %>/LITTLE_ENDIAN/; s/<%=CF_INCOMING_PDU_MID%>/0x1800/; s/<%=CF_SPACE_TO_GND_PDU_MID%>/0x0800/;' $i
done
cd ..

# Copy lib
cp -r ../../lib .

# Create plugin.txt
rm plugin.txt
for i in $targets
do
    if [ "$i" != "SYSTEM" ]
    then
        echo TARGET $i $i >> plugin.txt
    fi
done
echo "" >> plugin.txt
echo "INTERFACE DEBUG udp_interface.rb host.docker.internal 5012 5013 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" -a "$i" != "SYSTEM" ]
    then
        echo "   MAP_TARGET $i" >> plugin.txt
    fi
done
echo "" >> plugin.txt
echo "INTERFACE SIM_42_TRUTH_INT udp_interface.rb host.docker.internal 5110 5111 nil nil 128 10.0 nil" >> plugin.txt
echo "   MAP_TARGET SIM_42_TRUTH" >> plugin.txt

# Capture date created
echo "" >> plugin.txt
echo "# Created on " $DATE >> plugin.txt

# Build plugin
/opt/nos3/cosmos/openc3.sh cliroot rake build VERSION=1.0.$DATE

# Plugin ends up in $SCRIPT_DIR/../cosmos/build/openc3-cosmos-nos3/openc3-cosmos-nos3-1.0.0.gem

# Use plugin
cd $GSW_BIN
/opt/nos3/cosmos/openc3.sh cliroot geminstall ./openc3-cosmos-nos3-1.0.$DATE.gem
/opt/nos3/cosmos/openc3.sh cliroot load openc3-cosmos-nos3-1.0.$DATE.gem

