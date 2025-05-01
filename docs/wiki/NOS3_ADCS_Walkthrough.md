# Scenario - ADCS Walkthrough

This scenario was developed to give trainees a walkthrough of NOS3's basic Attitude Determination and Control System (ADCS).

## Learning Goals

By the end of this scenario you should be able to:

- Understand the basics of Attitude Determination and Control and Sensor Fusion components
- Understand some basic ADCS modes and how they operate
- Understand the various sensors and actuators utilized by ADCS to perform its function
- Understand the cFS bus and how it can be utilized by a sensor fusion component to take in data from other components and to command other components.

## Prerequisites

Before running the scenario, ensure the following steps are completed:

- [Getting Started](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md)
  - [Installation](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md#installation)
  - [Running](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md#running)
    
    ## Introduction
    
    The Attitude Determination and Control System (ADCS) is different from our other components, as instead of interfacing with a sensor or actuator directly, it serves as a sensor fusion component, taking in the inputs from various sensors and actuators, and using them to orient and navigate the craft as specified. NOS3's ADCS comes equipped with four main modes: Passive, Sunsafe, Inertial, and BDOT.
    
    ## Modes
- Passive turns ADCS control of the craft off, leaving it to manual commands of the actuators.
- Sunsafe utilizes the Fine Sun Sensor and Coarse Sun Sensor to determine if the craft is in sun and the orientation between the spacecraft body frame and the sun unit vector, and then uses the reaction wheels and magnetorquers to keep the craft pointing in a fairly optimal charging position while not in eclipse.
- Inertial mode utilizes the Star Tracker and Inertial Measurement Unit data and the reaction wheels and magnetorquers to orient the spacecraft relative to an inertial frame of reference.
- BDOT mode uses the IMU and Magnetometer data and the reaction wheels and magnetorquers to stabilize the craft and damp the rotation rate to nearly zero.
  
  ## Sensors
- The Coarse Sun Sensors (CSS) are located on each face of the spacecraft and provide a voltage output based on how much light is shining on them. This gives a rough idea of if a particular face is in the sun.
- The Fine Sun Sensor (FSS) provides a more accurate reading of the spacecraft's orientation relative to the sun, as long as the sensor is in sun.
- The Inertial Measurement Unit (IMU) utilizes accelerometers and gyroscopes to measure the linear acceleration and angular rate, respectively.
- The Magnetometer (MAG) measures the craft's orientation relative to the Earth's magnetic field. This allows the craft to know which direction each magnetorquer will move the craft by relating the body frame of the craft with the Earth's magnetic field.
- The Star Tracker (ST) uses images of the stars and a star catalog to determine the spacecraft orientation with respect to a fixed inertial frame of reference.
  
  ## Actuators:
- The Reaction Wheels (RWs) are rotating flywheels which allow the craft to generate torque and rotate and point itself. There is one for each axis, which can be torqued in either direction on that axis.
- The Magnetorquers, or Torquers, utilize electromagnets and the Earth's magnetic field to produce weaker torques than the Reaction Wheels, aligned to the Earth's magnetic field.
- Thrusters are not linked into ADCS or fully developed for this example mission, but they would allow orbital adjustments and navigation in the linear axes, rather than rotational adjustments like the other actuators.
  
  ## Ingest and Output:
  
  As this mission uses cFS, it is built on a bus-based architecture, where all messages are published to a single bus, which other components can subscribe to. Thus, ADCS's sensor fusion works by subscribing to the various sensors' messages in ADCS, so it can read them into itself and act on them. If you would like more information, look at the "generic_adcs_ingest.c" file.

Then, when ADCS needs to command an actuator, you would need to build and send a command message for that actuator from ADCS in order to control it. If you would like more information, look at the "generic_adcs_output.c" file.

## Example:

For Sunsafe mode, the ADCS system will take in the values for the sun vector from FSS and CSS, and their validity signals, and use those to determine if the spacecraft is in sun and the orientation between the spacecraft body frame and the sun unit vector. ADCS will then send torque commands to orient the spacecraft to properly face the sun. If you would like to see how it works in code, look at the 'AC_sunsafe' method in the "generic_adcs_adac.c" file.
