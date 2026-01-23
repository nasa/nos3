#!/bin/bash
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="openc3-openc3-operator-1"

# Check that local NOS3 directory exists
if [ ! -d $USER_NOS3_DIR ]; then
    echo ""
    echo "    Need to run make prep first!"
    echo ""
    exit 1
fi

echo "Prepare OpenC3 docker containers..."
cd $USER_NOS3_DIR
git clone https://github.com/nasa-itc/openc3-nos3.git -b dev $USER_NOS3_DIR/openc3
$DOCKER_COMPOSE_COMMAND -f $OPENC3_DIR/compose.yaml pull 
echo ""

# Check that openc3 directory exists
if [ ! -d $OPENC3_DIR ]; then
    echo ""
    echo "    OpenC3 Cloning Failed!"
    echo ""
    exit 1
fi

echo "Launch openc3 containers..."
cd $OPENC3_DIR
$OPENC3_PATH run
echo ""

#echo "Set a password in openc3 via firefox..."
#echo "  Refresh webpage if error page shown."
#echo ""
#sleep 5
#firefox localhost:2900 &

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
# cd build
$OPENC3_CLI generate plugin nos3 --ruby
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
cp -r $GSW_DIR/lib .

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
echo "INTERFACE DEBUG udp_interface.rb nos-fsw 5012 5013 nil nil 128 10.0 nil" >> plugin.txt
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

echo "INTERFACE RADIO udp_interface.rb cryptolib 6010 6011 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" -a "$i" != "SYSTEM" -a "$i" != "TO_DEBUG" ]
    then
        radio=$i"_RADIO"
        echo "   MAP_TARGET $radio" >> plugin.txt
    fi
done
echo "" >> plugin.txt

echo "INTERFACE SIM_42_TRUTH_INT udp_interface.rb truth42sim 5110 5111 nil nil 128 10.0 nil" >> plugin.txt
echo "   MAP_TARGET SIM_42_TRUTH" >> plugin.txt

# Capture date created
echo "" >> plugin.txt
echo "# Created on " $DATE >> plugin.txt
echo ""

# Build plugin
echo "Build plugin..."
$OPENC3_CLI rake build VERSION=1.0.$DATE
if [ ! -f "openc3-cosmos-nos3-1.0.$DATE.gem" ]
then
    echo ""
    echo "ERROR: cli rake build failed!"
    echo ""
    exit 1
fi
echo ""

## Install plugin
echo "Install plugin..."
cd $OPENC3_DIR/openc3-cosmos-nos3
$OPENC3_CLI geminstall ./openc3-cosmos-nos3-1.0.$DATE.gem
INSTALL_STATUS=$?

if [ $INSTALL_STATUS -eq 0 ]; then
    echo "Gem installation successful"
else
    echo "Gem installation failed with exit code: $INSTALL_STATUS"
    exit 1
fi
echo ""


echo "OpenC3 build script complete."
echo "Note that while this script is complete, OpenC3 is likely still be processing behind the scenes!"
sleep 15
echo "Done sleeping, but check cpu use prior to proceeding!"
echo ""
