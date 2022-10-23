# NASA Operational Simulator for Small Satellites
The NASA Operational Simulator for Small Satellites (NOS3) is a suite of tools developed by NASA's Katherine Johnson Independent Verification and Validation (IV&V) Facility to aid in areas such as software development, integration & test (I&T), mission operations/training, verification and validation (V&V), and software systems check-out. 
NOS3 provides a software development environment, a multi-target build system, an operator interface/ground station, dynamics and environment simulations, and software-based models of spacecraft hardware.

### Known Issues
1. Not all cFS delivered apps are included and supported at this time. Currently supported are: CI, TO, SCH, SC, HK, CFS_LIB
2. CentOS support not included in this release
These issues will be addressed in future updates

### Documentation
The best source of documentation can be found at [NOS3](www.nos3.org), as well as a PDF Users Guide attached to this Release on Github

### Prerequisites
Each of the applications listed below are required prior to performing the installation procedure:
* [Git 1.8+](https://git-scm.com/)
* [Vagrant 2.2.3+](https://www.vagrantup.com/)
* [VirtualBox 6.1+](https://www.virtualbox.org/)

### Installing
1. Open a terminal
2. Navigate to the desired location for the repository
3. Clone the repository `git clone https://github.com/nasa/nos3.git`
4. Navigate to `/nos3`
5. Clone the submodules `git submodule init` and `git submodule update` 
6. Navigate to `/nos3/deployment`
7. Run `vagrant up` and wait to return to a prompt
	- This can take anywhere from 20 minutes to hours depending on internet speeds and host PC specs
  	- The VM will reboot multiple times in order to finish install packages for you automatically so wait for that prompt!
	- **Sometimes ansible does not seem to install and there is an error like "Could not get lock /var/lib/apt/lists/lock".  If this happens run `vagrant provision` to install ansible and provision.**
8. Login to the nos3 user using the password `nos3123!` and get to work!
9. Try building and running following the instructions below

### Getting started
It is recommended to share the nos3 repository into the virtual machine
1. Open a terminal
2. To build use the `make` command from the nos3 repo
3. To run nos3 use the `make launch` command from the nos3 repo
4. To halt nos3 use the `make stop` command from the nos3 repo

### Directory Layout
* `/nos3/deployment` contains the repository for generating the nos3 virtual environment
* `/nos3/fsw` contains the repositories needed to build cFS FSW
	- /apps - the open source cFS apps
	- /cfe - the core flight system (cFS) source files
	- /components - the hardware component apps
	- /osal - operating system abstraction layer (OSAL), enables building for linux and flight OS
	- /psp - platform support package (PSP), enables use on multiple types of boards
	- /tools - standard cFS provided tools
* `/nos3/gsw` contains the nos3 ground station files, and other ground based tools
	- /ait - Ammos Instrument Toolkit (Untested for 1.05.0)
	- /cosmos - COSMOS files
	- /OrbitInviewPowerPrediction - OIPP tool for operators
	- /scripts - convience scripts
* `/nos3/sims` contains the nos3 simulators and configuration files
	- /cfg - 42 files and NOS3 top level config file
	- /_sim - a component simulator
	- /nos_time_driver - time syncronization for all components
	- /sim_common - common files used by component simulators
	- /sim_server - NOS Engine Server config and build files
	- /sim_terminal - terminal for testing on NOS Engine busses

## Support
If this project interests you or if you have any questions, please feel free to contact any developer directly or email `support@nos3.org`.

## Reporting Issues
Please report issues to the tracking system on Github [NOS3 Issues](www.github.com/nasa/nos3/issues)

### Frequently Asked Questions
* A GUI environment hasn't shown up after an extended period (> 1.5 hours), what should I do?
  - Stop the provision, delete the existing VM (if it was created), and try again
  - `CTRL + C`, `vagrant destroy`, `y`, `vagrant up`, wait, `vagrant reload`
* What is the root username and password?
  - `vagrant` with password `vagrant`
* Why doesn't the shared clipboard work?
  - You will most likely need to re-install / update the guest additions and reboot for this to function properly
  - In the VirtualBox menu select: Devices -> Insert Guest Additions CD Image...
  - Follow the instructions provided
* How can I mount a shared folder so that I edit on my host and compile / run in the VM?
  - In the VirtualBox menu select: Devices -> Shared Folders -> Shared Folders Settings...
  - Select the folder with a plus sign to add your folder
	  * Provide the path, name, mount point inside the VM
		* Select `Auto-mount`, `Make Permanent`, and `OK`
* How can I run 42 without the GUI?
  - Edit the `/nos3/sims/cfg/InOut/Inp_Sim.txt` and set Graphics Front End to `FALSE` 
* NOS Engine Standalone server reports `NOSEngine.Uart - close uart port failed` error?
	- This isn't actually an error and is scheduled to be removed, proceed as usual.

### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.

### License
This project is licensed under the NOSA (NASA Open Source Agreement) License. 

### Acknowledgments
* Special thanks to all the developers involved!

