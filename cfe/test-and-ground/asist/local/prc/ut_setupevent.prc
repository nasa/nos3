proc ut_setupevent (SCID, PROCESSORID, APPNAME, EVENTID, EVENTTYPE)
;
; History:
;
;
;
; ******************** SETUP FOR EVENT  ****************************
;
;

;;directive ut_setupevt (SCID, PROCESSORID, APPNAME, EVENTID, EVENTTYPE) is
;;begin

LOCAL SCNUM, PROCESSORNUM, TYPENUM
LOCAL s_under_bar="_"
LOCAL p_under_bar = "_"

SCID        = %upper(SCID)
PROCESSORID = %upper(PROCESSORID)
APPNAME     = %upper(APPNAME)
EVENTTYPE   = %upper(EVENTTYPE)

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

IF     (EVENTTYPE = "DEBUG")     THEN 
   TYPENUM = 1
ELSEIF (EVENTTYPE = "INFO")      THEN 
   TYPENUM = 2
ELSEIF (EVENTTYPE = "ERROR")     THEN 
   TYPENUM = 3
ELSEIF (EVENTTYPE = "CRITICAL")  OR (EVENTTYPE = "CRIT") THEN 
   TYPENUM = 4
ENDIF

{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].SCID"}        = SCNUM
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].PROCESSORID"} = PROCESSORNUM 
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].APPNAME"}     = APPNAME
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].EVENTID"}     = EVENTID 
{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].EVENTTYPE"}   = TYPENUM

{SCID & s_under_bar & PROCESSORID & p_under_bar & "FIND_EVENT[1].NUM_FOUND_MESSAGES"}   = 0

endproc
