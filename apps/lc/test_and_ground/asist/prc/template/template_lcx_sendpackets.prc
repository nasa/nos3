PROC $sc_$cpu_lcx_sendpackets(step_num,data_run)
;*******************************************************************************
;  Test Name:  lcx_sendpackets
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;  	This procedure sends data using the Limit Checker test application
;	(TST_LC) Send Packet command.
;
;  Prerequisite Conditions
;	The LC and TST_LC applications are loaded and running
;
;  Change History
;
;	Date	        Name		Description
;	09/27/12	Walt Moleski	Initial implementation for LCX
;
;  Arguments
;	step_num   The step number to write for the current call of this proc
;	data_run   The number representing the data to send
;
;  Procedures Called
;	ut_setupevt
;	ut_tlmupdate
;
;**********************************************************************

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"

;**********************************************************************
; Define local variables
;**********************************************************************
Local MsgId[20]
Local Size
Local Pattern[32]

write ";*********************************************************************"
write ";  Step ",step_num, ".1: Setting up Message IDs to use "
write ";*********************************************************************"
;; For CPU1 use CPU2 Message IDs
MsgId[1] = 0x987
MsgId[2] = 0x988
MsgId[3] = 0x989
MsgId[4] = 0x98a
MsgId[5] = 0x98b
MsgId[6] = 0x98c
MsgId[7] = 0x98d
MsgId[8] = 0x98e
MsgId[9] = 0x98f
MsgId[10] = 0x990
MsgId[11] = 0x991
MsgId[12] = 0x992
MsgId[13] = 0x993
MsgId[14] = 0x994
MsgId[15] = 0x995
MsgId[16] = 0x996
MsgId[17] = 0x997
MsgId[18] = 0x998
MsgId[19] = 0x999
MsgId[20] = 0x99a

if ("$CPU" = "CPU2") then 
  ;; Use CPU3 Message IDs
  MsgId[1] = 0xa87
  MsgId[2] = 0xa88
  MsgId[3] = 0xa89
  MsgId[4] = 0xa8a
  MsgId[5] = 0xa8b
  MsgId[6] = 0xa8c
  MsgId[7] = 0xa8d
  MsgId[8] = 0xa8e
  MsgId[9] = 0xa8f
  MsgId[10] = 0xa90
  MsgId[11] = 0xa91
  MsgId[12] = 0xa92
  MsgId[13] = 0xa93
  MsgId[14] = 0xa94
  MsgId[15] = 0xa95
  MsgId[16] = 0xa96
  MsgId[17] = 0xa97
  MsgId[18] = 0xa98
  MsgId[19] = 0xa99
  MsgId[20] = 0xa9a
elseif ("$CPU" = "CPU3") then 
  ;; Use CPU1 Message IDs
  MsgId[1] = 0x887
  MsgId[2] = 0x888
  MsgId[3] = 0x889
  MsgId[4] = 0x88a
  MsgId[5] = 0x88b
  MsgId[6] = 0x88c
  MsgId[7] = 0x88d
  MsgId[8] = 0x88e
  MsgId[9] = 0x88f
  MsgId[10] = 0x890
  MsgId[11] = 0x891
  MsgId[12] = 0x892
  MsgId[13] = 0x893
  MsgId[14] = 0x894
  MsgId[15] = 0x895
  MsgId[16] = 0x896
  MsgId[17] = 0x897
  MsgId[18] = 0x898
  MsgId[19] = 0x899
  MsgId[20] = 0x89a
endif

write ";*********************************************************************"
write ";  Step ",step_num, ".2: Sending Packets "
write ";*********************************************************************"

