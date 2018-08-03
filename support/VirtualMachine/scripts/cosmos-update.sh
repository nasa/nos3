#!/bin/sh
#
# Convenience script for NOS3
#

ICONIC='-iconic'

[ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

cd ~/Desktop/
rm -r cosmos
cosmos install cosmos
cp -R ~/nos3/support/cosmos/* ~/Desktop/cosmos/

chown -R nos3:nos3 cosmos
