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
    echo "Waiting for OpenC3 API to become ready for plugin upload..."

    cd "$OPENC3_DIR/openc3-cosmos-nos3" 2>/dev/null || exit
    LATEST_GEM=$(ls -t openc3-cosmos-nos3-*.gem 2>/dev/null | head -n 1)

    if [ -n "$LATEST_GEM" ]; then
        MAX_RETRIES=20
        RETRY_COUNT=0
        API_READY=false

        while [ $RETRY_COUNT -lt $MAX_RETRIES ]; do
            # Ping the API to see if it is awake
            STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:2900/openc3-api/plugins || echo "000")
            
            # If we get 401 (Unauthorized) or 200 (OK), the webserver is online
            if [ "$STATUS" != "000" ] && [ "$STATUS" != "502" ] && [ "$STATUS" != "503" ]; then
                API_READY=true
                break
            fi
            
            sleep 5
            RETRY_COUNT=$((RETRY_COUNT+1))
        done

        if [ "$API_READY" = true ]; then
            echo "-> API is awake! Extracting token from Docker..."
            
            # Reach into the running Operator container and grab the actual active token
            API_TOKEN=$(docker exec $GSW env 2>/dev/null | grep OPENC3_API_TOKEN | cut -d '=' -f 2 | tr -d '\r' | tr -d '"' | tr -d "'")

            if [ -n "$API_TOKEN" ]; then
                echo "-> Token acquired! Uploading $LATEST_GEM..."
                HTTP_STATUS=$(curl -s -o /dev/null -w "%{http_code}" \
                    -H "Authorization: Bearer $API_TOKEN" \
                    -X POST -F "file=@$LATEST_GEM" \
                    http://localhost:2900/openc3-api/plugins)

                if [ "$HTTP_STATUS" -eq 200 ] || [ "$HTTP_STATUS" -eq 201 ] || [ "$HTTP_STATUS" -eq 302 ]; then
                    echo "-> Auto-upload successful! (HTTP $HTTP_STATUS)"
                else
                    echo "-> Auto-upload failed (HTTP $HTTP_STATUS)."
                fi
            else
                echo "-> Failed to extract token from container $GSW."
                echo "-> Are you sure the container is running and has OPENC3_API_TOKEN set?"
            fi
        else
            echo "-> Timeout waiting for OpenC3 API."
        fi
    fi
) &