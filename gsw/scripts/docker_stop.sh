#!/bin/bash
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$(cd `dirname $0` && pwd)
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)

# cFS
killall -q -r -9 core-cpu*

# COSMOS
killall -q -9 ruby
yes | rm $BASE_DIR/gsw/cosmos/Gemfile 2> /dev/null
yes | rm $BASE_DIR/gsw/cosmos/Gemfile.lock 2> /dev/null
yes | rm -r $BASE_DIR/gsw/cosmos/COMPONENTS 2> /dev/null

# NOS3

# NOS3 GPIO
rm -rf /tmp/gpio_fake

# NOS3 Stored HK
rm -rf $BASE_DIR/fsw/build/exe/cpu1/scratch/*

# 42
killall -q 42
rm -rf /opt/nos3/42/NOS3InOut
rm -rf /tmp/gpio*

# Docker
for i in $(docker container ls -q); do
    docker container kill $i
done
docker container prune -f

exit 0