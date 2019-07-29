#!/bin/bash
#
# Shell script to provision the NOS^3 64-bit VM with additional custom packages

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_ubuntu_64_CUSTOM.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi
export BOOST_VERSION=1.58
export DEBIAN_FRONTEND=noninteractive

echo "Cleanup..."
    # Reset archive directory
    rm -r /var/cache/apt/archives
    mkdir -p /var/cache/apt/archives/partial
    touch /var/cache/apt/archives/lock
    chmod 640 /var/cache/apt/archives/lock     