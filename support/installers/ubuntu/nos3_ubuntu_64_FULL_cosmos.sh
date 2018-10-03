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
  STF_USER=nos3
else
  STF_USER=$(logname)
fi

DIR=/home/$STF_USER/nos3
export BOOST_VERSION=1.58

echo " "
echo "COSMOS"
echo " "
    # Install dependices 
    apt-get -y install libssl-dev libyaml-dev libffi-dev libreadline6-dev zlib1g-dev libgdbm3 libgdbm-dev libncurses5-dev git git-core cmake libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libgstreamer0.10-dev qt4-default qt4-dev-tools software-properties-common curl build-essential libreadline-dev libsqlite3-dev sqlite3 libxml2-dev libxslt1-dev libcurl4-openssl-dev python-software-properties libffi-dev libqtscript4-qtbindings libcanberra-gtk-module libcanberra-gtk3-module libreoffice-calc 1> /dev/null
    # Install ruby from source
    test -e /home/$STF_USER/Downloads || mkdir /home/$STF_USER/Downloads
    cd /home/$STF_USER/Downloads
    test -e ruby-2.3.1.tar.gz || wget --quiet http://ftp.ruby-lang.org/pub/ruby/2.3/ruby-2.3.1.tar.gz
    test -e ruby-2.3.1 || tar -xzvf ruby-2.3.1.tar.gz 1> /dev/null
    cd ruby-2.3.1/
    ./configure --enable-shared 1> /dev/null
    make 1> /dev/null
    make install 1> /dev/null
    ruby -v
    # Install COSMOS gem
    gem install cosmos 1> /dev/null
    gem install bundler 1> /dev/null

echo " "
echo "Setup Environment"
echo " "
    echo "    COSMOS install"
    echo " "
    cd /home/$STF_USER/Desktop
    test -e cosmos || cosmos install cosmos 1> /dev/null
    echo " "
    echo "    COSMOS cmd_tlm"
    echo " "
    cd $DIR
    cp -R $DIR/support/cosmos/* /home/$STF_USER/Desktop/cosmos/
    find /home/$STF_USER/Desktop/cosmos -type f | xargs dos2unix 2>&1 /dev/null
    chown -R $STF_USER:$STF_USER /home/$STF_USER/Desktop/cosmos
    cd /home/$STF_USER/Desktop/cosmos
    bundler install
    cd $DIR

echo " "
echo "Cleanup"
echo " "
    apt-get -y autoremove 1> /dev/null
    # Reset archive directory
    rm -r /var/cache/apt/archives
    mkdir -p /var/cache/apt/archives/partial
    touch /var/cache/apt/archives/lock
    chmod 640 /var/cache/apt/archives/lock