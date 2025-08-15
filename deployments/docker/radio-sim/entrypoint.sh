#!/bin/bash -x

cd /home/nos3/builds/nos3/sims/build/bin
pkill -f ./nos3-single-simulator
./nos3-single-simulator -f ./nos3-simulator.xml generic-radio-sim &

tail -f /dev/null
