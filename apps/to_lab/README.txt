This is the TO_LAB test application.

TO_LAB is a simple telemetry downlink application that sends CCSDS
telemetry packets over a UDP/IP port. The UDP port and IP address are
specified in the "Enable Telemetry" command. 

It provides a test interface to a CFS system in a lab environment.  
It does not provide a full CCSDS Telemetry stack implementation. 

To send telemtry to the "ground" or UDP/IP port, edit the 
subscription table in the platform include file:
build/<cpuX>/inc/to_lab_sub_table.h 
TO_LAB will subscribe to the packet IDs that are listed in this table
and send the telemetry packets it receives to the UDP/IP port.

To use this application, first copy this "to_lab" directory to the location
where the rest of the CFS applications are. Example:
/home/cfs/CFS_Project/apps


