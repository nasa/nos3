#!/bin/bash
#
# Convenience script for NOS3 development
# Builds the OpenC3 plugin
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="openc3-openc3-operator-1"

# Create a unique version string using the current date and time (YYYYMMDDHHMMSS)
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
if [ ! -d "openc3-cosmos-nos3" ]; then
    echo ""
    echo "ERROR: cli generate plugin nos3 failed!"
    echo ""
    exit 1
fi

# ==============================================================================
# PLUGIN POPULATION
# ==============================================================================
cd openc3-cosmos-nos3

echo "Populating plugin with Targets, Scripts, and Libraries..."

mkdir -p targets
mkdir -p scripts
mkdir -p lib

# 1. Grab base GSW libraries and scripts if they exist
cp -r $GSW_DIR/lib/* lib/ 2>/dev/null
cp -r $SCRIPT_DIR/*.py scripts/ 2>/dev/null
cp -r $SCRIPT_DIR/*.rb scripts/ 2>/dev/null

targets=""

# 2. Iterate over every component that has a target.txt
for target_txt in $(find $BASE_DIR/components $GSW_DIR/config/targets -name target.txt 2>/dev/null) 
do 
    target_dir=$(dirname "$target_txt")
    target_name=$(basename "$target_dir")
    
    echo "Processing target: $target_name"
    
    # Copy the whole target folder into the targets/ directory
    cp -r "$target_dir" targets/
    targets="$targets $target_name"

    # Move procedures (Python/Ruby tests) into the OpenC3 scripts/ folder 
    if [ -d "$target_dir/procedures" ]; then
        mkdir -p "scripts/$target_name"
        cp -r "$target_dir/procedures/"* "scripts/$target_name/" 2>/dev/null
        
        # Prevent OpenC3 from caching the legacy directory structure
        rm -rf "targets/$target_name/procedures"
    fi

    # Move component-specific libraries into the OpenC3 lib/ folder
    if [ -d "$target_dir/lib" ]; then
        cp -r "$target_dir/lib/"* "lib/" 2>/dev/null
        
        # Prevent OpenC3 from caching the legacy directory structure
        rm -rf "targets/$target_name/lib"
    fi
done

# 3. Patch the text dictionaries (Quotes added to prevent bash expansion issues)
echo "Patching target dictionaries..."
for i in $(find targets/ -name '*.txt')
do 
    sed -i -e 's/<%= CosmosCfsConfig::PROCESSOR_ENDIAN %>/LITTLE_ENDIAN/; s/<%=CF_INCOMING_PDU_MID%>/0x1800/; s/<%=CF_SPACE_TO_GND_PDU_MID%>/0x0800/;' "$i"
done

# ==============================================================================
# INTERFACE MAPPING (plugin.txt)
# ==============================================================================
echo "Create plugin.txt..."
rm -f plugin.txt

for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" ] && [ "$i" != "SYSTEM" ] && [ "$i" != "TO_DEBUG" ]; then
        debug="${i}_DEBUG"
        radio="${i}_RADIO"
        echo "TARGET $i $debug" >> plugin.txt
        echo "TARGET $i $radio" >> plugin.txt
    else
        echo "TARGET $i $i" >> plugin.txt
    fi
done

echo "" >> plugin.txt
echo "INTERFACE DEBUG udp_interface.rb nos-fsw 5012 5013 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" ] && [ "$i" != "SYSTEM" ] && [ "$i" != "TO_DEBUG" ]; then
        debug="${i}_DEBUG"
        echo "  MAP_TARGET $debug" >> plugin.txt
    fi
done
echo "  MAP_TARGET TO_DEBUG" >> plugin.txt
echo "" >> plugin.txt

echo "INTERFACE RADIO udp_interface.rb cryptolib 6010 6011 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" ] && [ "$i" != "SYSTEM" ] && [ "$i" != "TO_DEBUG" ]; then
        radio="${i}_RADIO"
        echo "  MAP_TARGET $radio" >> plugin.txt
    fi
done
echo "" >> plugin.txt

echo "INTERFACE SIM_42_TRUTH_INT udp_interface.rb truth42sim 5110 5111 nil nil 128 10.0 nil" >> plugin.txt
echo "  MAP_TARGET SIM_42_TRUTH" >> plugin.txt

echo "" >> plugin.txt
echo "# Created with Build Version: $BUILD_VERSION" >> plugin.txt

# Ensure permissions
chmod -R a+rX .

# ==============================================================================
# BUILD AND DEPLOY
# ==============================================================================
echo "Build plugin..."
$OPENC3_CLI rake build VERSION=$BUILD_VERSION
if [ ! -f "openc3-cosmos-nos3-${BUILD_VERSION}.gem" ]; then
    echo ""
    echo "ERROR: cli rake build failed!"
    echo ""
    exit 1
fi
echo ""

echo "================================================================="
echo " SUCCESS! Plugin gem built: openc3-cosmos-nos3-${BUILD_VERSION}.gem"
echo "================================================================="
echo ""