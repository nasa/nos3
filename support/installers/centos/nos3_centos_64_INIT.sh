#!/bin/bash
#
# Shell script to init the NOS^3 VM
#
# Note that this script is for initializing the vagrant box using CentOS 7

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "Installing additional software"
echo " "
yum -y update 1> /dev/null
yum -y install dkms epel-release 1> /dev/null
yum -y groupinstall "Development Tools" 1> /dev/null

echo " "
echo "Install the desktop"
echo " "
yum -y groupinstall "GNOME Desktop" 1> /dev/null
systemctl set-default graphical.target 1> /dev/null

echo " "
echo "Update locale"
echo " "
localectl set-locale LANG=en_US.UTF-8 

echo " "
echo "Adjusting Users"
echo " "
# Add nos3 user
useradd -p "aaNu0jP07m3AY" nos3
# Add nos3 to sudoers
echo 'nos3 ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/nos3
chmod 440 /etc/sudoers.d/nos3
# Hide vagrant user from login screen
echo "[User]" >> /var/lib/AccountsService/users/vagrant
echo "SystemAccount=true" >> /var/lib/AccountsService/users/vagrant

echo " "
echo "Prepare Guest Additions"
echo " "
test -e /home/nos3 || mkdir /home/nos3
test -e /home/nos3/Desktop || mkdir /home/nos3/Desktop
cp /vagrant_parent/support/installers/centos/centos_final_installer.sh /home/nos3/Desktop/
chown nos3:nos3 /home/nos3/Desktop/centos_final_installer.sh
chmod 755 /home/nos3/Desktop/centos_final_installer.sh
dos2unix /home/nos3/Desktop/centos_final_installer.sh

echo " "
echo "Preferences"
echo " "
# Change Unity/Gnome3/Nautilus settings so that double clicking an executable shell script will run it (instead of view it in an editor)
echo -e 'user-db:user\nsystem-db:local\n' > /etc/dconf/profile/user
mkdir -p /etc/dconf/db/local.d
echo -e "# dconf path\n[org/gnome/nautilus/preferences]\n\n# dconf key names / values\nexecutable-text-activation='launch'\n" > /etc/dconf/db/local.d/01-nautilus-preferences
# Change background
cp /vagrant/installers/nos3_background.png /usr/share/backgrounds/; chmod 644 /usr/share/backgrounds/nos3_background.png
echo -e "# dconf path\n[org/gnome/desktop/background]\n\n# dconf key names / values\npicture-uri='file:///usr/share/backgrounds/nos3_background.png'\n" >> /etc/dconf/db/local.d/02-gnome-desktop-background
dconf update
