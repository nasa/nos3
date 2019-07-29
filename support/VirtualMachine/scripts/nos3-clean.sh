#!/bin/bash
#
# Convenience script for NOS3 simulator development
#

xterm -hold -T "NOS3 Clean" -rv -e '
# Update the following path for shared folders if in use
DIR=/home/nos3/nos3
cd $DIR

rm -r ~/Desktop/nos3-build

echo "Done!"
'
