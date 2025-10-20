# Objective

This directory contains the capability to containerize both [openmct-yamcs](https://github.com/akhenry/openmct-yamcs)  and [yamcs](https://github.com/yamcs/yamcs) in two separate containers which are on the same container network

`make`
returns

```bash
#---------------------------------------------------------------------------------------- 
# Makefile targets                                                                        
#---------------------------------------------------------------------------------------- 

#-target-----------------------description----------------------------------------------- 
all-build                      build essentials
all-down                       Bring down all containers
all-pull                       Pre-pull images for openmct and yamcs, because in proxied enviro dockerfile cannot find docker.io!
all-up                         Bring up essentials in detached state (attached state doesn't work for some reason)
env-create                     Create .env file 
openmct-build                  Build openmct
openmct-down                   Bring down openmct
openmct-pull                   Pull ubuntu image for openmct, because in proxied enviro dockerfile cannot find docker.io!
openmct-up                     Bring up openmct
purge-containers               WARNING: purge docker/podman images, cache, volumes, and networks
yamcs-build                    Build yamcs
yamcs-down                     Bring down yamcs
yamcs-pull                     Pull maven image for yamcs, because in proxied enviro dockerfile cannot find docker.io!
yamcs-simulator                Run yamcs simulator
yamcs-up                       Bring up yamcs
```

`task` 

returns

```bash
* all-build:              Build all images
* all-down:               Bring down all containers
* all-pull:               Pre-pull images for openmct and yamcs, because in proxied enviro dockerfile cannot find docker.io!
* all-up:                 Bring up all containers
* default:                Shows this help message
* env-create:             Create .env file
* openmct-build:          Build openmct
* openmct-down:           Bring down openmct
* openmct-pull:           Pull ubuntu image for openmct, because in proxied enviro dockerfile cannot find docker.io!
* openmct-up:             Bring up openmct
* purge-containers:       WARNING: purge docker/podman images, cache, volumes, and networks
* setup-maven:            Setup maven repository
* yamcs-build:            Build yamcs
* yamcs-down:             Bring down yamcs
* yamcs-pull:             Pull maven image for yamcs, because in proxied enviro dockerfile cannot find docker.io!
* yamcs-simulator:        Run yamcs simulator
* yamcs-up:               Bring up yamcs
```

```bash
[+] Running 2/2
 ✔ deployments-yamcs            Built                                                                                                                                                                                                                                             
 ✔ Container deployments-yamcs  Running
task: [yamcs-simulator] docker exec -it deployments-yamcs bash -c "cd /opt/yamcs/quickstart && ./simulator.py --rate=1"
Using playback rate of 1Hz, TM host=127.0.0.1, TM port=10015, TC host=127.0.0.1, TC port=10025
Sent: 35 packets. Received: 0 commands. Last command: None

```

```
make all-up
```
or

```
task all-up
```

open http://localhost:8090/ 
open http://localhost:9000/
