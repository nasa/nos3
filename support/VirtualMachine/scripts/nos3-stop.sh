#!/bin/sh
#
# Convenience script for NOS3 simulator development
#

# cFS
kill $(ps -ef | grep core-linux | head -1 | awk '{ print $2 }')
killall -9 core-linux.bin
killall -9 core-linux

# AIT
kill $(ps -ef | grep ait-gui | head -1 | awk '{ print $2 }')
killall socat
killall chrome
killall python

# COSMOS
killall ruby

# NOS3
killall -r -9 'nos3.*simulator.*'
killall nos_engine_server_standalone
killall nos-time-driver
killall 42

(sudo mount -t mqueue none /mnt; rm /mnt/*; sudo umount /mnt)

# kill_ipcs.sh
ME=`whoami`
IPCS_S=`ipcs -s | egrep "0x[0-9a-f]+ [0-9]+" | grep $ME | cut -f2 -d" "`
IPCS_M=`ipcs -m | egrep "0x[0-9a-f]+ [0-9]+" | grep $ME | cut -f2 -d" "`
IPCS_Q=`ipcs -q | egrep "0x[0-9a-f]+ [0-9]+" | grep $ME | cut -f2 -d" "`
for id in $IPCS_M; do
  ipcrm -m $id;
done
for id in $IPCS_S; do
  ipcrm -s $id;
done
for id in $IPCS_Q; do
  ipcrm -q $id;
done