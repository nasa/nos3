#!/bin/bash -x

cd /home/nos3/builds/nos3/sims/build/bin
pkill -f ./nos3-single-simulator
./nos3-single-simulator -f ./nos3-simulator.xml generic-radio-sim &

cd /home/nos3/builds/nos3/gsw/build
pkill -f ./support/standalone
./support/standalone &

cd -

tail -f /dev/null
