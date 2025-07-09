# Scenario - cFS

This scenario was developed to provide an overview of the core Flight System (cFS) as implemented in the NASA Operational Simulator for Small Satellites (NOS3).

The "little c" in cFS signifies its design as a lean, modular core, built as an extension of the core Flight Executive (cFE).  While cFE provides the fundamental services for application loading, event logging, and resource management, cFS expands this functionality through:
* An Operating System Abstraction Layer (OSAL) and Platform Support Layer (PSP) enabling portability across various flight hardware.
* A collection of reusable applications that can be configured for specific mission requirements.
* A standardized framework for developing mission-specific components.

NOS3 serves as a specialized distribution of cFS, incorporating customized components while maintaining compatibility with the upstream codebase.
This approach allows teams to benefit from community updates while contributing improvements back to the open-source ecosystem.
This demonstration will walk through key cFS capabilities using NOS3's minimal mode, providing hands-on experience with its core applications and services.

This scenario was last updated on 5/23/25 and leveraged the `dev` branch at the time [a3e7c100].

## Learning Goals

By the end of this scenario, you should be able to:
* Understand what the Core Flight System (cFS) is and its capabilities as a flight software framework.
* Navigate the essential applications (CI/TO, DS, FM, LC, SC, SCH) and their interactions.
* Modify tables for various applications to customize their behavior for specific mission needs.
* Trace end-to-end command and telemetry flows through the system.
* Implement basic operational scenarios combining multiple cFS applications.

## Prerequisites

