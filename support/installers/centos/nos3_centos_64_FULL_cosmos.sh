#!/bin/bash
#
# Shell script to fully provision the NOS^3 64-bit VM on CentOS 7

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

echo " "
echo "COSMOS"
echo " "
    # Install dependencies 
    yum -y install openssl-devel yaml-cpp-devel libffi-devel readline-devel zlib-devel gdbm gdbm-devel ncurses-devel git cmake3 cmake3-gui gstreamer-devel gstreamer-plugins-base-devel qt qt-devel curl curl-devel sqlite sqlite-devel xml2 libxslt-devel libffi-devel qtscriptbindings libcanberra-devel libcanberra-gtk3 1> /dev/null
    ln -s /usr/bin/cmake3 /usr/bin/cmake 1> /dev/null
    ln -s /usr/bin/cmake3-gui /usr/bin/cmake-gui 1> /dev/null
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
    find /home/$STF_USER/Desktop/cosmos -type f | xargs dos2unix 1> /dev/null
    chown -R $STF_USER:$STF_USER /home/$STF_USER/Desktop/cosmos

#echo " "
#echo "Cleanup"
#echo " "
    # Reset archive directory
    #rm -r /var/cache/apt/archives
    #mkdir -p /var/cache/apt/archives/partial
    #touch /var/cache/apt/archives/lock
    #chmod 640 /var/cache/apt/archives/lock