# Generic_mag - NOS3 Component
This repository contains the NOS3 Generic_mag Component.
This includes flight software (FSW), ground software (GSW), simulation, and support directories.

## Overview
This generic_mag component is an SPI device that accepts multiple commands including requests for telemetry. 
The available FSW is for use in the core Flight System (cFS) while the GSw supports COSMOS.
A NOS3 simulation is available which includes both generic_mag and 42 data providers.

# Device Communications
The protocol, commands, and responses of the component are captured below.

## Protocol
The protocol in use is Serial Peripheral Interface (SPI). The generic_mag is slave to the SPI master bus on the spacecraft and operates on chip select 2.

## Commands
There are no commands for the generic_mag beyond the basic app commands available to all NOS3 components.

## Response
Telemetry is returned in big endian format.
Response formats are as follows:
* Telemetry
  * 0xDEADBEEF
  * uint32, magnetic field intensity (x)
  * uint32, magnetic field intensity (y)
  * uint32, magnetic field intensity (z)

# Configuration
The various configuration parameters available for each portion of the component are captured below.

## FSW

## Simulation

## 42

# Documentation
If this generic_mag application had an ICD and/or test procedure, they would be linked here.

# Commanding
Refer to the file `fsw/platform_inc/generic_mag_app_msgids.h` for the Generic_mag app message IDs
Refer to the file `fsw/src/generic_mag_app_msg.h` for the Generic_mag app command codes

## Releases
We use [SemVer](http://semver.org/) for versioning. For the versions available, see the tags on this repository.
