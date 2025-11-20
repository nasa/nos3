#!/bin/bash -i
#
# Convenience script for NOS# development

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh
echo ""
echo ""

cd $BASE_DIR
$DFLAGS_NOINT -v $BASE_DIR:$BASE_DIR -v ~/.fonts/:/home/jstar/.fonts -v /tmp/.X11-unix:/tmp/.X11-unix:ro -e DISPLAY=$DISPLAY -w $BASE_DIR --name "nos3-igniter" $DBOX python3 $BASE_DIR/cfg/gui/igniter_entrypoint.py &
echo ""
echo ""