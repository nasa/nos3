#!/bin/bash -x

cd /home/nos3/builds/nos3/sims/build/bin

./nos3-single-simulator -f ./nos3-simulator.xml generic-radio-sim &

tail -f /dev/null