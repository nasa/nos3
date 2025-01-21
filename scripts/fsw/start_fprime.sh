#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh


cd $BASE_DIR/fsw/fprime/fprime-nos3
. fprime-venv/bin/activate
# fprime-gds #http://127.0.0.1:5000
fprime-gds --gui-port 5000 --gui-addr 0.0.0.0
# cd fsw/fprime/fprme-nos3