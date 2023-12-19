# NASA Operational Simulator for Small Satellites
The NASA Operational Simulator for Small Satellites (NOS3) is a suite of tools developed by NASA's Katherine Johnson Independent Verification and Validation (IV&V) Facility to aid in areas such as software development, integration & test (I&T), mission operations/training, verification and validation (V&V), and software systems check-out. 
NOS3 provides a software development environment, a multi-target build system, an operator interface/ground station, dynamics and environment simulations, and software-based models of spacecraft hardware.

## Documentation
The best source of documentation can be found at [the wiki](https://github.com/nasa/nos3/wiki) or [NOS3](http://www.nos3.org).

### Prerequisites
Each of the applications listed below are required prior to performing the installation procedure:
* Option A
  * [Git 2.36+](https://git-scm.com/)
  * Linux with docker and docker compose installed
* Option B
  * [Git 2.36+](https://git-scm.com/)
  * [Vagrant 2.3.4+](https://www.vagrantup.com/)
  * [VirtualBox 7.0+](https://www.virtualbox.org/)

### Installing
1. Clone the repository `git clone https://github.com/nasa/nos3.git`
2. `cd nos3`
3. Clone the submodules `git submodule update --init --recursive`
4. Run `vagrant up` and wait to return to a prompt
    - This can take anywhere from a few minutes to hours depending on internet speeds and host PC specs
5. In VirtualBox `Devices > Upgrade Guest Additions...`
	- Wait for this to complete
6. Run `vagrant reload` to finish the upgrade
7. Login to the jstar user using the password `jstar123!` and get to work!
8. Try building and running following the instructions below

### Getting started
By default the nos3 repository is shared into the virtual machine at `/home/jstar/Desktop/github-nos3`
1. Open a terminal
2. Navigate to the nos3 repository
  - `cd /home/jstar/Desktop/github-nos3`
3. Prepare the environment with COSMOS and docker containers
  - `make prep`
4. Build FSW, GSW, and SIMS
  - `make`
5. Run NOS3 including FSW, GSW, and SIMS
  - `make launch`
6. Stop NOS3
  - `make stop`
  - Note that COSMOS will remaining running in the background until `make stop-gsw` is done

### Directory Layout
* `components` contains the repositories for the hardware component apps
	- /fsw - cFS application
	- /gsw - OpenC3 COSMOS database
	- /sim - NOS3 simulator
	- /support - Optional folder containing a standalone checkout application
* `fsw` contains the repositories needed to build cFS FSW
	- /apps - the open source cFS apps
	- /cfe - the core flight system (cFS) source files
	- /nos3_defs - cFS definitions to configure cFS for NOS3
	- /osal - operating system abstraction layer (OSAL), enables building for linux and flight OS
	- /psp - platform support package (PSP), enables use on multiple types of boards
	- /tools - standard cFS provided tools
* `gsw` contains the nos3 ground station files, and other ground based tools
	- /cosmos - OpenC3 COSMOS files
	- /OrbitInviewPowerPrediction - OIPP tool for operators
	- /scripts - convenience scripts
* `sims` contains the nos3 simulators and configuration files
	- /cfg - 42 configuration files and NOS3 top level configuration files
	- /nos_time_driver - time syncronization for all components
	- /sim_common - common files used by component simulators including the files that define the simulator plugin architecture
	- /sim_terminal - terminal for testing on NOS Engine busses
	- /truth_42_sim - interface between 42 and OpenC3 COSMOS to provide dynamics truth data

### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.

### License
This project is licensed under the NOSA (NASA Open Source Agreement) License. 

# Issues and Features
Please report issues and request features on the GitHub tracking system - [NOS3 Issues](https://www.github.com/nasa/nos3/issues).

## Contributions
If you would like to contribute to the repository, please complete the [NOS3_Indv_CLA](./doc/NOS3_Indv_CLA.pdf) form and submit it to gsfc-softwarerequest@mail.nasa.gov with John.P.Lucas@nasa.gov copied. Next please create an issue capturing work to be done noting you intend to work it, a related branch, and submit a pull request when ready and we'll work to get it integrated.

## Support
If this project interests you or if you have any questions, please feel free to contact any developer directly or email `support@nos3.org`.
