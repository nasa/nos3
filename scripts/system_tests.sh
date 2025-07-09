#!/bin/bash 
set -e

# Sleep for a moment to sensure the system is up and running
sleep 10

# Set of System Tests to be called

#Sample
docker exec -it cosmos-openc3-operator-1 ruby $SYSTEM_TEST_FILE_PATH
