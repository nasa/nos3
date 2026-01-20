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

For additional details see [Scenario - Installation](./Scenario_Install.md).

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

For additional details see [Scenario - Demonstration](./Scenario_Demo.md).

## Igniter

Igniter is installed and displayed via `make prep` and can be brought back up via `make igniter`.

Igniter is a tool for NOS3 that provides a simple Graphical User Interface to manage NOS3 Configurations, Components, Apps, and the like to allow the user to customize NOS3 to use only the components and apps they want or need. It also allows users to configure different spacecraft in a constellation differently if that is desired. One can load a configuration file, and then edit each spacecraft's configuration separately in the GUI if they desire. Igniter is split into 3 main views: the Configuration Tab, the Build Tab, and the Launch Tab.

### The Configuration Tab

In order to load a configuration, the user simply needs to click browse next to the current configuation box and select their desired configuation. Configuration files are simple XML documents, and are separated into two main types: the Master Config and the Spacecraft Configs. There is only one Master Config for NOS3, but there may be multiple Spacecraft Configs.

![IgniterConfigTab](./_static/NOS3_Igniter_Config.png)

#### Master Config
- The Master Config, contains parameters for the number of satellites being simulated, the mission start time, and links to the configs for each individual spacecraft.
   
![IgniterMissionConfig](./_static/NOS3_Igniter_MC.png)
  
#### Spacecraft Config
- The Spacecraft Configs contain parameters to enable or disable each Application and Component, to enable the 42 GUI to view the Dynamics Sim, and options to configure Orbital X, Y, and Z parameters.
  
![IgniterSCConfig1](./_static/NOS3_Igniter_SCC1.png)

![IgniterSCConfig2](./_static/NOS3_Igniter_SCC2.png)

The default master configuation file is found at the path *'cfg/nos3-mission.xml'* within the base NOS3 directory and the default Spacecraft config file, which has all the default components and cFS applications enabled, is found at the path *'cfg/spacecraft/sc-mission-config.xml'* in the base NOS3 Directory.

Configurations may also be edited on the fly within the Igniter GUI before the mission is built and launched. There are buttons to Save or Save As so a user may use these edited configurations for future runs. The Spacecraft Config XML files will be saved under the same directory as the Master Config XML.

### The Build Tab

The Build Tab allows the user to clean or build the flight software, ground software, or the simulator parameters separately - or all of them together - at the push of a button. It will open a terminal window to execute the command, then returns a prompt instructing the user to press ENTER to close.

*NOTE: In Future Versions, the terminal output will be viewable in the console output section of this tab.*

![IgniterBuildTab](./_static/NOS3_Igniter_Build.png)

### The Launch Tab

The Launch Tab allows the user to launch and stop the simulation with the buttons at the top of the pane without having to utilize the terminal.

*NOTE: In Future Versions, the Time Driver Output will display on this tab in the console output tab. The options below that pane will allow the user to specify a run time for the simulator in seconds, or to set a time for the simulator to automatically pause. This time is based on the system time for your machine, which is in UTC on the NOS3 VM. The Simulator will also be able to be paused or resumed manually with the buttons at the bottom of the pane.*

![IgniterLaunchTab](./_static/NOS3_Igniter_Launch.png)
