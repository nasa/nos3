#!/bin/bash
#
# Shell script to init the NOS^3 VM
#
# Note that this script is for initializing the vagrant box

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "Installing additional software"
echo " "
# apt-add-repository multiverse # For VirtualBox Guest Additions
apt-get -y update 1> /dev/null
apt-get --force-yes upgrade 1> /dev/null
apt-get -y install linux-headers-$(uname -r) 1> /dev/null # Headers needed for Guest Additions
apt-get -y install linux-image-extra-$(uname -r) 1> /dev/null # Just to get UTF-8 in the kernel so VirtualBox can mount an optical drive!!

echo " "
echo "Install the desktop"
echo " "
# Minimal Gnome
  # apt-get -y install install xorg gnome-core gnome-system-tools gnome-app-install
# Minimal LXDE
  # apt-get -y install lxde-core
# Lubuntu
  # apt-get -y install --no-install-recommends lubuntu-desktop
# Ubuntu Desktop
apt-get -y install --no-install-recommends ubuntu-desktop 1> /dev/null
apt-get -y install indicator-session gnome-terminal firefox unity-lens-applications 1> /dev/null 

echo " "
echo "Update locale"
echo " "
locale-gen "en_US.UTF-8" 1> /dev/null
dpkg-reconfigure locales 1> /dev/null
echo "LC_ALL=en_US.UTF-8" >> /etc/environment
echo "LANG=en_US.UTF-8" >> /etc/environment

echo " "
echo "Disable apport crash reporting so that we can analyze our own core files, also allow attaching gdb"
echo " "
apt-get -y purge apport 1> /dev/null
grep core_pattern /etc/sysctl.conf &> /dev/null || echo 'kernel.core_pattern=core.%e.%p.%t' >> /etc/sysctl.conf # Permanently changes value in /proc/sys/kernel/core_pattern
grep 'soft.*core.*unlimited' /etc/security/limits.conf &> /dev/null || (echo '*                soft    core            unlimited' >> /etc/security/limits.conf; echo '*                hard    core            unlimited' >> /etc/security/limits.conf)
sed -i 's/kernel.yama.ptrace_scope = 1/kernel.yama.ptrace_scope = 0/' /etc/sysctl.d/10-ptrace.conf

echo " "
echo "Adjusting Users"
echo " "
id -u ubuntu &> /dev/null && deluser --remove-home ubuntu
# Add nos3 user; insecure due to plain text in this file?; add nos3 to sudoers
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

echo " "
echo "Change Unity/Gnome3/Nautilus settings so that double clicking an executable shell script will run it (instead of view it in an editor)"
echo " "
echo -e 'user-db:user\nsystem-db:local\n' > /etc/dconf/profile/user
mkdir -p /etc/dconf/db/local.d
echo -e "# dconf path\n[org/gnome/nautilus/preferences]\n\n# dconf key names / values\nexecutable-text-activation='launch'\n" > /etc/dconf/db/local.d/01-nautilus-preferences

echo ""
echo "Change user background"
echo ""
cp /vagrant/installers/nos3_background.png /usr/share/backgrounds/; chmod 644 /usr/share/backgrounds/nos3_background.png
echo -e "# dconf path\n[org/gnome/desktop/background]\n\n# dconf key names / values\npicture-uri='file:///usr/share/backgrounds/nos3_background.png'\n" >> /etc/dconf/db/local.d/02-gnome-desktop-background
dconf update

echo " "
echo "Change greeter background and customize greeter"
echo " "    
sed -i 's/warty-final-ubuntu.png/nos3_background.png/' /usr/share/glib-2.0/schemas/com.canonical.unity-greeter.gschema.xml
glib-compile-schemas /usr/share/glib-2.0/schemas/
sed -i "s/\\\[\\\]/['vagrant']/" /usr/share/glib-2.0/schemas/com.canonical.unity-greeter.gschema.xml
glib-compile-schemas /usr/share/glib-2.0/schemas/
