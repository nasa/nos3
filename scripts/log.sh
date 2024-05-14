#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
DATE=$(date "+%Y%m%d_%H%M")
CAP_NAME=$DATE'_console.txt'

echo "Move minicom.cap for archival..."
mv $BASE_DIR/minicom.cap $BASE_DIR/gsw/cosmos/outputs/logs/$CAP_NAME 2> /dev/null

#echo "/tmp/data for archival..."
#mv /tmp/data $BASE_DIR/gsw/cosmos/outputs/logs/data 2> /dev/null

echo "Processing L0 CSVs..."
cd $SCRIPT_DIR
./l0.sh

echo "Tar COSMOS files..."
cd $BASE_DIR/gsw/cosmos/outputs/logs
tar -czvf $BASE_DIR/$DATE.tar.gz 20* data && yes | rm $BASE_DIR/gsw/cosmos/outputs/logs/20* && yes | rm -r $BASE_DIR/gsw/cosmos/outputs/logs/data
