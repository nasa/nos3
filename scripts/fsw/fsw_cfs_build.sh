#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

# Note this is copied to ./cfg/build as part of `make config`
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../../scripts/env.sh

# Check that local NOS3 directory exists
if [ ! -d $USER_NOS3_DIR ]; then
    echo ""
    echo "    Need to run make prep first!"
    echo ""
    exit 1
fi

# Check that configure build directory exists
if [ ! -d $BASE_DIR/cfg/build ]; then
    echo ""
    echo "    Need to run make config first!"
    echo ""
    exit 1
fi

# Make flight software build directory
mkdir -p $BASE_DIR/fsw/build

# SpaceCOP needs the OpenSSL development files, which the stock NOS3 image
# does not carry. Layer them onto whatever image env.sh selected. Docker
# caches the layers, so this is a no-op after the first run. Remove this
# block once libssl-dev is available in the upstream deployment image.
if [ -z "$NOS3_SKIP_OPENSSL_LAYER" ]; then
    echo "Ensuring build image carries the OpenSSL development files..."
    if ! $DCALL build -q -t nos3-openssl:local --build-arg BASE_IMAGE="$DBOX" -f "$BASE_DIR/support/Dockerfile.openssl" "$BASE_DIR/support" > /dev/null; then
        echo ""
        echo "    Failed to build the OpenSSL build image."
        echo "    Set NOS3_SKIP_OPENSSL_LAYER=1 to build without it."
        echo ""
        exit 1
    fi
    DBOX="nos3-openssl:local"
fi

# Build
$DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR --name "nos_build_fsw" -w $BASE_DIR $DBOX make -j$NUM_CPUS -e FLIGHT_SOFTWARE=cfs build-fsw
