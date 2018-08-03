#!/bin/bash
#
# Shell script to provision the NOS^3 32-bit VM for developers
#
# Note that the commands in this script should be able to be run more than once with no ill effects, since
# this script could be run multiple times to provision/reprovision a machine.  If they cannot, they should be fixed!
#

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_pi_32_DEV.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  STF_USER=nos3
else
  STF_USER=$(echo "$USER")
fi

DIR=/home/$STF_USER/nos3
export BOOST_VERSION=1.55

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
    cp -R $DIR/support/planning /home/$STF_USER/Desktop
    dos2unix /home/$STF_USER/Desktop/planning/OrbitInviewPowerPrediction/* 1> /dev/null
    chown -R $STF_USER:$STF_USER /home/$STF_USER/Desktop/planning
    chmod -R ugo+x /home/$STF_USER/Desktop/planning
    cd /home/$STF_USER/Desktop/planning
    test -e /home/$STF_USER/Desktop/planning/STF1-TLE.txt || ln -s /home/$STF_USER/Desktop/nos3-42/NOS3-42InOut/STF1-TLE.txt
    cd /home/$STF_USER/Desktop
    test -e /home/$STF_USER/Desktop/stf1-oipp-demo.sh || ln -s /home/$STF_USER/Desktop/planning/OrbitInviewPowerPrediction/stf1-oipp-demo.sh

echo " "
echo "Cleanup"
echo " "
    apt-get -y autoremove 1> /dev/null
    # Reset archive directory
    rm -r /var/cache/apt/archives
    mkdir -p /var/cache/apt/archives/partial
    touch /var/cache/apt/archives/lock
    chmod 640 /var/cache/apt/archives/lock   