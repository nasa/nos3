#!/bin/bash -i
#
# Convenience script for NOS# development

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh
echo ""
echo ""

cd $BASE_DIR
python3 $BASE_DIR/cfg/gui/cfg_gui_main.py &
echo ""
echo ""