#!/bin/bash
#
# Script to start FSW and restart it if it dies/is killed
#

python3.10 -m pip install setuptools wheel coverage numpy pytest pytest-mock pytest-randomly redis

# sleep 30

python3.10 driver.py