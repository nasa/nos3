#!/bin/bash -i
#
# Convenience script for NOS3 development
#
prepare_fail()
{
    echo "ERROR: NOS3 preparation failed: $*" >&2
    exit 1
}

# env.sh sets SCRIPT_DIR itself. Resolve its location once, before sourcing it.
PREPARE_ENV=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )/.." && pwd )/env.sh
if [ ! -f "$PREPARE_ENV" ] || [ ! -r "$PREPARE_ENV" ]; then
    prepare_fail "environment file is missing or unreadable: $PREPARE_ENV"
fi
source "$PREPARE_ENV" || prepare_fail "could not load environment: $PREPARE_ENV"
for required_variable in BASE_DIR USER_NOS3_DIR DCALL DFLAGS_CPUS DBOX; do
    [ -n "${!required_variable}" ] || prepare_fail "environment did not define $required_variable"
done
echo ""
echo ""

echo "Create local user directory..."
mkdir -p -- "$USER_NOS3_DIR" || prepare_fail "could not create $USER_NOS3_DIR"
echo "  $USER_NOS3_DIR"
echo ""
echo ""

echo "Prepare nos3 docker container..."
$DCALL image pull "$DBOX" || prepare_fail "could not pull the NOS3 container image"
echo ""
echo ""

echo "Prepare 42..."
# Re-running preparation rebuilds the existing checkout without replacing it.
if [ ! -d "$USER_NOS3_DIR/42/.git" ] && [ ! -f "$USER_NOS3_DIR/42/.git" ]; then
    git clone https://github.com/nasa-itc/42.git --depth 1 -b dev_20260403 "$USER_NOS3_DIR/42" ||
        prepare_fail "could not clone 42"
fi
cd -- "$USER_NOS3_DIR/42" || prepare_fail "could not enter the 42 source directory"
$DFLAGS_CPUS -v "$BASE_DIR:$BASE_DIR" -v "$USER_NOS3_DIR:$USER_NOS3_DIR" -w "$USER_NOS3_DIR/42" --name "nos3_42_build" "$DBOX" make ||
    prepare_fail "could not build 42"
echo ""
echo ""

echo "NOS3 required preparations complete!"
echo "Proceeding to optional additions."
echo ""
echo ""

echo "Preparing Shared Folders for YAMCS..."
mkdir -p -- "$USER_YAMCS_PATH" || echo "WARNING: optional YAMCS directory preparation failed" >&2
echo ""
echo ""

echo "Preparing Shared Folders for Fprime..."
mkdir -p -- "$USER_FPRIME_PATH" || echo "WARNING: optional Fprime directory preparation failed" >&2
echo ""
echo ""

echo "Prepare Igniter (optional)..."
pip3 install pyside6 xmltodict || echo "WARNING: optional Igniter dependencies could not be installed" >&2
if cd -- "$BASE_DIR"; then
    python3 "$BASE_DIR/cfg/gui/cfg_gui_main.py" &
else
    echo "WARNING: could not enter the source directory for optional Igniter launch" >&2
fi
echo ""
echo ""

sleep 3
echo ""
echo ""

echo "NOS3 prep script complete:"
echo "  Some above optional installations may have failed, that's ok. You just may not have those extra features."
echo "  You can choose to use the Igniter GUI or close it in favor of the command line."
echo "  To launch igniter again simply run the following in the terminal:"
echo "    make igniter"
echo ""
echo ""
