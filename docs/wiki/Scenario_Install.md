# Scenario - Installation

This scenario was developed to walkthrough the NASA Operational Simulator for Small Satellites (NOS3) installation.

This scenario was last updated on 5/23/25 and leveraged the `dev` branch [900f0e9].

## Learning Goals
By the end of this scenario, you should be able to:
* Install NOS3 on your desired platform

## Prerequisites

There are no prerequisites for this scenario apart from reading the initial documentation set:
* [Getting Started](./Getting_Started.md)
  * [Installation](./Getting_Started.md#installation)
  * [Running](./Getting_Started.md#running)

## Walkthrough

Two options are currently captured for a NOS3 installation based on your current setup.
Option A is to create a local virtual machine.
This option is traditionally used by NOS3 developers as we are provided Windows machines for our work and may have multiple projects that require different environments.
Option B is to bring your own Linux.
This may be either your own VM or because your host machine happens to be Linux already.
While we do have NOS3 developers who do this, they are advanced Linux users and follow best practices to ensure isolation and understand the risks involved.
Running directly without an additional virtualization layer may improve run speeds.

---
### Option A, create a local virtual machine (VM)
To run NOS3 in VirtualBox you will need to install git, Vagrant, and VirtualBox.
* [Git 2.47+](https://git-scm.com/)
* [Vagrant 2.4.3+](https://www.vagrantup.com/)
* [VirtualBox 7.1.6+](https://www.virtualbox.org/)

#### Cloning the repository 
Clone and setup the directory with via a terminal or command prompt:
* `git clone https://github.com/nasa/nos3.git`

<TODO: Insert image here showing successful git clone>

* `cd nos3`
* `git pull`
* `git submodule update --init --recursive`

<TODO: Insert image here showing successful git submodule update>

Note that the repository we just cloned will be shared into the VM in the default setup.
This means you man edit the code locally on your host and then still build and run inside the Virtual Machine.
Some prefer this as they have a tendency to make Linux environments unusable or crash, but can still have the code they were leveraging.
VM snapshots, good git practices, and other means exist to combat this as well.

#### Deploying the VM
Inside the NOS3 repository we cloned, the [./Vagrantfile](https://github.com/nasa/nos3/blob/b76e6844b5c707af53d4265d93e7802872df88c0/Vagrantfile) contains the instructions Vagrant itself needs to create the VM.
Note that a number of these options are editable and may be modified before continuing further:
* `config.vm.synced_folder`
  * The NOS3 repository on the host is shared into the VM at this defined location
* `config.vm.disk`
  * This is the size of the virtual disk used by the VM
  * 32GB may be able to run NOS3, but 64GB+ is recommended if you are adding additional mission specifics or editing docker files
  * Note that this is difficult to reconfigure after deployment
* `vbox.cpus`
  * While the default 4 CPUs will run, we'd recommend you set this to half your available CPU count of the host
  * If you don't know what your CPU count is - continue and edit this after deployment
* `vbox.memory`
  * While 8192 MB of memory will suffice, we'd recommend you double that or set it to half available on the host

If you are making this your own, you can add additional provisioning steps using `config.vm.provision`.
The NOS3 team generates the VM that will be downloaded using the files in the [https://github.com/nasa-itc/deployment](https://github.com/nasa-itc/deployment) repository if you wanted to go even further to examine or modify the process.

One your Vagrantfile has been updated to suit your needs, we go back to a terminal or command prompt from the NOS3 directory:
* `vagrant up`
* Hit enter a couple times while waiting to ensure it's not locked up

Depending on internet speeds and if you have this imaged cached from previous attempts this may take minutes or hours

---
### Option B, you already use Linux or have a VM

* You will need to have Docker, python3-pip, and python3-venv installed.

#### Docker install
* The following instructions are replicated from https://docs.docker.com/desktop/setup/install/linux/ubuntu/.
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
* Then download the .deb file from the linked site
* Finally run to finish the docker install
<pre>
 sudo apt update
 sudo apt install ./docker-desktop-amd64.deb
</pre>

#### NOS3 install
* Now, clone NOS3 directly into the VM:
`git clone https://github.com/nasa/nos3`
`git submodule update --init --recursive`

#### Python setup
* Run `sudo apt update` to start
* Then run `sudo apt install python3-pip python3-venv python3-dev`
* cd into your top level nos3 directory
* Run `python3 -m venv .venv` and `source .venv/bin/activate` this will spawn a python virtual environment and enable it so that pip packages installed from make prep are localized to this directory.
