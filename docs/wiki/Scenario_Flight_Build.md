# Scenario - Flight Build

This scenario was developed to demonstrate the process of integrating a flight toolchain into the NOS3 environment.
While NOS3 does not support processor emulation, you can still build both for NOS3 and for other targets.

This scenario was last updated on 5/22/25 and leveraged the `dev` branch [a3e7c100].

## Learning Goals

By the end of this scenario, you should be able to:
* Integrate a flight toolchain into docker.
* Build the develop docker container and select it for use.
* Update the configuration files in NOS3 to build for another target.
* Build flight software for the desired target.

## Prerequisites

Before running the scenario, complete the following steps:
* [Getting Started](./NOS3_Getting_Started.md)
  * [Installation](./NOS3_Getting_Started.md#installation)
  * [Running](./NOS3_Getting_Started.md#running)
* Clone [https://github.com/nasa-itc/deployment](https://github.com/nasa-itc/deployment) 
  * `main` branch commit [55f6b01] at the time of writing

## Walkthrough

Some terminology should be clarified before we begin:
* A "target" could refer to a specific setup for development or to the flight processor.
  * In this example, we'll use "target" to refer to the flight processor.
* A toolchain for the target will be required to enable cross compilation.
  * Be sure to select the correct toolchain required for your particular effort!

---
### Target Toolchain

Identifying the flight target is the first priority.
For this scenario we're going to be adding a Raspberry Pi (64-bit ARM) target.
The 64-bit compiler packages to use for this are `gcc-aarch64-linux-gnu` and `g++-aarch64-linux-gnu` toolchains; both are readily available for install from the package manager.

Installing the above toolchain directly to the VM or your host machine won't work for NOS3 as we leverage Docker containers to build and run everything.
Let's start by editing what is going to be included in this docker file.
As a prerequisite we asked you clone the nasa-itc/deployment repository, open the `./deployment/Dockerfile` for editing.

The [DockerFile](https://github.com/nasa-itc/deployment/blob/2ec5f5748cbca37e00e7b21b0b7084e50df077f7/Dockerfile) uses staged builds in order to expedite testing as new things are added:
* The start of each stage begins with the `FROM` statement.
* At the time of writing, see above link, we have the following breakdown:
  * nos0 - initial packages installed via apt-get and pip3
  * nos1 - installation of the NOS3 middleware, NOS Engine and ITC Common
  * nos2 - installation of CryptoLib

We're going to extend this existing file with the following stage:
```
# Add the toolchain for the Raspberry Pi
FROM nos2 AS nos3
RUN apt-get update -y \
    && apt-get install -y \
        gcc-arm-linux-gnueabihf \
        g++-arm-linux-gnueabihf \
    && rm -rf /var/lib/apt/lists/*
```

Once we've added and saved the above, we can follow the steps at the top of the Dockerfile from a terminal to build this container locally:
* cd deployment
* docker build -t rpi_flight .

---
### NOS3 Modifications

We can go ahead and start editing the files in NOS3 while that container builds as it takes awhile:
* From the NOS3 repo edit the following:
  * [./scripts/env.sh](https://github.com/nasa/nos3/blob/900f0e9eb5754014cec1a43fb630adae6d93bec5/scripts/env.sh#L51)
    * The `DBOX` line should be updated to use the new box name and version
    * `DBOX="rpi_flight:latest"`

Note that, from this point on, we don't technically have any other top level NOS3 modifications. We are directly modifying the underlying FSW.
This will enable us to still build and run NOS3 as we have, while also building FSW specifically for our desired flight target. This will permit the continuing use of NOS3 for development while also letting us test it on a development board or FlatSat as they are available for use.

---
### cFS Modifications

A few cFS modifications must be made to ensure we setup our new target:
* Copy [./cfg/nos3_defs/toolchain-amd64-posix.cmake](https://github.com/nasa/nos3/blob/900f0e9eb5754014cec1a43fb630adae6d93bec5/cfg/nos3_defs/toolchain-amd64-posix.cmake) in place and name it `toolchain-arm64-posix.cmake`.
  * Edit the `CMAKE_C_COMPILER` to be `/usr/bin/arm-linux-gnueabihf-gcc`.
  * Edit the `CMAKE_CXX_COMPILER` to be `/usr/bin/arm-linux-gnueabihf-g++`.
  * Note that because our example RPI is running posix linux we do not need to change anything else in this file.
    * If your target isn't running such an OS, you'd need to edit the `OSAL_SYSTEM_OSTYPE` and review the additional "Build Specific" section of this file.
* Modify [./cfg/nos3_defs/arch_build_custom.cmake](https://github.com/nasa/nos3/blob/900f0e9eb5754014cec1a43fb630adae6d93bec5/cfg/nos3_defs/arch_build_custom.cmake#L43):
  * Remove `-Werror` from the `CMAKE_C_FLAGS` string.
* Delete the following tables; without them here, cFS will leverage the defaults stored in each app:
  * `./cfg/nos3_defs/tables/sch_def_msgtbl.c`
  * `./cfg/nos3_defs/tables/sch_def_schtbl.c`
  * `./cfg/nos3_defs/tables/to_lab_sub.c`
* [./cfg/nos3_defs/targets.cmake](https://github.com/nasa/nos3/blob/900f0e9eb5754014cec1a43fb630adae6d93bec5/cfg/nos3_defs/targets.cmake)
  * This file lists all the libraries and applications included in the build.
  * The NOS3 spacecraft configuration file simply changes what is run, but we build everything every time to keep things easy with dependencies.
  * Replace this file with the one reproduced below, noting that a number of targets are commented out due to missing libraries needed on our flight target:
```
SET(MISSION_NAME "NOS3")

# SPACECRAFT_ID gets compiled into the build data structure and the PSP may use it.
# should be an integer.
SET(SPACECRAFT_ID 42)

# The "MISSION_GLOBAL_APPLIST" is a set of apps/libs that will be built
# for every defined target.  These are built as dynamic modules
# and must be loaded explicitly via startup script or command.
# This list is effectively appended to every TGTx_APPLIST in targets.cmake.
# Example:
list(APPEND MISSION_GLOBAL_APPLIST
    #
    # Libraries
    #
        #cryptolib
        #hwlib
        #io_lib
    #
    # cFS Apps
    #
        #cf
        #ci
        ci_lab
        #ds
        #fm
        #lc
        #sbn
        #sbn_tcp
        #sbn_client
        #sc
        sch
        #to
        to_lab
    #
    # Components
    #
        #arducam/fsw/cfs
        #generic_adcs/fsw/cfs
        #generic_css/fsw/cfs
        #generic_eps/fsw/cfs
        #generic_fss/fsw/cfs
        #generic_imu/fsw/cfs
        #generic_mag/fsw/cfs
        #generic_reaction_wheel/fsw/cfs
        #generic_radio/fsw/cfs
        #generic_star_tracker/fsw/cfs
        #generic_thruster/fsw/cfs
        #generic_torquer/fsw/cfs
        #mgr/fsw/cfs
        #novatel_oem615/fsw/cfs
        #onair
        #sample/fsw/cfs
        #syn/fsw/cfs
)

# Create Application Platform Include List
FOREACH(X ${MISSION_GLOBAL_APPLIST})
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/mission_inc)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/cfs/mission_inc)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/mission_inc)

    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/inc)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/cfs/inc)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/inc)

    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/platform_inc)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/cfs/platform_inc)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/platform_inc)

    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/public_inc)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/cfs/public_inc)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/public_inc)

    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/../shared)

    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/src)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/cfs/src)
    LIST(APPEND APPLICATION_PLATFORM_INC_LIST ${${X}_MISSION_DIR}/fsw/src)
ENDFOREACH(X)

# FT_INSTALL_SUBDIR indicates where the black box test data files (lua scripts) should
# be copied during the install process.
# Each target board can have its own HW arch selection and set of included apps
SET(MISSION_CPUNAMES cpu1 cpu2)

# NASA Operational Simulator for Space Systems (NOS3) - Host Linux
SET(cpu1_PROCESSORID 1)
SET(cpu1_APPLIST hwlib) # Note: Using all ${MISSION_GLOBAL_APPLIST} automatically
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

Additionally you need to copy each `./cfg/nos3_defs/cpu1*` file in the same directory with the `cpu2*` prefix instead.
This change of name will enable further configuration of those specifically for our flight target, necessary since it will be different than our NOS3 build (cpu1).

---
### Building the Flight Target

Hopefully at this point your docker container is now built and ready for use.
From our terminal at the top level of the NOS3 repository we've been editing, you can simply:
* make clean
* make

Both the NOS3 build (cpu1) and the flight target (cpu2) will be built for FSW with simulators also building for use with NOS3.
The NOS3 files can be found at `./fsw/build/exe/cpu1` while the flight target is at `./fsw/build/exe/cpu2`.
Note you'll need everything in that directory and subdirectories in addition to launching cFS from that location.

### Conclusion
Through this Scenario, you learned how to build NOS3 for use in flight - while also maintaining its availability for development.
