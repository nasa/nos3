# Overview

![NOS3-Logo](./_static/jstar_nos3.png)

Welcome to the NOS3 User's Manual and Developer's Guide. This documentation is designed to provide information for users and developers that intend to utilize, enhance, and extend the NASA Operational Simulator for Small Satellites (NOS3).

## NASA Operational Simulator for Small Satellites (NOS3)

The NASA Independent Verification and Validation (IV&V) Independent Test Capability (ITC) team developed West Virginia's first satellite, which is a 3U CubeSat (Small Satellite) named Simulation-to-Flight 1 (STF-1). The primary goal of this Small Satellite was to develop and demonstrate the lifecycle value of a software-only small satellite simulator. This simulator is called the NASA Operational Simulator for Small Satellites or NOS3.

NOS3 is an open-source, software-only test bed for small satellites available via the NASA Open Source Agreement. It is a collection of Linux executables, libraries, and source code. Current simulations are based on commercial off-the-shelf (COTS) hardware that has been used on several CubeSats. It is intended to easily interface with flight software developed using the NASA Core Flight System (cFS).

NOS3 serves as a baseline architecture for satellite missions, offering a robust and flexible framework that can be tailored to a specific mission. Its component-based structure facilitates integration of software modules, enabling developers to simulate spacecraft behavior, test instrument interfaces, and perform software/hardware integration. As an open-source platform, NOS3 serves as a valuable reference mission, providing teams with a flexible foundation to adapt to their mission requirements.

### Documentation

- [Home](Home.md)
- [Architecture](NOS3_Architecture.md)
- [Engine](NOS3_Engine.md)
- [Ground Systems](NOS3_Ground_Systems.md)
- [Install, Build, Run](NOS3_Install_Build_Run_QuickStart.md)
- [Components, Repository and Directory Structure](NOS3_Components_Repository_Directory_Structure.md)
- [Running Executables and Windows](NOS3_Executables_and_Windows.md)
- [Workflows](NOS3_Workflows.md)
- [Hardware Simulator Framework / Example Simulator](NOS3_Simulators.md)
- [42 Orbit and Attitude Dynamics](NOS3_42.md)
- [cFS Development](NOS3_cFS_Development.md)
- [Component Directory](NOS3_Component_Directory.md)
- [Component Development Flow](NOS3_Component_Development.md)
- [Custom cFS Table Development](NOS3_Custom_cFS_Table_Guide.md)
- [OIPP](NOS3_OIPP.md)
- [CryptoLib](CryptoLib.md)
- [Igniter](NOS3_Igniter.md)


### Components

NOS3 is comprised of a number of components. These components are listed in the following table:

| Component | Description |
| --- | --- |
| Vagrant | Vagrant is an open source solution that can be used to script the creation of Oracle VirtualBox virtual machines and the provisioning of such machines, including package installation, user creation, file and directory manipulation, etc. |
| VirtualBox | Oracle VirtualBox is an open source solution for creating and running virtual machines. |
| NOS Engine | NASA Operational Simulator (NOS) Engine is a NASA developed solution for simulating hardware busses as software only busses. This component provides the connectivity between the flight software and the simulated hardware components. |
| Simulated Hardware Components | A collection of simulated hardware components which connect to NOS Engine and provide hardware input and output to the flight software. |
| 42  | Some of the hardware components require dynamic environmental data. 42 is an open source visualization and simulation tool for spacecraft attitude and orbital dynamics developed by NASA Goddard Space Flight Center (GSFC) which is used to provide dynamic environmental data. |
| cFS | NASA Core Flight Software (cFS) is an open source Fight Software used as the base system which STF-1 flight software is developed on top of. |
| COSMOS | COSMOS is open source ground system software developed by Ball Aerospace which is used to provide command and control of the flight software. |
| AIT | AIT is a light weight open source ground system developed by JPL that provides command and control to the flight software. |
| OIPP | Orbit , Inview, and Power Planning (OIPP) is an ITC developed planning tool which can use current two line element (TLE) sets from the internet or a TLE file to project satellite to ground station inview times and satellite eclipse and sunlight times. |

### Why should NOS3 be used?

NOS3 should be used to validate the functionality and performance of satellite systems before deployment. NOS3 can be used as a starting point for development and throughout the rest of the mission lifecycle. It enables developers to test different scenarios and configurations, identify potential issues, and refine the system design. By utilizing NOS3, the risks associated with satellite missions can be reduced and operational efficiency can be improved. Some of NOS3 features include:

