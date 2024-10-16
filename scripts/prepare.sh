#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh
echo ""
echo ""

echo "Create local user directory..."
mkdir $USER_NOS3_DIR 2> /dev/null
echo "  "$USER_NOS3_DIR
mkdir $USER_NOS3_DIR/42 2> /dev/null
echo ""
echo ""

echo "Prepare nos3 docker container..."
$DCALL image pull $DBOX
echo ""
echo ""

echo "Prepare 42..."
cd $USER_NOS3_DIR
git clone https://github.com/nasa-itc/42.git --depth 1 -b nos3-main
cd $USER_NOS3_DIR/42
$DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -w $USER_NOS3_DIR/42 --name "nos3_42_build" $DBOX make
echo ""
echo ""

echo "NOS3 required preparations complete!"
echo "Proceeding to optional additions."
echo ""
echo ""

echo "Prepare Igniter (optional)..."
pip3 install pyside6 xmltodict
cd $BASE_DIR
python3 $BASE_DIR/cfg/gui/cfg_gui_main.py &
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
