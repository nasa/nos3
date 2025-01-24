#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh

echo "YAMCS Launch... "
gnome-terminal --tab --title="YAMCS" -- $DFLAGS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -p 8090:8090 -p 5012:5012 --name cosmos_openc3-operator_1 -h cosmos --network=nos3_core --network-alias=cosmos -w $USER_NOS3_DIR/yamcs $DBOX mvn -Dmaven.repo.local=$USER_NOS3_DIR/.m2/repository yamcs:run

pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    sleep 20 && firefox localhost:8090 &
fi
