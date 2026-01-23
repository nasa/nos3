# Scenario - Patching an App or Table

This scenario was developed to explain and demonstrate the process by which a satellite operator could patch an app or table onboard a satellite, using NASA Operational Simulator for Space Systems (NOS3).
It demonstrates the use of NOS3 to test a patch, and subsequently the use of ground software (GSW) to go from merely commanding to sending updated code for an app or table. 

This scenario was last updated on 06/09/2025 and leveraged the `dev` branch at the time [a3e7c100].

## Learning Goals

By the end of this scenario, you should be able to:
 * Understand the use of NOS3 to test a patch for a satellite table.
 * Understand the use of GSW to transmit a patch to satellite apps or tables.

## Prerequisites

Before running the scenario, complete the following steps:
* [Getting Started](./NOS3_Getting_Started.md)
  * [Installation](./NOS3_Getting_Started.md#installation)
  * [Running](./NOS3_Getting_Started.md#running)

It may also be helpful to have gone through both the [Scenario - Nominal Operations](./Scenario_Nominal_Ops.md) and [Scenario - cFS](./Scenario_cFS.md) prior to this one.

## Walkthrough

Using `make launch`, launch NOS3 and open COSMOS as in previous scenarios:

![Scenario Patching - Organized NOS3](./_static/scenario_demo/scenario_demo_organized.png)

Before we can send a patch to the spacecraft, however, we must first create and test it.
For simplicity, we will walk through the steps to patch an RTS, although the steps to patch a simulator would be very similar. 

### Testing a Patch to an RTS

First, we will try adjusting an RTS in NOS3 and running it as a simulator/FlatSat to confirm that our changes have the desired effect.
 * We will begin by copying RTS006 from 'fsw/apps/sc/fsw/tables/sc_rts006.c' to 'cfg/nos3_defs/tables/sc_rts006.c'.  Then, open your new sc_rts006.c:
![RTS006 Before Edits](./_static/scenario_patching/rts006_pre_edits.png)
 * Edit this to send NOOP commands to the sample simulator three times, once every five seconds, until it looks something like the following:
![RTS006 Edited](./_static/scenario_patching/rts006_edited.png)
 * Also include `#include "sample_app.h"` at the top, or there will be a compiler error.

In a real scenario, nothing would be pushed directly to the spacecraft without first being tested.
We will do this in NOS3 by running the typical sequence of commands:
 * `make clean`
 * `make`
 * `make launch`
 * Confirm the behavior of the RTS:
 ![RTS006 Kickoff](./_static/scenario_patching/rts006_kickoff.png)
 ![RTS006 Running](./_static/scenario_patching/rts006_running.png)
 * `make stop`

Now that we have tested the new changes to an RTS on NOS3, using it as a simulated FlatSat, we can use COSMOS to simulate uploading this new file to a spacecraft on orbit.

### Patching an RTS

First make sure the compiled RTS file is saved somewhere on your computer so you can upload it later:
 * `cp ./fsw/build/exe/cpu1/cf/sc_rts006.tbl /tmp/sc_rts006.tbl`

Next, we will want to actually send the changed file up to the spacecraft.
We have to first ensure that the simulation does not launch with the new file, which we can do via the following commands:
 * `make clean`
 * `git reset --hard --recurse-submodules`
 * `git clean -x -d -f`

Now, run `make` then `make launch` and confirm the old RTS006 is active (i.e., NOOP commands from the SC app and no NOOP commands from the sample app).
This is where you would start if you were doing the upload on a real spacecraft, after testing the change (as above) on NOS3 and/or a FlatSat.

Copy the file `/tmp/sc_rts006.tbl` to `/tmp/nos3/sc_rts006.tbl`.  Now use the CFDP command to upload the new sc_rts006.c file to the correct place ('/cf/sc_rts006.tbl') on the simulated spacecraft.

Go to the COSMOS Command Sender and select "CFDP" in the upper-left hand corner and "SEND_FILE" to the right:

![COSMOS CFDP](./_static/scenario_patching/scenario_patching_CFDP.png)

This COSMOS command allows us to send a new file from the ground station to a simulated spacecraft on orbit.
Looking into the list of parameters, one can see a path to a local file (SRCFILENAME) and to a remote, spacecraft file (DSTFILENAME). 

The other two parameters are CLASS and DEST_ID:  
 * The CLASS denotes whether to use Class 1 or Class 2 data transfers:
    * Class 1 transfers are equivalent to UDP, so any data lost is lost for good.
    * Class 2 transfers are equivalent to TCP, and use the ACK/NAK process to confirm transmission of the data.
 * Class 2 is typically preferred, and this is also true for patching the spacecraft, where a partial file could cause significant problems.  Accordingly, we will leave the CLASS parameter as 2.
 * DEST_ID determines where the data is being sent. More specifically, it will only change when multiple spacecraft are present to receive data.  Accordingly, we will also leave it as-is.  

Next reboot CFE so that FSW will automatically reload all the tables.
![SC Reboot](./_static/scenario_patching/sc_reboot_cmd.png)

Now execute RTS 6:
![Execute RTS 6](./_static/scenario_patching/execute_rts006.png)

As before, you should see three SAMPLE NOOP commands, one every five seconds, come through on the NOS3 FSW terminal.
If looking exclusively at COSMOS, the Command Counter of the SAMPLE app should increment at the same rate, three times.

You have now successfully simulated both the testing and patching of a spacecraft RTS table!
Patching an app would be done in the same manner, and is left as an exercise to the reader.

**_NOTE:_** For those wishing to slightly extend this scenario, they may wish to try running again but with the `make cosmos-operator` command instead of `make launch`.
The former command launches all of NOS3, but only shows COSMOS (since that is all that would be seen by a spacecraft operator in a real scenario).  

