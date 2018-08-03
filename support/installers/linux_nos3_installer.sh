#!/bin/bash
# This script assumes it is located
# one level above the repo.
clear

cd $PWD/nos3/support/
echo " "
echo "Welcome to the Nasa Operational Simulator for Small Satellites (NOS3) Installer!"
echo " "
echo "----------------"
echo "-- Disclaimer --"
echo "----------------" 
echo "This software is provided ''as is'' without any warranty of any, kind either express, implied, or statutory, including, but not limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or any warranty that the software will be error free.  In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages, arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty, contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software, documentation or services provided hereunder."
echo " "
echo "Please enter `Y` to confirm you have read the disclaimer:"
echo " "
q1=("Y" "N")
select opt in "${q1[@]}"
do
    case $opt in
        "Y")
            break;;
        "N")
            exit 1
            break;;
    esac
done

echo "----------------"
echo " "
echo "Do you have virtualbox 5.1+ installed?"
echo " "
echo $(vboxmanage --version)
q2=("Y" "N")
select opt in "${q2[@]}"
do
    case $opt in
        "Y")
            break;;
        "N")
            echo " "
            echo "Please download from the following link and install manually, be sure to install dependencies!"
            echo "https://www.virtualbox.org/wiki/Downloads"
            sleep 30
            exit 1
            break;;
    esac
done

echo "----------------"
echo " "
echo "Do you have vagrant 1.9+ installed?"
echo " "
echo $(vagrant version)
q3=("Y" "N")
select opt in "${q3[@]}"
do
    case $opt in
        "Y")
            break;;
        "N")
            echo " "
            echo "Please download from the following link and install manually, be sure to install dependencies!"
            echo "https://www.vagrantup.com/docs/installation/"
            sleep 30
            exit 1
            break;;
    esac
done

echo " "
echo "Configuration complete, beginning install..."
echo " "
echo "This process may take time, but will complete automatically."
echo "Please wait to use NOS3 until this script is completed."
echo " "
vagrant up
vagrant reload

echo " "
echo "Exiting the NOS3 installer now! NOS3 is ready for use!"