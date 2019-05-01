#!/bin/bash
#
# Shell script to minimally provision the NOS^3 64-bit VM on Ubuntu 16.04

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_64_MIN.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

DIR=/home/$NOS3_USER/nos3
export BOOST_VERSION=1.58

echo " "
echo "Archive old versions of NOS3"
echo " "
    NOW=$(date +%Y%m%d)
    if [[ -d ~/nos3 ]]; then
    mv -f ~/nos3 ~/nos3_archived_$NOW
    fi

echo " "
echo "Copy NOS3 to ~/nos3"
echo " "
    if [[ -d /vagrant ]]; then
    cp -R /vagrant_parent /home/$NOS3_USER/nos3
    else
    cd ..
    cp -R /$(pwd) /home/$NOS3_USER/nos3
    fi
    chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/nos3 
    chmod -R 755 /home/$NOS3_USER/nos3

echo " "
echo "Baseline"
echo " "
    apt-get update 1> /dev/null
    apt-get install -y binutils gcc-multilib cmake cmake-qt-gui g++ g++-multilib build-essential google-mock libgtest-dev dos2unix gdb libpython3.5-dev python python-dev dwarves lcov gedit libfltk1.1-dev lib32z1 1> /dev/null
    apt-get autoremove -y 1> /dev/null
    # gtest
    cd /usr/src/gtest
    sudo cmake . 1> /dev/null
    sudo make 1> /dev/null
    sudo mv libg* /usr/lib/ 

echo " "
echo "Boost"
echo " "
    apt-get install -y libxerces-c-dev libboost-chrono-dev libboost-date-time$BOOST_VERSION-dev libboost-filesystem$BOOST_VERSION-dev libboost-program-options$BOOST_VERSION-dev libboost-program-options-dev libboost-regex$BOOST_VERSION-dev libboost-system$BOOST_VERSION-dev libboost-system-dev libboost-thread$BOOST_VERSION-dev 1> /dev/null

echo " "
echo "ITC Common and NOS Engine"
echo " "
    find $DIR/support/packages/ubuntu/ -name \*_amd64.deb -exec dpkg --install '{}' + 
    apt-get -f -y install 1> /dev/null

echo " "
echo "42"
echo " "
    apt-get -y install freeglut3 freeglut3-dev libgl1-mesa-dev git git-core cmake 1> /dev/null
    cd /opt 
    sudo chown -R $NOS3_USER:$NOS3_USER /opt
sudo su $NOS3_USER << `EOF`
    cd /opt
    git clone https://github.com/ericstoneking/42.git
`EOF`
    chown -R $NOS3_USER:$NOS3_USER /opt/42
    make -C 42 NOS3FSWFLAG='-D _ENABLE_NOS3_FSW_'
    chmod 755 42/42
    chmod -R ugo+w 42/InOut 

echo " "
echo "Fixes and Environmental Varibles"
echo " "
    grep msg_max /etc/sysctl.conf &> /dev/null || echo 'fs.mqueue.msg_max=100' >> /etc/sysctl.conf
    echo "handle all ignore nostop noprint" > /home/$NOS3_USER/.gdbinit

echo " "
echo "Setup Environment"
echo " "
    test -e /home/$NOS3_USER/Desktop || mkdir /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop; chmod 755 /home/$NOS3_USER/Desktop
    echo " "
    echo "    NOS3"
    echo " "
    # Additional Libraries
        # SOFA_C
        cp -R $DIR/support/packages/libraries/sofa_c-20150209_a /usr/local/lib/
        mkdir /usr/local/lib/sofa_c-20150209_a/build
        cd /usr/local/lib/sofa_c-20150209_a/build
        cmake ../ 1> /dev/null
        make 1> /dev/null
        chown $NOS3_USER:$NOS3_USER /usr/local/lib/sofa_c-20150209_a/*
        # Geotrans3.5
        cp -R $DIR/support/packages/libraries/geotrans3.5 /usr/local/lib/
        mkdir /usr/local/lib/geotrans3.5/build
        cd /usr/local/lib/geotrans3.5/build
        cmake ../ 1> /dev/null
        make 1> /dev/null
        chown $NOS3_USER:$NOS3_USER /usr/local/lib/geotrans3.5/*
    # Build
    cp $DIR/support/VirtualMachine/scripts/nos3-build.sh /home/$NOS3_USER/Desktop 
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-build.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-build.sh
    dos2unix /home/$NOS3_USER/Desktop/nos3-build.sh 1> /dev/null
    # Run
    cp $DIR/support/VirtualMachine/scripts/nos3-run.sh /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-run.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-run.sh 
    dos2unix /home/$NOS3_USER/Desktop/nos3-run.sh 1> /dev/null
    # Stop
    cp $DIR/support/VirtualMachine/scripts/nos3-stop.sh /home/$NOS3_USER/Desktop 
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-stop.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-stop.sh 
    dos2unix /home/$NOS3_USER/Desktop/nos3-stop.sh 1> /dev/null
    echo " "
    echo "    42"
    echo " "
    mkdir -p /home/$NOS3_USER/Desktop/nos3-42
    cd /home/$NOS3_USER/Desktop/nos3-42
    test -e Model || ln -s /opt/42/Model
    test -e World || ln -s /opt/42/World
    test -e Kit || ln -s /opt/42/Kit
    cp -R $DIR/sims/sim_common/cfg/NOS3-42InOut .
    dos2unix NOS3-42InOut/* 1> /dev/null
    chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-42

if [[ -d /vagrant ]]; then
    echo " "   
    echo "Modify Unity Toolbar"
    echo " "
    echo "gsettings set com.canonical.Unity.Launcher favorites \"['application://nautilus.desktop','application://firefox.desktop','application://gedit.desktop','application:///home/nos3/nos3/support/VirtualMachine/launcher-shortcuts/nos3.desktop','application:///home/nos3/nos3/support/VirtualMachine/launcher-shortcuts/nos3.desktop','unity://running-apps','application://gnome-terminal.desktop','unity://expo-icon','unity://devices']\" " > /etc/profile.d/all_users.sh
fi

echo " "
echo "Cleanup"
echo " "
    apt-get -y autoremove 1> /dev/null
    # Reset archive directory
    rm -r /var/cache/apt/archives
    mkdir -p /var/cache/apt/archives/partial
    touch /var/cache/apt/archives/lock
    chmod 640 /var/cache/apt/archives/lock