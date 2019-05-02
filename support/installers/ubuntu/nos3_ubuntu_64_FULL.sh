#!/bin/bash
#
# Shell script to fully provision the NOS^3 64-bit VM on Ubuntu 16.04

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_64_FULL.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

DIR=/home/$NOS3_USER/nos3
export BOOST_VERSION=1.58

if [[ "$1" == "kubos" ]]
echo " "
echo "KUBOS"
echo " "
    # Install dependices 
    apt-get -y install libssl-dev libyaml-dev libffi-dev libreadline6-dev zlib1g-dev libgdbm3 libgdbm-dev libncurses5-dev git git-core cmake libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libgstreamer0.10-dev qt4-default qt4-dev-tools software-properties-common curl build-essential libreadline-dev libsqlite3-dev sqlite3 libxml2-dev libxslt1-dev libcurl4-openssl-dev python-software-properties libffi-dev libqtscript4-qtbindings libcanberra-gtk-module libcanberra-gtk3-module libreoffice-calc 1> /dev/null
    test -e /home/$NOS3_USER/Downloads || mkdir /home/$NOS3_USER/Downloads
    cd /home/$NOS3_USER/Downloads
    
# Set ownership
chown -R $NOS3_USER:root /usr/local/bin

then
echo "Rust installation"
cd /home/$NOS3_USER
sudo su $NOS3_USER << `EOF`
    curl https://sh.rustup.rs -sSf | sh -s -- -y --default-host x86_64-unknown-linux-gnu --default-toolchain stable
`EOF`

echo " "
echo "Clone KubOS master repo"
echo " "
cd /home/nos3
sudo chown -R $NOS3_USER:$NOS3_USER /home/nos3/
sudo su $NOS3_USER << `EOF`
    cd /home/nos3
    git clone https://github.com/kubos/kubos.git
`EOF`
chown -R $NOS3_USER:$NOS3_USER /home/nos3/kubos
fi

echo " "
echo "Cleanup"
echo " "
    apt-get -y autoremove 1> /dev/null
    # Reset archive directory
    rm -r /var/cache/apt/archives
    mkdir -p /var/cache/apt/archives/partial
    touch /var/cache/apt/archives/lock
    chmod 640 /var/cache/apt/archives/lock