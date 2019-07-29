#!/bin/bash
#
# Shell script to fully provision the NOS^3 64-bit VM on Ubuntu 18.04

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_ubuntu_64_COSMOS.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi
export DEBIAN_FRONTEND=noninteractive

# Based upon the following COSMOS script:
# https://github.com/BallAerospace/COSMOS/blob/v4.3.0/vendor/installers/linux_mac/INSTALL_COSMOS.sh

echo "Dependencies..."
apt-get -y install curl gcc g++ libssl-dev libyaml-dev libffi-dev libreadline6-dev zlib1g-dev libgdbm5 libgdbm-dev libncurses5-dev git libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev cmake freeglut3 freeglut3-dev qt4-default qt4-dev-tools libsmokeqt4-dev 1> /dev/null

echo "Ruby installation..."
apt-get -y install  ruby ruby-dev 1> /dev/null

# Set ownership
chown -R $NOS3_USER:root /usr/local/bin
chown -R $NOS3_USER:root /var/lib/gems

echo "Gem installation..."
cd /home/$NOS3_USER
sudo su $NOS3_USER << `EOF`
    export NOS3_USER=$(whoami)

    # Create necessary directories
    test -e /home/$NOS3_USER/Desktop || mkdir /home/$NOS3_USER/Desktop

    # Install gems
    echo 'gem: --no-ri --no-rdoc' >> /home/$NOS3_USER/.gemrc
    gem install ruby-termios -v 0.9.6 1> /dev/null
    gem install rake -v 12.3.2 1> /dev/null
    gem install bundler -v 1.3.0 1> /dev/null
    gem install collection-json -v 0.1.7 1> /dev/null
    gem install multipart-post -v 2.0.0 1> /dev/null
    gem install faraday -v 0.15.4 1> /dev/null
    gem install faraday_middleware -v 0.13.1 1> /dev/null
    gem install faraday_collection_json -v 0.0.1 1> /dev/null
    gem install middleware -v 0.1.0 1> /dev/null
    gem install rack -v 2.0.6 1> /dev/null
    gem install rack-cache -v 1.8.0 1> /dev/null
    gem install ffi -v 1.10.0 1> /dev/null
    gem install ethon -v 0.12.0 1> /dev/null
    gem install typhoeus -v 1.3.1 1> /dev/null
    gem install cosmos -v 4.3.0  1> /dev/null

    echo "COSMOS installation..."
    cd /home/$NOS3_USER/Desktop
    test -e cosmos || cosmos install cosmos 1> /dev/null
    cd /home/$NOS3_USER/Desktop/cosmos
    cd /home/$NOS3_USER/nos3
    cp -R /home/$NOS3_USER/nos3/support/cosmos/* /home/$NOS3_USER/Desktop/cosmos/
    find /home/$NOS3_USER/Desktop/cosmos -type f | xargs dos2unix -q
`EOF`
    
echo "Cleanup..."
# Reset archive directory
rm -r /var/cache/apt/archives
mkdir -p /var/cache/apt/archives/partial
touch /var/cache/apt/archives/lock
chmod 640 /var/cache/apt/archives/lock