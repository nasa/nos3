#!/bin/bash

rm -f /tmp/.X1-lock || true
rm -f /tmp/.X11-unix/X1 || true

pkill -f vncserver || true
pkill -f Xvnc || true
pkill -f websockify || true

/opt/TurboVNC/bin/vncserver -securitytypes tlsnone,x509none,none && \
  websockify -D \
    --web=/usr/share/novnc/ \
    --cert=~/novnc.pem 80 localhost:5901 
    
export DISPLAY=:1

DIR=/opt/nasa-itc

xterm&

#export GIT_COMMIT=858ea2ef6df1cb25537df3594463f55e92359cc0
export GIT_COMMIT=nos3-main

cd ${DIR} && git clone https://github.com/nasa-itc/42.git || true && \
cd ${DIR}/42 && \
  git fetch && \
  git checkout ${GIT_COMMIT} && \
  git submodule update && \
  make clean && \
  make -j7

cd ${DIR}/42 && xterm -e ./42 &
echo "Started 42 in xterm with PID $!"

xterm &

tail -f /dev/null


# /startapp.sh

# cd /tmp
# wget https://github.com/nasa-itc/deployment/raw/refs/heads/main/nos3_filestore/packages/ubuntu/itc-common-Release_1.10.2_amd64.deb && \
#   apt install ./itc-common-Release_1.10.2_amd64.deb && \
#   rm ./itc-common-Release_1.10.2_amd64.deb

# wget https://github.com/nasa-itc/deployment/raw/refs/heads/main/nos3_filestore/packages/ubuntu/nos-engine-Release_1.6.2_amd64.deb && \
#   apt install ./nos-engine-Release_1.6.2_amd64.deb && \
#   rm ./nos-engine-Release_1.6.2_amd64.deb

tail -f /dev/null