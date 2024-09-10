#!/bin/bash -i
#
# Convenience script for NOS3 development
#

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh
export GSW="ait"

echo "AIT launch..."
gnome-terminal --tab --title="AIT" -- docker run --rm -it -v $BASE_DIR:$BASE_DIR -v /tmp/nos3:/tmp/nos3 --name ait -h ait -p 8001:8001 --network=nos3_core ghcr.io/sphinxdefense/gsw-ait:main "source ~/.bashrc && ait-server"
