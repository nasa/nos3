

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

# Method 1: Vagrant + Virtualbox
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
* 

