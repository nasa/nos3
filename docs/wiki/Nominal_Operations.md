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
Telemetry and files will be collected to assess the health of each subsystem and experiment.

Once the subsystems and experiments are verified to be working, commissioning will be complete and it will be possible to begin performing normal science operations.

# Per Pass Operations
Prior to the pass, the spacecraft operator should make note of the contact start and end times and the maximum elevation of the antenna during the pass.
The operator should identify any files that are planned to be uploaded and that they exist in the correct location.
The operator should formulate a game plan of goals for the pass including commands and files to uplink and telemetry and files to downlink.
Because each pass is only a fairly brief amount of time, the operator should determine beforehand what data is to be downlinked during the pass and what other operations should be performed during the pass (such as taking pictures or turning experiments on or off).
The operator should log into the console where they will be commanding/controlling the satellite and make sure that all ground software applications and windows are functional and ready.
Just before the pass, the ground antenna will connect the satellite ground software to the antenna.

During the pass, the spacecraft operator should communicate with the antenna operator regarding the antenna status, the commands being sent and the telemetry being received.
During the pass, the first thing the spacecraft operator should do is command the spacecraft radio to be enabled.
The next thing the spacecraft operator should do is request and confirm key health and status telemetry for the spacecraft including what mode the spacecraft is in (science, sunsafe, etc.), the state of charge of the battery, whether or not the reboot counter has incremented, etc.
The operator should also see that the command and telemetry counters are incrementing as commanding is performed and telemetry is received.
Once this necessary information has been collected, the spacecraft operator should issue commands to start or stop relative time sequences, to start telemetry collection, to uplink or downlink files, to command the state of the spacecraft and any onboard experiments, etc.  
The spacecraft operator should also manage the onboard storage of the spacecraft such as monitoring the free storage remaining and deleting files/telemetry that have been verified to have been downlinked.
The spacecraft operator should also monitor the health and status of the spacecraft during the pass.
Essentially, each pass should begin with confirming the spacecraft mode and general health, and then proceed to sending commands to change the state of the spacecraft or its onboard experiments.  This latter category also includes downlinking data from the spacecraft to the ground, which is likely what will comprise the bulk of most passes.

Near the end of the pass, the spacecraft operator should disable the spacecraft radio.
At the end of the pass, the spacecraft operator should confirm with the antenna operator the time of the next contact.
After the pass, the spacecraft operator should also kickoff any procedures needed to process the telemetry and files received during the pass.

