#!/bin/sh
#
# Convenience script for NOS3 simulator development
#

ICONIC='-iconic'

# Update the following path if build path modified
DIR=/home/nos3/Desktop/nos3-build

# Update the KubOS repo path if you wish to use a modified version
KUBOS=/home/nos3/kubos

cd ~/Desktop/nos3-42
gnome-terminal -t '42 Dynamic Simulator' -e '/opt/42/42 NOS3-42InOut' &

sleep 5

cd ~/Desktop/nos3-build/bin
gnome-terminal \
--tab -t 'NOS Engine Standalone Server' -e 'nos_engine_server_standalone -f /home/nos3/Desktop/nos3-build/bin/nos_engine_server_simulator_config.json' \
--tab -t 'NOS Time Driver' -e '/home/nos3/Desktop/nos3-build/bin/nos-time-driver' \
--tab -t 'Simulator Terminal' -e '/home/nos3/Desktop/nos3-build/bin/nos3-simulator-terminal' \
--tab -t 'GPS Simulator' -e '/home/nos3/Desktop/nos3-build/bin/nos3-gps-simulator' &
# For future compatibility
# --tab -t 'GPS Simulator' -e '/home/nos3/Desktop/nos3-build/bin/nos3-gps-simulator' \
# --tab -t 'EPS Simulator' -e '/home/nos3/Desktop/nos3-build/bin/nos3-eps-simulator --config eps.json'  \
# --tab -t 'AntS Simulator' -e '/home/nos3/Desktop/nos3-build/bin/nos3-ants-simulator' &

sleep 10

cd $KUBOS/target/debug
gnome-terminal \
--tab -t 'Novatel GPS Service' -e '$KUBOS/target/debug/novatel-oem6-service' \
--tab -t 'Clyde EPS Service' -e '$KUBOS/target/debug/clyde-3g-eps-service' \
--tab -t 'ISIS AntS Service' -e '$KUBOS/target/debug/isis-ants-service' &
