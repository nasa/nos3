# NASA Operational Simulator for Small Satellites
The NASA Operational Simulator for Small Satellites (NOS3) is a suite of tools developed by NASA's Katherine Johnson Independent Verification and Validation (IV&V) Facility to aid in areas such as software development, integration & test (I&T), mission operations/training, verification and validation (V&V), and software systems check-out. 
NOS3 provides a software development environment, a multi-target build system, an operator interface/ground station, dynamics and environment simulations, and software-based models of spacecraft hardware.

## Documentation
The best source of documentation can be found at [the wiki](https://github.com/nasa/nos3/wiki) or [NOS3](http://www.nos3.org).

### Prerequisites
Each of the applications listed below are required prior to performing the installation procedure:
* [Git 1.8+](https://git-scm.com/)
* [Vagrant 2.2.3+](https://www.vagrantup.com/)
* [VirtualBox 6.1+](https://www.virtualbox.org/)

### Installing
1. Clone the repository `git clone https://github.com/nasa/nos3.git`
2. `cd nos3`
3. Clone the submodules `git submodule update --init --recursive`
4. Run `vagrant up` and wait to return to a prompt
    - This can take anywhere from a few minutes to hours depending on internet speeds and host PC specs
5. Login to the nos3 user using the password `nos3123!` and get to work!
6. Try building and running following the instructions below

### Getting started
It is recommended to share the nos3 repository into the virtual machine (e.g. `/home/nos3/Desktop/github-nos3`)
1. Open a terminal (to `/home/nos3/Desktop/github-nos3`)
2. To build use the `make` command from the nos3 repo
3. To run nos3 use the `make launch` command from the nos3 repo
4. To halt nos3 use the `make stop` command from the nos3 repo

### Directory Layout
* `components` contains the repositories for the hardware component apps; each repository contains the app, an associated sim, and COSMOS command and telemetry tables
* `fsw` contains the repositories needed to build cFS FSW
	- /apps - the open source cFS apps
	- /cfe - the core flight system (cFS) source files
	- /nos3_defs - cFS definitions to configure cFS for NOS3
	- /osal - operating system abstraction layer (OSAL), enables building for linux and flight OS
	- /psp - platform support package (PSP), enables use on multiple types of boards
	- /tools - standard cFS provided tools
* `gsw` contains the nos3 ground station files, and other ground based tools
	- /ait - Ammos Instrument Toolkit (Untested for 1.05.0)
	- /cosmos - COSMOS files
	- /OrbitInviewPowerPrediction - OIPP tool for operators
	- /scripts - convenience scripts
* `sims` contains the nos3 simulators and configuration files
	- /cfg - 42 configuration files and NOS3 top level configuration files
	- /nos_time_driver - time syncronization for all components
	- /sim_common - common files used by component simulators including the files that define the simulator plugin architecture
	- /sim_terminal - terminal for testing on NOS Engine busses
	- /truth_42_sim - interface between 42 and COSMOS to provide dynamics truth data to COSMOS

### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.

### License
This project is licensed under the NOSA (NASA Open Source Agreement) License. 

# Issues and Features
Please report issues and request features on the GitHub tracking system - [NOS3 Issues](https://www.github.com/nasa/nos3/issues).

## Support
If this project interests you or if you have any questions, please feel free to contact any developer directly or email `support@nos3.org`.
