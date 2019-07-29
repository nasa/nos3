#!/bin/bash
#
# Shell script to provision the NOS^3 64-bit VM on CentOS 7 with additional developer tools

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_centos_64_CUSTOM.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

DIR=/home/$NOS3_USER/nos3

# Update grub to use the newest kernel
sed -i -e 's/GRUB_DEFAULT=saved/GRUB_DEFAULT=0/;' /etc/default/grub  1> /dev/null
grub2-mkconfig -o /boot/grub2/grub.cfg 2> /dev/null

echo "Cleanup..."
# Rebuild yum database
cd /var/lib/rpm
rm -rf __db*
rpm --rebuilddb