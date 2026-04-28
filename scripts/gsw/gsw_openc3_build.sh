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
mkdir -p scripts      # <-- Changed back to scripts
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
    
    cp -r "$target_dir" targets/
    targets="$targets $target_name"

    # Inside your gsw_build.sh PLUGIN POPULATION loop:
    if [ -d "$target_dir/procedures" ]; then
        mkdir -p "targets/$target_name/scripts"
        cp -r "$target_dir/procedures/"* "targets/$target_name/scripts/" 2>/dev/null
        rm -rf "targets/$target_name/procedures"
    fi

    # Move component-specific libraries into the OpenC3 lib/ folder
    if [ -d "$target_dir/lib" ]; then
        # 1. Copy to the global lib/ folder so target.txt REQUIRE statements pass validation
        cp -r "$target_dir/lib/"* "lib/" 2>/dev/null
        
        # 2. ALSO copy to the scripts/ folder so you can see them in the Script Runner GUI
        mkdir -p "targets/$target_name/scripts"
        cp -r "$target_dir/lib/"* "targets/$target_name/scripts/" 2>/dev/null
        
        # Prevent OpenC3 from packaging the legacy directory structure
        rm -rf "targets/$target_name/lib"
    fi
done

# 3. Patch the text dictionaries
echo "Patching target dictionaries..."
for i in $(find targets/ -name '*.txt')
do 
    sed -i -e 's/<%= *CosmosCfsConfig::PROCESSOR_ENDIAN *%>/LITTLE_ENDIAN/g; s/<%= *CF_INCOMING_PDU_MID *%>/0x1800/g; s/<%= *CF_SPACE_TO_GND_PDU_MID *%>/0x0800/g;' "$i"
done

# 4. Patch Python imports and create packages
echo "Patching Python script imports..."
# Create __init__.py files so Python recognizes the directories as importable packages
touch lib/__init__.py 2>/dev/null
for target_dir in targets/*; do
    if [ -d "$target_dir/scripts" ]; then
        touch "$target_dir/scripts/__init__.py" 2>/dev/null
        
        # Inject sys.path.append into every python file so it can find 'sample_lib' automatically
        for pyfile in "$target_dir/scripts/"*.py; do
            if [ -f "$pyfile" ]; then
                sed -i '1i import sys, os\nsys.path.append(os.path.dirname(__file__))\nsys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "lib")))' "$pyfile"
            fi
        done
    fi
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
echo "INTERFACE DEBUG UdpInterface nos-fsw 5012 5013 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" ] && [ "$i" != "SYSTEM" ] && [ "$i" != "TO_DEBUG" ]; then
        debug="${i}_DEBUG"
        echo "  MAP_TARGET $debug" >> plugin.txt
    fi
done
echo "  MAP_TARGET TO_DEBUG" >> plugin.txt
echo "" >> plugin.txt

echo "INTERFACE RADIO UdpInterface cryptolib 6010 6011 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" ] && [ "$i" != "SYSTEM" ] && [ "$i" != "TO_DEBUG" ]; then
        radio="${i}_RADIO"
        echo "  MAP_TARGET $radio" >> plugin.txt
    fi
done
echo "" >> plugin.txt

echo "INTERFACE SIM_42_TRUTH_INT UdpInterface truth42sim 5110 5111 nil nil 128 10.0 nil" >> plugin.txt
echo "  MAP_TARGET SIM_42_TRUTH" >> plugin.txt

echo "" >> plugin.txt
echo "# Created with Build Version: $BUILD_VERSION" >> plugin.txt

# Ensure permissions
chmod -R a+rX .

# ==============================================================================
# BUILD AND DEPLOY
# ==============================================================================
echo "Patching gemspec to forcefully include all files..."

# This entirely replaces the spec.files definition to recursively grab all files, 
# ensuring your 'procedures' and 'scripts' folders are packed into the gem.
sed -i 's/spec.files.*=.*/spec.files = Dir.glob("**\/*").reject { |f| File.directory?(f) }/g' *.gemspec

echo "Build plugin..."
$OPENC3_CLI rake build VERSION=$BUILD_VERSION
if [ ! -f "openc3-cosmos-nos3-${BUILD_VERSION}.gem" ]; then
    echo ""
    echo "ERROR: cli rake build failed!"
    echo ""
    exit 1
fi
echo ""