#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="ait"

echo "AIT launch..."
cd $BASE_DIR/gsw/ait
gnome-terminal --window-with-profile=KeepOpen --title="AIT" -- $DFLAGS -v $BASE_DIR:$BASE_DIR -v /tmp/nos3:/tmp/nos3 -w $BASE_DIR/gsw/ait --name ait -h ait -p 8001:8001 --network=nos3_core $DBOX "export AIT_CONFIG=$BASE_DIR/gsw/ait ; ait-server"

pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    firefox localhost:80 &
fi
