#!/bin/bash
#
# Shell script to fully provision the NOS^3 64-bit VM on CentOS 7

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_centos_64_AIT.sh ---"
echo "--- "

echo 'nos3 ALL=(ALL) NOPASSWD: ALL' | sudo tee -a /etc/sudoers

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

echo "Dependencies..."
yum -y install python-pip rawsocket socat 1> /dev/null
pip install pyzmq virtualenv virtualenvwrapper 1> /dev/null

echo "Create necessary directories..."
test -e /home/$NOS3_USER/AIT || mkdir /home/$NOS3_USER/AIT

echo "Setup Environment..."
cp /vagrant_parent/support/installers/common/nos3_99-firstboot.sh /home/$NOS3_USER/99-firstboot.sh 
dos2unix -q /home/$NOS3_USER/99-firstboot.sh 
chmod 777 /home/$NOS3_USER/99-firstboot.sh 
chown $NOS3_USER:$NOS3_USER /home/nos3/99-firstboot.sh
cp /vagrant_parent/support/installers/centos/rc.local /etc/rc.local
echo "export NOS3_USER=$NOS3_USER" >> /home/$NOS3_USER/.bashrc
echo "export WORKON_HOME=/home/$NOS3_USER/.virtualenvs" >> /home/$NOS3_USER/.bashrc
echo "export PROJECT_HOME=/home/$NOS3_USER/Devel" >> /home/$NOS3_USER/.bashrc
echo "source /usr/bin/virtualenvwrapper.sh" >> /home/$NOS3_USER/.bashrc
	
echo "Setup ait.service..."
cp /vagrant_parent/support/installers/centos/ait.service /etc/systemd/system/ait.service

echo "Setup Postactivate..."
cp /vagrant_parent/support/installers/centos/postactivate /home/$NOS3_USER/
dos2unix -q /home/$NOS3_USER/postactivate
chown nos3:nos3 /home/$NOS3_USER/postactivate

systemctl daemon-reload
systemctl start ait.service
systemctl enable ait.service

echo "Cloning AIT Core and GUI..."
cd /home/$NOS3_USER/AIT
git clone --quiet https://github.com/NASA-AMMOS/AIT-Core.git
git clone --quiet https://github.com/NASA-AMMOS/AIT-GUI.git
git clone --quiet https://github.com/NASA-AMMOS/AIT-CFS.git
chown -R nos3:nos3 /home/$NOS3_USER/AIT

echo "Install google chrome..."
cd /tmp
wget --quiet https://dl.google.com/linux/direct/google-chrome-stable_current_x86_64.rpm
yum -y install libXScrnSaver libappindicator-gtk3 liberation-fonts 1> /dev/null
yum -y install redhat-lsb-core 1> /dev/null
rpm -i google-chrome-stable_current_x86_64.rpm

echo "Cleanup..."
# Rebuild yum database
cd /var/lib/rpm
rm -rf __db*
rpm --rebuilddb