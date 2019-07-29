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
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

DIR=/home/$NOS3_USER/nos3

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

echo "Baseline..."
    yum -y -q install epel-release 1> /dev/null
    yum -y -q update 1> /dev/null
    yum -y -q install kernel-devel 1> /dev/null
    yum -y -q install gmock dpkg-devel dpkg-dev 1> /dev/null
    yum -y -q install binutils-devel cmake3 cmake3-gui gcc-c++ gmock gmock-devel gtest-devel dos2unix gdb python-libs python python-devel dwarves lcov gedit xterm glibc-devel libstdc++-devel zlib 1> /dev/null
    ln -sf /usr/bin/cmake3 /usr/bin/cmake 1> /dev/null
    ln -sf /usr/bin/cmake3-gui /usr/bin/cmake-gui 1> /dev/null

    # baseline - x86_64
    yum -y install fltk-devel fltk-fluid 1> /dev/null

    # baseline - i686
    yum -y install glibc-devel.i686 libstdc++-devel.i686 fltk-devel.i686 fltk-fluid.i686 1>/dev/null
    
    # googletest
    cd /usr/local/src/
    test -e release-1.8.0.tar.gz || wget --quiet https://github.com/google/googletest/archive/release-1.8.0.tar.gz
    test -e googletest-release-1.8.0 || tar -xzvf release-1.8.0.tar.gz 1> /dev/null
    cd googletest-release-1.8.0/
    # googletest - x86_64
    mkdir /usr/local/src/googletest-release-1.8.0/build
    cd /usr/local/src/googletest-release-1.8.0/build
    cmake .. 1> /dev/null
    make 1> /dev/null
    mv /usr/local/src/googletest-release-1.8.0/build/googlemock/libg* /usr/lib64/
    mv /usr/local/src/googletest-release-1.8.0/build/googlemock/gtest/libg* /usr/lib64/
    # googletest - i686
    mkdir /usr/local/src/googletest-release-1.8.0/build32
    cd /usr/local/src/googletest-release-1.8.0/build32
    cmake .. -DCMAKE_CXX_FLAGS=-m32 1> /dev/null
    make 1> /dev/null
    mv /usr/local/src/googletest-release-1.8.0/build32/googlemock/libg* /usr/lib/
    mv /usr/local/src/googletest-release-1.8.0/build32/googlemock/gtest/libg* /usr/lib/

echo "Boost..."
    # boost x86_64
    #yum -y install xerces-c xerces-c-devel boost boost-devel boost-locale boost-atomic boost-chrono boost-date-time boost-filesystem boost-program-options boost-regex boost-serialization boost-system boost-thread 1> /dev/null

    # boost i686
    yum -y install xerces-c.i686 xerces-c-devel.i686 boost.i686 boost-devel.i686 boost-locale.i686 boost-atomic.i686 boost-chrono.i686 boost-date-time.i686 boost-filesystem.i686 boost-program-options.i686 boost-regex.i686 boost-serialization.i686 boost-system.i686 boost-thread.i686 1> /dev/null
    
echo "ITC Common and NOS Engine..."
    find $DIR/support/packages/centos/ -name \*_i386.rpm -exec rpm -U --force '{}' + 
    yum -y update 1> /dev/null

echo "42..."
    # 42 i686 and x86_64
    yum -y install freeglut.i686 freeglut-devel.i686 mesa-libGL.i686 mesa-libGL-devel.i686 mesa-libGLU-devel.i686 mesa-dri-drivers.i686 1> /dev/null
    cd /opt 
    git clone --quiet https://github.com/ericstoneking/42.git
    cd 42
    git reset --hard --quiet fe112678681bf752eb84fecf302b71117956846c
    sed -i -e 's/#NOS3FSW/NOS3FSW/; s/ARCHFLAG = /ARCHFLAG = -m32 /; s/LFLAGS = /LFLAGS = -m32 /;' Makefile
    make &> /dev/null
    chmod -R ugo+w /opt/42/InOut 
    chown -R $NOS3_USER:$NOS3_USER /opt/42 

echo "Fixes and environmental variables..."
    grep msg_max /etc/sysctl.conf &> /dev/null || echo 'fs.mqueue.msg_max=500' >> /etc/sysctl.conf
    echo "handle all ignore nostop noprint" > /home/$NOS3_USER/.gdbinit
    # Fix cmake problems finding boost on CentOS 7:
    mkdir /usr/lib/i386-linux-gnu
    cd /usr/lib/i386-linux-gnu
    ln -s ../libboost_filesystem.so
    ln -s ../libboost_program_options.so
    ln -s ../libboost_system.so

echo "Environment setup..."
    test -e /home/$NOS3_USER/Desktop || mkdir /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop; chmod 755 /home/$NOS3_USER/Desktop
    # Additional Libraries
        # i386 build
        export CFLAGS=-m32
        export CXXFLAGS=-m32
        # SOFA_C
        cp -R $DIR/support/packages/libraries/sofa_c-20150209_a /usr/local/lib/
        chown -R $NOS3_USER:$NOS3_USER /usr/local/lib/sofa_c-20150209_a
        mkdir /usr/local/lib/sofa_c-20150209_a/build
        cd /usr/local/lib/sofa_c-20150209_a/build
        cmake .. 1> /dev/null
        make &> /dev/null
        chown -R $NOS3_USER:$NOS3_USER /usr/local/lib/sofa_c-20150209_a/*
        # Geotrans3.5
        cp -R $DIR/support/packages/libraries/geotrans3.5 /usr/local/lib/
        chown -R $NOS3_USER:$NOS3_USER /usr/local/lib/geotrans3.5
        mkdir /usr/local/lib/geotrans3.5/build
        cd /usr/local/lib/geotrans3.5/build
        cmake .. 1> /dev/null
        make &> /dev/null
        chown -R $NOS3_USER:$NOS3_USER /usr/local/lib/geotrans3.5/*
    # Build
    cp $DIR/support/VirtualMachine/scripts/nos3-build.sh /home/$NOS3_USER/Desktop 
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-build.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-build.sh
    dos2unix -q /home/$NOS3_USER/Desktop/nos3-build.sh 
    # Clean
    cp $DIR/support/VirtualMachine/scripts/nos3-clean.sh /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-clean.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-clean.sh 
    dos2unix -q /home/$NOS3_USER/Desktop/nos3-clean.sh
    # Run
    cp $DIR/support/VirtualMachine/scripts/nos3-run.sh /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-run.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-run.sh 
    dos2unix -q /home/$NOS3_USER/Desktop/nos3-run.sh
    # Stop
    cp $DIR/support/VirtualMachine/scripts/nos3-stop.sh /home/$NOS3_USER/Desktop 
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-stop.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-stop.sh 
    dos2unix -q /home/$NOS3_USER/Desktop/nos3-stop.sh
    # 42
    mkdir -p /home/$NOS3_USER/Desktop/nos3-42
    cd /home/$NOS3_USER/Desktop/nos3-42
    test -e Model || ln -s /opt/42/Model
    test -e World || ln -s /opt/42/World
    test -e Kit || ln -s /opt/42/Kit
    cp -R $DIR/sims/sim_common/cfg/NOS3-42InOut .
    dos2unix -q NOS3-42InOut/*
    chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-42

echo "Cleanup..."
# Rebuild yum database
cd /var/lib/rpm
rm -rf __db*
rpm --rebuilddb