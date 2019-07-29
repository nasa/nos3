# NASA Operational Simulator for Small Satellites
The NASA Operational Simulator for Small Satellites (NOS3) is a suite of tools developed by NASA's Katherine Johnson Independent Verification and Validation (IV&V) Facility to aid in areas such as software development, integration & test (I&T), mission operations/training, verification and validation (V&V), and software systems check-out. 
NOS3 provides a software development environment, a multi-target build system, an operator interface/ground station, dynamics and environment simulations, and software-based hardware models.

## Getting Started
This repository allows the user to generate a VM in which to develop and test utilizing Vagrant and Virtual Box enabling every user to have an identical environment.

Various open source packages have been utilized and will be installed, these are explained minimally below:
* 42 - Spacecraft dynamics and visualization, NASA GSFC
* AMMOS Instrument Toolkit (AIT) - Ground Station from NASA JPL
* cFS - core Flight System, NASA GSFC
* COSMOS - Ground Station from Ball Aerospace
* ITC Common - Loggers and developer tools, NASA IV&V ITC  
* NOS Engine - Middleware bus simulator, NASA IV&V ITC

### Prerequisites
Each of the applications listed below are required prior to performing the installation procedure:
* [Git 1.8+](https://git-scm.com/)
* [Vagrant 2.2.3+](https://www.vagrantup.com/)
* [VirtualBox 6.0+](https://www.virtualbox.org/)

### Installing
1. Open a terminal
2. Navigate to the desired location for the repository
3. Clone the repository
4. Navigate to `/nos3/support`
5. Run `vagrant up` and wait to return to a prompt
	- This can take anywhere from 20 minutes to hours depending on internet speeds and host PC specs
  - The VM will reboot multiple times in order to finish install packages for you automatically so wait for that prompt!
6. Login to the nos3 user using the password `nos3123!` and get to work!
7. Try building and running following the instructions below

### Running
In both CentOS and Ubuntu, scripts are located on the desktop that enable the user to easily work:
* `nos3-build.sh` - Build FSW and simulators
* `nos3-run.sh` - Launch the ground station, FSW, and simulators
* `nos3-stop.sh` - Safely stop all components
Note that in Ubuntu the toolbar may be right clicked to select one of these options and defaults to simply running.

### Directory Layout
* `~/nos3/` contains the repository at the time of the build locally in the VM.
	- /apps - the open source cFS apps
	- /cfe - the core flight system (cFS) source files
	- /components - the hardware component apps
	- /osal - operating system abstraction layer (OSAL), enables building for linux and flight OS
	- /psp - platform support package (PSP), enables use on multiple types of boards
	- /sims - the component simulators
	- /support - all the files needed for ground stations, and installation
		* /ait - AIT database files
		* /cosmos - COSMOS database files
		* /installers - installation scripts
		* /packages	- installation packages
		* /planning - pass planning software
		* /VirtualMachine - files directly releated to the VM, such as desktop scripts and launchers
		* `Vagrantfile` - main provisioner file used to generate the VM
	- /tools - standard cFS provided tools
	- `.gitignore` - list of files and directories to be ommitted from git
	- `CMakeLists.txt` - top level make file wrapping CMake functions for ease of use
	- `README.md` - this file

## Development
Work occurs inside the VM, via shared folders, or on the host.
This repository is copied into the `~/nos3` directory on installation, but can be moved / shared to a different location.
Note that changing this directory will cause the scripts provided on the desktop to malfunction. 

### Git Flow
The following flow for development should be adhered to when possible:

* Master
    * Tested and verified using the approved test bed
    * Always demonstration ready and proven
    * Tagged releases should be from this branch

* Development
    * Always builds and runs
    * The head of the project with the most features
    * Commits direct to this branch are only to be hot fixes and are not desired

* Feature Branch
    * Have an issue describing the end goal and path forward
    * Based out of development and merged back in after testing

### Example Flow
Note that the git command line may be substituted for a GUI tool without issue.

1. Create an issue describing the goal and the path forward
2. Open a terminal
2. Navigate to the top level directory of the working tree.
4. Checkout the development branch
 - `git checkout development`
5. Create a new branch for development
 - `git checkout -b ##-Example-Issue`
6. Stage and commit work
  - `git add *`
	- `git commit -m "Detailed commit message explaining what changed"`
7. Commit as many times as needed
8. Checkout the development branch
 - `git checkout development`
9. Pull latest development branch
 - `git pull`
10. Merge feature branch into development
 - `git merge ##-Example-Issue`

## Support
If this project interests you or if you have any questions, please feel free to contact any developer directly or email `support@nos3.org`.

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
* Additional libraries / tools are needed for a specific mission, how can I include them during the provisioning process?
  - The `support/installers/$OS/*_CUSTOM.sh` script is designed specifically for this purpose
* How can I run 42 without the GUI?
  - Edit the `~/Desktop/nos3-42/NOS3-42InOut/Inp_Sim.txt` and set Graphics Front End to `FALSE` 
* NOS Engine Standalone server reports `NOSEngine.Uart - close uart port failed` error?
	- This isn't actually an error and is scheduled to be removed, proceed as usual.
* `core-linux` fails with a `__pthread_mutex_lock_full` error repeatedly?
	- This is a known issue that is being investigated, for now simply try to run `core-linux` again.
* `core-linux` hangs with a `NAV_LibInit()` call?
	- This is another known issue that is under investigation, for now simply try to run `core-linux` again.

### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.

### License
This project is licensed under the NOSA (NASA Open Source Agreement) License. 

### Acknowledgments
* Special thanks to all the developers involved!
