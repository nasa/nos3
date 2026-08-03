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
    # git clone https://github.com/nasa-itc/openc3-nos3.git -b 839-openc3-cfdp "$USER_NOS3_DIR/openc3"
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

# Ensure send_files/received_files volumes are mounted for CFDP (not part of upstream openc3-nos3)
if ! grep -q "send_files:/send_files" "$OPENC3_DIR/compose.yaml"; then
  sed -i '/\.\/cacert\.pem:\/devel\/cacert\.pem:z/a\      - "./send_files:/send_files"\n      - "./received_files:/received_files"' "$OPENC3_DIR/compose.yaml"
  mkdir -p "$OPENC3_DIR/send_files" "$OPENC3_DIR/received_files"
  echo "Patched compose.yaml with CFDP send_files/received_files volumes"
fi

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

# 1. Grab global base files for the Ruby side
cp -r $GSW_DIR/lib/*.rb lib/ 2>/dev/null
cp -r $SCRIPT_DIR/*.rb scripts/ 2>/dev/null
cp -r $SCRIPT_DIR/*.py scripts/ 2>/dev/null
mkdir -p microservices
cp -r $SCRIPT_DIR/../gsw/openc3/microservices/CFDP microservices/

targets=""

for target_txt in $(find $BASE_DIR/components $GSW_DIR/config/targets -name target.txt 2>/dev/null) 
do 
    target_dir=$(dirname "$target_txt")
    target_name=$(basename "$target_dir")
    
    echo "Processing target: $target_name"
    cp -r "$target_dir" targets/
    targets="$targets $target_name"

    # --- Create the local Python package inside the target! ---
    mkdir -p "targets/$target_name/scripts/nos3"
    touch "targets/$target_name/scripts/nos3/__init__.py"
    
    # Copy all global Python libraries into this local package
    cp -r $GSW_DIR/lib/*.py "targets/$target_name/scripts/nos3/" 2>/dev/null
    # ----------------------------------------------------------

    if [ -d "$target_dir/procedures" ]; then
        # Copy runnable scripts to the root of the scripts folder
        cp "$target_dir/procedures/"*.* "targets/$target_name/scripts/" 2>/dev/null
        
        # Copy Python helpers to the local nos3 package
        cp "$target_dir/procedures/"*.py "targets/$target_name/scripts/nos3/" 2>/dev/null
        
        if [ -d "$target_dir/procedures/tests" ]; then
            cp "$target_dir/procedures/tests/"*.* "targets/$target_name/scripts/" 2>/dev/null
            cp "$target_dir/procedures/tests/"*.py "targets/$target_name/scripts/nos3/" 2>/dev/null
        fi
        rm -rf "targets/$target_name/procedures"
    fi

    if [ -d "$target_dir/lib" ]; then
        # Copy Ruby libs globally
        cp -r "$target_dir/lib/"*.rb "lib/" 2>/dev/null
        # Copy Python component libs into the local nos3 package
        cp -r "$target_dir/lib/"*.py "targets/$target_name/scripts/nos3/" 2>/dev/null
        rm -rf "targets/$target_name/lib"
    fi
done
rm -rf targets/CFDP
cp -r $SCRIPT_DIR/../gsw/openc3/targets/CFDP targets/

# Copy Sim Bridge commands into new target
echo "Populating SIM_CMDBUS_BRIDGE with component dictionaries..."
mkdir -p targets/SIM_CMDBUS_BRIDGE/cmd_tlm
for i in $(find $BASE_DIR/components/ -name "gsw" -type d 2>/dev/null)
do
    cp $i/*.txt targets/SIM_CMDBUS_BRIDGE/cmd_tlm/ 2> /dev/null
done

echo "Patching target dictionaries..."
for i in $(find targets/ -name '*.txt')
do 
    sed -i -e 's/<%= *CosmosCfsConfig::PROCESSOR_ENDIAN *%>/LITTLE_ENDIAN/g; s/<%= *CF_INCOMING_PDU_MID *%>/0x1800/g; s/<%= *CF_SPACE_TO_GND_PDU_MID *%>/0x0800/g;' "$i"
done

# ==============================================================================
# INTERFACE MAPPING (plugin.txt)
# ==============================================================================
echo "Create plugin.txt..."
rm -f plugin.txt

# 1. Target Declarations
for i in $targets
do
    if [ "$i" != "SIM_42_TRUTH" ] && [ "$i" != "SYSTEM" ] && [ "$i" != "TO_DEBUG" ] && [ "$i" != "SIM_CMDBUS_BRIDGE" ]; then
        echo "TARGET $i ${i}_DEBUG" >> plugin.txt
        echo "TARGET $i ${i}_RADIO" >> plugin.txt
    else
        echo "TARGET $i $i" >> plugin.txt
    fi
done
echo "" >> plugin.txt

# 2. DEBUG Interface (For standard spacecraft targets)
echo "INTERFACE DEBUG UdpInterface nos-fsw 5012 5013 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets; do
    if [ "$i" != "SIM_42_TRUTH" ] && [ "$i" != "SYSTEM" ] && [ "$i" != "TO_DEBUG" ] && [ "$i" != "SIM_CMDBUS_BRIDGE" ]; then
        echo "  MAP_TARGET ${i}_DEBUG" >> plugin.txt
    fi
done
echo "" >> plugin.txt

# 3. RADIO Interface (For standard spacecraft targets)
echo "INTERFACE RADIO UdpInterface cryptolib 6010 6011 nil nil 128 10.0 nil" >> plugin.txt
for i in $targets; do
    if [ "$i" != "SIM_42_TRUTH" ] && [ "$i" != "SYSTEM" ] && [ "$i" != "TO_DEBUG" ] && [ "$i" != "SIM_CMDBUS_BRIDGE" ]; then
        echo "  MAP_TARGET ${i}_RADIO" >> plugin.txt
    fi
done
echo "" >> plugin.txt

# 4. 42 Truth Interface
echo "INTERFACE SIM_42_TRUTH_INT UdpInterface nil nil 5111 nil nil" >> plugin.txt
echo "  OPTION BIND_ADDRESS 0.0.0.0" >> plugin.txt
echo "  MAP_TARGET SIM_42_TRUTH" >> plugin.txt
echo "" >> plugin.txt

# 5. SIM_CMDBUS_BRIDGE Interface
echo "INTERFACE SIM_BRIDGE_INT TcpipClientInterface nos-sim-bridge 12020 12020 10.0 nil" >> plugin.txt
echo "  PROTOCOL READ_WRITE TemplateProtocol 0x0A 0x0A" >> plugin.txt
echo "  MAP_TARGET SIM_CMDBUS_BRIDGE" >> plugin.txt
echo "# Created with Build Version: $BUILD_VERSION" >> plugin.txt

# 6. CFDP Microservice (debug + radio)
echo "MICROSERVICE CFDP cfdp-debug-microservice" >> plugin.txt
echo "  CMD python cfdp.py" >> plugin.txt
echo "  ENV CFDP_TARGET_NAME CFDP_DEBUG" >> plugin.txt
echo "" >> plugin.txt

echo "MICROSERVICE CFDP cfdp-radio-microservice" >> plugin.txt
echo "  CMD python cfdp.py" >> plugin.txt
echo "  ENV CFDP_TARGET_NAME CFDP_RADIO" >> plugin.txt
echo "" >> plugin.txt

chmod -R a+rX .

# ==============================================================================
# BUILD AND DEPLOY
# ==============================================================================
echo "Patching gemspec to forcefully include all files..."
sed -i 's/s\.files.*=.*/s.files = Dir.glob("**\/*").reject { |f| File.directory?(f) }/g' *.gemspec
sed -i 's/spec\.files.*=.*/spec.files = Dir.glob("**\/*").reject { |f| File.directory?(f) }/g' *.gemspec

echo "Build plugin..."
$OPENC3_CLI rake build VERSION=$BUILD_VERSION
if [ ! -f "openc3-cosmos-nos3-${BUILD_VERSION}.gem" ]; then
    echo ""
    echo "ERROR: cli rake build failed!"
    echo ""
    exit 1
fi

# Commented out debug Verification
# echo "--- GEM VERIFICATION ---"
# tar -xf "openc3-cosmos-nos3-${BUILD_VERSION}.gem" data.tar.gz
# echo "Contents of Target SAMPLE/scripts/nos3/ directory:"
# tar -tvf data.tar.gz | grep "targets/SAMPLE/scripts/nos3/"
# rm -f data.tar.gz 
# echo "------------------------"
# echo ""
