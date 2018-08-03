#!/bin/bash
#
# Shell script to provision the NOS^3 64-bit VM  on CentOS 7 with additional developer tools

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_centos_64_DEV.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  STF_USER=nos3
else
  STF_USER=$(logname)
fi

DIR=/home/$STF_USER/nos3

echo " "
echo "Developer Tools"
echo " "
    echo "    CFC"
    echo " "
    # CFC doesn't actually need installed - python scripts
    echo " "
    echo "    OIPP"
    echo " "
    yum -y install python-pip 1> /dev/null
    pip install --upgrade pip 1> /dev/null
    pip install pytz pyorbital astropy sgp4 geocoder 1> /dev/null
    cp -R $DIR/support/planning /home/$STF_USER/Desktop
    dos2unix /home/$STF_USER/Desktop/planning/OrbitInviewPowerPrediction/* 1> /dev/null
    chown -R $STF_USER:$STF_USER /home/$STF_USER/Desktop/planning
    chmod -R ugo+x /home/$STF_USER/Desktop/planning
    cd /home/$STF_USER/Desktop/planning
    test -e /home/$STF_USER/Desktop/planning/STF1-TLE.txt || ln -s /home/$STF_USER/Desktop/nos3-42/NOS3-42InOut/STF1-TLE.txt
    cd /home/$STF_USER/Desktop
    test -e /home/$STF_USER/Desktop/stf1-oipp-demo.sh || ln -s /home/$STF_USER/Desktop/planning/OrbitInviewPowerPrediction/stf1-oipp-demo.sh

#echo " "
#echo "Cleanup"
#echo " "
    # Reset archive directory
    #rm -r /var/cache/apt/archives
    #mkdir -p /var/cache/apt/archives/partial
    #touch /var/cache/apt/archives/lock
    #chmod 640 /var/cache/apt/archives/lock     