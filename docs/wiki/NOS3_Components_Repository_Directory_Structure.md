# Components
NOS3 is comprised of a core set of common code and a custom set of application/simulators.  To promote this separation between core and custom, NOS3 has a specific directory structure and makes extensive use of git submodules.

## Components, the foundation of customization

NOS3 has been reorganized around the foundational concept of a component.  The intent is that a spacecraft be made up of a core set of common functionality and then a custom set of components.  Each component is represented by a cFS application which is placed in an `fsw` subdirectory of the component.  In order for the COSMOS ground software to control the component application, a component has a collection of COSMOS command and telemetry tables which are placed in a `gsw` subdirectory of the component.  In many cases (but not all), a component is a hardware component on the spacecraft and thus it is appropriate to have a NOS3 hardware simulator for the component which is placed in a `sims` subdirectory of the component.

### How the build finds components
1.  Flight software:  Each component is listed in the fsw/nos3_defs/targets.cmake file.  The location of the components directory is specified by the CFS_APP_PATH environment variable in the top level Makefile.
2.  Simulators:  The sims/CMakeLists.txt file has logic to include all non-template simulators found in the components directory.

### How launch finds components
1.  Flight software:  Flight software is located at fsw/build/exe/cpuN/core-cpuN.  The cf subdirectory contains a shared object library for each flight software app built.  The cf subdirectory also contains the cfe_es_startup.scr startup script that lists the apps and components to execute as part of flight software; this script is created during the build process from startup scripts in the fsw/nos3_defs directory.
2.  Simulators:  The script gsw/scripts/launch.sh is hardwired with the list of simulators to launch.

### How COSMOS finds components
Relative paths are added to the file gsw/cosmos/config/system/MISSION_system.txt to locate the COSMOS command and telemetry definition files for each component.

## What to customize

Most of the subdirectories/submodules of the NOS3 repository are common core code that should not be modified.  The following is the list of custom subdirectories/submodules that should be customized based on the component applications/simulators used by your spacecraft.

1.  components
    1.  Add a repository submodule for each component that you need for your spacecraft
2.  fsw/nos3_defs
    1.  targets.cmake - Add the list of components to be built as part of flight software to this file
    2.  cpuN_cfe_es_startup.scr - Add the list of component apps to be run as part of flight software to this file
    3.  Customize other files in this directory as appropriate
3.  gsw/scripts/launch.sh - Add a line for each simulator to be executed
4.  sims/cfg
    1.  nos3-simulator.xml - Add a simulator block for each simulator to be executed (blocks for core simulators like truth42sim, time driver, and terminal should also be present)
    2.  InOut - Customize the files in this directory to control the 42 dynamic simulator and to connect the 42 dynamic simulator to hardware simulators (Inp_IPC.txt)
5.  gsw/cosmos/config/
    1.  system/MISSION_system.txt - declare targets for the component gsw directories


## The Virtual Machine

The NOS3 repository contains a Vagrantfile for provisioning the NOS3 virtual machine.  This Vagrantfile is extremely simplistic and makes use of prebuilt virtual machines.  These prebuilt virtual machines are created from community provided Ubuntu Linux, Oracle Linux, and Rocky Linux baseboxes by an extensive provisioning process but are then stored for ease of provisioning from this repository.  For more information on the extensive provisioning process, please refer to the [NOS3 deployment repository](https://github.com/nasa-itc/deployment).  This repository was once a submodule of the main NOS3 repository, but is no longer since the prebuilt virtual machines are now used. 


## Directory Structure

Do not modify these files/directories/submodules other than as specified in _What to customize_ above.

* Vagrantfile - file for creating NOS3 virtual machine using Vagrant and Virtual Box
* Makefile - top level Makefile with convenience targets for preparing the build, building, cleaning, launching and stopping
* components
  * ComponentSettings.cmake - Common build settings
  * One submodule per component that is part of the spacecraft/mission; each component has the following subdirectories:
    * fsw - application flight software
    * gsw - COSMOS command and telemetry tables
    * sim - hardware simulation software
* fsw
  * apps - core Flight Software applications
  * build - flight software build artifacts location
  * cfe - core Flight Executive
  * nos3_defs - Mission specific definitions.  These should be customized.
  * osal - Operating System Abstraction Layer
  * psp - Platform Support Package
  * tools - miscellaneous cFS tools
* gsw
  * ait - AIT configuration files
  * cosmos - COSMOS configuration files.  Only one file in here should be customized.
  * OrbitInviewPowerPrediction
  * scripts - Convenience scripts.  Only launch.sh should be customized.
* sims
  * build - simulations build artifacts location
  * cfg - configuration data for simulators and for the 42 dynamics simulator.  These should be customized.
  * nos_time_driver - core functionality to drive time throughout flight software, simulators, and 42
  * sim_common - core common framework code for implementing the plugin system and other core functionality of simulators
  * sim_server - core configuration file for the NOS Engine Standalone Server
  * sim_terminal - core functionality to provide a terminal for out of band control of hardware simulators
  * truth_42_sim - core functionality to forward 42 dynamical truth data to COSMOS for telemetry displays


