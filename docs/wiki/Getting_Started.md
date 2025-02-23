# Getting Started

## Installation

Each of the applications listed below are required prerequisites to performing the installation procedure:
* Option A, create a local virtual machine (VM)
  * [Git 2.47+](https://git-scm.com/)
  * [Vagrant 2.4.3+](https://www.vagrantup.com/)
  * [VirtualBox 7.1.6+](https://www.virtualbox.org/)
* Option B, you already use Linux or have a VM
  * [Git 2.47+](https://git-scm.com/)
  * Linux with docker and docker compose installed

Steps:
* Open a command prompt or terminal
* Clone the repository - `git clone https://github.com/nasa/nos3.git`
  * Note that by default the latest release or `main` branch is pulled
* Enter the repository - `cd nos3`
* Clone submodules - `git submodule update --init --recursive`
* Option A only
  * Run `vagrant up` and wait for a return prompt
    * This step can take minutes or hours depending on your internet speeds and host computer performance
  * Run `vagrant halt`
  * Close the command prompt or terminal
  * (Optional) Manually in VirtualBox increase the resources to up to half what is available
  * Start the VM directly in VirtualBox
  * Login to the `jstar` user with the password `jstar123!`
  * In the VirtualBox toolbar select `Devices > Upgrade Guest Additions...`
  * Reboot the VM and then try the building and running steps below

## Running

From inside your new VM or existing Linux environment:
* Open a terminal (CTRL + ALT + T)
* Navigate into the repository
  * For Option A - `cd ~/Desktop/github-nos3`
* Prepare the environment for use
  * `make prep`
* Build everything
  * `make`
* Run
  * `make launch`
* Explore
  * Send commands, monitor telemetry, change modes, ingest faults, etc.
* Stop
  * `make stop`
* Clean build files
  * `make clean`
* Modify the code as needed
* Do it all again starting at `make` until you have a custom mission

If you no longer want NOS3 installed, you can run `make uninstall` to undo the initial prep step.
