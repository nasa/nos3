#!/bin/bash
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

echo "GSW = FPRIMEGDS, Built into FSW"
#creating dummy container for 42 truth sim, since gds is included in fprime.
docker run -dit --network nos3-core --name cosmos-openc3-operator-1 --network-alias cosmos alpine tail -f /dev/null

