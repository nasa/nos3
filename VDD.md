# Version Description Document #
# NASA Operational Simulator for Small Satellites (NOS3) #
# BUILD: 1.05.00 #
## RELEASE DATE: 7/10/2020 ##

### Software Versions ###
This version of NOS3 includes the following software components with minimal changes to allow NOS3 compatibility. Where changes are necessay, the original repo is forked to https://github.com/nasa-itc. The upstream version or commit is noted below

- **core Flight Executive: [cFE](https://github.com/nasa/cFE)** - v6.7.0a
- **Operating System Abstraction Layer: [OSAL](https://github.com/nasa/osal)** - v5.0.0
- **Platform Support Package: [PSP](https://github.com/nasa-itc/PSP)** - v1.4.0 
  - nos-linux psp added for NOS3 compatibilty
- cFS Lab Tools
  - **Sample Ground System: [cFS-GroundSystem](https://github.com/nasa/cFS-GroundSystem)** - v2.1.0
  - **ELF to cFE Table Converter: [elf2cfetbl](https://github.com/nasa/elf2cfetbl)** - v3.1.0
- cFS Applications
  - **Command Ingest: [CI](https://github.com/nasa/cfs_ci)** - master commit: 1bcf88d
    - Changes to CMake for NOS3 build system
  - **Scheduler: [SCH](https://github.com/nasa/sch)** - v2.2.2
    - Updated table locations and changed CMake for NOS3
  - **Stored Command: [SC](https://github.com/nasa/sc)** - v2.5.0
    - Updated table locations
  - **House Keeping: [HK](https://github.com/nasa/hk)** - v2.4.1
    - Updated table locations and changed CMake for NOS3
  - **Telemetry Output: [TO](https://github.com/nasa/cfs_to)** - master commit:  4589edb
    - Updated table locations and changed CMake for NOS3
  - **CFS_LIB: [CFS_LIB](https://github.com/nasa/cfs_lib)** - v2.2.0
    - Added cmake files for build
- NOS3 Applications
  - **Arducam OV2640**
  - **Clyde EPS**
  - **Generic Reaction Wheels**
  - **HWLIB**
  - **Novatel OEM615**
  - **Sample**
- NOS3 Simulators
  - **Arducam OV2640**
  - **Clyde Battery**
  - **Clyde EPS**
  - **NOS3 Time Driver**
  - **Novatel OEM615**
  - **Sample**
  - **Sim Command Terminal**
- NOS3 Supporting Packages
  - **NOS Engine** - v1.5.1
  - **ITC Common** - v1.9.1
- **42: [42](https://github.com/ericstoneking/42)** - master commit: b70508fd
- **COSMOS: [COSMOS](https://github.com/ballaerospace/cosmos)** - master
- **AIT: [AIT](https://github.com/nasa-ammos/ait-core)** - v2.0.0
- **Ubuntu** - 18.04 LTS

### Summary of NOS3 changes for this release ###
- Updated to cFS v6.7.0a
- Included all NOS3 components as submodules for better tracking
- Included ability to command 42 from simulators
- Included better support for missions varying from the default configuration in 42
- Updated 42 version
- General bug fixes