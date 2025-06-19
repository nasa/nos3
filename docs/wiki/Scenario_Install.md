# Scenario - Installation

This scenario was developed to capture the NASA Operational Simulator for Small Satellites (NOS3) installation process.

This scenario was last updated on 05/28/2025 and leveraged the `dev` branch [a3e7c100].

## Learning Goals
By the end of this scenario, you should be able to:
* Install NOS3 on your desired platform.
* Understand different ways to install and run NOS3.

## Prerequisites

There are no prerequisites for this scenario, but reading the Installation instructions may help: 
* [Getting Started](./NOS3_Getting_Started.md)
  * [Installation](./NOS3_Getting_Started.md#installation)

## Walkthrough

Two options are described for a NOS3 installation based on your current setup and desired ends:
* Option A is to create a local NOS3 virtual machine (VM).
  * This option is traditionally used by NOS3 developers as we are provided Windows machines for our work and may have multiple projects that require different environments.
  * Using one image for every VM ensures all prerequisites (such as Docker) are installed to begin with.
* Option B is to bring your own Linux.
  * This involves either creating your own VM or running NOS3 directly on a Linux computer. 
  * Depending on the flavor of Linux, this can be very challenging or only slightly more difficult than using a NOS3 VM.
  * Running directly without an additional virtualization layer may improve run speeds.

---
### Option A: create a local virtual machine (VM)
To run NOS3 in VirtualBox you will need to install Git, Vagrant, and VirtualBox.
* [Git 2.47+](https://git-scm.com/)
* [Vagrant 2.4.3+](https://www.vagrantup.com/)
* [VirtualBox 7.1.6+](https://www.virtualbox.org/)

#### Cloning the repository 
Clone and set up the directory using a Git Bash terminal:
* `git clone https://github.com/nasa/nos3.git`

![ScenarioInstallationClone](./_static/scenario_installation/scenario_installation_clone.png)

* `cd nos3`
* `git pull`
* `git submodule update --init --recursive`

![ScenarioInstallationSubmoduleUpdate](./_static/scenario_installation/scenario_installation_submodule_update.png)

You can run submodule update again if anything failed during the first attempt, if all successful it simply reports nothing as shown above.
Note that the repository we just cloned will be shared into the VM in the default setup.
Such a setup means you can edit the code locally on your host and then still build and run inside the Virtual Machine.
This can be useful in allowing a user to have the same IDE for multiple projects, but may require running `dos2unix` on certain files for everything to work properly. 


#### Deploying the VM
Inside the NOS3 repository we cloned, the [./Vagrantfile](https://github.com/nasa/nos3/blob/b76e6844b5c707af53d4265d93e7802872df88c0/Vagrantfile) contains the instructions Vagrant itself needs to create the VM.
Note that a number of these options are editable and may be modified before continuing further:
* `config.vm.synced_folder`
  * The NOS3 repository on the host is shared into the VM at this defined location.
* `config.vm.disk`
  * This is the size of the virtual disk used by the VM.
  * 32GB may be able to run NOS3, but 64GB+ is recommended if you are adding additional mission specifics or editing Docker files.  This is especially true because it is difficult to increase the size of a VM after its creation.
* `vbox.cpus`
  * While the default 4 CPUs will run, we'd recommend you set this to half your available CPU count of the host.
  * If you don't know what your CPU count is, continue and edit this after deployment - this is fairly simple to adjust afterwards.
* `vbox.memory`
  * While 8192 MB of memory will suffice, we'd recommend you double that or set it to half available on the host.
  * This setting is also quite simple to change after creation of the VM.

You can add additional provisioning steps using `config.vm.provision`.  Additionally, if you wished to further examine or modify the process, take a look at the [https://github.com/nasa-itc/deployment](https://github.com/nasa-itc/deployment) repository.

One your Vagrantfile has been updated to suit your needs, we go back to a terminal or command prompt from the NOS3 directory:
* `vagrant up`
* Hit enter a couple times while waiting to ensure it's not locked up.

Depending on internet speeds and if you have this imaged cached from previous attempts, this can take anywhere from minutes to hours.

---
### Option B: you already use Linux or wish to create your own Linux VM

You will need to have Docker, python3-pip, and python3-venv installed.

#### Docker install
* The following instructions are replicated from [docker.com](https://docs.docker.com/engine/install/ubuntu/#install-using-the-repository).
* You will need to add docker to the apt repository.
<pre>

#### Add Docker's official GPG key:
`sudo apt-get update`
`sudo apt-get install ca-certificates curl`
`sudo install -m 0755 -d /etc/apt/keyrings`
`sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc`
`sudo chmod a+r /etc/apt/keyrings/docker.asc`
#### Add the repository to Apt sources:
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "${UBUNTU_CODENAME:-$VERSION_CODENAME}") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null```
sudo apt-get update
</pre>

Now that Docker has been added to the apt repository, simply use apt to install Docker:

* `sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin`

In order to get Docker running properly without having to use sudo, it may be necessary to create a docker group and add the current user to it:
* `sudo groupadd docker`
* `sudo usermod -aG docker $USER`
* `sudo reboot now`

#### NOS3 install
Now, clone NOS3 directly into the VM:
* `git clone https://github.com/nasa/nos3`
* `cd nos3`
* `git submodule update --init --recursive`

Before NOS3 can be run, it has to have the containers and such things downloaded.  For the user's convenience, we have a command `make prep` which does this all automatically.  As such, after cloning NOS3, run:
* `make prep`
* Ensure that everything runs without trouble and that no warnings or errors are evident.

#### Python setup

First, be sure that everything is up-to-date:
* `sudo apt update` 

Then install the relevant python packages:
* `sudo apt install python3-pip python3-venv python3-dev`

Finally, cd into your top level NOS3 directory and run
* `python3 -m venv .venv` 
* `source .venv/bin/activate` 

This will spawn a python virtual environment and enable it so that pip packages installed from make prep are localized to this directory.
