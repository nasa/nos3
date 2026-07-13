# CFDP for cFS

## APID 

CMD = 0x185A
FILEDOWNLOAD = 0x085C

## FCN

NOOP = 0
RESET_COUNTERS = 1
DOWNLOADFILE_CC = 29
UPLOADFILE_CC = 30
UPLOADFILE_DATA_CC = 31 #Do not actually send commands to this! Used to transfer file data only for upload