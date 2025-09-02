#!/bin/bash

cd /home/nos3/builds/nos3/fsw/build/exe/cpu1 && \
  /home/nos3/builds/nos3/scripts/fsw/onair_launch.sh &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml camsim &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-css-sim &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-eps-sim &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-fss-sim &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml gps &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-imu-sim  &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-mag-sim  &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim0 &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim1 &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-reactionwheel-sim2 &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml sample-sim &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-star-tracker-sim &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-thruster-sim &

cd /home/nos3/builds/nos3/sims/build/bin && \
  ./nos3-single-simulator -f ./nos3-simulator.xml generic-torquer-sim &

tail -f /dev/null
