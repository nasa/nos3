# STF - Verification and Validation (V&V) Plan

## 1 Introduction
This verification and validation plan applies to the NASA Operational Simulator for Small Satellites (NOS3) and its Simulation to Flight (STF) design reference mission.  
NOS3 is described in more detail in the Read the Docs Wiki and section 3.

### 1.1 Purpose and Scope
NOS3 is a suite of tools developed by NASA's Katherine Johnson Independent Verification and Validation (IV&V) Facility to aid in areas such as software development, integration & test (I&T), mission operations/training, verification and validation (V&V), and software systems check-out. 
NOS3 provides a software development environment, a multi-target build system, an operator interface/ground station, dynamics and environment simulations, and software-based models of spacecraft hardware.
NOS3 provides an example mission implementation called STF where various use cases are demonstrated.

The purpose of this plan is to identify the activities that will establish compliance with the requirements (verification) and to establish that the system will meet the customers' expectations (validation).

### 1.2 Responsibility and Change Authority
The NASA Independent Verification and Validation (IV&V) Jon McBride Software Testing and Research (JSTAR) team is responsible for maintenance of this plan.  
This group has the authority to approve changes to this plan.

## 2 Applicable and Reference Documents
### 2.1 Applicable Documents
1. [Read the Docs Wiki](https://nos3.readthedocs.io/en/latest)
2. [STF Concept of Operations](STF_ConOps.md)

### 2.2 Reference Documents
1. [Software Development / Management Plan](Software_Development_Management_Plan.md)

### 2.3 Order of Precedence
In case of conflicts, the applicable documents take precedence over this document.  
This document takes precedence over the reference documents.

## 3 System Description
### 3.1 System Requirements Flowdown
The requirements for this system come from the Concept of Operations / Architecture, the User's Manual and Developer's Guide, and the Requirements document.

### 3.2 System Architecture
NOS provides a complete model of a satellite and the operational ground system used to control it.  
The satellite model enables development of the satellite flight software.  
This model includes simulators for the various hardware components on the satellite such as sun sensors, star trackers, thrusters, payloads, etc.  The diagram below depicts the NOS architecture.

![NOS Architecture](./_static/NOS3-Architecture.png)

More details about the system architecture for this system can be found in the Concept of Operations / Architecture document.

## 4 Verification and Validation Process
### 4.1 Verification and Validation Management Responsibilities
The NASA IV&V JSTAR team is responsible for verification and validation of NOS.

### 4.2 Verification and Validation Methods
#### 4.2.1 Analysis
Analysis: Use of mathematical modeling and analytical techniques to predict the compliance of a design to its requirements based on calculated data or data derived from lower system structure end product validations.

#### 4.2.2 Inspection
Inspection: The visual examination of a realized end product. Inspection is generally used to verify physical design features or specific manufacturer identification. 
For example, if there is a requirement that the safety arming pin has a red flag with the words “Remove Before Flight” stenciled on the flag in black letters, a visual inspection of the arming pin flag can be used to determine if this requirement was met.

#### 4.2.3 Demonstration
Demonstration: Showing that the use of an end product achieves the individual specified requirement (verification) or stakeholder expectation (validation). 
It is generally a basic confirmation of performance capability, differentiated from testing by the lack of detailed data gathering. 
Demonstrations can involve the use of physical models or mock-ups; for example, a requirement that all controls shall be reachable by the pilot could be verified by having a pilot perform flight-related tasks in a cockpit mock-up or simulator. 
A demonstration could also be the actual operation of the end product by highly qualified personnel, such as test pilots, who perform a one-time event that demonstrates a capability to operate at extreme limits of system performance.

#### 4.2.4 Test
Test: The use of a realized end product to obtain detailed data to verify or validate performance or to provide sufficient information to verify or validate performance through further analysis.

## 5 Verification and Validation Implementation
### 5.1 System Design and Verification and Validation Flow
The typical flow for development of a component is:
1.  Checkout application development
2.  Simulator development and test with the checkout application
3.  Flight software development reusing code from the checkout application
4.  Ground software command and telemetry database development
5.  Flight software test with ground software and simulator
6.  Flight software test with ground software and actual hardware

### 5.2 Test Articles
The flight software source code is the test article.  
This is the actual software to be used in flight but recompiled for a Linux environment.  
In addition, the ground software command and telemetry databases that are used for command and control of the flight software can also be treated as test articles.

### 5.3 Support Equipment
The ground software system Advanced Multi-Mission Operations System (AMMOS) Instrument Toolkit (AIT), COSMOS, OpenC3, or Yet Another Mission Control System (YAMCS) and the 42 dynamic software are required by the NOS system.  
In addition, simulations for the components being tested are required for testing.

### 5.4 Facilities
No special facilities are needed for NOS verification and validation since all hardware is simulated in software.  
A standard engineering laptop with git, Vagrant, and VirtualBox installed is sufficient for performing NOS verification and validation.  
For testing with actual hardware, the ground software will be run on a laptop and the laptop will be connected to the actual hardware either by radio or by a hard line connection.

## 6 System Verification and Validation
### 6.1 Complete System Integration
NOS can be used to perform flight and ground software verification and validation on the whole system or component by component.

#### 6.1.1 Developmental/Engineering Unit Evaluations
NOS mainly eliminates the need for performing evaluations on developmental or engineering units.  
It provides the ability to evaluate the actual flight code but recompiled for Linux.

#### 6.1.2 Verification Activities
##### 6.1.2.1 Verification Testing
Testing will be the main method of verification.  
As much as possible, it should be automated using the UT-Assert framework and, if possible, should provide pass/fail results.

##### 6.1.2.2 Verification Analysis
Analysis will be used in cases where deeper analysis is required to verify correct behavior of the software.

##### 6.1.2.3 Verification Inspection
Inspection will rarely be used.

##### 6.1.2.4 Verification Demonstration
Demonstration will be used, if necessary, to verify larger end-to-end type requirements that do not have a simple pass/fail criteria.

#### 6.1.3 Validation Activities
##### 6.1.3.1 Validation by Testing
Testing will be the main method of validation.  
As much as possible, it should be automated using the UT-Assert framework and, if possible, should provide pass/fail results.

##### 6.1.3.2 Validation by Analysis
Analysis will be used in cases where deeper analysis is required to validate correct behavior of the software.

##### 6.1.3.3 Validation by Inspection
Inspection will rarely be used.

##### 6.1.3.4 Validation by Demonstration
Demonstration will be used, if necessary, to validate larger end-to-end type scenarios that do not have a simple pass/fail criteria.

## 7 Program Verification and Validation
Once the flight and ground software have been verified and validated in the NOS environment, they can be verified and validated on the real flight hardware including the flight computer.

## 8 System Certification Products
The output products of the verification and validation activities include test plans, test procedures, test reports, and a completed requirements traceability matrix.

## Appendix A:  Acronyms and Abbreviations
- AIT:  AMMOS Instrument Toolkit
- AMMOS:  Advanced Multi-Mission Operations System
- I&T:  Integration and Test
- IV&V:  Independent Verification and Validation
- JSTAR:  Jon McBride Software Testing and Research
- NOS3:  NASA Operational Simulator for Small Satellites
- RTM:  Requirements Traceability Matrix
- V&V:  Verification and Validation
- YAMCS:  Yet Another Mission Control System

## Appendix B:  Definition of Terms
Validation (of a product): The process of showing proof that the product accomplishes the intended purpose based on stakeholder expectations and the Concept of Operations. 
May be determined by a combination of test, analysis, demonstration, and inspection. (Answers the question, “Am I building the right product?”)

Verification (of a product): Proof of compliance with specifications. 
Verification may be determined by test, analysis, demonstration, or inspection or a combination thereof. (Answers the question, “Did I build the product right?”)

## Appendix C:  Requirement Verification Matrix
TODO

## Appendix D:  Validation Matrix
TODO
