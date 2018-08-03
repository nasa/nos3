#!/bin/sh
gsettings set org.gnome.nautilus.icon-view default-zoom-level 'small'

[ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

xterm -T "CentOS Final Installer" -rv -e '
    [ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

    echo " "
    echo "This script will reboot the VM automatically once complete!"
    echo " "

    KERN_DIR=/usr/src/kernels/$(uname -r)*

    cd /tmp
    mount -o loop,ro "$(ls /tmp/VBoxGuestAdditions_* | tail -n 1)" /media
    sh /media/VBoxLinuxAdditions.run 
    rm -f /home/nos3/Desktop/centos_final_installer.sh
    reboot
'

