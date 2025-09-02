#!/bin/bash

DIR=/home/nos3

pkill -f ./nos3-single-simulator
pkill -f ./support/standalone

# Radio Sim and Cryptolib
cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-radio-sim 2>&1 \
    | tee -a ${DIR}/radio-sim.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/gsw/build && \
  ./support/standalone 2>&1 \
    | tee -a ${DIR}/cryptolib.log | tee -a ${DIR}/hw-components.log &

# HW Components
cd /home/nos3/builds/nos3/fsw/build/exe/cpu1 && \
  /home/nos3/builds/nos3/scripts/fsw/onair_launch.sh 2>&1 \
    | tee -a ${DIR}/onair_launch.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml camsim 2>&1 \
    | tee -a ${DIR}/camsim.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-css-sim 2>&1 \
    | tee -a ${DIR}/css.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-eps-sim 2>&1 \
    | tee -a ${DIR}/eps.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-fss-sim 2>&1 \
    | tee -a ${DIR}/fss.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml gps 2>&1 \
    | tee -a ${DIR}/gps.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-imu-sim 2>&1 \
    | tee -a ${DIR}/imu.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-mag-sim 2>&1 \
    | tee -a ${DIR}/mag.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim0 2>&1 \
    | tee -a ${DIR}/rw0.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim1 2>&1 \
    | tee -a ${DIR}/rw1.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim2 2>&1 \
    | tee -a ${DIR}/rw2.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml sample-sim 2>&1 \
    | tee -a ${DIR}/sample-sim.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-star-tracker-sim 2>&1 \
    | tee -a ${DIR}/star-tracker.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-thruster-sim 2>&1 \
    | tee -a ${DIR}/thruster.log | tee -a ${DIR}/hw-components.log &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-torquer-sim 2>&1 \
    | tee -a ${DIR}/torquer.log | tee -a ${DIR}/hw-components.log &

# cd /home/nos3/builds/nos3/sims/build/bin && \
#   ./nos3-single-simulator -f ./nos3-simulator.xml truth42sim 2>&1 \
#     | tee -a ${DIR}/truth42.log | tee -a ${DIR}/hw-components.log &

tail -f /dev/null
