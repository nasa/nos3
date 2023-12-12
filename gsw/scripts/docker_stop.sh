#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

# NOS3 GPIO
rm -rf /tmp/gpio_fake

# NOS3 Stored HK
rm -rf $BASE_DIR/fsw/build/exe/cpu1/scratch/*

# Docker stop
cd $SCRIPT_DIR; $DFLAG compose down > /dev/null 2>&1
$DCALL ps --filter=name="sc_*" -aq | xargs $DCALL stop > /dev/null 2>&1 &
$DCALL ps --filter=name="nos_*" -aq | xargs $DCALL stop > /dev/null 2>&1 &

# Intentionally wait to complete
wait 

# Docker cleanup
$DCALL container prune -f > /dev/null 2>&1
$DNETWORK ls --filter=name="nos" | xargs $DNETWORK rm > /dev/null 2>&1

# 42
rm -rf $USER_NOS3_DIR/42/NOS3InOut
rm -rf /tmp/gpio*

# COSMOS
yes | rm $GSW_DIR/Gemfile > /dev/null 2>&1
yes | rm $GSW_DIR/Gemfile.lock > /dev/null 2>&1
yes | rm -r $GSW_DIR/COMPONENTS > /dev/null 2>&1

exit 0
