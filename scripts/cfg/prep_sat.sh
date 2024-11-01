#!/bin/bash

sudo ip addr add 10.10.10.101 dev eth1
sudo ip route add default via 10.10.10.101 dev eth1

# Open the relevant ports for a Docker overlay network?
# 2377, 4789, and 7946 might be all; the first and last
# on tcp, and the last two on udp.

