#!/bin/bash

if [[ $UID != 0 ]]; then
    echo "This script MUST be run as superuser!  (e.g. sudo $0 $*)"
    exit 1
fi

# Initialize variables
if [[ -d /vagrant ]]; then
  NOS3_USER=nos3
else
  NOS3_USER=$(logname)
fi

FLAG="/home/nos3/AIT_FIRSTBOOT_COMPLETE.log"
U_FILE="/usr/local/bin/virtualenvwrapper.sh"
C_FILE="/usr/bin/virtualenvwrapper.sh"

if [ ! -f $FLAG ]; then
	echo "First Time Login - Setting up AIT"
	
	WORKON_HOME=/home/nos3/.virtualenvs
	PROJECT_HOME=/home/nos3/Devel
	if [ -f $U_FILE ]; then
		echo "Sourcing for Ubuntu"
		source $U_FILE
	else
	    echo "Sourcing for CentOS"
		source $C_FILE
	fi
	
	chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/.virtualenvs
	
	mkvirtualenv ait
	workon ait
	cd /home/$NOS3_USER/AIT/AIT-Core
	git checkout 1.3.0
	pip install . 
	cd /home/$NOS3_USER/AIT/AIT-GUI 
	git checkout 1.3.0
	pip install . 
	deactivate

	mkvirtualenv ait-cfs
	workon ait-cfs
	cd /home/$NOS3_USER/AIT/AIT-Core
	git checkout 1.3.0
	pip install . 
	cd /home/$NOS3_USER/AIT/AIT-GUI 
	git checkout 1.3.0
	pip install . 	
	cd /home/$NOS3_USER/AIT/AIT-CFS 
	pip install .
	deactivate

	echo "Copying CFS Telemetry & GUI Files"
	cp -rf /home/$NOS3_USER/nos3/support/ait/config/* /home/$NOS3_USER/AIT/AIT-CFS/config/
	cp -rf /home/$NOS3_USER/nos3/support/ait/gui/* /home/$NOS3_USER/AIT/AIT-CFS/gui/
    
	chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/.virtualenvs
	chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/AIT

	rm /home/nos3/.virtualenvs/postactivate
	cp /home/$NOS3_USER/postactivate /home/$NOS3_USER/.virtualenvs/postactivate
	rm -f /home/$NOS3_USER/postactivate
	chown -R $NOS3_USER:$NOS3_USER /home/$NOS3_USER/.virtualenvs/postactivate

	touch /home/$NOS3_USER/AIT_FIRSTBOOT_COMPLETE.log
	
else
	echo "AIT Already Setup!"
fi
