#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="openc3-openc3-operator-1"

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
    echo "Waiting for OpenC3 API to initialize..."
    until docker exec openc3-openc3-cosmos-cmd-tlm-api-1 curl -s http://localhost:2901/ > /dev/null; do
        sleep 2
    done
    
    echo "API is up! Waiting 30 seconds for MinIO buckets and scope to initialize..."
    sleep 30
    
    cd "$OPENC3_DIR/openc3-cosmos-nos3" 2>/dev/null || exit
    LATEST_GEM=$(ls -t openc3-cosmos-nos3-*.gem 2>/dev/null | head -n 1)

    if [ -n "$LATEST_GEM" ]; then
        echo "Found local plugin: $LATEST_GEM"
        
        API_RESPONSE=$(docker exec openc3-openc3-cosmos-cmd-tlm-api-1 curl -s http://localhost:2901/api/plugins)
        
        if echo "$API_RESPONSE" | grep -q "$LATEST_GEM"; then
            echo "Plugin is already installed in the database! Skipping upload."
        else
            echo "Loading directly via OpenC3 cmd-tlm-api container..."
            docker cp "$LATEST_GEM" openc3-openc3-cosmos-cmd-tlm-api-1:/tmp/"$LATEST_GEM" 
            docker exec openc3-openc3-cosmos-cmd-tlm-api-1 openc3cli load /tmp/"$LATEST_GEM" DEFAULT
            
            if [ $? -eq 0 ]; then
                echo "Auto-upload successful!" 
            else
                echo "Auto-upload failed."
            fi
        fi
    else
        echo "-> No gem file found to upload."
    fi
) > /tmp/nos3/openc3_upload.log 2>&1 &