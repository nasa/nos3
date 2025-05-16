

## Learning Goals
By the end of this scenario, you should be able to:
* Install and run NOS3 on a Windows/Linux platform in VirtualBox via Vagrant.
* Run it in Docker Linux.

## Prerequisites

There are no prerequisites for this scenario.

### Method 1: Vagrant + Virtualbox (Windows and Linux)
* To run NOS3 in VirtualBox you will need to install git, Vagrant, VirtualBox, and (optionally) the VirtualBox Extension Pack.
* First, clone and setup the directory with
* `git clone https://github.com/nasa/nos3.git`
* `cd nos3`
* `git pull`
* `git submodule update`

#### Editing the Vagrantfile
* The Vagrantfile is what spawns the initial VM and can be customized to change RAM, CPU count, and disk size.
* NOTE: RAM and CPU can both be adjusted after initial spawning.  Disk Size, however, is difficult to change once the VM is created.  In order to set a larger/smaller disk you may need to spawn an entirely new VM, so make sure that you are sure what disk size you need.
* Open the Vagrantfile in a text editor or vscode and scroll to the bottom.
* On line 23 you should see `config.vm.disk: disk, size: "64GB", primary: true`.  The `64GB` section is where the size of the disk is set.  You can change this to be larger or smaller depending on your needs, but it should not be set below 32GB.  This is very difficult to change after the VM is created without spawning an entirely new VM.
* On line 28 you should see `vbox.cpus = 4`.  This line sets the cpu number for the VM, and you can adjust it as you like - but keep it under half of you host machines total cpu count.  This can be changed in VirtualBox settings later on.
* On line 29 you should see `vbox.memory = "8192`.  This specifies the amount of RAM that the VM will start with.  Like cpu count, it can be adjusted as you'd like but should remain under half your host machine's total RAM for optimal perfomance.  It can also be changed later in VirtualBox settings.
* Finally, once you have configured your Vagrantfile, run `vagrant up` in the top level NOS3 directory and allow the script to run to completion.  When it's done, your VM will be running and you will be at the login screen.

### Method 2 Docker (Linux Only)
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
