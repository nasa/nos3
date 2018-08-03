This is the SCH_LAB test application.

SCH_LAB is a simple packet scheduler application with a one second resoluton. 
It is intended to send housekeeping requests and other periodic packets 
in a lab/test CFS system.

For a more accurate flight scheduler application see the CFS SCH application. 

To use this application, first copy this "sch_lab" directory to the location
where the rest of the CFS applications are. Example:
/home/cfs/CFS_Project/apps

This application depends on the cFE core TIME 1Hz packet, which is disabled by default.
When using this app, edit the file: build/<cpuX>/inc/cfe_platform_cfg.h
and change:
#define CFE_TIME_ENA_1HZ_CMD_PKT  FALSE
to 
#define CFE_TIME_ENA_1HZ_CMD_PKT  TRUE

If it is already set to TRUE, then no change is required. 

To change the list of packets that SCH_LAB sends out, edit the schedule table
located in the platform include file:
build/<cpuX>/inc/sch_lab_sched_tab.h


