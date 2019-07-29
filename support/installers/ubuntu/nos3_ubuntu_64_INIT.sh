#!/bin/bash
#
# Shell script to initially provision the NOS^3 64-bit VM on Ubuntu 18.04
#
# Note that this script is for initializing the vagrant box

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_ubuntu_64_INIT.sh ---"
echo "--- "

# Initialize variables
export DEBIAN_FRONTEND=noninteractive

echo "Installing additional software..."
apt-get -y update 1> /dev/null
DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" upgrade 1> /dev/null
apt-get -y install linux-headers-$(uname -r) 1> /dev/null # Headers needed for Guest Additions

echo "Install the desktop..."
# Ubuntu Desktop
apt-get -y dist-upgrade 1> /dev/null
apt-get -y install --no-install-recommends ubuntu-desktop 1> /dev/null
apt-get -y install indicator-session gnome-terminal firefox unity-lens-applications 1> /dev/null 

echo "Update locale..."
locale-gen --purge en_US.UTF-8 1> /dev/null
echo -e 'LANG="en_US.UTF-8"\nLANGUAGE="en_US:en"\n' > /etc/default/locale

echo "Disable apport crash reporting so that we can analyze our own core files, also allow attaching gdb..."
apt-get -y purge apport 1> /dev/null
grep core_pattern /etc/sysctl.conf &> /dev/null || echo 'kernel.core_pattern=core.%e.%p.%t' >> /etc/sysctl.conf # Permanently changes value in /proc/sys/kernel/core_pattern
grep 'soft.*core.*unlimited' /etc/security/limits.conf &> /dev/null || (echo '*                soft    core            unlimited' >> /etc/security/limits.conf; echo '*                hard    core            unlimited' >> /etc/security/limits.conf)
sed -i 's/kernel.yama.ptrace_scope = 1/kernel.yama.ptrace_scope = 0/' /etc/sysctl.d/10-ptrace.conf

echo "Adjusting users..."
id -u ubuntu &> /dev/null && deluser --remove-home ubuntu
# Create user - `mkpasswd -m sha-512 <<< password`
id -u nos3 &> /dev/null || (adduser --disabled-password --gecos "" nos3 ; \
echo 'nos3:$6$.mG1a/zL$f1LcckhnvYRUxQZrGeWVBh.nNAJu9qNIX9v1zvivsc67SjqGapbXNFS4e2/uInkqSas64WwmBRJ45uqSB.nSZ1' | chpasswd -e; \
echo 'nos3 ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/nos3; chmod 440 /etc/sudoers.d/nos3)
# Disable guest
grep 'allow-guest=false' /usr/share/lightdm/lightdm.conf.d/50-ubuntu.conf &> /dev/null || \
  [ -e /usr/share/lightdm/lightdm.conf.d/50-ubuntu.conf ] && \
  echo 'allow-guest=false' >> /usr/share/lightdm/lightdm.conf.d/50-ubuntu.conf 
# Hide vagrant user from login screen
echo "[User]" >> /var/lib/AccountsService/users/vagrant
echo "SystemAccount=true" >> /var/lib/AccountsService/users/vagrant
# Add nos3 user to vboxsf group
usermod -a -G vboxsf nos3
# Add nos3 user to dialout group
usermod -a -G dialout nos3

echo "Preferences..."
# Launch executable shell scripts instead of display 
echo "gsettings set org.gnome.nautilus.preferences executable-text-activation launch" >> /etc/profile.d/all_users.sh
# Change user background
cp /vagrant/installers/nos3_background.png /usr/share/backgrounds/
chmod 644 /usr/share/backgrounds/nos3_background.png >> /etc/profile.d/all_users.sh
echo "gsettings set org.gnome.desktop.background picture-uri \"file:///usr/share/backgrounds/nos3_background.png\"" >> /etc/profile.d/all_users.sh
# Change default zoom level
echo "gsettings set org.gnome.nautilus.icon-view default-zoom-level 'small'" >> /etc/profile.d/all_users.sh
