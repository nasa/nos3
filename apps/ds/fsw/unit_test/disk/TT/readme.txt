/************************************************************************
**   $Id: readme.txt 1.2 2009/09/02 11:44:10EDT lwalling Exp  $
**
**  CFS Data Storage (DS) unit test destination file folder
**
** $Log: readme.txt  $
** Revision 1.2 2009/09/02 11:44:10EDT lwalling 
** Provide description of virtual destination file folder location
*************************************************************************/

Some of the Data Storage (DS) task unit tests require the creation of
data files in a predictable folder location.  This file marks the file
folder location "fsw/unit_test/disk/TT" which is mapped to the virtual
folder "/tt" during unit test initialization.  The following line of
code has been excerpted from the unit test file "ds_utest.c":

UTF_add_volume("/TT", "disk", FS_BASED, FALSE, FALSE, TRUE, "TT", "/tt", 0);

The data files created in this folder are not saved - they are created
only to provide a successful code path when testing the DS functions
that create and write data to a destination file.

