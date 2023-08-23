#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://docs.docker.com/engine/install/ubuntu/
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)

docker run --rm -v $BASE_DIR:$BASE_DIR -w $BASE_DIR -it ivvitc/nos3 make fsw
