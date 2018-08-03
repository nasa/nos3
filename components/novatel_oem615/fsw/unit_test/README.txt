
README for CFS Limit Checker (LC) unit tests run on 1/15/09

Platform
--------
Cygwin on Windows XP 

Supporting Software Used:
-------------------------
cFE v5.2/OSAL 2.12 with bundled UTF

Unit Test Files
---------------
lc_utest.c - This is the main unit test driver. This
             file when compiled is the executable
             used to unit test LC.
                
lc_utest.in - This is a script of input data used
              to test high level software bus message
              processing.            

lc_utest.out - Program output from lc_utest.exe 

Makefile - This is the make file used to build lc_utest.exe
           Be aware that this file may need to be modified if
           a different platform or directory structure is used.

/output_CDS - Directory that contains the executable and output for unit
              tests compiled and ran with the platform configuration 
              parameter LC_SAVE_TO_CDS defined

/output_noCDS - Directory that contains the executable and output for unit
                tests compiled and ran with the platform configuration 
                parameter LC_SAVE_TO_CDS NOT defined

Coverage Summary
----------------
See /output_CDS/README_CDS.txt or 
    /output_noCDS/README_noCDS.txt
