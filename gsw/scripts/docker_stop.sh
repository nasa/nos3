#!/bin/bash
#
# Convenience script for NOS3 development
# https://docs.docker.com/engine/install/ubuntu/
#

export SCRIPT_DIR=$(cd `dirname $0` && pwd)
export BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
export FSW_BIN=$BASE_DIR/fsw/build/exe/cpu1
export SIM_DIR=$BASE_DIR/sims/build
export SIM_BIN=$SIM_DIR/bin
export SIMS=$(cd $SIM_BIN; ls nos3*simulator)

# NOS3

# NOS3 GPIO
rm -rf /tmp/gpio_fake

# NOS3 Stored HK
rm -rf $BASE_DIR/fsw/build/exe/cpu1/scratch/*

# Docker
cd $SCRIPT_DIR; docker compose down
for i in $(docker container ls -q); do
    docker container kill $i
done
docker container prune -f

# 42
rm -rf /opt/nos3/42/NOS3InOut
rm -rf /tmp/gpio*

# cFS

# COSMOS
yes | rm $BASE_DIR/gsw/cosmos/Gemfile 2> /dev/null
yes | rm $BASE_DIR/gsw/cosmos/Gemfile.lock 2> /dev/null
yes | rm -r $BASE_DIR/gsw/cosmos/COMPONENTS 2> /dev/null

exit 0