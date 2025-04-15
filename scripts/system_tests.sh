#!/bin/bash 
set -e

# Sleep for a moment to sensure the system is up and running
sleep 10

# Set of System Tests to be called

#Sample
docker exec -it cosmos_openc3-operator_1 ruby $SYSTEM_TEST_FILE_PATH