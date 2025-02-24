#!/bin/bash

sudo ip addr add 10.10.10.100 dev eth1
sudo ip route add default via 10.10.10.100 dev eth1

# The next thing to do is to 'create' a docker swarm. Swarm containers are not
# necessary here, but to connect the two computers it is necessary to run
# docker swarm init on the main computer (which I have been using as the gsw
# machine); then "docker swarm join" must be run on the satellite machine. 
# 
# The successful output of "docker swarm init" will give the command which must
# be run on the satellite VM.

docker swarm init --advertise-addr 10.10.10.100