;;********************
;;  Data Run #1 
;;********************
if (data_run = 1) then
  size = 4
  Pattern[1] = 0x19
  Pattern[2] = 0xff
  Pattern[3] = 0x24
  Pattern[4] = 0xf1
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[1], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0x05
  Pattern[2] = 0x43
  Pattern[3] = 0xff
  Pattern[4] = 0x45
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[2], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0x13
  Pattern[9] = 0x46
  Pattern[10] = 0xff
  Pattern[11] = 0xf4
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[3], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0xff
  Pattern[2] = 0x25
  Pattern[3] = 0x54
  Pattern[4] = 0x00
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[4], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0x60
  Pattern[7] = 0xaa
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[5], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xc5
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0x30
  Pattern[16] = 0x02
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[6], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 32
  Pattern[1] = 0x00
  Pattern[2] = 0x12
  Pattern[3] = 0x54
  Pattern[4] = 0x6f
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xc1
  Pattern[13] = 0x23
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  Pattern[17] = 0xff
  Pattern[18] = 0xff
  Pattern[19] = 0xff
  Pattern[20] = 0xff
  Pattern[21] = 0xff
  Pattern[22] = 0xff
  Pattern[23] = 0xff
  Pattern[24] = 0xff
  Pattern[25] = 0xff
  Pattern[26] = 0xff
  Pattern[27] = 0xff
  Pattern[28] = 0xff
  Pattern[29] = 0xff
  Pattern[30] = 0xff
  Pattern[31] = 0xff
  Pattern[32] = 0xff
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[7], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0x00
  Pattern[9] = 0x13
  Pattern[10] = 0x45
  Pattern[11] = 0x23
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xac
  Pattern[16] = 0x09
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[8], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0x00
  Pattern[2] = 0x00
  Pattern[3] = 0x05
  Pattern[4] = 0x46
  Pattern[5] = 0xfa
  Pattern[6] = 0xcc
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[9], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0x77
  Pattern[3] = 0x06
  Pattern[4] = 0xff
  Pattern[5] = 0x43
  Pattern[6] = 0x15
  Pattern[7] = 0xab
  Pattern[8] = 0xf0
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[10], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x3f
  Pattern[2] = 0x9d
  Pattern[3] = 0xdc
  Pattern[4] = 0xc6
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[11], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0x99
  Pattern[6] = 0x99
  Pattern[7] = 0xa0
  Pattern[8] = 0x43
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[12], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xad
  Pattern[12] = 0xf9
  Pattern[13] = 0x83
  Pattern[14] = 0x42
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[13], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 32
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  Pattern[17] = 0xff
  Pattern[18] = 0xff
  Pattern[19] = 0xff
  Pattern[20] = 0xff
  Pattern[21] = 0xff
  Pattern[22] = 0xff
  Pattern[23] = 0x40
  Pattern[24] = 0x62
  Pattern[25] = 0xf1
  Pattern[26] = 0xa9
  Pattern[27] = 0xff
  Pattern[28] = 0xff
  Pattern[29] = 0xff
  Pattern[30] = 0xff
  Pattern[31] = 0xff
  Pattern[32] = 0xff
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[14], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x26
  Pattern[2] = 0x11
  Pattern[3] = 0x11
  Pattern[4] = 0x11
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[15], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0x12
  Pattern[6] = 0xaa
  Pattern[7] = 0xbb
  Pattern[8] = 0xcc
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[16], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x75
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[17], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xab
  Pattern[2] = 0xcd
  Pattern[3] = 0x12
  Pattern[4] = 0xca
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[18], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0x10
  Pattern[8] = 0x00
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[19], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0x13
  Pattern[4] = 0x50
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[20], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC
;;********************
;;  Data Run #2 
;;********************
elseif (data_run = 2) then
  size = 4
  Pattern[1] = 0x19
  Pattern[2] = 0xff
  Pattern[3] = 0x24
  Pattern[4] = 0xf1
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[1], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0x05
  Pattern[2] = 0x43
  Pattern[3] = 0xff
  Pattern[4] = 0x45
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[2], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0x13
  Pattern[9] = 0x46
  Pattern[10] = 0xff
  Pattern[11] = 0xf4
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[3], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0xff
  Pattern[2] = 0x25
  Pattern[3] = 0x45
  Pattern[4] = 0x00
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[4], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0x54
  Pattern[7] = 0xaa
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[5], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xc5
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0x30
  Pattern[16] = 0x01
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[6], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 32
  Pattern[1] = 0x00
  Pattern[2] = 0x12
  Pattern[3] = 0x45
  Pattern[4] = 0x6f
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xc1
  Pattern[13] = 0x23
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  Pattern[17] = 0xff
  Pattern[18] = 0xff
  Pattern[19] = 0xff
  Pattern[20] = 0xff
  Pattern[21] = 0xff
  Pattern[22] = 0xff
  Pattern[23] = 0xff
  Pattern[24] = 0xff
  Pattern[25] = 0xff
  Pattern[26] = 0xff
  Pattern[27] = 0xff
  Pattern[28] = 0xff
  Pattern[29] = 0xff
  Pattern[30] = 0xff
  Pattern[31] = 0xff
  Pattern[32] = 0xff
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[7], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0x00
  Pattern[9] = 0x12
  Pattern[10] = 0x45
  Pattern[11] = 0x23
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xac
  Pattern[16] = 0x09
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[8], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0x00
  Pattern[2] = 0x00
  Pattern[3] = 0x05
  Pattern[4] = 0x42
  Pattern[5] = 0xfa
  Pattern[6] = 0xcc
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[9], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0x77
  Pattern[3] = 0x06
  Pattern[4] = 0xff
  Pattern[5] = 0x43
  Pattern[6] = 0x15
  Pattern[7] = 0xab
  Pattern[8] = 0xf0
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[10], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x3f
  Pattern[2] = 0x9d
  Pattern[3] = 0xdc
  Pattern[4] = 0xc6
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[11], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0x99
  Pattern[6] = 0x99
  Pattern[7] = 0xa0
  Pattern[8] = 0x43
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[12], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xad
  Pattern[12] = 0xf9
  Pattern[13] = 0x83
  Pattern[14] = 0x42
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[13], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 32
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  Pattern[17] = 0xff
  Pattern[18] = 0xff
  Pattern[19] = 0xff
  Pattern[20] = 0xff
  Pattern[21] = 0xff
  Pattern[22] = 0xff
  Pattern[23] = 0x40
  Pattern[24] = 0x62
  Pattern[25] = 0xf1
  Pattern[26] = 0xa9
  Pattern[27] = 0xff
  Pattern[28] = 0xff
  Pattern[29] = 0xff
  Pattern[30] = 0xff
  Pattern[31] = 0xff
  Pattern[32] = 0xff
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[14], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x26
  Pattern[2] = 0x11
  Pattern[3] = 0x11
  Pattern[4] = 0x11
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[15], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0x12
  Pattern[6] = 0xaa
  Pattern[7] = 0xbb
  Pattern[8] = 0xcc
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[16], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x75
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[17], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xab
  Pattern[2] = 0xcd
  Pattern[3] = 0x12
  Pattern[4] = 0xca
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[18], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0x10
  Pattern[8] = 0x00
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[19], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0x13
  Pattern[4] = 0x50
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[20], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