1. Enabling multiple developers to build and test flight software with simulated hardware models
2. Serving as an interface simulator for science instrument / payload teams to communicate with prior to hardware integration
3. Supporting software development activities
4. Enabling hardware integration to parallel software development
5. Providing an automated testing framework
6. Increasing available test resources
7. Enabling operation of the simulated spacecraft using the ground software command and telemetry databases

### When should NOS3 be used?

NOS3 ideally is used for initial developer training to flight and ground software while components are still being selected. Once component selection occurs development can begin - this early start is important because software does not scale with the size of the mission.

### How to get started with NOS3? Is there a demo or tutorial?

To get started with NOS3, you can visit the official NOS3 website, nos3.org, or the GitHub repository maintained by NASA. The documentation provides detailed information on installation, configuration, and usage. Additionally, you can find tutorials and examples to help you understand and utilize NOS3 effectively.

### How should I go from NOS3 as is to my specific mission?

To tailor NOS3 for your specific mission, you would typically create new components within the framework that represent the unique hardware of your satellite system. This involves defining the hardware interfaces, software behavior, and operational procedures specific to your mission. By customizing and extending the existing components or creating new ones, you can tailor NOS3 to match your mission requirements.

### How to move from software / simulator to hardware once it arrives?

NOS3 isn't a substitute for hardware testing, just a tool to augment the number of types of tests possible. The component development flow includes the development of a standalone checkout application that can more rapidly be developed, deployed, and tested on hardware. The flight software will use the same functions and code developed and tested through that process allowing focus to shift to data flow and system level tests cleanly.

### What can NOS3 give me during I&T?

During Integration and Testing (I&T) activities, NOS3 provides valuable capabilities. It enables you to perform integration testing of your satellite system, validate the operational procedures, and verify the overall system performance. NOS3 allows you to conduct end-to-end simulations, test different mission scenarios, and assess the behavior of the satellite system under various conditions.

### What are NOS3 uses during operation?

During mission operation, NOS3 can continue to be used for several purposes. It can support mission planning and rehearsal activities, aid in real time operations monitoring and analysis, and assist in anomaly resolution and fault diagnosis. NOS3 allows operators to simulate and evaluate different operational scenarios, predict the behavior of the satellite system, and make informed decisions based on the simulated environment.

### How to a move from software / simulator to hardware once it arrives?

NOS3 isn't a substitute for hardware testing, just a tool to augment the number of types of tests possible. The component development flow includes the development of a standalone checkout application that can more rapidly be developed, deployed, and tested on hardware. The flight software will use the same functions and code developed and tested through that process allowing focus to shift to data flow and system level tests cleanly.

### How is NOS3 classified?

According to the [NASA Software Classification guidelines](https://nodis3.gsfc.nasa.gov/displayDir.cfm?Internal_ID=N_PR_7150_002D_&page_name=AppendixD), NOS3 is Class D software, since it is used to support both engineering development and mission planning. More information about NASA software classification can be found [here](https://nodis3.gsfc.nasa.gov/displayDir.cfm?t=NPR&c=7150&s=2D).

## FAQ

1. How do I login to the NOS3 virtual machine?
    - user: jstar, password: jstar123!
    - user: vagrant, password: vagrant
2. NOS Engine bus ports fail on launch
    - NOS Engine allows dynamic connections and disconnects and ensures ports are closed before connecting. Ports may work again after an initial "Not connect".
3. When the cFS flight software starts it cannot find my application or startup script
    - Make sure that your application is built correctly and the shared object library (.so) is present. Likewise, ensure that the app name is correctly listed in `fsw/nos3_defs/cpuN_cfe_es_startup.scr` and `fsw/nos3_defs/targets.cmake`.
    - For further information, please check the NASA/cFS git repository and documentation.
4. How do I connect my own standalone flight software?
    - Be sure to have all port numbers consistent between all components in NOS3, including 42.
5. Why does cFS constantly crash on start-up and/or force me to restart my PC to rerun?
    - NASA's cFS is safety-critical flight software. Make sure you are building your applications to specification and that you are properly using the PSP and OSAL calls from within your apps.
    - It is best to **_not_** run cFS as sudo. If you are doing this, make sure you have configured for your host or are providing appropriate run-time arguments with cFS.
6. Can NOS3 be run across multiple computers?
    - Yes - the satellite and ground software can be split apart and run on their own VMs. The instructions can be found [here](https://github.com/nasa/nos3/wiki/NOS3-Build-and-Run-on-Multiple-VMs).