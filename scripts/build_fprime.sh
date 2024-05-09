#!/bin/bash
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

cd fsw/fprime/fprime-nos3
. fprime-venv/bin/activate
fprime-util build