;;********************
;;  Data Run #3 
;;********************
elseif (data_run = 3) then
  size = 4
  Pattern[1] = 0x19
  Pattern[2] = 0xff
  Pattern[3] = 0x24
  Pattern[4] = 0xf1
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[1], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0x05
  Pattern[2] = 0x43
  Pattern[3] = 0xff
  Pattern[4] = 0x45
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[2], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0x13
  Pattern[9] = 0x45
  Pattern[10] = 0xff
  Pattern[11] = 0xf4
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[3], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0xff
  Pattern[2] = 0x25
  Pattern[3] = 0x45
  Pattern[4] = 0x00
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[4], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0x54
  Pattern[7] = 0xaa
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[5], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xc5
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0x33
  Pattern[16] = 0x01
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[6], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 32
  Pattern[1] = 0x00
  Pattern[2] = 0x12
  Pattern[3] = 0x54
  Pattern[4] = 0x6f
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xc1
  Pattern[13] = 0x23
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  Pattern[17] = 0xff
  Pattern[18] = 0xff
  Pattern[19] = 0xff
  Pattern[20] = 0xff
  Pattern[21] = 0xff
  Pattern[22] = 0xff
  Pattern[23] = 0xff
  Pattern[24] = 0xff
  Pattern[25] = 0xff
  Pattern[26] = 0xff
  Pattern[27] = 0xff
  Pattern[28] = 0xff
  Pattern[29] = 0xff
  Pattern[30] = 0xff
  Pattern[31] = 0xff
  Pattern[32] = 0xff
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[7], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0x00
  Pattern[9] = 0x12
  Pattern[10] = 0x45
  Pattern[11] = 0x23
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xac
  Pattern[16] = 0x09
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[8], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0x00
  Pattern[2] = 0x00
  Pattern[3] = 0x05
  Pattern[4] = 0x42
  Pattern[5] = 0xfa
  Pattern[6] = 0xcc
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[9], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0x77
  Pattern[3] = 0x06
  Pattern[4] = 0xff
  Pattern[5] = 0x43
  Pattern[6] = 0x15
  Pattern[7] = 0xab
  Pattern[8] = 0x00
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[10], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x3f
  Pattern[2] = 0x9d
  Pattern[3] = 0xdc
  Pattern[4] = 0xc6
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[11], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0x99
  Pattern[6] = 0x99
  Pattern[7] = 0xa0
  Pattern[8] = 0x43
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[12], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xad
  Pattern[12] = 0xf9
  Pattern[13] = 0x83
  Pattern[14] = 0x42
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[13], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 32
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  Pattern[17] = 0xff
  Pattern[18] = 0xff
  Pattern[19] = 0xff
  Pattern[20] = 0xff
  Pattern[21] = 0xff
  Pattern[22] = 0xff
  Pattern[23] = 0x40
  Pattern[24] = 0x62
  Pattern[25] = 0xf1
  Pattern[26] = 0xa9
  Pattern[27] = 0xff
  Pattern[28] = 0xff
  Pattern[29] = 0xff
  Pattern[30] = 0xff
  Pattern[31] = 0xff
  Pattern[32] = 0xff
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[14], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x26
  Pattern[2] = 0x11
  Pattern[3] = 0x11
  Pattern[4] = 0x11
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[15], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0x12
  Pattern[6] = 0xaa
  Pattern[7] = 0xbb
  Pattern[8] = 0xcc
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[16], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x75
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[17], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xab
  Pattern[2] = 0xcd
  Pattern[3] = 0x12
  Pattern[4] = 0xca
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[18], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0x10
  Pattern[8] = 0x00
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[19], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0x13
  Pattern[4] = 0x50
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[20], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

