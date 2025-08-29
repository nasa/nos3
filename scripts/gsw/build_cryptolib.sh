#!/bin/bash -i
#
# Convenience script for NOS3 development
# Use with the Dockerfile in the deployment repository
# https://github.com/nasa-itc/deployment
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/../env.sh

GROUND_SOFTWARE=$(sed -n 's:.*<gsw>\(.*\)</gsw>.*:\1:p' ./cfg/nos3-mission.xml)

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

# Make ground software build directory
mkdir -p $BASE_DIR/gsw/build

# Build
$DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -e LD_LIBRARY_PATH="/usr/local/lib" -e GROUND_SOFTWARE=$GROUND_SOFTWARE -e CRYPTO_RX_GROUND_PORT=$CRYPTO_RX_GROUND_PORT -e CRYPTO_TX_GROUND_PORT=$CRYPTO_TX_GROUND_PORT -e CRYPTO_TX_RADIO_PORT=$CRYPTO_TX_RADIO_PORT -e CRYPTO_RX_RADIO_PORT=$CRYPTO_RX_RADIO_PORT --name "nos_build_cryptolib" -w $BASE_DIR $DBOX make -j$NUM_CPUS build-cryptolib
