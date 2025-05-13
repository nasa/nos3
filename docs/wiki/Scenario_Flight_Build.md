# Scenario - Flight Build

This scenario was developed to demonstrate the process of integrating a flight toolchain into the NOS3 environment.
While NOS3 does not support processor emulation, you can still build both for NOS3 and for other targets.

This scenario was last updated on 5/13/25 and leveraged the `dev` branch at the time [900f0e9].

## Learning Goals

By the end of this scenario, you should be able to:
* Integrate a flight toolchain into docker
* Build the develop docker container and select it for use
* Update the configuration files in NOS3 to build for another target
* Build flight software for the desired target

## Prerequisites

Before running the scenario, ensure the following steps are completed:
* [Getting Started](./Getting_Started.md)
  * [Installation](./Getting_Started.md#installation)
  * [Running](./Getting_Started.md#running)
* Clone [https://github.com/nasa-itc/deployment](https://github.com/nasa-itc/deployment) 
  * `main` branch commit [55f6b01] at the time of writing

## Walkthrough

Some terminology should be clarified before we begin:
* When mentioning a "target" this could be a specific setup for a development or the flight processor for example.
  * We'll be assuming this is the flight processor for this example.
* A toolchain for the target will be required to enable cross compilation.
  * Be sure to select the correct toolchain required for your particular effort!

Identifying the flight target is the first priority.
For this scenario we're going to be adding a Raspberry Pi (64-bit ARM) target.
A quick google search pointed me to the `gcc-arm-linux-gnueabihf` and `g++-arm-linux-gnueabihf` toolchains that are readily available for install from the package manager.

Installing the above toolchain directly to the VM or your host machine won't work for NOS3 as we leverage docker containers to build and run everything.
Let's start by editing what is going to be included in this docker file.
As a prerequisite we asked you clone the nasa-itc/deployment repository, open the `./deployment/Dockerfile` for editing.

The [DockerFile](https://github.com/nasa-itc/deployment/blob/2ec5f5748cbca37e00e7b21b0b7084e50df077f7/Dockerfile) is using staged builds in order to expedite testing as new things are added.
The start of each stage begins with the `FROM` statement.
At the time of writing, see above link, we have the following breakdown:
* nos0 - initial packages installed via apt-get and pip3
* nos1 - installation of the NOS3 middleware, NOS Engine and ITC Common
* nos2 - installation of CryptoLib

We're going to extend this existing file with the following:
```
# Add the toolchain for the Raspberry Pi
FROM nos2 AS nos3
RUN dpkg --add-architecture arm64 \
    && apt-get update -y \
    && apt-get install -y \
        gcc-arm-linux-gnueabihf \
        g++-arm-linux-gnueabihf \
    && rm -rf /var/lib/apt/lists/*
```

Once we'ved added and saved the above, we can follow the steps at the top of the Dockerfile from a terminal to build this container locally:
* cd deployment
* docker build -t rpi_flight .

We can go ahead and start editing the files in NOS3 while that container builds as it takes awhile.
From the NOS3 repo edit the following:
* [./scripts/env.sh](https://github.com/nasa/nos3/blob/900f0e9eb5754014cec1a43fb630adae6d93bec5/scripts/env.sh#L51)
  * The `DBOX` line should be updated to use the new box name and version
  * `DBOX="rpi_flight:latest"`
* Copy [././cfg/nos3_defs/toolchain-amd64-posix.cmake](https://github.com/nasa/nos3/blob/900f0e9eb5754014cec1a43fb630adae6d93bec5/cfg/nos3_defs/toolchain-amd64-posix.cmake) in place and name it `toolchain-arm64-posix.cmake`
  * Edit the `CMAKE_C_COMPILER` to be `/usr/bin/arm-linux-gnueabihf-gcc`
  * Edit the `CMAKE_CXX_COMPILER` to be `/usr/bin/arm-linux-gnueabihf-g++`
  * Edit the `CI_TRANSPORT` to be `udp`
  * Note that because our example RPI is running posix linux we do not need to change anything else in this file
    * If your target isn't you'd need to edit the `OSAL_SYSTEM_OSTYPE` and review the additional :Build Specific" section of this file
* [./cfg/nos3_defs/targets.cmake](https://github.com/nasa/nos3/blob/900f0e9eb5754014cec1a43fb630adae6d93bec5/cfg/nos3_defs/targets.cmake)
  * This file lists all the libraries and applications included in the build
  * The NOS3 spacecraft configuration file simply changes what is run, but we build everything every time to keep things easy with dependencies
  * Replace the bottom part of this file with the following adding the additional cpu2 target:
```
# Each target board can have its own HW arch selection and set of included apps
SET(MISSION_CPUNAMES cpu1 cpu2)

# NASA Operational Simulator for Small Satellites (NOS3) - Host Linux
SET(cpu1_PROCESSORID 1)
SET(cpu1_APPLIST) # Note: Using all ${MISSION_GLOBAL_APPLIST} automatically
SET(cpu1_FILELIST cfe_es_startup.scr)
if (ENABLE_UNIT_TESTS)
    SET(cpu1_SYSTEM amd64-posix)
else() 
    SET(cpu1_SYSTEM amd64-nos3)
endif()

# RPI Flight Target
SET(cpu2_PROCESSORID 2)
SET(cpu2_APPLIST)
SET(cpu2_FILELIST cfe_es_startup.scr)
SET(cpu2_SYSTEM arm64-posix)
```

Additionally we need to copy each `./cfg/nos3_defs/cpu1*` file in the same directory with the `cpu2*` prefix instead.
This will enable us to configure each of those specifically for our flight target is it is likely different that our NOS3 build (cpu1).
Note that for this RPI example we will need to edit the likely don't need to modify any of these to make it work, but this has yet to be confirmed.

Hopefully at this point your docker container is now built and ready for use.
From our terminal you can simply:
* make clean
* make

Attempting to build for this flight target will fail due to additional missing dependencies and new warnings being treated as errors.
One would need to build the required packaged missing from the cross compilation or find a means to install them for use to proceed.

Assuming you can work through those issues or reduce the current NOS3 build down to the bare minimum the output files that could be executed on the RPI would be at `./fsw/build/exe/cpu2`.
Note you'll need everything in that directory and subdirectories in addition to launching cFS from that location.
