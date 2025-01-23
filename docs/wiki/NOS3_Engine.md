# Engine

## About
NOS Engine is a message passing middleware designed specifically for use in simulation. With a modular design, the library provides a powerful core layer that can be extended to simulate specific communication protocols, including I2C, SPI, and CAN Bus. With advanced features like time synchronization, data manipulation, and fault injection, NOS Engine provides a fast, flexible, and reusable system for connecting and testing the pieces of a simulation.

NOS Engine is built on a conceptual model based on two fundamental types of objects: nodes and buses. A node is any type of endpoint in the system capable of sending and/or receiving messages. Any node in the system has to belong to a group, referred to as a bus. A bus can have an arbitrary number of nodes, and each node on the bus must have a name that is unique from other member nodes. The nodes of a bus operate in a sandbox; a node can communicate with another node on the same bus, but cannot talk to nodes that are members of a different bus.

Within NOS3, NOS Engine is used to provide software simulations of hardware buses. NOS Engine provides the infrastructure for each hardware simulator to be a node on the appropriate bus and for the flight software to interact with hardware simulator nodes on their bus. NOS Engine also provides plug-ins for various protocols such as MIL-STD-1553, SpaceWire, I2C, SPI, CAN, and UART. These plug-ins allow each bus and the nodes on that bus to communicate using calls and concepts specific to that protocol.
