# Scenario - Commanding ADCS in Science Mode

This scenario was developed to demonstrate how to adapt an existing mission to changing science goals, with the specific example of a request for a different pointing routine during Science Mode.

This scenario was last updated on 5/7/25 and leveraged the `dev` branch at the time [53d4627].

## Learning Goals
By the end of this scenario you should be able to:
- Understanding how to plan effectively before making changes to ensure you 
    - Identify everything you need to do to cover the behavior you intend 
    - Ensure things are returned to a known state afterwards
- Understand how to modify existing RTSs to match new parameters
- Understand how to test these changes and related edge cases in NOS3

## Prerequisites
Before running the scenario, ensure the following steps are completed:

* [Getting Started](./Getting_Started.md)
  * [Installation](./Getting_Started.md#installation)
  * [Running](./Getting_Started.md#running)

You should also review the following before beginning this scenario:
- [STF - Quick Look](./STF_QuickLook.md)
* [Scenario - ADCS Walkthrough](./Scenario_ADCS_Walkthrough.md)
* [Scenario - Device Fault in Science Mode](./Scenario_Fault_Science.md)

## Introduction
For this scenario, we can assume the following setup: 
After a period of routine mission operations, the Science Team has noticed that the implemented sunpoint angles lead to a non-optimal rotation of the craft for 
collecting science data. They therefore request that the Operations Team ensure the craft is pointed to a rotational quaternion of `[0, 0, 0, 1]` while taking science data. Thus, we will need to modify our existing RTS tables 
to assure the new mission parameters are being fulfilled, and any new edge cases are being covered.

## Step 1 - Planning
Before any changes to mission behavior, one should consider what exactly needs to be done, how to do it, where the changes need to be made, and any new edge cases that may arise from this new behavior.

### Part A: Determining Scope
In this case, Science Mode is governed by RTS tables which are triggered when passing certain LC watch points, as covered in the quick look. Since we are only changing behavior _during_ Science Mode, and not 
the behavior of when or how it is triggered, this means we should only have to modify the RTS tables that govern Science Mode behavior, and not the associated LC watchpoints.

### Part B: Determining Modifications for Behavior
Now that we've determined the scope, we need to consider what is necessary for inertial pointing that differs from our default configuration. Inertial pointing utilizes data from the spacecraft's Star Tracker in order to 
determine the current Quaternion values. This allows the ADCS to adjust the spacecraft attitude to point towards a specified quaternion, as mentioned in the ADCS Walkthrough. Referencing the Quick Look, we see that EPS Switch 1 is tied to the 
Star Tracker, and it, along with the Star Tracker App, are off by default. Thus, we learn we need to turn on the EPS Switch associated with the Star Tracker (Set Switch 1 to `0xAA` or `170`), Enable the Star Tracker App, 
and then we can set the ADCS Mode to Inertial and set the Inertial Quaternion for Pointing in ADCS to `[0, 0, 0, 1]`.

### Part C: Considering Edge Cases
Finally, now that we've determined what is necessary to achieve that behavior during Science Mode, we need to consider the new issues and edge cases that could or will be created when we leave active Science Mode or transition to 
Safe Mode. Firstly, the app and switch for Star Tracker should be disabled when leaving active Science Mode, just as Sample and its switch are. Doing otherwise would waste power and could lead to an inability to charge 
the craft, or at the least inefficient charging. Secondly, the ADCS mode should be restored to Sunsafe once we have left active Science Mode, so that our charging angle is again optimized, and so that we are always in a 
known-safe state if we exit active science for geographic reasons or due to a fault or ground command. Thus, we also need to modify any RTS tables associated with transitions out of active Science or back to Safe Mode.

Now, after careful planning, we should be prepared to move on to implementing our changes.

## Step 2: Implementation
Referencing the Quick Look or the code itself and searching through the other relevant commands (e.g. Sample Switch/App Toggles), we find that RTS tables 30, 31, and 32 are our cases where we enter Active Science, 
and that RTS tables 27, 29, 33, 34, and 35 are cases where we exit Active Science Mode. The former and latter groups will each have their own set of changes, though their numbers in the RTS may change depending on 
which table you are modifying.

### Part A: Enabling and Setting Up Inertial Mode
As discussed in planning, this is a process that will require 4 extra commands:
 - Enabling EPS Switch 1
 - Enabling the Star Tracker cFS Application
 - Setting the ADCS Mode to `INERTIAL_MODE`
 - Setting the ADCS Inertial Quaternion to `[0, 0, 0, 1]` 
	 - _Note: For this, we needed to switch the ADCS Command Structure for the Quaternion command to use `float` instead of `double` (which was used originally and is still used elsewhere in ADCS).  This is because doubles are not
     currently compatible with cFS tables. This may be switched back once a patch is worked into NOS3, and cFS if it hasn't been already_

To achieve this, three areas of each RTS table need to be changed:
- The required headers for Star Tracker and ADCS need to be added to the `#include` section of the file, as below:
  ![adcsDuringScience_EnableIncludes](https://github.com/user-attachments/assets/e64a40dd-705a-4f68-80dd-c6358fe4320e)
- Then, add the extra commands and headers to the RTS in that section, and shift any existing headers and commands to accommodate:
  ![adcsDuringScience_EnableHeaders](https://github.com/user-attachments/assets/6d3f7a8a-cefb-4c07-9251-b498ba87a542)
- Finally, add the definitions for those commands and headers to the actual execution section of the RTS, and shift pre-existing commands to accommodate.
  ![adcsDuringScience_EnableCommands](https://github.com/user-attachments/assets/800147f0-33e1-4172-a16b-43df9faba11e)

Note that although the changes are only shown in RTS table 30, they will need to be made in RTS tables 30, 31, and 32. The commands should be the same, but their location may vary depending on what else the table is doing (though in this case, these 3 tables are quite similar). You can go through each aforementioned table to see how the changes are adapted for that table in particular.

### Part B: Disabling Inertial Mode and Resetting to Sunsafe
As discussed in planning, this is a process that will require 3 extra commands:
 - Disabling the Star Tracker cFS Application
 - Disabling EPS Switch 1
 - Setting the ADCS Mode to `SUNSAFE_MODE`

To achieve this, three areas of each RTS table needs to be changed:
- The required headers for Star Tracker and ADCS need to be added to the `#include` section of the file
  ![adcsDuringScience_DisableIncludes](https://github.com/user-attachments/assets/aaf1abde-139e-4c90-b397-1e1f854b9951)
- Then, add the extra commands and headers to the RTS in that section, and shift any existing headers and commands to accommodate
  ![adcsDuringScience_DisableHeaders](https://github.com/user-attachments/assets/4cc073ee-8276-43bf-afee-82ce609dc10b)
- Finally, add the definitions for those commands and headers to the actual execution section of the RTS, and shift pre-existing commands to accommodate.
  ![adcsDuringScience_DisableCommands](https://github.com/user-attachments/assets/45af81af-928b-48e0-aa2b-ddfc551edd55)

As above, the changes are only shown in RTS table 33, but they will need to be made to tables 27, 29, 33, 34, and 35. The commands should be the same, but where they are located will vary depending on what else the table is doing. You can go through each aforementioned table to see how the changes are adapted for that table in particular.

## Step 3: Verifying Intended Behavior
With this, you should be able to test that it works by bringing up NOS3, launching COSMOS, and commanding the craft to enable data collection over the various regions and to enter Science Mode. 
![adcsDuringScience_EnableAlaska](https://github.com/user-attachments/assets/581b6510-3c56-4ef3-80fd-219d63a37471)
![adcsDuringScience_EnableCONUS](https://github.com/user-attachments/assets/07b36a4b-af76-419f-86d7-32af6af0a543)
![adcsDuringScience_EnableHawaii](https://github.com/user-attachments/assets/5f1386ff-6747-4205-952d-6e541f5628de)
![adcsDuringScience_ScienceMode](https://github.com/user-attachments/assets/04b82d2b-c266-4743-b70a-834652e348c2)

Then, launch your Telemetry Grapher to the EPS_Test preset and add your Star Tracker's Enabled Value and the EPS Switch 1 State to the bottom table. The former should be on the left axis, the latter on the right axis and 
shifted by -85.0 (Star Tracker Enabled should be similar to Sample Enabled's configuration, and the EPS Switch 1 should be similar to the EPS Switch 0's configuration). Then, once the vehicle enters Science Active, verify that 
all 4 variables flipped from their low, disabled states to their high, enabled states. Also verify that when the spacecraft leaves CONUS, goes to Safe Mode (manually or otherwise), or has its state of charge go below 60% 
(which you can cause with the Sim Bridge commands) you observe that the switches return to their disabled states once the associated RTS has finished running.
![adcsDuringScience_SciencePass](https://github.com/user-attachments/assets/f7414a42-973b-483b-aacf-ecbff497035c)
![adcsDuringScience_SciencePass_SimBridgePowerExit](https://github.com/user-attachments/assets/bdafd3ff-6eb5-4bc3-8795-1d2a2a2cf0b6)
![adcsDuringScience_SciencePass_SimBridgePowerReentry_CONUSExit](https://github.com/user-attachments/assets/21e16bab-78e0-4c9c-901d-488c27a75c9a)

## Conclusions
At this point, you should be comfortable with the thought process behind making adaptations to a mission to meet new needs, and adapting RTS tables to do so. In future Scenarios, we will build upon this with more 
complex scenarios that may develop in flight.