;;********************
;;  Data Run #4 
;;********************
elseif (data_run = 4) then
  size = 4
  Pattern[1] = 0x19
  Pattern[2] = 0xff
  Pattern[3] = 0x24
  Pattern[4] = 0xf1
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[1], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0x05
  Pattern[2] = 0x43
  Pattern[3] = 0xff
  Pattern[4] = 0x50
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[2], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0x13
  Pattern[9] = 0x45
  Pattern[10] = 0xff
  Pattern[11] = 0xf4
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[3], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0xff
  Pattern[2] = 0x25
  Pattern[3] = 0x45
  Pattern[4] = 0x00
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[4], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0x60
  Pattern[7] = 0xaa
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[5], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xc5
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0x30
  Pattern[16] = 0x01
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[6], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 32
  Pattern[1] = 0x00
  Pattern[2] = 0x12
  Pattern[3] = 0x45
  Pattern[4] = 0x6f
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xc1
  Pattern[13] = 0x23
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  Pattern[17] = 0xff
  Pattern[18] = 0xff
  Pattern[19] = 0xff
  Pattern[20] = 0xff
  Pattern[21] = 0xff
  Pattern[22] = 0xff
  Pattern[23] = 0xff
  Pattern[24] = 0xff
  Pattern[25] = 0xff
  Pattern[26] = 0xff
  Pattern[27] = 0xff
  Pattern[28] = 0xff
  Pattern[29] = 0xff
  Pattern[30] = 0xff
  Pattern[31] = 0xff
  Pattern[32] = 0xff
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[7], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0x00
  Pattern[9] = 0x13
  Pattern[10] = 0x45
  Pattern[11] = 0x23
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xac
  Pattern[16] = 0x09
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[8], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0x00
  Pattern[2] = 0x00
  Pattern[3] = 0x05
  Pattern[4] = 0x42
  Pattern[5] = 0xfa
  Pattern[6] = 0xcc
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[9], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0x77
  Pattern[3] = 0x06
  Pattern[4] = 0xff
  Pattern[5] = 0x43
  Pattern[6] = 0x15
  Pattern[7] = 0xab
  Pattern[8] = 0xf1
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[10], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x3f
  Pattern[2] = 0x9e
  Pattern[3] = 0x04
  Pattern[4] = 0x19
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[11], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0x99
  Pattern[6] = 0x99
  Pattern[7] = 0xa0
  Pattern[8] = 0x43
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[12], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 16
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xad
  Pattern[12] = 0xf9
  Pattern[13] = 0x83
  Pattern[14] = 0x42
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  for index = 17 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[13], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 32
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  Pattern[9] = 0xff
  Pattern[10] = 0xff
  Pattern[11] = 0xff
  Pattern[12] = 0xff
  Pattern[13] = 0xff
  Pattern[14] = 0xff
  Pattern[15] = 0xff
  Pattern[16] = 0xff
  Pattern[17] = 0xff
  Pattern[18] = 0xff
  Pattern[19] = 0xff
  Pattern[20] = 0xff
  Pattern[21] = 0xff
  Pattern[22] = 0xff
  Pattern[23] = 0x40
  Pattern[24] = 0x62
  Pattern[25] = 0xf1
  Pattern[26] = 0xa9
  Pattern[27] = 0xff
  Pattern[28] = 0xff
  Pattern[29] = 0xff
  Pattern[30] = 0xff
  Pattern[31] = 0xff
  Pattern[32] = 0xff
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[14], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x26
  Pattern[2] = 0x11
  Pattern[3] = 0x11
  Pattern[4] = 0x11
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[15], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0x12
  Pattern[6] = 0xaa
  Pattern[7] = 0xbb
  Pattern[8] = 0xcc
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[16], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0x75
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[17], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xab
  Pattern[2] = 0xcd
  Pattern[3] = 0x12
  Pattern[4] = 0xca
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0xff
  Pattern[8] = 0xff
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[18], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 8
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0xff
  Pattern[4] = 0xff
  Pattern[5] = 0xff
  Pattern[6] = 0xff
  Pattern[7] = 0x10
  Pattern[8] = 0x00
  for index = 9 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[19], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC

  size = 4
  Pattern[1] = 0xff
  Pattern[2] = 0xff
  Pattern[3] = 0x13
  Pattern[4] = 0x50
  for index = 5 to 32 do 
    Pattern[index] = 0
  enddo
  /$SC_$CPU_TST_LC_SENDPACKET MsgId=MsgId[20], DataSize=Size, DataPattern=Pattern
  ut_tlmupdate $SC_$CPU_TST_LC_CMDPC
endif

ENDPROC
