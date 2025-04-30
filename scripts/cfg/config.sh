#!/bin/bash -i

# Convenience script for NOS3 development

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source "$SCRIPT_DIR/../env.sh"

ORIGINAL_CONFIG="$BASE_DIR/cfg/nos3-mission.xml"
CONFIG_FILE="$ORIGINAL_CONFIG"

# Make flight software configuration directory
mkdir -p "$BASE_DIR/cfg/build/temp_mission/"
cp -r "$BASE_DIR/cfg/nos3-mission.xml" "$BASE_DIR/cfg/build/temp_mission/"

# Copy baseline configurations into build directory
cp -r "$BASE_DIR/cfg/InOut" "$BASE_DIR/cfg/build/"
cp -r "$BASE_DIR/cfg/nos3_defs" "$BASE_DIR/cfg/build/"
cp -r "$BASE_DIR/cfg/sims" "$BASE_DIR/cfg/build/"

# If SC1_CFG is passed in, validate and patch
if [ -n "${SC1_CFG// }" ]; then
    REL_SC1_CFG="${SC1_CFG#cfg/}"   # Strip leading cfg/ if present
    FULL_SC1_CFG="$BASE_DIR/cfg/$REL_SC1_CFG"

    if [ ! -f "$FULL_SC1_CFG" ]; then
        echo "ERROR: Config file '$FULL_SC1_CFG' does not exist."
        exit 1
    fi

    echo "Overriding <sc-1-cfg> with: $REL_SC1_CFG"
    TEMP_CONFIG=$(mktemp "$BASE_DIR/cfg/build/temp_mission/XXXXXX.xml")
    sed "s|<sc-1-cfg>.*</sc-1-cfg>|<sc-1-cfg>$REL_SC1_CFG</sc-1-cfg>|" "$ORIGINAL_CONFIG" > "$TEMP_CONFIG"
    CONFIG_FILE="$TEMP_CONFIG"

    echo "$CONFIG_FILE" > "$BASE_DIR/cfg/build/current_config_path.txt"
else
    echo "$ORIGINAL_CONFIG" > "$BASE_DIR/cfg/build/current_config_path.txt"
fi

# Run the configuration Python script
python3 "$SCRIPT_DIR/cfg/configure.py" "$CONFIG_FILE"
