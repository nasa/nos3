

## Learning Goals
By the end of this scenario, you should be able to:
* Install and run nos3 on a window/linux platform in Virtual Box via vagrant
* Run it in docker linux

## Prerequisites

Before running the scenario, ensure the following steps are completed:
* [Getting Started](./Getting_Started.md)
  * [Installation](./Getting_Started.md#installation)
  * [Running](./Getting_Started.md#running)
* No additional file changes or special setup is needed for this scenario

### Method 1: Vagrant + Virtualbox (Windows and Linux)
* To run nos3 in virtualbox you will need to inastall git, vagrant, Virtualbox, and the Virtualbox Extension Pack
* First clone and setup the directory with
* `git clone https://github.com/nasa/nos3.git`
* `cd nos3`
* `git pull`
* `git submodule update`

# Editing the Vagrant file
* The Vagrantfile is what spawns the initial VM and can be customized to chang RAM, CPU count, and disk size
* NOTE: RAM and CPU can both be adjusted after initial spawning however Disk Size is unchangeable once the VM is created, in order to set a larger/smaller disk you will need to spawn and entirely new vm so make sure that you are sure what disk size you need
* Open the vagrant file in a text editor or vscode and scroll to the bottom
* On line 23 you should see `config.vm.disk :disk, size: "64GB", primary: true` the `64GB` section is where the size of the disk is set you can change this to be larger or smaller depending on your needs but it should not be set below 32GB. This can NOT be changed after the VM is created without spawning an entirely new VM
* On line 28 you should see `vbox.cpus = 4` this line sets the cpu number for the vm, you can adjust it as you like but keep it under half of you host machines total cpu count, this can be changed in Virtualbox settings later on.
* On line 29 you should see `vbox.memory = "8192` this specified the amount of RAM that the VM will start with like cpu count it can be adjusted as you'd like but should remain under half your host machine's total ram for optimal perfomance, it can also be changed later in virtualbox settings
* Finally, once you have configured your Vagrantfile run `vagrant up` in the top level nos3 directory and allow the script to run to completion, when its done your VM will be running and you will be at the login screen

### Method 2 Docker (Linux Only)
* You will need to have docker, python3-pip, and python3-venv installed

# Docker install
* Instructions from https://docs.docker.com/desktop/setup/install/linux/ubuntu/
* You will need to add docker to the apt repository
<pre>
# Add Docker's official GPG key:
`sudo apt-get update`
`sudo apt-get install ca-certificates curl`
`sudo install -m 0755 -d /etc/apt/keyrings`
`sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc`
`sudo chmod a+r /etc/apt/keyrings/docker.asc`
# Add the repository to Apt sources:
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "${UBUNTU_CODENAME:-$VERSION_CODENAME}") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null```
sudo apt-get update
</pre>
* Then download the .deb file from the linked site
* Finally run to finish the docker install
<pre>
 sudo apt-get update
 sudo apt-get install ./docker-desktop-amd64.deb
</pre>

# Python setup
* Run `sudo apt update` to start
* Then run `sudo apt install python3-pip python3-venv python3-dev`
* cd into your top level nos3 directory
* Run `python3 -m venv .venv` and `source .venv/bin/activate` this will spawn a python virtual environment and enable it so that pip packages installed from make prep are localized to this directory.
