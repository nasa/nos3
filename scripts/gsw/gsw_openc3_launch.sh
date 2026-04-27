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
    sleep 15
    
    cd "$OPENC3_DIR/openc3-cosmos-nos3" 2>/dev/null || exit
    LATEST_GEM=$(ls -t openc3-cosmos-nos3-*.gem 2>/dev/null | head -n 1)

    if [ -n "$LATEST_GEM" ]; then
        echo "-> Found plugin: $LATEST_GEM"
        
        MAX_RETRIES=24
        RETRY_COUNT=0
        API_READY=false

        while [ $RETRY_COUNT -lt $MAX_RETRIES ]; do
            STATUS=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:2900/openc3-api/plugins || echo "000")
            
            if [ "$STATUS" != "000" ] && [ "$STATUS" != "502" ] && [ "$STATUS" != "503" ]; then
                API_READY=true
                break
            fi
            sleep 5
            RETRY_COUNT=$((RETRY_COUNT+1))
        done

        if [ "$API_READY" = true ]; then
            echo "-> API is awake! Logging in to generate access token..."
            
            # OpenC3 Open Source expects ONLY the password field in the JSON payload
            LOGIN_RESPONSE=$(curl -s -X POST http://localhost:2900/openc3-api/login \
                -H "Content-Type: application/json" \
                -d '{"password":"jstar123!"}')
            
            API_TOKEN=$(echo "$LOGIN_RESPONSE" | grep -o '"token":"[^"]*"' | cut -d'"' -f4)

            if [ -n "$API_TOKEN" ]; then
                echo "-> Login successful! Uploading gem to DEFAULT scope..."
                
                HTTP_STATUS=$(curl -s -o /dev/null -w "%{http_code}" \
                    -H "Authorization: Bearer $API_TOKEN" \
                    -X POST \
                    -F "file=@$LATEST_GEM" \
                    -F "scope=DEFAULT" \
                    http://localhost:2900/openc3-api/plugins)

                if [ "$HTTP_STATUS" -eq 200 ] || [ "$HTTP_STATUS" -eq 201 ] || [ "$HTTP_STATUS" -eq 302 ]; then
                    echo "-> Auto-upload successful! (HTTP $HTTP_STATUS)"
                else
                    echo "-> Auto-upload failed (HTTP $HTTP_STATUS). Printing API response:"
                    curl -s -H "Authorization: Bearer $API_TOKEN" -X POST -F "file=@$LATEST_GEM" -F "scope=DEFAULT" http://localhost:2900/openc3-api/plugins
                    echo ""
                fi
            else
                echo "-> Web login failed. API Response: $LOGIN_RESPONSE"
                echo "-> Attempting fallback: Loading directly via cmd-tlm-api container..."
                
                # Fallback: Execute CLI inside cmd-tlm-api-1 (which has Write Access to the filesystem)
                docker cp "$LATEST_GEM" openc3-openc3-cosmos-cmd-tlm-api-1:/tmp/"$LATEST_GEM"
                docker exec openc3-openc3-cosmos-cmd-tlm-api-1 openc3cli load /tmp/"$LATEST_GEM" DEFAULT
            fi
        else
            echo "-> Timeout waiting for OpenC3 API."
        fi
    else
        echo "-> No gem file found to upload."
    fi
) &