#!/bin/bash
# fprimecmding.sh - Send REQUEST_HOUSEKEEPING command to F Prime via Docker

# Get script directory and source environment
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh

# Set paths and configuration
CONTAINER_NAME="sc_1_fprime"
FPRIME_PROJECT_PATH="$BASE_DIR/fsw/fprime/fprime-nos3"
COMMAND="sampleSim.REQUEST_HOUSEKEEPING"

# Execute the command in the Docker container
docker exec "$CONTAINER_NAME" bash -c "cd $FPRIME_PROJECT_PATH && fprime-cli command-send $COMMAND"


