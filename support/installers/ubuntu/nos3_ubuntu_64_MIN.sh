#!/bin/bash
#
# Shell script to minimally provision the NOS^3 64-bit VM on Ubuntu 18.04

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_ubuntu_64_MIN.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi
export BOOST_VERSION=1.65
export DEBIAN_FRONTEND=noninteractive

echo "Archive old versions of NOS3..."
    NOW=$(date +%Y%m%d)
    if [[ -d ~/nos3 ]]; then
    mv -f ~/nos3 ~/nos3_archived_$NOW
    fi

echo "Copy NOS3 to ~/nos3..."
    if [[ -d /vagrant ]]; then
    cp -R /vagrant_parent /home/$NOS3_USER/nos3
    else
    cd ..
    cp -R /$(pwd) /home/$NOS3_USER/nos3
    fi
    chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/nos3 
    chmod -R 755 /home/$NOS3_USER/nos3
    mkdir /usr/local/cpu1
    chown -R $NOS3_USER:$NOS3_USER /usr/local/cpu*

echo "Baseline..."
    apt-get update 1> /dev/null
    apt-get install -y binutils gcc-multilib cmake cmake-qt-gui g++ g++-multilib build-essential google-mock libgtest-dev dos2unix gdb libpython3.6-dev python python-dev dwarves lcov gedit lib32z1 unzip xterm gnome-terminal firefox bash-completion gedit 1> /dev/null
    apt-get autoremove -y 1> /dev/null

    # EPS Sim GUI Fix
    apt-get install -y libgnomeui-0 1> /dev/null 

    # baseline - x86_64 
    #apt-get install -y libfltk1.1-dev 1> /dev/null

    # googletest - x86_64
    mkdir /usr/src/googletest/build
    cd /usr/src/googletest/build
    cmake .. 1> /dev/null
    make 1> /dev/null
    mv /usr/src/googletest/build/googlemock/libg* /usr/lib/x86_64-linux-gnu/
    mv /usr/src/googletest/build/googlemock/gtest/libg* /usr/lib/x86_64-linux-gnu/

    # baseline - i386 
    dpkg --add-architecture i386
    apt-get update 1> /dev/null
    apt-get -y install libfltk1.1-dev:i386 1> /dev/null
    apt-get -y install libstdc++6:i386 1> /dev/null

    # googletest - i386
    mkdir /usr/src/googletest/build32
    cd /usr/src/googletest/build32
    cmake .. -DCMAKE_CXX_FLAGS=-m32 1> /dev/null
    make 1> /dev/null
    mv /usr/src/googletest/build32/googlemock/libg* /usr/lib/i386-linux-gnu/ 
    mv /usr/src/googletest/build32/googlemock/gtest/libg* /usr/lib/i386-linux-gnu/

echo "Boost..."
    # boost x86_64
    #apt-get install -y libboost-date-time$BOOST_VERSION-dev libboost-chrono$BOOST_VERSION-dev  libboost-filesystem$BOOST_VERSION-dev libboost-program-options$BOOST_VERSION-dev libboost-regex$BOOST_VERSION-dev libboost-system$BOOST_VERSION-dev  libboost-thread$BOOST_VERSION-dev 1> /dev/null
    #apt-get install -y libxerces-c-dev libboost-chrono-dev libboost-program-options-dev  libboost-system-dev 1> /dev/null

    # boost i386
    apt-get install -y libboost$BOOST_VERSION-dev:i386 1> /dev/null
    apt-get install -y libboost-date-time$BOOST_VERSION-dev:i386 libboost-chrono$BOOST_VERSION-dev:i386  libboost-filesystem$BOOST_VERSION-dev:i386 libboost-program-options$BOOST_VERSION-dev:i386 libboost-regex$BOOST_VERSION-dev:i386 libboost-system$BOOST_VERSION-dev:i386  libboost-thread$BOOST_VERSION-dev:i386 1> /dev/null
    apt-get install -y libicu-dev:i386 libxerces-c-dev:i386 libxerces-c3.2 libboost-chrono-dev:i386 libboost-program-options-dev:i386 libboost-regex$BOOST_VERSION-dev:i386 libboost-system-dev:i386 1> /dev/null

echo "ITC Common and NOS Engine..."
    find /home/$NOS3_USER/nos3/support/packages/ubuntu/ -name \*_i386.deb -exec dpkg --install '{}' + 
    apt-get -f -y install 1> /dev/null

