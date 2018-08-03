#!/bin/bash
#
# Shell script to fully provision the NOS^3 64-bit VM on CentOS 7

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

echo " "
echo "--- "
echo "--- nos3_64_FULL.sh ---"
echo "--- "

# Initialize variables
if [[ -d /vagrant ]]; then
  STF_USER=nos3
else
  STF_USER=$(logname)
fi

DIR=/home/$STF_USER/nos3

echo " "
echo "ITOS Dependencies"
echo " "
    # Packages
    yum -y groupinstall @additional-devel @base @compat-libraries @core @debugging @desktop-debugging @development @dial-up @directory-client @emacs @fonts @gnome-apps @gnome-desktop @guest-desktop-agents @input-methods @internet-applications @internet-browser @java-platform @legacy-x @multimedia @network-file-system-client @performance @perl-runtime @postgresql @print-client @ruby-runtime @virtualization-client @virtualization-hypervisor @virtualization-tools @web-server @x11 1> /dev/null
    yum -y install chrony kexec-tools 1> /dev/null
    # 32-bit compatibility
    yum -y install glibc.i686 libgcc.i686 libstdc++.i686 openssl-libs.i686 expat.i686 audit-libs.i686 libcap-ng.i686 tcp_wrappers-libs.i686 libSM.i686 motif.i686 libXtst.i686 mesa-libGL.i686 libpng12.i686 1> /dev/null
    # For recreport
    yum -y install ghostscript texinfo-tex texlive-latex texlive-latex-bin texlive-titlesec texlive-mdwtools texlive-epsf 1> /dev/null
    # Network Authentication
    yum -y install openldap-clients sssd-client.i686 sssd-krb5-common sssd-krb5-common.i686 1> /dev/null

echo " "
echo "ITOS Install"
echo " "
    # Unpack
    test -e /opt/itos_8.24.0 || mkdir /opt/itos_8.24.0
    cd /opt/itos_8.24.0
    tar -xzvf /home/$STF_USER/nos3/support/packages/centos/ITOS-8.24.0-16233-RHEL6.tar.gz 1> /dev/null
    cp /home/$STF_USER/nos3/support/packages/centos/itos_license .
    cd ..
    ln -s itos_8.24.0 itos
    chown -R $STF_USER:$STF_USER itos_8.24.0

echo " "
echo "ITOS Post-Install"
echo " "
    # Fix VM time to allow 'Event Viewer' to function
    rm -f /etc/localtime
    cp /usr/share/zoneinfo/UTC /etc/localtime
    # Postgresql
    postgresql-setup initdb
    systemctl enable postgresql
    systemctl start postgresql 
    service postgresql start
    sudo -u postgres createuser -d -s -w vagrant
    sudo -u postgres createuser -d -s -w root
    sudo -u postgres createuser -d -s -w nos3
    cd /opt/itos/bin
    sudo -u postgres sh create_logging_database
    cd /var/lib/pgsql/data
    # Update config file
    echo "local    all    all                   trust" > /var/lib/pgsql/data/pg_hba.conf
    echo "host     all    all    127.0.01/32    trust" >> /var/lib/pgsql/data/pg_hba.conf
    echo "host     all    all    ::1/128        trust" >> /var/lib/pgsql/data/pg_hba.conf
    service postgresql restart
    echo "net.unix.max_dgram_qlen=4096" >> /etc/sysctl.conf
    # Install 32-bit JDK (version 1.7+)
    cd /opt/
    test -e jdk-8u141-linux-i586.tar.gz || wget --quiet --no-cookies --no-check-certificate --header "Cookie: gpw_e24=http%3A%2F%2Fwww.oracle.com%2F; oraclelicense=accept-securebackup-cookie" "http://download.oracle.com/otn-pub/java/jdk/8u141-b15/336fa29ff2bb4ef291e347e091f7f4a7/jdk-8u141-linux-i586.tar.gz" 1> /dev/null
    test -e jdk1.8.0_141 || tar xzf jdk-8u141-linux-i586.tar.gz 1> /dev/null
    cd /opt/jdk1.8.0_141/
    alternatives --install /usr/bin/java java /opt/jdk1.8.0_141/bin/java 2
    alternatives --set java /opt/jdk1.8.0_141/bin/java
    alternatives --install /usr/bin/jar jar /opt/jdk1.8.0_141/bin/jar 2
    alternatives --install /usr/bin/javac javac /opt/jdk1.8.0_141/bin/javac 2
    alternatives --set jar /opt/jdk1.8.0_141/bin/jar
    alternatives --set javac /opt/jdk1.8.0_141/bin/javac
    java -version
    echo "export JAVA_HOME=/opt/jdk1.8.0_141" >> /etc/profile.d/all_users.sh
    echo "export JRE_HOME=/opt/jdk1.8.0_141/jre" >> /etc/profile.d/all_users.sh
    echo "export PATH=$PATH:/opt/jdk1.8.0_141/bin:/opt/jdk1.8.0_141/jre/bin" >> /etc/profile.d/all_users.sh

echo " "
echo "Setup Environment"
echo " "
    test -e /home/$STF_USER/Desktop/itos/ || mkdir /home/$STF_USER/Desktop/itos/
    cp -R $DIR/support/itos/* /home/$STF_USER/Desktop/itos/
    echo "export PATH=$PATH:/opt/itos_8.24.0/bin" >> /etc/profile.d/all_users.sh
    export PATH=$PATH:/opt/itos_8.24.0/bin
    echo "export ITOS_DIR=/opt/itos_8.24.0" >> /etc/profile.d/all_users.sh
    echo "export ITOS_LICENSE_FILE=/opt/itos_8.24.0/itos_license" >> /etc/profile.d/all_users.sh
    export ITOS_LICENSE_FILE=/opt/itos_8.24.0/itos_license
    cd /home/$STF_USER/Desktop/itos
    sh makeodb &> makeodb_output.log
    chown -R $STF_USER:$STF_USER /home/$STF_USER/Desktop/itos
    # ITOS Update Script
    cp $DIR/support/VirtualMachine/scripts/itos-update.sh /home/$STF_USER/Desktop 
    chown $STF_USER:$STF_USER /home/$STF_USER/Desktop/itos-update.sh 
    chmod 755 /home/$STF_USER/Desktop/itos-update.sh 
    dos2unix /home/$STF_USER/Desktop/itos-update.sh

echo " "
echo "Cleanup"
echo " "
    # One of the group installs reinstalls the old version of cmake
    yum -y install cmake3 cmake3-gui
    rm -r /usr/bin/cmake /usr/bin/cmake-gui
    ln -s /usr/bin/cmake3 /usr/bin/cmake 1> /dev/null
    ln -s /usr/bin/cmake3-gui /usr/bin/cmake-gui 1> /dev/null