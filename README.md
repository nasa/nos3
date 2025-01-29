# NASA Operational Simulator for Small Satellites
The NASA Operational Simulator for Small Satellites (NOS3) is a suite of tools developed by NASA's Katherine Johnson Independent Verification and Validation (IV&V) Facility to aid in areas such as software development, integration & test (I&T), mission operations/training, verification and validation (V&V), and software systems check-out. 
NOS3 provides a software development environment, a multi-target build system, an operator interface/ground station, dynamics and environment simulations, and software-based models of spacecraft hardware.

## Documentation
The best source of documentation can be found at [NOS3 - ReadTheDocs](https://nos3.readthedocs.io/en/latest/).  The Wiki linked to the NOS3 repository is in the process of being deprecated.  While the information may be close to accurate there, it is not the source of truth, and will be removed in the near future.  Please refer to the ReadTheDocs page, or build the documentation locally yourself.

Documentation Dependencies:
> python3-sphinx, python3-sphinx-rtd-theme, phythin3-myst-parser

Build from the docs/wiki directory:  `make html`

---


### Prerequisites
Each of the applications listed below are required prior to performing the installation procedure:
* Option A, you already use Linux
  * [Git 2.36+](https://git-scm.com/)
  * Linux with docker and docker compose installed
* Option B, deployment of a virtual machine (VM)
  * [Git 2.36+](https://git-scm.com/)
  * [Vagrant 2.3.4+](https://www.vagrantup.com/)
  * [VirtualBox 7.0+](https://www.virtualbox.org/)

### Installing
Option B only.
Will provision a VM with all required packages installed to be used immediately.
1. Clone the repository `git clone https://github.com/nasa/nos3.git`
2. `cd nos3`
3. Clone the submodules `git submodule update --init --recursive`
4. Run `vagrant up` and wait to return to a prompt
    - This can take anywhere from a few minutes to hours depending on internet speeds and host PC specs
_It may also be wise at around this stage to shutdown the VM once it starts and to allocate it more resources if possible, preferably 8 cores and 16 GB of RAM._
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
  - Note that OpenC3, if in use, will remaining running in the background until `make stop-gsw` is done

To uninstall the hidden directories created, run `make uninstall`.

### Directory Layout
* `cfg` contains the configuration files for the mission and spacecraft
* `components` contains the repositories for the hardware component apps
* `docs` contains various documentation related to the project
* `fsw` contains the repositories needed to build cFS FSW
* `gsw` contains the nos3 ground station files, and other ground based tools
* `scripts` contains various convenience scripts
* `sims` contains the nos3 simulators and configuration files

### Versioning
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.

### License
This project is licensed under the NOSA 1.3 (NASA Open Source Agreement) License. 

# Issues and Features
Please report issues and request features on the GitHub tracking system - [NOS3 Issues](https://www.github.com/nasa/nos3/issues).

## Contributions
If you would like to contribute to the repository, please complete this [NASA Form][def] and submit it to gsfc-softwarerequest@mail.nasa.gov with John.P.Lucas@nasa.gov CC'ed.
Next, please create an issue describing the work to be performed noting that you intend to work it, create a related branch, and submit a pull request when ready. When complete, we will review and work to get it integrated.

## Support
If this project interests you or if you have any questions, please feel free to contact any developer directly or email `support@nos3.org`.


[def]: https://github.com/nasa/nos3/files/14578604/NOS3_Invd_CLA.pdf "NOS3 NASA Contributor Form PDF"