echo "42..."
    # 42 i386 and x86_64
    apt-get -y install freeglut3:i386 freeglut3-dev:i386 libgl1-mesa-dev:i386 1> /dev/null
    cd /opt   
    git clone --quiet https://github.com/ericstoneking/42.git
    cd 42
    git reset --hard --quiet fe112678681bf752eb84fecf302b71117956846c
    sed -i -e 's/#NOS3FSW/NOS3FSW/; s/ARCHFLAG = /ARCHFLAG = -m32 /; s/LFLAGS = /LFLAGS = -m32 /;' Makefile
    make &> /dev/null
    chmod -R ugo+w 42/InOut 
    chown -R $NOS3_USER:$NOS3_USER /opt/42 

echo "Fixes and environmental variables..."
    grep msg_max /etc/sysctl.conf &> /dev/null || echo 'fs.mqueue.msg_max=500' >> /etc/sysctl.conf
    echo "handle all ignore nostop noprint" > /home/$NOS3_USER/.gdbinit

echo "Environment setup..."
    test -e /home/$NOS3_USER/Desktop || mkdir /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop; chmod 755 /home/$NOS3_USER/Desktop
    # Additional Libraries
        # i386 build
        export CFLAGS=-m32
        export CXXFLAGS=-m32
        # SOFA_C
        cp -R /home/$NOS3_USER/nos3/support/packages/libraries/sofa_c-20150209_a /usr/local/lib/
        mkdir /usr/local/lib/sofa_c-20150209_a/build
        cd /usr/local/lib/sofa_c-20150209_a/build
        cmake ../ &> /usr/local/lib/sofa_c-20150209_a/build/log.txt
        make &>> /usr/local/lib/sofa_c-20150209_a/build/log.txt
        chown $NOS3_USER:$NOS3_USER /usr/local/lib/sofa_c-20150209_a/*
        # Geotrans3.5
        cp -R /home/$NOS3_USER/nos3/support/packages/libraries/geotrans3.5 /usr/local/lib/
        mkdir /usr/local/lib/geotrans3.5/build
        cd /usr/local/lib/geotrans3.5/build
        cmake ../ &> /usr/local/lib/geotrans3.5/build/log.txt
        make &>> /usr/local/lib/geotrans3.5/build/log.txt
        chown $NOS3_USER:$NOS3_USER /usr/local/lib/geotrans3.5/*
    # Build
    cp /home/$NOS3_USER/nos3/support/VirtualMachine/scripts/nos3-build.sh /home/$NOS3_USER/Desktop 
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-build.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-build.sh
    dos2unix -q /home/$NOS3_USER/Desktop/nos3-build.sh 1> /dev/null
    # Clean
    cp /home/$NOS3_USER/nos3/support/VirtualMachine/scripts/nos3-clean.sh /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-clean.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-clean.sh 
    dos2unix -q /home/$NOS3_USER/Desktop/nos3-clean.sh 1> /dev/null
    # Run
    cp /home/$NOS3_USER/nos3/support/VirtualMachine/scripts/nos3-run.sh /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-run.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-run.sh 
    dos2unix -q /home/$NOS3_USER/Desktop/nos3-run.sh 1> /dev/null
    # Stop
    cp /home/$NOS3_USER/nos3/support/VirtualMachine/scripts/nos3-stop.sh /home/$NOS3_USER/Desktop 
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-stop.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-stop.sh 
    dos2unix -q /home/$NOS3_USER/Desktop/nos3-stop.sh 1> /dev/null
    # 42
    mkdir -p /home/$NOS3_USER/Desktop/nos3-42
    cd /home/$NOS3_USER/Desktop/nos3-42
    test -e Model || ln -s /opt/42/Model
    test -e World || ln -s /opt/42/World
    test -e Kit || ln -s /opt/42/Kit
    cp -R /home/$NOS3_USER/nos3/sims/sim_common/cfg/NOS3-42InOut .
    dos2unix -q NOS3-42InOut/* 1> /dev/null
    chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-42

if [[ -d /vagrant ]]; then
    echo "Modify Unity Toolbar..."
    cp /home/$NOS3_USER/nos3/support/VirtualMachine/icons/* /usr/share/icons/
    cp /home/$NOS3_USER/nos3/support/VirtualMachine/launcher-shortcuts/nos3.desktop /usr/share/applications/
    echo "gsettings set org.gnome.shell favorite-apps \"['org.gnome.Nautilus.desktop', 'firefox.desktop', 'org.gnome.gedit.desktop', 'nos3.desktop', 'org.gnome.Terminal.desktop']\" " >> /etc/profile.d/all_users.sh
fi

echo "Cleanup..."
    # Reset archive directory
    rm -r /var/cache/apt/archives
    mkdir -p /var/cache/apt/archives/partial
    touch /var/cache/apt/archives/lock
    chmod 640 /var/cache/apt/archives/lock