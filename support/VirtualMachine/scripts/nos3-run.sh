#!/bin/sh
#
# Convenience script for NOS3 simulator development
#

ICONIC='-iconic'

# Update the following path if build path modified
DIR=/home/nos3/Desktop/nos3-build

# Ground Station
    # COSMOS
        cd ~/Desktop/cosmos
        ruby Launcher &
        ruby tools/CmdTlmServer --config cmd_tlm_server.txt &
        ruby tools/CmdSender &
        ruby tools/DataViewer  --config data_viewer_mission.txt &

cd ~/Desktop/nos3-42
gnome-terminal -t '42 Dynamic Simulator' -e '/opt/42/42 NOS3-42InOut' &

sleep 5

cd ~/Desktop/nos3-build/bin
gnome-terminal \
--tab -t 'NOS Engine Standalone Server' -e 'nos_engine_server_standalone -f /home/nos3/Desktop/nos3-build/bin/nos_engine_server_simulator_config.json' \
--tab -t 'NOS Time Driver' -e '/home/nos3/Desktop/nos3-build/bin/nos-time-driver' \
--tab -t 'Simulator Terminal' -e '/home/nos3/Desktop/nos3-build/bin/nos3-simulator-terminal' \
--tab -t 'CAM Simulator' -e '/home/nos3/Desktop/nos3-build/bin/nos3-cam-simulator'  \
--tab -t 'GPS Simulator' -e '/home/nos3/Desktop/nos3-build/bin/nos3-gps-simulator' &

sleep 5

# Flight Software
cd $DIR/linux/linux
gnome-terminal -t 'NOS3 Flight Software' -e ~/Desktop/nos3-build/linux/linux/core-linux &
