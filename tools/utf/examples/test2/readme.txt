File: test/readme.txt
Created: 12/1/05
Updated: 1/20/06

1) The command input file for this test is 'ttscript1.in'.  
It resides on the MKS server under examples/test2.

2) Note that the .dat files which contain table data used in the table loads
are created by the test driver at run time.  They are placed at the /ram directory.
They are named tt_table_values1.dat, etc.

3) Test output which can be used for comparison is in file 'test_tbl_api2.ActualOutput'.

4) Known issue:
The output shows the following where housekeeping requests have been sent:

EVENT: Invalid command pipe message ID: 0x180E

I haven't yet investigated this.
