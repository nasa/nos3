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
echo "Please enter 1 to confirm you have read the disclaimer:"
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
echo "Please be sure you are connected to the internet and have performed an apt-get update, apt-get upgrade before continuing."
echo " "
echo "Please enter 1 to confirm:"
echo " "
q2=("Y" "N")
select opt in "${q2[@]}"
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
echo "Please select the level of install:"
echo " "
q3=("MIN" "FULL" "DEV" "QUIT")
select opt in "${q3[@]}"
do
    case $opt in
        "MIN")
            export LEVEL=1
            break;;
        "FULL")
            export LEVEL=2
            break;;
        "DEV")
            export LEVEL=3
            break;;
        "QUIT")
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
cd installers
if [[ $LEVEL -ge 1 ]]; then 
    bash ./nos3_pi_32_MIN.sh
fi
if [[ $LEVEL -ge 2 ]]; then 
    bash ./nos3_pi_32_FULL.sh
fi
if [[ $LEVEL -ge 3 ]]; then 
    bash ./nos3_pi_32_DEV.sh
fi

echo " "
echo "Exiting the NOS3 installer now! NOS3 is ready for use!"