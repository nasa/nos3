#!/bin/bash
#
# Shell script to fully provision the NOS^3 64-bit VM on Ubuntu 18.04

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_ubuntu_64_AIT.sh ---"
echo "--- "

echo 'nos3 ALL=(ALL) NOPASSWD: ALL' | sudo tee -a /etc/sudoers

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

echo "Dependencies..."
apt-get -y install python-pip socat 1> /dev/null
pip install rawsocket pyzmq virtualenv virtualenvwrapper 1> /dev/null

echo "Create necessary directories..."
test -e /home/$NOS3_USER/AIT || mkdir /home/$NOS3_USER/AIT

echo "Setup Environment..."
cp /vagrant_parent/support/installers/common/nos3_99-firstboot.sh /home/$NOS3_USER/99-firstboot.sh 
dos2unix -q /home/$NOS3_USER/99-firstboot.sh 
chmod 777 /home/$NOS3_USER/99-firstboot.sh 
chown $NOS3_USER:$NOS3_USER /home/nos3/99-firstboot.sh
cp /vagrant_parent/support/installers/centos/rc.local /etc/rc.local
dos2unix -q /etc/rc.local
echo "export NOS3_USER=$(whoami)" >> /home/$NOS3_USER/.bashrc
echo "export WORKON_HOME=/home/$NOS3_USER/.virtualenvs" >> /home/$NOS3_USER/.bashrc
echo "export PROJECT_HOME=/home/$NOS3_USER/Devel" >> /home/$NOS3_USER/.bashrc
echo "source /usr/local/bin/virtualenvwrapper.sh" >> /home/$NOS3_USER/.bashrc
	
echo "Setup rc.local..."
cp /vagrant_parent/support/installers/ubuntu/rc.local /etc/rc.local
dos2unix -q /etc/rc.local

echo "Setup Postactivate..."
cp /vagrant_parent/support/installers/ubuntu/postactivate /home/$NOS3_USER/
dos2unix -q /home/$NOS3_USER/
chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/postactivate

echo "Cloning AIT Core and GUI..."
cd /home/$NOS3_USER/AIT/

git clone --quiet https://github.com/NASA-AMMOS/AIT-Core.git
git clone --quiet https://github.com/NASA-AMMOS/AIT-GUI.git
git clone --quiet https://github.com/NASA-AMMOS/AIT-CFS.git
chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/AIT

echo "Install google chrome..."
cd /tmp
wget --quiet https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
apt-get -y install fonts-liberation libxss1 xdg-utils 1> /dev/null
dpkg -i google-chrome-stable_current_amd64.deb

echo "Cleanup..."
# Reset archive directory
rm -r /var/cache/apt/archives
mkdir -p /var/cache/apt/archives/partial
touch /var/cache/apt/archives/lock
chmod 640 /var/cache/apt/archives/lock