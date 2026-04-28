#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="openc3-openc3-operator-1"

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#exit

#echo "Make /tmp folders..."
#mkdir /tmp/data 2> /dev/null
#mkdir /tmp/data/hk 2> /dev/null
#mkdir /tmp/uplink 2> /dev/null

echo "Prepare openc3 containers..."
cd $OPENC3_DIR
$OPENC3_PATH run
echo ""

echo "OpenC3 launch..."
pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    firefox localhost:2900 &
fi

# ==============================================================================
# OPENC3 PLUGIN AUTO-UPLOAD (Asynchronous)
# ==============================================================================
(
    echo "Waiting 15 seconds for OpenC3 to initialize..."
    sleep 15
    
    cd "$OPENC3_DIR/openc3-cosmos-nos3" 2>/dev/null || exit
    LATEST_GEM=$(ls -t openc3-cosmos-nos3-*.gem 2>/dev/null | head -n 1)

    if [ -n "$LATEST_GEM" ]; then
        echo "-> Found plugin: $LATEST_GEM"
        echo "-> Loading directly via OpenC3 cmd-tlm-api container..."
        
        # 1. Copy the gem into the backend API container
        docker cp "$LATEST_GEM" openc3-openc3-cosmos-cmd-tlm-api-1:/tmp/"$LATEST_GEM"
        
        # 2. Execute the CLI load command targeting the DEFAULT scope
        docker exec openc3-openc3-cosmos-cmd-tlm-api-1 openc3cli load /tmp/"$LATEST_GEM" DEFAULT
        
        if [ $? -eq 0 ]; then
            echo "-> Auto-upload successful!"
        else
            echo "-> Auto-upload failed."
        fi
    else
        echo "-> No gem file found to upload."
    fi
) &