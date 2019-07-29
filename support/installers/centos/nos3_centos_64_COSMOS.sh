#!/bin/bash
#
# Shell script to fully provision the NOS^3 64-bit VM on CentOS 7

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_centos_64_COSMOS.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

DIR=/home/$NOS3_USER/nos3

# Based upon the following COSMOS script:
# https://github.com/BallAerospace/COSMOS/blob/v4.3.0/vendor/installers/linux_mac/INSTALL_COSMOS.sh

echo "Dependencies..."
yum -y install openssl-devel yaml-cpp-devel libffi-devel readline-devel zlib-devel gdbm gdbm-devel ncurses-devel git gstreamer-devel gstreamer-plugins-base-devel qt qt-devel smokeqt 1> /dev/null
    
echo "Ruby installation..."
# Install ruby from source
test -e /home/$NOS3_USER/Downloads || mkdir /home/$NOS3_USER/Downloads
cd /home/$NOS3_USER/Downloads
test -e ruby-2.3.1.tar.gz || wget --quiet http://ftp.ruby-lang.org/pub/ruby/2.3/ruby-2.3.1.tar.gz
test -e ruby-2.3.1 || tar -xzvf ruby-2.3.1.tar.gz 1> /dev/null
cd ruby-2.3.1/
./configure --enable-shared &> ruby_configure.log
make &> ruby_make.log
make install &> ruby_make_install.log
/usr/local/bin/ruby -v
# Set ownership
chown -R $NOS3_USER:root /usr/local/bin
chown -R $NOS3_USER:root /usr/local/lib

echo "Gem installation..."
cd /home/$NOS3_USER
sudo su $NOS3_USER << `EOF`
    export NOS3_USER=$(whoami)

    # Create necessary directories
    test -e /home/$NOS3_USER/Desktop || mkdir /home/$NOS3_USER/Desktop

    # Install gems
    echo 'gem: --no-ri --no-rdoc' >> /home/$NOS3_USER/.gemrc 
    cd /usr/local/bin/
    ./gem install ruby-termios -v 0.9.6 1> /dev/null
    ./gem install rake -v 12.3.2 1> /dev/null
    ./gem install bundler -v 1.3.0 1> /dev/null
    ./gem install collection-json -v 0.1.7 1> /dev/null
    ./gem install multipart-post -v 2.0.0 1> /dev/null
    ./gem install faraday -v 0.15.4 1> /dev/null
    ./gem install faraday_middleware -v 0.13.1 1> /dev/null
    ./gem install faraday_collection_json -v 0.0.1 1> /dev/null
    ./gem install middleware -v 0.1.0 1> /dev/null
    ./gem install rack -v 2.0.6 1> /dev/null
    ./gem install rack-cache -v 1.8.0 1> /dev/null
    ./gem install ffi -v 1.10.0 1> /dev/null
    ./gem install ethon -v 0.12.0 1> /dev/null
    ./gem install typhoeus -v 1.3.1 1> /dev/null
    ./gem install cosmos -v 4.3.0  1> /dev/null

    echo "COSMOS installation..."
    cd /home/$NOS3_USER/Desktop
    test -e cosmos || mkdir cosmos
    cd /home/$NOS3_USER/Desktop/cosmos
    cp -R /usr/local/lib/ruby/gems/2.3.0/gems/cosmos-4.3.0/install/* .
    cd /home/$NOS3_USER/nos3
    cp -R /home/$NOS3_USER/nos3/support/cosmos/* /home/$NOS3_USER/Desktop/cosmos/
    find /home/$NOS3_USER/Desktop/cosmos -type f | xargs dos2unix -q
`EOF`

echo "Cleanup..."
# Rebuild yum database
cd /var/lib/rpm
rm -rf __db*
rpm --rebuilddb