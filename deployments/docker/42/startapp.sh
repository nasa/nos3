#!/bin/bash

export DISPLAY=:1

DIR=/opt/nasa-itc

xterm&

cd ${DIR} && git clone https://github.com/nasa-itc/42.git || true && \
cd ${DIR}/42 && \
  git fetch && \
  git checkout nos3-main && \
  make clean && \
  make -j7

cd ${DIR}/42 && xterm -e ./42 &
echo "Started 42 in xterm with PID $!"

xterm &

tail -f /dev/null

