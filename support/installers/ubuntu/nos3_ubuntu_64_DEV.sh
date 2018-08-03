#!/bin/bash
#
# Shell script to provision the NOS^3 64-bit VM with additional developer tools

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_64_DEV.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

DIR=/home/$NOS3_USER/nos3
export BOOST_VERSION=1.58

echo " "
echo "Developer Tools"
echo " "
    echo "    CFC"
    echo " "
    # CFC doesn't actually need installed - python scripts
    echo " "
    echo "    OIPP"
    echo " "
    apt-get install -y python-pip 1> /dev/null
    pip install pytz pyorbital astropy sgp4 geocoder 1> /dev/null
    cp -R $DIR/support/planning /home/$NOS3_USER/Desktop
    dos2unix /home/$NOS3_USER/Desktop/planning/OrbitInviewPowerPrediction/* 1> /dev/null
    chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/planning
    chmod -R ugo+x /home/$NOS3_USER/Desktop/planning
    cd /home/$NOS3_USER/Desktop/planning
    test -e /home/$NOS3_USER/Desktop/planning/STF1-TLE.txt || ln -s /home/$NOS3_USER/Desktop/nos3-42/NOS3-42InOut/STF1-TLE.txt
    cd /home/$NOS3_USER/Desktop
    test -e /home/$NOS3_USER/Desktop/stf1-oipp-demo.sh || ln -s /home/$NOS3_USER/Desktop/planning/OrbitInviewPowerPrediction/stf1-oipp-demo.sh

echo " "
echo "Cleanup"
echo " "
    apt-get -y autoremove 1> /dev/null
    # Reset archive directory
    rm -r /var/cache/apt/archives
    mkdir -p /var/cache/apt/archives/partial
    touch /var/cache/apt/archives/lock
    chmod 640 /var/cache/apt/archives/lock     