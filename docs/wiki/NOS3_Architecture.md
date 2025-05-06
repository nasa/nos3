# Architecture

When NOS3 launches, it spawns docker containers to reduce the need to install and manage packages directly in Linux.

![NOS Basic Architecture](./_static/NOS_Basic.drawio.png)

The above image depicts installation Option A with a Linux Virtual Machine (VM) encasing everything.
When running in the VM, docker containers are networked to provide the modules expected in an operational simulator.

![image](./_static/NOS3-Container-Deployment.png)

Every process in NOS3 runs in its own container (as is best practice) and Docker networks are used to separate different groups of containers from one another.
On the top of the graphic is a cloud labeled 'COSMOS', but in current versions that can be either OpenC3, COSMOS, or YAMCS, the latter being the default.
This is the ground software with which the satellite(s) can be commanded.
Each satellite consists of a group of containers placed in its own network, illustrated in the grey cloud and labeled 'nos3_sc_1'.
Then there exists a group of universally necessary containers which can be shared between the different satellites, which are assigned to 'nos3_core'.

## Satellite(s)

Within each satellite, there are a variety of different "component" simulators to represent different parts of the vehicle.
These component simulators communicate via the NOS Engine middleware which provides interfaces via the hardware library (HWLIB) that model the behaviors at the bits and bytes level.
Flight software has no knowledge that it is not executing in space.
The dynamics engine (42) maintains the state of the spacecraft attitude, orientation, and environment and interfaces with the simulation.
These component simulators can have back doors for testing various fault scenarios that are accessible via the NOS Terminal or NOS UDP interfaces to enable scripting.

Note that multiple spacecraft have been tested as a proof of concept.
The configuration files can be edited to recreate this, but various limitations are present.

## Middleware - NOS Engine

NOS Engine is a message passing middleware designed specifically for use in simulation.
With a modular design, the library provides a powerful core layer that can be extended to simulate specific communication protocols, including I2C, SPI, and CAN Bus.
With advanced features like time synchronization, data manipulation, and fault injection, NOS Engine provides a fast, flexible, and reusable system for connecting and testing the pieces of a simulation.

NOS Engine is built on a conceptual model based on two fundamental types of objects: nodes and buses.
A node is any type of endpoint in the system capable of sending and/or receiving messages.
Any node in the system has to belong to a group, referred to as a bus.
A bus can have an arbitrary number of nodes, and each node on the bus must have a name that is unique from other member nodes.
The nodes of a bus operate in a sandbox; a node can communicate with another node on the same bus, but cannot talk to nodes that are members of a different bus.

Within NOS3, NOS Engine is used to provide software simulations of hardware buses.
NOS Engine provides the infrastructure for each hardware simulator to be a node on the appropriate bus and for the flight software to interact with hardware simulator nodes on their bus.
NOS Engine also provides plug-ins for various protocols such as I2C, SPI, CAN, and UART.
These plug-ins allow each bus and the nodes on that bus to communicate using calls and concepts specific to that protocol.

## Dynamics

42 is a general-purpose, multi-body, multi-spacecraft simulation.
For NOS3, it simulates the motion of the simulated spacecraft.
The progression of time for 42 is driven through NOS Engine and 42 provides output ephemeris, attitude, sun vector, magnetic field vector, and other environmental data to simulators that are part of NOS3.
42 is open source C code. For NOS3 it is installed on the virtual machine in the directory /home/jstar/.nos3/42.

## Directory Layout

The top level of NOS3 contains the following:
* `cfg` the configuration files for the mission and spacecraft
* `components` the repositories for the hardware component apps
* `docs` various documentation related to the project
* `fsw` the repositories for flight software
* `gsw` the repositories for ground station files and other ground tools
* `scripts` various convenience scripts
* `sims` the common simulators

Once you have forked or mirrored the top level NOS3 repository for your own use, it it recommended that only specific files be edited.
The directory structure has been setup to enable this and quick review of merge requests as they are ready:
* `cfg`, define your mission parameters
* `components`, develop custom components
* `gsw`, develop custom procedures and command/telemetry databases
* `scripts`, modify Dockerfile and various scripts to ensure the required toolchains for flight are included
