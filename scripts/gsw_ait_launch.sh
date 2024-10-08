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
gnome-terminal --tab --title="AIT" -- $DFLAGS -v $BASE_DIR:$BASE_DIR -v /tmp/nos3:/tmp/nos3 -w $BASE_DIR/gsw/ait -e AIT_ROOT=$BASE_DIR/gsw/ait -e AIT_CONFIG=$BASE_DIR/gsw/ait/config/config.yaml --name cosmos_openc3-operator_1 -h ait -p 8080:8080 --network=nos3_core $DBOX ait-server

sleep 3

pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    firefox http://localhost:8080 &
fi
