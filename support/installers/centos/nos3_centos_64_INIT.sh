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
echo "--- "
echo "--- nos3_centos_64_INIT.sh ---"
echo "--- "

echo "Rebuilding yum database..."
cd /var/lib/rpm
rm -rf __db*
rpm --rebuilddb

echo "Installing additional software..."
yum -y -q makecache fast 2>&1 /dev/null
yum -y -q clean all 1> /dev/null
yum -y -q update 2>&1 /dev/null
yum -y -q install dkms epel-release 2>&1 /dev/null
yum -y -q groupinstall "Development Tools" --setopt=group_package_types=mandatory,default,optional 2>&1 /dev/null

echo "Install the desktop..."
yum -y groupinstall "GNOME Desktop" 1> /dev/null
systemctl set-default graphical.target 1> /dev/null

echo "Update locale..."
localectl set-locale LANG=en_US.UTF-8 

echo "Adjusting Users..."
# Add nos3 user
useradd -p "aaNu0jP07m3AY" nos3
# Add nos3 to sudoers
echo 'nos3 ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/nos3
chmod 440 /etc/sudoers.d/nos3
# Hide vagrant user from login screen
echo "[User]" >> /var/lib/AccountsService/users/vagrant
echo "SystemAccount=true" >> /var/lib/AccountsService/users/vagrant
# Add nos3 user to vboxsf group
usermod -a -G vboxsf nos3

echo "Preferences..."
# Change Unity/Gnome3/Nautilus settings so that double clicking an executable shell script will run it (instead of view it in an editor)
echo -e 'user-db:user\nsystem-db:local\n' > /etc/dconf/profile/user
mkdir -p /etc/dconf/db/local.d
echo -e "# dconf path\n[org/gnome/nautilus/preferences]\n\n# dconf key names / values\nexecutable-text-activation='launch'\n" > /etc/dconf/db/local.d/01-nautilus-preferences
# Change user background
cp /vagrant/installers/nos3_background.png /usr/share/backgrounds/; chmod 644 /usr/share/backgrounds/nos3_background.png
echo -e "# dconf path\n[org/gnome/desktop/background]\n\n# dconf key names / values\npicture-uri='file:///usr/share/backgrounds/nos3_background.png'\n" >> /etc/dconf/db/local.d/02-gnome-desktop-background
dconf update
# Change greeter background
# TODO: This ^
# Change default zoom level
echo "gsettings set org.gnome.nautilus.icon-view default-zoom-level 'small'" >> /etc/profile.d/all_users.sh

echo "Cleanup..."
# Rebuild yum database
cd /var/lib/rpm
rm -rf __db*
rpm --rebuilddb