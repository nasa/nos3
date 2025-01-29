# Component Directory

NOS3 provides a baseline set of generic components in order to provide an example reference mission on which to build and develop additional tools and technologies.

## Component Information

High level reference information has been compiled for the various components.
Note that it is assumed telemetry messages are also within range, but just follow the form 0x0XXX opposed to 0x1XXX for command MIDs.
* Attitude Determination and Control System (ADCS)
  - Protocol(s): N/A
  - MSGID range: 0x1940 - 0x194F
  - Perf_IDs: 777
* Camera - Arducam
  - Protocol(s): I2C and SPI
  - MSGID range: 0x18C8 - 0x18CA
  - Perf_IDs: 105, 106  
* Coarse Sun Sensors (CSS)
  - Protocol(s): I2C
  - MSGID range: 0x1910 - 0x1911
  - Perf_IDs: 600, 601
* CryptoLib
  - Protocol(s): N/A
  - MSGID range: 0x1915 - 0x1916
  - Perf_IDs: 
* Electrical Power System (EPS)
  - Protocol(s): I2C
  - MSGID range: 0x191A - 0x191B
  - Perf_IDs: 401
* Fine Sun Sensors (FSS)
  - Protocol(s): SPI
  - MSGID range: 0x1920 - 0x1921
  - Perf_IDs: 510, 511
* Global Positioning System (GPS) - Novatel OEM615
  - Protocol(s): streaming UART
  - MSGID range: 0x1870 - 0x1871
  - Perf_IDs: 48
* Inertial Measurement Unit (IMU)
  - Protocol(s): CAN
  - MSGID range: 0x1925 - 0x1926
  - Perf_IDs: 530, 531
* Magnetometer
  - Protocol(s): SPI
  - MSGID range: 0x192A - 0x192B
  - Perf_IDs: 540, 541
* Radio
  - Protocol(s): Sockets
  - MSGID range: 0x1930 - 0x1931
  - Perf_IDs: 520
* Reaction Wheel
  - Protocol(s): UART
  - MSGID range: 0x1992 - 0x1993
  - Perf_IDs: 77
* Sample
  - Protocol(s): UART
  - MSGID range: 0x18FA - 0x18FB
  - Perf_IDs: 500
* Star Tracker
  - Protocol(s): SpaceWire
  - MSGID range: 0x1935 - 0x1936
  - Perf_IDs: 550
* Synopsis
  - Protocol(s):
  - MSGID range: 0x18FC - 0x18FD
  - Perf_IDs: 560
* Torquers
  - Protocol(s): PWM via HWLIB's TRQ commands
  - MSGID range: 0x193A - 0x193B
  - Perf_IDs: 505
* Thrusters
  - Protocol(s):  UART
  - MSGID range: 0x18EA - 0x18EB
  - Perf IDs: 508

## cFS App Information
High level reference information has been compiled for the various cFS Applications.
Note that it is assumed telemetry messages are also within range without the 0x1XXX indicating a command.
* cf - CCSDS File Delivery Protocol
  - Protocol(s): CFDP and UDP
  - MSGID range: 0x18B3 - 0x18B5 
  - Perf_ID ranges: 11-20, 30+x, 40+x
* ci - Command Injest
  - Protocol(s): CCSDS and UDP
  - MSGID range: 0x1884-0x1887
  - Perf_IDs: 0x0070, 0x0071
* ci_lab - Command Injest Lab
  - Protocol(s): CCSDS and UDP
  - MSGID range: 0x18E0-0x18E1
  - Perf_IDs: 32, 33  
* ds - Data Storage
  - Protocol(s): CCSDS
  - MSGID range: 0x18BB-0x18BC
  - Perf_IDs: 38
* fm - File Manager
  - Protocol(s): CCSDS
  - MSGID range: 0x188C - 0x188D
  - Perf_IDs: 39, 44
* hwlib - Hardware Library
  - Protocol(s): CCSDS
  - MSGID range: N/A
  - Perf_IDs: 50
* lc - Limit Checker
  - Protocol(s): CCSDS
  - MSGID range: 0x18A4-0x18A6
  - Perf_IDs: 28, 43
* sc - Stored Commands
  - Protocol(s): CCSDS
  - MSGID range: 0x18A9-0x18AB
  - Perf_IDs: 37
* sch - Scheduler
  - Protocol(s): CCSDS
  - MSGID range: 0x1895-0x1897
  - Perf_IDs: 36
* to - Telemetry Output
  - Protocol(s): CCSDS
  - MSGID range: 0x1880-0x1882
  - Perf_IDs: 0x0072
* to_lab - Telemetry Output Lab
  - Protocol(s): CCSDS
  - MSGID range: 0x18E8-0x18E9
  - Perf_IDs: 34, 35
