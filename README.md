# NASA Operational Simulator for Small Satellites

## Getting Started
This repository allows the user to generate a VM in which to develop and test utilizing Vagrant and Virtual Box enabling every user to have an identical environment.

Various open source packages have been utilized and will be installed, these are explained minimally below:
* 42 - Spacecraft dynamics and visualization, NASA GSFC
* cFS - core Flight System, NASA GSFC
* COSMOS - Ball Aerospace
* ITC Common - Loggers and developer tools, NASA IV&V ITC  
* NOS Engine - Middleware bus simulator, NASA IV&V ITC

### Prerequisites
Each of the applications listed below are required prior to performing the installation procedure:
* [Git 1.8+](https://git-scm.com/)
* [Vagrant 1.9+](https://www.vagrantup.com/)
* [VirtualBox 5.1+](https://www.virtualbox.org/)

### Installing
1. Open a terminal
2. Navigate to the desired location for the repository
3. Clone the repository
4. Navigate to `/nos3/support`
5. Run `vagrant up` and wait to return to a prompt
6. Run `vagrant reload` and wait for the VM to restart
7. Login to the nos3 user using the password `nos3123!` and get to work!

### Running
In both CentOS and Ubuntu, scripts are located on the desktop that enable the user to easily work:
* `nos3-build.sh` - Build FSW and simulators
* `nos3-run.sh` - Launch the ground station, FSW, and simulators
* `nos3-stop.sh` - Safely stop all components

### Directory Layout
* `~/nos3/` contains the repository at the time of the build locally in the VM.
	* /apps - the open source cFS apps
	* /build - the unarchived build directory
	* /cfe - the core flight system (cFS) source files
	* /docs - documentation related to cFS
	* /osal - operating system abstraction layer (OSAL), enables building for linux and flight OS
	* /psp - platform support package (PSP), enables use on multiple types of boards
	* /support - all the files needed for ground stations, ION, and installation
		* /cosmos - COSMOS database files
		* /installers - installation scripts
		* /packages	- installation packages
		* /VirtualMachine - files directly releated to the VM, such as desktop scripts and launchers
		* `Vagrantfile` - main provisioner file used to generate the VM
	* /tools - standard cFS provided tools
	* `.gitignore` - list of files and directories to be ommitted from git
	* `.gitmodules` - list of git submodules utilized
	* `CMakeLists.txt` - top level cmake file to be used from inside the build directory
	* `README.md` - this file

## Development
Work occurs inside the VM, via shared folders, or on the host.  This repository is copied into the `~/nos3` directory on installation, but can be moved / shared to a different location.  Note that changing this directory will cause the scripts provided on the desktop to malfunction. 

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
    * Have a Gitlab issue describing the end goal and path forward
    * Based out of development and merged back in after testing

### Example Flow
Note that the git command line may be substituted for a GUI tool without issue.

1. Create GitLab issue describing the goal and the path forward
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

### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.

## Support
If this project interests you or if you have any questions, please feel free to contact any developer directly or email `support@nos3.org`.

### License
This project is licensed under the NOSA (NASA Open Source Agreement) License. 

### Acknowledgments
* Special thanks to all the developers involved!
