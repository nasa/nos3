#!/bin/bash
#
# Convenience script for NOS3 development
# https://docs.docker.com/engine/install/ubuntu/
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
GSW_DIR=$BASE_DIR/gsw/cosmos
if [ -f "/etc/redhat-release" ]; then
    # https://github.com/containers/podman/issues/14284#issuecomment-1130113553
    # sudo sed -i 's/runtime = "runc"/runtime = "crun" # "runc"/g' /usr/share/containers/containers.conf 
    DFLAG="sudo docker"
else
    DFLAG="docker"
fi
SIM_DIR=$BASE_DIR/sims/build
SIM_BIN=$SIM_DIR/bin
SIMS=$(cd $SIM_BIN; ls nos3*simulator)

# NOS3 GPIO
rm -rf /tmp/gpio_fake

# NOS3 Stored HK
rm -rf $BASE_DIR/fsw/build/exe/cpu1/scratch/*

# Docker
cd $SCRIPT_DIR; $DFLAG compose down > /dev/null 2>&1
#for i in $($DFLAG container ls -q); do
#    if [ "$i" == "openc3"* ]
#    then
#        $DFLAG container kill $i > /dev/null 2>&1
#    fi
#done
docker ps --filter=name="sc_*" -aq | xargs docker stop > /dev/null 2>&1
docker ps --filter=name="nos_*" -aq | xargs docker stop > /dev/null 2>&1

$DFLAG container prune -f > /dev/null 2>&1
$DFLAG network rm NOS3_GC > /dev/null 2>&1
$DFLAG network rm sc_1_satnet > /dev/null 2>&1
$DFLAG network rm sc_2_satnet > /dev/null 2>&1
$DFLAG network rm sc_3_satnet > /dev/null 2>&1
$DFLAG network rm sc_21_splitnet > /dev/null 2>&1
$DFLAG network rm sc_32_splitnet > /dev/null 2>&1

# 42
rm -rf /opt/nos3/42/NOS3InOut
rm -rf /tmp/gpio*

# COSMOS
yes | rm $GSW_DIR/Gemfile > /dev/null 2>&1
yes | rm $GSW_DIR/Gemfile.lock > /dev/null 2>&1
yes | rm -r $GSW_DIR/COMPONENTS > /dev/null 2>&1

exit 0
