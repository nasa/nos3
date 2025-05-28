# Scenario - ADCS Walkthrough

This scenario was developed to give trainees a walkthrough of NOS3's basic Attitude Determination and Control System (ADCS).

This scenario was last updated on 5/7/25 and leveraged the `dev` branch at the time [c37ab5b].

## Learning Goals
By the end of this scenario you should be able to:

- Understand the basics of the Attitude Determination and Control and Sensor Fusion components
- Understand some basic ADCS modes and how they operate
- Understand the various sensors and actuators utilized by ADCS to perform its function
- Understand the cFS bus and how it can be utilized by a sensor fusion component to take in data and command other components

## Prerequisites
Before running the scenario, ensure the following steps are completed:

- [Getting Started](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md)
  - [Installation](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md#installation)
  - [Running](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md#running)
    
## Introduction
The Attitude Determination and Control System (ADCS) is different from other components in NOS3. This is because it serves as a sensor fusion component rather than interfacing with a sensor or actuator directly. It takes in the inputs from various sensors and actuators, and uses them to orient and navigate the spacecraft as specified. NOS3's ADCS comes equipped with four main modes: Passive, Sunsafe, Inertial, and BDOT.
    
## Modes
- Passive mode turns ADCS control of the spacecraft off, leaving it to manual commands of the actuators.

  ![ADCSWalkthrough_Passive](https://github.com/user-attachments/assets/0c56fd62-48f5-40c7-935d-1d0623697dfa)

- Sunsafe mode utilizes the Fine Sun Sensor and Coarse Sun Sensor to determine if the spacecraft is in sun and the orientation between the spacecraft body frame and the sun unit vector, and then uses the reaction wheels and magnetorquers to keep the spacecraft pointing in an optimal charging position while not in eclipse.
  
  ![ADCSWalkthrough_Sunsafe](https://github.com/user-attachments/assets/9c6d060a-7a1c-40f3-9fe6-fbba0077a46e)

- Inertial mode utilizes the Star Tracker and Inertial Measurement Unit data and the reaction wheels and magnetorquers to orient the spacecraft relative to an inertial frame of reference.
  
  ![ADCSWalkthrough_InertialMode](https://github.com/user-attachments/assets/9359778c-a36e-4f3f-9a64-02d490d457cc)

- BDOT mode uses the IMU and Magnetometer data and the reaction wheels and magnetorquers to stabilize the spacecraft and damp the rotation rate to approach zero.

  ![ADCSWalkthrough_BDOT](https://github.com/user-attachments/assets/f0e2e847-b81c-4221-9648-2fdcc48461f6)

## Sensors
- The Coarse Sun Sensors (CSS) are located on each face of the spacecraft and provide a voltage output based on how much light is shining on them. This gives a rough idea of if a particular face is in the sun.
  
  ![ADCSWalkthrough_CSS](https://github.com/user-attachments/assets/559ceff7-8410-4cb9-bd8a-0e382bd24412)

- The Fine Sun Sensor (FSS) provides a more accurate reading of the spacecraft's orientation relative to the sun, as long as the sensor is in sun.
  
  ![ADCSWalkthrough_FSS](https://github.com/user-attachments/assets/b010228f-e52c-42d8-b89d-a52d95128f73)

- The Inertial Measurement Unit (IMU) utilizes accelerometers and gyroscopes to measure the linear acceleration and angular rate, respectively.
  
  ![ADCSWalkthrough_IMU](https://github.com/user-attachments/assets/afdb96a7-1fdc-46e7-a9a8-016015e656ee)

- The Magnetometer (MAG) measures the spacecraft's orientation relative to the Earth's magnetic field. This allows the spacecraft to know which direction each magnetorquer will move the spacecraft by relating the body frame of the spacecraft with the Earth's magnetic field.
  
  ![ADCSWalkthrough_MAG](https://github.com/user-attachments/assets/74f746d8-7e3d-4b60-a619-918e23c9148b)

- The Star Tracker (ST) uses images of the stars and a star catalog to determine the spacecraft orientation with respect to a fixed inertial frame of reference.
  
  ![ADCSWalkthrough_StarTracker](https://github.com/user-attachments/assets/cbc26d4a-718c-4e17-b8c8-9c14504df716)

## Actuators:
- The Reaction Wheels (RWs) are rotating flywheels which allow the spacecraft to generate torque, rotate, and point itself. There is one for each axis, which can be spun in either direction on that axis.

  ![ADCSWalkthrough_ReactionWheelTLM](https://github.com/user-attachments/assets/f24917e9-1b6c-4e0c-a951-f1d38f377668)
  ![ADCSWalkthrough_ReactionWheel_SunsafeCommands](https://github.com/user-attachments/assets/4af6668e-69b8-4e98-b7bf-06d42b59f6b0)
  ![ADCSWalkthrough_ReactionWheel_ManualCommands](https://github.com/user-attachments/assets/ef461643-5b78-41ed-abfb-a6d4f8c720cb)

- The Magnetorquers, or Torquers, utilize electromagnets and the Earth's magnetic field to produce weaker torques than the Reaction Wheels, aligned to the Earth's magnetic field.

  ![ADCSWalkthrough_TorquerTLM](https://github.com/user-attachments/assets/7bcffb7d-61a0-4457-86c4-6ef5523b1b94)
  ![ADCSWalkthrough_Torquer_SunsafeCommands](https://github.com/user-attachments/assets/de3a2875-0790-4534-a715-5d36d31b66ff)

- Thrusters are not linked into ADCS or fully developed for this example mission, but they would allow orbital adjustments and navigation in the linear axes, rather than rotational adjustments like the other actuators.
  
## Ingest and Output:
As this mission uses cFS, it is built on a bus-based architecture, where all messages are published to a single bus, which other components can subscribe to. Thus, ADCS's sensor fusion works by subscribing to the various sensors' messages in ADCS, so it can read them into itself and act on them. This subscription process happens in the `Generic_ADCS_AppInit` method of **/nos3/components/generic_adcs/fsw/cfs/src/generic_adcs_app.c**. As can be observed there, it subscribes to other cFS apps for the sensors/actuators (MAG, FSS, CSS, IMU, RW, and ST), but also subscribes to or initializes to various 42 configurations tied to ADCS, specifically Inp_DI.txt, Inp_ADAC.txt, and Inp_DO.txt. This assures it is properly linked in with our dynamics simulator.

Then, as the ADCS app processes commands, it will filter out its own ground commands and telemetry requests, and then check for each of the telemetry packets from the aforementioned sensors. At this point, it will call ingest methods on those to parse out the data into its own "*structs*", and update its own telemetry points. If you would like more information, look at the "generic_adcs_ingest.c" file. This file can be found in **/nos3/components/generic_adcs/fsw/cfs/src/generic_adcs_ingest.c**.

This data is then used internally by the ADAC (Attitude Determination and Attitude Control) method. This is called on the scheduler and parsed in the same function in app as mentioned above, which calls a method within **/nos3/components/generic_adcs/fsw/cfs/src/generic_adcs_adac.c**. This method then loads the ingested data into internal data structs, and determines what mode ADCS is in. Next, it will call the associated function, which will utilize that data to determine what actuator commands need to be sent out to achieve its desired result, and then will build up those commands.

Then, those commands built by ADAC are sent out through the output methods, which build proper CCSDS packets which may be sent over the cFS bus, which will then be received by the associated actuator and processed to produced the desired effect. If you would like more information, look at the "generic_adcs_output.c" file. This file can be found in **/nos3/components/generic_adcs/fsw/cfs/src/generic_adcs_output.c**.

## Example:
For Sunsafe mode, the ADCS system will take in the values for the sun vector from FSS and CSS, and their validity signals. It will then utilize that telemetry to determine if the spacecraft is in sun and the orientation between the spacecraft body frame and the sun unit vector. ADCS will then send torque commands to orient the spacecraft to properly face the sun. If you would like to see how it works in code, look at the 'AC_sunsafe' method in **/nos3/components/generic_adcs/fsw/cfs/src/generic_adcs_adac.c**. This file can be found in **/nos3/components/generic_adcs/fsw/cfs/src/**.
