#!/bin/bash
#
# Shell script to minimally provision the NOS^3 32-bit on a Raspberry Pi running:
#   raspbian-jessie
#
# Note that the commands in this script should be able to be run more than once with no ill
# effects, since this script could be run multiple times to provision/reprovision a machine.  
# If they cannot, they should be fixed!
#

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_pi_32_MIN.sh ---"
echo "--- "

# Initialize variables
STF_USER=$(users | awk '{print $1;}')

DIR=/home/$STF_USER/nos3
export BOOST_VERSION=1.55

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
    test -e /home/$STF_USER/nos3 || mkdir /home/$STF_USER/nos3
    cd ../../
    cp -R * /home/$STF_USER/nos3/
    chown -R $STF_USER:$STF_USER /home/$STF_USER/nos3
    chmod -R 755 /home/$STF_USER/nos3 

echo " "
echo "Modify files for the Pi"
echo " "
    sed -i 's/include(SetupGTest)//g' /home/$STF_USER/nos3/CMakeLists.txt
    sed -i "s#/home/nos3#/home/$STF_USER#g" /home/$STF_USER/nos3/support/VirtualMachine/scripts/nos3-run.sh 

echo " "
echo "Baseline"
echo " "
    apt-get update 1> /dev/null
    apt-get install -y binutils cmake cmake-qt-gui g++ build-essential google-mock libgtest-dev dos2unix gdb libpython3.4-dev python python-dev dwarves lcov gedit 1> /dev/null
    # Additional terminals
    apt-get install -y xterm gnome-terminal 1> /dev/null
    # gtest
    cd /usr/src/gtest
    sudo cmake . 1> /dev/null
    sudo make 1> /dev/null
    sudo mv libg* /usr/lib/ 
    # HWIL I2C Library
    apt-get install -y libi2c-dev 1> /dev/null
    apt-get autoremove -y 1> /dev/null
    # zip
    apt-get install -y zip unzip 1> /dev/null
    
echo " "
echo "Boost"
echo " "
    apt-get install -y libxerces-c-dev libboost-chrono-dev libboost-date-time$BOOST_VERSION-dev libboost-filesystem$BOOST_VERSION-dev libboost-program-options$BOOST_VERSION-dev libboost-program-options-dev libboost-regex$BOOST_VERSION-dev libboost-system$BOOST_VERSION-dev libboost-system-dev libboost-thread$BOOST_VERSION-dev libfltk1.3-dev 1> /dev/null

echo " "
echo "ITC Common and NOS Engine"
echo " "
    find $DIR/support/packages -name \*_armhf.deb -exec dpkg --install '{}' + 
    apt-get -f -y install 1> /dev/null

echo " "
echo "42"
echo " "
    apt-get -y install freeglut3 freeglut3-dev libgl1-mesa-dev 1> /dev/null
    cd /opt 
    git clone https://github.com/ericstoneking/42.git
    cd 42
    git reset --hard fe112678681bf752eb84fecf302b71117956846c
    sed -i -e 's/#NOS3FSW/NOS3FSW/; s/ARCHFLAG = /ARCHFLAG = -m32 /; s/LFLAGS = /LFLAGS = -m32 /;' Makefile
    make
    chmod -R ugo+w 42/InOut 
    chown -R $STF_USER:$STF_USER /opt/42 

echo " "
echo "Fixes and Environmental Varibles"
echo " "
    grep msg_max /etc/sysctl.conf &> /dev/null || echo 'fs.mqueue.msg_max=500' >> /etc/sysctl.conf
    echo "handle all ignore nostop noprint" > /home/$STF_USER/.gdbinit

echo " "
echo "Setup Environment"
echo " "
    test -e /home/$STF_USER/Desktop || mkdir /home/$STF_USER/Desktop
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop; chmod 755 /home/$STF_USER/Desktop
    echo " "
    echo "    NOS3"
    echo " "
    # Build
    cp $DIR/support/VirtualMachine/scripts/nos3-build.sh /home/$STF_USER/Desktop 
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop/nos3-build.sh 
    chmod 755 /home/$STF_USER/Desktop/nos3-build.sh
    chmod +x /home/$STF_USER/Desktop/nos3-build.sh
    dos2unix /home/$STF_USER/Desktop/nos3-build.sh 1> /dev/null
    # Clean
    cp $DIR/support/VirtualMachine/scripts/nos3-clean.sh /home/$NOS3_USER/Desktop
    chown $NOS3_USER:$NOS3_USER /home/$NOS3_USER/Desktop/nos3-clean.sh 
    chmod 755 /home/$NOS3_USER/Desktop/nos3-clean.sh 
    dos2unix /home/$NOS3_USER/Desktop/nos3-clean.sh 1> /dev/null
    # Run
    cp $DIR/support/VirtualMachine/scripts/nos3-run.sh /home/$STF_USER/Desktop
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop/nos3-run.sh 
    chmod 755 /home/$STF_USER/Desktop/nos3-run.sh 
    chmod +x /home/$STF_USER/Desktop/nos3-run.sh 
    dos2unix /home/$STF_USER/Desktop/nos3-run.sh 1> /dev/null
    # Stop
    cp $DIR/support/VirtualMachine/scripts/nos3-stop.sh /home/$STF_USER/Desktop 
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop/nos3-stop.sh 
    chmod 755 /home/$STF_USER/Desktop/nos3-stop.sh 
    chmod +x /home/$STF_USER/Desktop/nos3-stop.sh
    dos2unix /home/$STF_USER/Desktop/nos3-stop.sh 1> /dev/null
    echo " "
    echo "    42"
    echo " "
    mkdir -p /home/$STF_USER/Desktop/nos3-42
    cd /home/$STF_USER/Desktop/nos3-42
    test -e Model || ln -s /opt/42/Model
    test -e World || ln -s /opt/42/World
    test -e Kit || ln -s /opt/42/Kit
    cp -R $DIR/sims/sim_common/cfg/NOS3-42InOut .
    dos2unix NOS3-42InOut/* 1> /dev/null
    chown -R $STF_USER:$STF_USER /home/$STF_USER/Desktop/nos3-42
    # Disable graphics
    sed -i 's/TRUE                            !  Graphics Front End?/FALSE                         !  Graphics Front End?/g' /home/$STF_USER/Desktop/nos3-42/NOS3-42InOut/Inp_Sim.txt
    sed -i 's/TRUE                            !  Graphics Front End?/FALSE                         !  Graphics Front End?/g' /home/$STF_USER/Desktop/nos3-42/NOS3-42InOut-OpenSource/Inp_Sim.txt
echo " "

echo " "
echo "Cleanup"
echo " "
    apt-get -y autoremove 1> /dev/null
    # Reset archive directory
    rm -r /var/cache/apt/archives
    mkdir -p /var/cache/apt/archives/partial
    touch /var/cache/apt/archives/lock
    chmod 640 /var/cache/apt/archives/lock
