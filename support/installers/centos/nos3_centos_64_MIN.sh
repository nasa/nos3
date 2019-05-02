#!/bin/bash
#
# Shell script to minimally provision the NOS^3 64-bit VM on CentOS 7

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_centos_64_MIN.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  STF_USER=nos3
else
  STF_USER=$(logname)
fi

DIR=/home/$STF_USER/nos3

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
    cp -R /vagrant_parent /home/$STF_USER/nos3
    else
    cd ..
    cp -R /$(pwd) /home/$STF_USER/nos3
    fi
    chown -R $STF_USER:$STF_USER /home/$STF_USER/nos3 
    chmod -R 755 /home/$STF_USER/nos3

echo " "
echo "Baseline"
echo " "
    yum -y install epel-release 1> /dev/null
    yum repolist 1> /dev/null
    yum -y update 1> /dev/null
    yum -y install kernel-devel 1> /dev/null
    yum install gmock dpkg-devel dpkg-dev 1> /dev/null
    yum -y install binutils-devel cmake3 cmake3-gui gcc-c++ gmock gmock-devel gtest-devel dos2unix gdb libpython3.5-dev python python-devel dwarves lcov gedit xterm glibc-devel.i686 glibc-devel 1> /dev/null
    ln -s /usr/bin/cmake3 /usr/bin/cmake 1> /dev/null
    ln -s /usr/bin/cmake3-gui /usr/bin/cmake-gui 1> /dev/null

echo " "
echo "Boost"
echo " "
    yum -y install xerces-c-devel boost boost-devel boost-locale boost-atomic boost-chrono boost-date-time boost-filesystem boost-program-options boost-regex boost-serialization boost-system libboost-thread fltk-devel fltk-fluid zlib 1> /dev/null
    
echo " "
echo "ITC Common and NOS Engine"
echo " "
    find $DIR/support/packages/centos/ -name \*_amd64.rpm -exec rpm -U --force '{}' + 
    yum -y update 1> /dev/null

echo " "
echo "42"
echo " "
    yum -y install freeglut freeglut-devel mesa-libGL mesa-libGL-devel git git-core 1> /dev/null
    cd /opt 
    sudo chown -R $NOS3_USER:$NOS3_USER /opt
    # Pull the 42 repo at a known, working version (a83d449... ParmLoad Bug Fix)
sudo su $NOS3_USER << `EOF`
    cd /opt 
    git clone https://github.com/ericstoneking/42.git
    git -C 42 checkout a83d449c155c5af8fec505c6271c6ae09b924c06
`EOF`
    chown -R $NOS3_USER:$NOS3_USER /opt/42
    make -C 42 NOS3FSWFLAG='-D _ENABLE_NOS3_FSW_'
    chmod 755 42/42
    chmod -R ugo+w 42/InOut 

echo " "
echo "Fixes and Environmental Varibles"
echo " "
    grep msg_max /etc/sysctl.conf &> /dev/null || echo 'fs.mqueue.msg_max=100' >> /etc/sysctl.conf
    echo "handle all ignore nostop noprint" > /home/$STF_USER/.gdbinit

echo " "
echo "Setup Environment"
echo " "
    test -e /home/$STF_USER/Desktop || mkdir /home/$STF_USER/Desktop
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop; chmod 755 /home/$STF_USER/Desktop
    echo " "
    echo "    NOS3"
    echo " "
    # Additional Libraries
        # SOFA_C
        echo "--- SOFA_C"
        cp -R $DIR/support/packages/libraries/sofa_c-20150209_a /usr/local/lib/
        chown -R $STF_USER:$STF_USER /usr/local/lib/sofa_c-20150209_a
        mkdir /usr/local/lib/sofa_c-20150209_a/build
        cd /usr/local/lib/sofa_c-20150209_a/build
        cmake ../ 1> /dev/null
        make 1> /dev/null
        chown -R $STF_USER:$STF_USER /usr/local/lib/sofa_c-20150209_a/*
        # Geotrans3.5
        echo "--- Geotrans3.5"
        cp -R $DIR/support/packages/libraries/geotrans3.5 /usr/local/lib/
        chown -R $STF_USER:$STF_USER /usr/local/lib/geotrans3.5
        mkdir /usr/local/lib/geotrans3.5/build
        cd /usr/local/lib/geotrans3.5/build
        cmake ../ 1> /dev/null
        make 1> /dev/null
        chown -R $STF_USER:$STF_USER /usr/local/lib/geotrans3.5/*
        # Google Test
        echo "--- Google Test"
        cd /usr/local/lib/
        test -e release-1.8.0.tar.gz || wget --quiet https://github.com/google/googletest/archive/release-1.8.0.tar.gz
        test -e googletest-release-1.8.0 || tar -xzvf release-1.8.0.tar.gz 1> /dev/null
        chown -R $STF_USER:$STF_USER /usr/local/lib/googletest-release-1.8.0
        cd googletest-release-1.8.0/
        cmake . 1> /dev/null
        make install 1> /dev/null
        chown -R $STF_USER:$STF_USER /usr/local/lib/googletest-release-1.8.0/*
        echo "export GTEST_ROOT=/usr/local/lib/googletest-release-1.8.0" >> /etc/profile.d/all_users.sh
    echo "--- Scripts"
    # Build
    cp $DIR/support/VirtualMachine/scripts/nos3-build.sh /home/$STF_USER/Desktop 
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop/nos3-build.sh 
    chmod 755 /home/$STF_USER/Desktop/nos3-build.sh
    dos2unix /home/$STF_USER/Desktop/nos3-build.sh 
    # Run
    cp $DIR/support/VirtualMachine/scripts/nos3-run.sh /home/$STF_USER/Desktop
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop/nos3-run.sh 
    chmod 755 /home/$STF_USER/Desktop/nos3-run.sh 
    dos2unix /home/$STF_USER/Desktop/nos3-run.sh
    # Stop
    cp $DIR/support/VirtualMachine/scripts/nos3-stop.sh /home/$STF_USER/Desktop 
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop/nos3-stop.sh 
    chmod 755 /home/$STF_USER/Desktop/nos3-stop.sh 
    dos2unix /home/$STF_USER/Desktop/nos3-stop.sh
    echo " "
    echo "    42"
    echo " "
    mkdir -p /home/$STF_USER/Desktop/nos3-42
    cd /home/$STF_USER/Desktop/nos3-42
    test -e Model || ln -s /opt/42/Model
    test -e World || ln -s /opt/42/World
    test -e Kit || ln -s /opt/42/Kit
    cp -R $DIR/sims/sim_common/cfg/NOS3-42InOut .
    dos2unix NOS3-42InOut/*
    chown -R $STF_USER:$STF_USER /home/$STF_USER/Desktop/nos3-42

echo " "
echo "Cleanup"
echo " "
    # Reset archive directory
    #rm -r /var/cache/apt/archives
    #mkdir -p /var/cache/apt/archives/partial
    #touch /var/cache/apt/archives/lock
    #chmod 640 /var/cache/apt/archives/lock