proc ut_setupevents (SCID, PROCESSORID, APPNAME, EVENTID, EVENTTYPE, EVENTLOC)
;
; Description:
;	This proc utilizes three (3) global arrays to implement the capability
;	of capturing multiple Event IDs for a given sequence of commands. The
;	arrays will hold the AppName, EventID, and EventType respectively that
;	the caller is looking for.
;
; Assumptions and Constraints:
;	This proc assumes that the SCID and PROCESSORID are the same for all
;	events to capture.
;
; History:
;
;   03/15/06        W. Moleski		Created this proc to handle
;					multiple events
; ******************** SETUP FOR EVENT  ****************************
;
;
;directive ut_setupevt (SCID, PROCESSORID, APPNAME, EVENTID, EVENTTYPE, EVENTLOC) is
;begin

LOCAL SCNUM, PROCESSORNUM, TYPENUM
LOCAL s_under_bar="_"
LOCAL p_under_bar = "_"
LOCAL i, j

SCID        = %upper(SCID)
PROCESSORID = %upper(PROCESSORID)
EVENTTYPE = %upper(EVENTTYPE)

; set up the event codes
IF (SCID = "SCX") THEN
   SCNUM = 66
ELSEIF (SCID="") THEN
   SCNUM = ""
   s_under_bar=""
ENDIF

IF     (PROCESSORID = "CPU1" OR PROCESSORID = "CDH") THEN
   PROCESSORNUM = 1
ELSEIF (PROCESSORID = "CPU2" OR PROCESSORID = "PSE") THEN
   PROCESSORNUM = 2
ELSEIF (PROCESSORID = "CPU3" OR PROCESSORID = "ACE") THEN
   PROCESSORNUM = 3
ELSEIF (PROCESSORID = "") THEN
   PROCESSORNUM = ""
   p_under_bar=""
ENDIF

IF (EVENTTYPE = "DEBUG")     THEN 
   TYPENUM = 1
ELSEIF (EVENTTYPE = "INFO")      THEN 
   TYPENUM = 2
ELSEIF (EVENTTYPE = "ERROR")     THEN 
   TYPENUM = 3
ELSEIF (EVENTTYPE = "CRITICAL")  OR (EVENTTYPE = "CRIT") THEN 
   TYPENUM = 4
ENDIF

j = EVENTLOC

;; Set the pseudo telemetry values
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].SCID"} = SCNUM
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].PROCESSORID"} = PROCESSORNUM 
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].NUM_FOUND_MESSAGES"}   = 0
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].APPNAME"} = %upper(APPNAME)
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].EVENTID"} = EVENTID
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT["&j&"].EVENTTYPE"} = TYPENUM

;;write ";***********************************************************************"
;;write "; Setup event ", j, " with ", APPNAME, " ", EVENTTYPE, " ", EVENTID
;;write ";***********************************************************************"

;end
endproc
