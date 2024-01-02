#!/bin/bash
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

# Check that local NOS3 directory exists
if [ ! -d $USER_NOS3_DIR ]; then
    echo ""
    echo "    Need to run make prep first!"
    echo ""
    exit 1
fi

# Start by changing to a known location
cd $OPENC3_DIR

# Delete any previous run info
rm -rf build
if [ -d "build" ]
then
    echo ""
    echo "ERROR: Failed to delete build directory!"
    echo ""
    exit 1
fi

# Start generating the plugin
mkdir build
cd build
$OPENC3_PATH cli generate plugin nos3
if [ ! -d "openc3-cosmos-nos3" ]
then
    echo ""
    echo "ERROR: cli generate plugin nos3 failed!"
    echo ""
    exit 1
fi

# Copy targets
mkdir openc3-cosmos-nos3/targets
cd openc3-cosmos-nos3/targets
targets=""
for i in $(find $BASE_DIR/components -name target.txt) 
do 
    j=$(dirname $i)
    cp -r $j .
    targets="$targets $(basename $j)"
done
for i in $(find $GSW_DIR/config/targets -name target.txt) 
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
echo "Create plugin..."
rm plugin.txt
if [ -f "plugin.txt" ]
then
    echo ""
    echo "ERROR: Failed to remove plugin.txt file!"
    echo ""
    exit 1
fi

for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" -a "$i" != "SYSTEM" -a "$i" != "TO_DEBUG" ]
    then
        debug=$i"_DEBUG"
        radio=$i"_RADIO"
        echo TARGET $i $debug >> plugin.txt
        echo TARGET $i $radio >> plugin.txt
    else
        echo TARGET $i $i >> plugin.txt
    fi
done
echo "" >> plugin.txt
echo "INTERFACE DEBUG udp_interface.rb nos_fsw 5012 5013 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" -a "$i" != "SYSTEM" -a "$i" != "TO_DEBUG" ]
    then
        debug=$i"_DEBUG"
        echo "   MAP_TARGET $debug" >> plugin.txt
    fi
done
echo "   MAP_TARGET TO_DEBUG" >> plugin.txt
echo "" >> plugin.txt

echo "INTERFACE RADIO udp_interface.rb radio_sim 6010 6011 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" -a "$i" != "SYSTEM" -a "$i" != "TO_DEBUG" ]
    then
        radio=$i"_RADIO"
        echo "   MAP_TARGET $radio" >> plugin.txt
    fi
done
echo "" >> plugin.txt

echo "INTERFACE SIM_42_TRUTH_INT udp_interface.rb host.docker.internal 5110 5111 nil nil 128 10.0 nil" >> plugin.txt
echo "   MAP_TARGET SIM_42_TRUTH" >> plugin.txt

# Capture date created
echo "" >> plugin.txt
echo "# Created on " $DATE >> plugin.txt
echo ""

# Build plugin
echo "Build plugin..."
$OPENC3_PATH cli rake build VERSION=1.0.$DATE
if [ ! -f "openc3-cosmos-nos3-1.0.$DATE.gem" ]
then
    echo ""
    echo "ERROR: cli rake build failed!"
    echo ""
    exit 1
fi
echo ""

## Install plugin
#echo "Install plugin..."
#cd $GSW_BIN
#$OPENC3_PATH cli geminstall ./openc3-cosmos-nos3-1.0.$DATE.gem
#echo ""

# Load plugin
echo "Load plugin..."
$OPENC3_PATH cliroot load openc3-cosmos-nos3-1.0.$DATE.gem
echo ""

## Set permissions on build files
#chmod -R 777 $BASE_DIR/gsw/cosmos/build

echo "Create COSMOS gem script complete."
echo "Note that while this script is complete, COSMOS is likely still be processing behind the scenes!"
echo ""
