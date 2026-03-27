#!/bin/bash
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="openc3-openc3-operator-1"

# Create a unique version string using the current date and time (YYYYMMDDHHMMSS)
# This prevents OpenC3 from caching an older version of the plugin built on the same day
BUILD_VERSION="1.0.$(date +%Y%m%d%H%M%S)"

# Check that local NOS3 directory exists
if [ ! -d $USER_NOS3_DIR ]; then
    echo ""
    echo "    Need to run make prep first!"
    echo ""
    exit 1
fi

echo "Prepare OpenC3 docker containers..."
cd $USER_NOS3_DIR

# --- Safe OpenC3 clone/pull ---
if [ -d "$USER_NOS3_DIR/openc3" ]; then
    echo "openc3 repository already exists, pulling latest..."
    git -C "$USER_NOS3_DIR/openc3" pull || echo "Warning: git pull failed, using existing local files."
else
    echo "Cloning openc3 repository..."
    git clone https://github.com/nasa-itc/openc3-nos3.git -b dev "$USER_NOS3_DIR/openc3"
fi

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

# Start by changing to a known location
cd $OPENC3_DIR

# --- Delete any previous run info including the plugin folder ---
rm -rf build openc3-cosmos-nos3
if [ -d "build" ]; then
    echo ""
    echo "ERROR: Failed to delete build directory!"
    echo ""
    exit 1
fi

# Start generating the plugin
mkdir build
$OPENC3_CLI generate plugin nos3 --ruby
if [ ! -d "openc3-cosmos-nos3" ]
then
    echo ""
    echo "ERROR: cli generate plugin nos3 failed!"
    echo ""
    exit 1
fi

# Copy targets
mkdir -p openc3-cosmos-nos3/targets
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
echo "Copying library files..."
cp -r $GSW_DIR/lib .

# --- Copy scripts into the plugin ---
echo "Copying Python test scripts..."
mkdir -p openc3-cosmos-nos3/scripts
# Note: Adjust the source path '$SCRIPT_DIR' below if your test python 
# files are stored in a different directory in your repository.
cp -r $SCRIPT_DIR/*.py openc3-cosmos-nos3/scripts/

# Create plugin.txt
echo "Create plugin.txt..."
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
        echo "  MAP_TARGET $debug" >> plugin.txt
    fi
done
echo "  MAP_TARGET TO_DEBUG" >> plugin.txt
echo "" >> plugin.txt

echo "INTERFACE RADIO udp_interface.rb cryptolib 6010 6011 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" -a "$i" != "SYSTEM" -a "$i" != "TO_DEBUG" ]
    then
        radio=$i"_RADIO"
        echo "  MAP_TARGET $radio" >> plugin.txt
    fi
done
echo "" >> plugin.txt

echo "INTERFACE SIM_42_TRUTH_INT udp_interface.rb truth42sim 5110 5111 nil nil 128 10.0 nil" >> plugin.txt
echo "  MAP_TARGET SIM_42_TRUTH" >> plugin.txt

# Capture date created
echo "" >> plugin.txt
echo "# Created with Build Version: $BUILD_VERSION" >> plugin.txt
echo ""

# Ensure the generated plugin tree is world-readable/traversable for gem build
echo "Fixing permissions on generated plugin tree before building..."
chmod -R a+rX .

# Build plugin
echo "Build plugin..."
$OPENC3_CLI rake build VERSION=$BUILD_VERSION
if [ ! -f "openc3-cosmos-nos3-${BUILD_VERSION}.gem" ]
then
    echo ""
    echo "ERROR: cli rake build failed!"
    echo ""
    exit 1
fi
echo ""

# --- Auto-upload to OpenC3 using the REST API ---
echo "================================================================="
echo " SUCCESS! Plugin gem built: openc3-cosmos-nos3-${BUILD_VERSION}.gem"
echo "================================================================="
echo "Auto-uploading plugin to OpenC3..."

# Wait a few seconds to ensure the API is fully responsive
sleep 5 

# Upload via OpenC3 REST API using curl
HTTP_STATUS=$(curl -s -o /dev/null -w "%{http_code}" -X POST -F "file=@openc3-cosmos-nos3-${BUILD_VERSION}.gem" http://localhost:2900/openc3-api/plugins)

if [ "$HTTP_STATUS" -eq 200 ] || [ "$HTTP_STATUS" -eq 201 ] || [ "$HTTP_STATUS" -eq 302 ]; then
    echo "-> Auto-upload successful! (HTTP $HTTP_STATUS) The plugin is now active."
    echo "-> View your scripts at: http://localhost:2900/tools/script-runner"
else
    echo "-> Auto-upload failed or API not ready (HTTP $HTTP_STATUS)."
    echo "-> You can manually upload via the Web Interface:"
    echo "   1. Go to http://localhost:2900/tools/admin"
    echo "   2. Click the 'Plugins' tab and upload:"
    echo "      $(pwd)/openc3-cosmos-nos3-${BUILD_VERSION}.gem"
fi
echo "================================================================="
echo ""