
;********************************************************************
;  cfe_evs_bin_fltr_reqts.h
;
;  Date         Tester:
;  FEB 13-2006  Eva I. Stattel Original.
;  March 2007   N. Schweiss Updated for cFE 4.0 test.
;
;  Purpose:
;  This is a header file created for use by the 
;  scx_cpux_evs_bin_fltr.prc and all its subordinate
;  procedures.
;  In it are defined all the requirements tested by
;  scx_cpux_evs_bin_fltr.prc
;
;*******************************************************************

; ut_req_array_size is actually index for last requirement
; in zero-based ut_requirement and cfe_requirements arrays.
global ut_req_array_size = 22
global ut_requirement[0 .. ut_req_array_size]

; Define these values to be used by ut_setrequirements directive.
#define cEVS3003 0
#define cEVS3004 1
#define cEVS3009 2
#define cEVS3010 3
#define cEVS3011 4
#define cEVS3012 5
#define cEVS3019 6
#define cEVS3019_1 7
#define cEVS3019_2 8
#define cEVS3020 9
#define cEVS3020_1 10
#define cEVS3100 11
#define cEVS3100_1 12
#define cEVS3103 13
#define cEVS3103_3 14
#define cEVS3103_3_1 15
#define cEVS3104 16
#define cEVS3104_1 17
#define cEVS3105 18
#define cEVS3105_1 19
#define cEVS3106 20
#define cEVS3107 21
#define cEVS3302 22

