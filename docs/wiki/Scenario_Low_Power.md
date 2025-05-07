# Scenario - Low Power

This scenario was developed to demonstrate how to identify and resolve an emergency situation that develops in flight, specifically one related to low power.
## Learning Goals
By the end of this scenario you should be able to:
- Analyze a complex situation with limited information to determine the cause of an anomaly
- Learn about low power contingencies in space operations
- Learn how to patch a mission in flight to

## Prerequisites
Before running the scenario, ensure the following steps are completed:

- [Getting Started](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md)
    - [Installation](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md#installation)
    - [Running](https://github.com/nasa/nos3/blob/6fc41656447de78689dedfc770c0809dddad6231/docs/wiki/Getting_Started.md#running)

You should also likely review the following lessons before this one:
- [STF - Quick Look](https://github.com/nasa/nos3/blob/dev/docs/wiki/STF_QuickLook.md)
- [Scenario - ADCS Walkthrough](https://github.com/nasa/nos3/blob/dev/docs/wiki/Scenario_ADCS_Walkthrough.md)
- [Scenario - In Flight Patching](https://github.com/nasa/nos3/blob/dev/docs/wiki/Scenario_Patching.md)
- [Scenario - Adding FDC Check for Sample Disabled to Science Mode](https://github.com/nasa/nos3/blob/dev/docs/wiki/Scenario_Fault_Science.md)
- [Scenario - Commanding ADCS in Science Mode](https://github.com/nasa/nos3/blob/dev/docs/wiki/Scenario_Controlling_ADCS_During_Science.md)
## Introduction
The goal of this scenario is to build off of much of the knowledge gained in previous scenarios to identify and solve a low power issue that develops on-orbit during mission operations. The following is the situation
presented:

You are the Software Maintenance Team for the STF mission. It has launched, been commissioned, and has been running over the US. Its power state has slowly decline over time, though still within normal limits, and still 
recharging during daytime when science data is not being gathered or a pass is not being taken. The Mission Operations Center (MOC) reports that, after a routine partial check of the EPS sent from the ground based off the 
System Tests for EPS, and reactivation of Science Mode for the next Pass, Active Science was inhibited due to a low power state, and the satellite is not charging at normal during the last few minutes before it enters 
eclipse, but instead discharging. With it entering eclipse shortly, there is concern that the craft will drop to a dangerously low power state before it can charge again, assuming whatever is preventing charging is 
resolved. You are called by the MOC to triage the situation and provide an in-flight patch to try and save the mission.

## Step 1 - Scenario Setup
In order to set up this scenario, a few configuration changes need to be made to simulate the scenario above. First, you will need to set the starting time of the mission to `814202000.526` in your `cfg/nos3_mission.xml`. 
This should set you into a situation with an evening/night science pass over the US.

(picture here)

Additionally, you will need to go into the `cfg/InOut/Orb_LEO.txt` file and change the `True Anomaly` parameter from `0.0` to `75.0`. This should start your spacecraft in a position near Alaska.

(picture here)

Finally, you will want to change your `<battery-charge-state>` parameter under the EPS section of your `cfg/sim/nos3-simulator.xml` from `1.0`, which denotes a 100% state of charge, to around `0.65`, which would denote a 
65% charge. This will simulate the craft already being in a fairly low power state from other operations.

(picture here)

From there, rebuild and launch NOS3 as you would normally. I would encourage also minimizing the 42 GUI and FSW consoles and using COSMOS for all your information, as this would put you in a perspective most similar to 
that of an Operator in the MOC.
- _Note: In the future, you could launch NOS3 in operator mode with `make cosmos-operator` and follow the instructions in the terminal to make sure everything launches. This would put you in a more operator-like 
mode to start, though the 42 GUI will still launch. However, this feature is still in development, so simply using `make launch` as usual is advised._

(picture here)

From there, wait a few moments then open the Script Runner and Telemetry Grapher. In Telemetry Grapher, launch the `EPS_test.rb` and then hit Start. This will track your power level and switch/in sun statuses in the 
graph. Then, go to Script Runner, hit open, and go to `cosmos/procedures`. Then select `PassSetupEPSCheck_LowPowerScen.rb` in the new window, and once it is open, hit Start. This is the setup and test file the MOC ran 
that seems to have caused the issue. It was intended to test the EPS, and then set up Science Mode. Then, let the simulator run, and observe that the power starts dropping after the science pass starts as you would 
expect, but then the science pass stops prematurely, and the power is still draining despite that. This is the point where you would enter to begin triage with the night approaching.

(picture here)

## Step 2 - Identification and Triage
This section will cover how to utilize tools in COSMOS and a logical thinking process to pinpoint points of failure that may be causing the issue.

### Part A: Pinpointing Possible Issues
Since the issue appears to be with an unexpected power drain, even during daytime, the first place that should make sense to look would be the EPS Telemetry. Go to your Packet Viewer window, and navigate to `GENERIC_EPS` 
in the `Target` dropdown. There should be only one Packet for it, so simply scroll through and observe. Switches 0 and 1, which are used, should be off, but you may notice that switch 7, which should be unused, is on and 
drawing quite a bit of power.

(picture here)

The temporary fix at this stage would be to go to your command sender and command switch 7 off manually, and verify that it turns off in your Telemetry Viewer. This should at least decrease the power draw to safer levels.

(picture here)

### Part B: Finding the Cause
Now that the issue has been identified and temporarily fixed, the next move should be to determine what may have caused the problem. The first two places that should come to mind to look are a faulty RTS table which has 
the switch being activated, or an errant ground command. In this case, if we check the script that was run to set up the pass, you may notice an errant switch command, which turns on switch 7. This seems to be the point 
of failure here, so you should assure it is removed or fixed for future passes.

(picture here)

If it was *not* in the ground script, then you would likely wish to assure that backup versions of the RTS tables that have been checked for the error are sent up to overwrite any currently onboard, as discussed in the 
In-Flight Patching Scenario. Though, with the error identified, and the speed of discharged observed, the Operations Team deems it wise to add in some sort of failsafe to assure operator error cannot lead to the complete 
death of the craft.

## Step 3 - Planning the Failsafe
As emphasized in previous scenarios, before any changes to mission behavior are made, one should consider what exactly needs to be done, how to do it, where the changes need to be made, and any new edge cases that may 
arise from this new behavior.

### Part A: Determining Scope
In this case, we want to add a new case where, if the battery gets below a critical level, it will go into a known, safe state. We may also want to make sure any existing power-related triggers go to a fully determined 
EPS state, though that is up to your discretion. The Avionics Team reports that the battery being used should be recoverable as long as it does not drop below 25%. Thus, 40% is deemed a safe cutoff margin to go into a 
known safe state in the case of extreme power loss while still being able to survive a night and recharge. 

This means we will need a new LC Watchpoint for 40% power, designed similarly to the existing 60% watchpoint, and we will need to assure it is activated when entering Science Mode. Then, we need to create a new RTS that 
will transition to Safe Mode if the watchpoint is triggered. Additionally, if it *is* determined that we want to add additional safety measures to our existing low power Science Pause at 60%, we would need to modify that 
RTS as well.

### Part B: Determining Modifications for Behavior
Now that we've determined the scope, we need to consider what is necessary for these changes. The LC tables and RTS tables will need to be modified or added as shown in the FDC Check Scenario to add a new watchpoint at 
40% power, and to assure it is activated. Then, any safe mode transitions need to add new commands added to turn off any switches that are not being currently. Currently, only switches 0 and 1 are being turned off, so 
thus new commands for switches 2 through 7 need to be added, and the new Safe Mode Entry tables should have switches 0 through 7 and any associated apps (in this case Sample and Star Tracker) turned off or disabled, 
respectively.

### Part C: Considering Edge Cases
Finally, now that we've determined what is necessary to achieve that behavior during Science Mode, we need to consider the new issues and edge cases that creates for when we leave active Science Mode or transition to 
Safe Mode. Due to the nature of this, most edge cases should be covered, but it must be assured these changes are added to all safe mode transitions, and all power-related transitions if deemed necessary. Additionally, 
with a copy error starting all of this issue, there should be abundant caution to avoid that in the new tables. That is another point where testing in simulation software, such as NOS3, becomes extremely valuable in 
actual operations - to assure that all the edge cases are tested and that patches do not have unintended negative consequences or points of failure.

## Step 4: Implementation
Implementation is left open for you to determine based on previous lessons, but the basic steps would be to add the watchpoint, add any new RTS tables and modify the existing ones to use this watchpoint and reflect the 
planned behavior, and then to either reboot the system and compile the new tables that way, or compile the tables in a testing environment such as NOS3, copy the compiled .tbl files during execution, and then utilize 
CFDP and cFS's existing table commands to hot swap in the new tables as shown in the In Flight Patching Scenario.

## Step 3: Verifying Intended Behavior
With this, you should be able to test that it works by bringing up NOS3, launching COSMOS, and running the low power scenario as described in Step 1, but with your patches. Then, observe the telemetry and see if it 
enters your mode at 40%, and that if you added the commands to toggle all switches off at 60% power, then observe that charging resumes for any remaining daylight after it enters science passive. If necessary, you can 
test your patches through the Sim Bridge commands by manually setting your state of charge to 40% and making sure the failsafe triggers.

## Conclusions
Hopefully, through this Scenario, you have gained a degree of confidence in putting all the previous lessons together to address a real scenario and see it successfully resolved, and gained a better understanding of 
how to find the cause of an emergency utilizing the data available to a spacecraft operator on the ground..
