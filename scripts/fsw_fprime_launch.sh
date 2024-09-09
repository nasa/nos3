#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

cd fsw/fprime/fprime-nos3/
. fprime-venv/bin/activate
# fprime-util build
fprime-gds --gui-port 5000 --gui-addr 0.0.0.0