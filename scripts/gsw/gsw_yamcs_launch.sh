#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh

# if $YAMCS_BINDING_HOST has a value then use the given value, else use 0.0.0.0
YAMCS_BINDING_HOST=${YAMCS_BINDING_HOST}
BINDING_HOST=${YAMCS_BINDING_HOST:-"0.0.0.0"}

gnome-terminal --tab --title="YAMCS" -- $DFLAGS  -e COMPONENT_DIR=$COMPONENT_DIR -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -p ${BINDING_HOST}:8090:8090 -p 5012:5012 --name cosmos-openc3-operator-1 -h cosmos --network=nos3-core --network-alias=cosmos -w $USER_NOS3_DIR/yamcs $DBOX mvn ${MAVEN_HTTPS_PROXY} -Dmaven.repo.local=$USER_NOS3_DIR/.m2/repository -DCOMPONENT_DIR=$COMPONENT_DIR yamcs:run

pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    curl -sf --retry 30 --retry-delay 1 --retry-all-errors --retry-connrefused \
    http://localhost:8090/ >/dev/null && firefox http://localhost:8090/ &
fi
