# Introduction
This document provides an overview of operations for the design reference mission.
This includes the weekly planning cycle for contacts (passes) and the per pass operations.

# Weekly Planning
The antenna used in the design reference mission is assumed to be a shared resource.
As such, the antenna operators produce a plan each week listing the contact times (passes) the antenna will support for each mission they support.
The plan will also list the maximum elevation of the antenna during each pass.  This is useful for determining the duration of any given pass. 

This plan is shared with the spacecraft control team.
If necessary, negotiations and iterations may occur between the design reference mission spacecraft control team and the antenna operations team to make changes to the schedule.

# Early Operations / Commissioning
When the spacecraft is first launched, the first passes will involve trying various two-line element sets with the antenna to both locate the spacecraft and determine which spacecraft identifier is the correct one for the spacecraft in question.

After that, various subsystems and experiments will be turned on and off to verify that they are operational.
Telemetry will be collected to assess the health of each subsystem and experiment.

Once the subsystems and experiments are verified to be working, commissioning will be complete and it will be possible to begin performing normal science operations.

# Per Pass Operations
Prior to the pass, the spacecraft operator should make note of the contact start and end times and the maximum elevation of the antenna during the pass.
Because each pass is only a fairly brief amount of time, the operator should determine beforehand what data is to be downlinked during the pass and what other operations should be performed during the pass (such as taking pictures or turning experiments on or off).

During the pass, the spacecraft operator should communicate with the antenna operator regarding the antenna status, the commands being sent and the telemetry being received.
During the pass, the first thing the spacecraft operator should do is confirm key health and status telemetry for the spacecraft including what mode the spacecraft is in (science, sunsafe, etc.)
Once this necessary information has been collected, the spacecraft operator should issue commands to start or stop relative time sequences, to start telemetry collection, and to command the state of the spacecraft and any onboard experiments.
The spacecraft operator should also monitor the health and status of the spacecraft during the pass.
Essentially, each pass should begin with confirming the spacecraft mode and general health, and then proceed to sending commands to change the state of the spacecraft or its onboard experiments.  This latter category also includes downlinking data from the spacecraft to the ground, which is likely what will comprise the bulk of most passes.

At the end of the pass, the spacecraft operator should confirm with the antenna operator the time of the next contact.
After the pass, the spacecraft operator should also kickoff any procedures needed to process the telemetry received during the pass.

