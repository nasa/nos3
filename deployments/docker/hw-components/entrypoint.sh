#!/bin/bash

DIR=/home/nos3/

cd /home/nos3/builds/nos3/fsw/build/exe/cpu1 && \
  /home/nos3/builds/nos3/scripts/fsw/onair_launch.sh 2>&1 \
    | tee -a /home/nos3/onair_launch.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml camsim 2>&1 \
    | tee -a /home/nos3/camsim.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-css-sim 2>&1 \
    | tee -a /home/nos3/css.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-eps-sim 2>&1 \
    | tee -a /home/nos3/eps.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-fss-sim 2>&1 \
    | tee -a /home/nos3/fss.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml gps 2>&1 \
    | tee -a /home/nos3/gps.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-imu-sim 2>&1 \
    | tee -a /home/nos3/imu.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-mag-sim 2>&1 \
    | tee -a /home/nos3/mag.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim0 2>&1 \
    | tee -a /home/nos3/rw0.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim1 2>&1 \
    | tee -a /home/nos3/rw1.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim2 2>&1 \
    | tee -a /home/nos3/rw2.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml sample-sim 2>&1 \
    | tee -a /home/nos3/sample-sim.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-star-tracker-sim 2>&1 \
    | tee -a /home/nos3/star-tracker.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-thruster-sim 2>&1 \
    | tee -a /home/nos3/thruster.log | tee -a /home/nos3/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-torquer-sim 2>&1 \
    | tee -a /home/nos3/torquer.log | tee -a /home/nos3/hw-components.log &

tail -f /dev/null