Before running the scenario, complete the following steps:
* [Getting Started](./NOS3_Getting_Started.md)
  * [Installation](./NOS3_Getting_Started.md#installation)
  * [Running](./NOS3_Getting_Started.md#running)
* [Scenario - Demonstration](./Scenario_Demo.md)
* Modify the top level [./cfg/nos3-mission.xml](https://github.com/nasa/nos3/blob/122d9fbb3d8095f8614b8cd18f188d10e643a08a/cfg/nos3-mission.xml)
  * Change `<sc-1-cfg>spacecraft/sc-mission-config.xml</sc-1-cfg>` to `<sc-1-cfg>spacecraft/sc-minimal-config.xml</sc-1-cfg>`

## Walkthrough

The core Flight System (cFS) was developed to achieve the following goals:
* Reduce time to deploy high quality flight software.
* Reduce project schedule and cost uncertainty.
* Directly facilitate formalized software reuse.
* Enable collaboration across organizations.
* Simplify sustaining engineering:
  * AKA On Orbit FSW maintenance - missions often last 10 years or more.
* Scale from small instruments to Hubble class missions.
* Build a platform for advanced concepts and prototyping.
* Create common standards and tools across NASA.

* Architecture Layers (Bottom Up)
  * RTOS / BOOT:
    * The RTOS/Boot Layer provides the operating system services, device drivers, and C library that the cFS depends on.
    * Supported operating systems include VxWorks, RTEMS, and Linux. Most flight projects use a real time operating system.
  * Platform Abstraction:
    * The OS Abstraction Layer (OSAL) is a software library that provides a single Application Program Interface (API) to the core Flight Executive (cFE) regardless of the underlying real-time operating system.
    * The Platform Support Package (PSP) is a software library that provides a single Application Program Interface (API) to underlying avionics hardware and board support package. It contains startup code, interfaces to hardware such as nonvolatile memory, watchdog, and timers.
  * core Flight Executive:
    * The core Flight Executive (cFE) is a portable, platform-independent framework that creates an application runtime environment by providing services that are common to most flight applications.
    * cFE Services include the following:
      * Executive Services (ES) manage the software system and create an application runtime environment.
      * Event Services (EVS) provide a service for sending, filtering, and logging event messages.
      * Table Services (TBL) manage application table images.
      * Time Services (TIME) manage the spacecraft time.
      * Software Bus (SB) provides an application publish/subscribe message service.
  * Application:
    * Applications provide mission functionality using a combination of cFS community apps and mission-specific apps. 
  * Development Tools and Ground Systems:
    * Development tools and ground systems are used to test and run the cFS. 
    * A variety of ground systems can be used with cFS. 
    * Ground system and tool selection generally vary by project.

![Scenario cFS - Architecture](./_static/scenario_cfs/scenario_cfs_architecture.png)

* cFS APIs support add and remove functions.
* Applications can be switched in and out at runtime without rebooting or rebuilding the system.
* Changes can be dynamically added during development, test, and even on-orbit.

![Scenario cFS - Plug and Play](./_static/scenario_cfs/scenario_cfs_plug_and_play.png)

* The addition of new applications can be completed very easily.
* An entire generic design reference mission is provided by default in NOS3.
* This looks like the following, with each app communicating with the others as needed via the SB:

![Scenario cFS - FSW Diagram](./_static/scenario_cfs/scenario_cfs_fsw_diagram.png)

---
### NOS3 Implementation

With a terminal navigated to the top level of your NOS3 repository:
* `make clean`
  * Note that we clean here just in case previous files exist from other scenarios now that we have changed the configuration.
* `make`
* `make launch`
Let's dive into the sc_1 - NOS3 Flight Software window and break down what all is happening:

![Scenario cFS - Boot 0](./_static/scenario_cfs/scenario_cfs_boot0.png)

In the above we see a standard splash screen honoring the Simulation To Flight - 1 mission which started NOS3.  Then:
* PSP initializes tasks and memory.
* The various pieces of cFE start executing.
* ES determines its boot-up state (power on reset in this case).
* Versions are printed and logged for records on the spacecraft.

![Scenario cFS - Boot 1](./_static/scenario_cfs/scenario_cfs_boot1.png)

Additional version info is printed for each "Module" loaded, and then:
* The various shared objects from the cFS startup script are loaded:
  * Each is loaded down the list and the specified initialization function provided in the startup script is executed.
  * They each typically print a success message upon completion.

![Scenario cFS - Boot 2](./_static/scenario_cfs/scenario_cfs_boot2.png)

* Once all applications have initialized we enter the OPERATIONAL state.
* Depending on the boot type, a Relative Time Sequence (RTS) executes:
  * RTS1 - Power On Reset (POR)
  * RTS2 - Processor Reset
* In NOS3 we always do a hard POR each time we start up.

---
### Core Services

* Command Ingest (CI) and Telemetry Output (TO):
  * These applications handle the essential functions of receiving commands and outputting telemetry.
  * The NASA JSC CI and TO applications used in NOS3 leverage a modular architecture, and allow setting different desired modules for each.
  * The different modules can handle different interfaces or frame types for use.
    * Any frames received by CI must be broken down into the desired CCSDS Space Packets to be published on the software bus.
    * TO collects published software bus messages, and can then package multiple collected messages together into a large frame (CCSDS TM for example).
* Scheduler (SCH):
  * SCH is responsible for triggering periodic activities like telemetry collection or process wake-ups.
  * By default SCH runs at 100Hz, where each slot in the provided table executes repeatably in its slot.
  * The SCH table also lets you set both a delay and offset for a specific packet.

![Scenario cFS - CI TO SCH](./_static/scenario_cfs/scenario_cfs_ci_to_sch.png)

---
### Data Management

* File Manager (FM):
  * FM provides a means to create or remove directories and files, as well as poll what currently exists.
* Data Storage (DS):
  * DS creates files based on its defined tables.
  * It also listens on the software bus for telemetry, down samples as desired, and writes the data to one or more files.
  * It can have a maximum file size and age, as well as specific names for files determined by count or by current time at creation.
* CCSDS File Delivery Protocol (CFDP):
  * CFDP enables file transfers to and from the spacecraft.
  * Class 1 transfers are equivalent to UDP - if data is lost, it is lost.
  * Class 2 transfers are equivalent to TCP - any data that is transmitted will be transmitted via the ACK and NAK process.
  * Class 2 is typically preferred, but class 1 is sometimes used instead to minimize complexity or when data quantity supersedes quality.

![Scenario cFS - Data Management](./_static/scenario_cfs/scenario_cfs_data_management.png)

---
### Advanced Operations

* Limit Checker (LC)
  * LC monitors table specific telemetry, called watch points.
  * It also evaluates action points, statements (such as greater than a set value), and acts on them.
* Stored Command (SC)
  * Relative Time Sequence (RTS) tables are used to simplify operations or actions on the vehicle:
    * An RTS can send a set of commands with varying time delays between them.
    * SC has no means to provide logic, and must therefore rely on LC's action points or an operator to fire them.
  * Absolute Time Sequence (ATS) tables are also available to run various commands at a set time:
    * These mimic an RTS except for the fact that they use real times for the execution of each command which they contain.
    * An ATS is usually leveraged for advanced science purposes, to schedule future ground passes or collection periods.

![Scenario cFS - LC SC](./_static/scenario_cfs/scenario_cfs_lc_sc.png)

---
### Additional References

Some additional references and training related to cFS may be found at the following links:
* [cFS Github](https://github.com/nasa/cFS)
* [NASA cFS Training](https://ntrs.nasa.gov/api/citations/20205000691/downloads/TM%2020205000691.pdf)
