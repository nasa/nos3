#!/bin/bash -i
#
# Convenience script for NOS3 development
#

# # Function to check if a Firefox window exists
# wait_for_firefox_window() {
#   while ! wmctrl -l | grep -i "firefox" >/dev/null; do
#     sleep 0.5
#   done
# }

# # Run the wait function in the background
# wait_for_firefox_window &

# # Continue without holding the terminal and open a new tab when ready
# (
#   wait_for_firefox_window
#   firefox --new-tab "$1"
# ) &

export SC_NUM="sc_"1
export SC_NETNAME="nos3_"$SC_NUM

CFG_BUILD_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SCRIPT_DIR=$CFG_BUILD_DIR/../../scripts
source $SCRIPT_DIR/env.sh

echo "YAMCS Launch... "
# gnome-terminal --tab --title="YAMCS" -- $DFLAGS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -p 8090:8090 -p 5012:5012 --name cosmos_openc3-operator_1 -h cosmos --network=nos3_core --network-alias=cosmos -w $USER_NOS3_DIR/yamcs $DBOX mvn -Dmaven.repo.local=$USER_NOS3_DIR/.m2/repository yamcs:run


# --network=$SC_NETNAME -h nos_fsw (works below)
gnome-terminal --tab --title="YAMCS" -- $DFLAGS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -p 8090:8090 -p 5012:5012 --name yamcs-operator_1 --network=$SC_NETNAME -w $USER_NOS3_DIR/yamcs $DBOX mvn -Dmaven.repo.local=$USER_NOS3_DIR/.m2/repository yamcs:run

pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    # wait_for_firefox_window &
    sleep 20 && firefox --new-tab localhost:8090 &
fi



