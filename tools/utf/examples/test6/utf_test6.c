
/*
** File: utf_test6.c
**
** Purpose: This test makes calls to OS file system api.
**
** References:
**
** Assumptions and Notes:
**
** $Date: 2009/10/13 14:23:33GMT-05:00 $
** $Revision: 1.5 $
** $Log: utf_test6.c  $
** Revision 1.5 2009/10/13 14:23:33GMT-05:00 wmoleski 
** 
** Revision 1.4 2008/09/04 07:19:35EDT wfmoleski 
** Function hook support was added for the OS_close function. Test code was also added.
** Revision 1.3 2008/06/03 13:52:10EDT wfmoleski 
** Added the OS_opendir and OS_readdir function hooks that were needed by the CFS File Manager
** to complete unit testing.
** Revision 1.2 2008/06/03 13:39:04EDT wfmoleski 
** Updated to contain the OS_FD _Table structure and the code to populate and remove entries from it.
** Revision 1.1 2008/04/17 08:09:23EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/tools/utf/examples/test6/project.pj
** Revision 1.5 2008/04/07 09:39:34EDT wfmoleski 
** Added the capability for the specified hook functions. Added return code setting code for several
** OS functions that seemed appropriate.
** Revision 1.4 2006/12/21 08:36:53EST wfmoleski 
** Added this new function to the UTF and modified the example to test this new function.
** Revision 1.3 2006/08/01 08:51:02EDT wfmoleski 
** Changes to upgrade this example to cFE 3.3.0
** Revision 1.2 2006/04/10 16:29:31EDT sslegel 
** Updated test to use the volume table correctly
**
*/

#include "cfe.h"
#include "ccsds.h"           
#include "utf_custom.h"      
#include "utf_types.h"       
#include "utf_cfe_sb.h"      
#include "utf_osapi.h"
#include "utf_osfileapi.h"
#include "utf_osfilesys.h"

/* Function Hook Test routines */
int32 OS_creatHook(const char *path, int32  access)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(0);
    else
        return(OS_FS_ERR_INVALID_POINTER);
}

int32 OS_writeHook(int32  filedes, void *buffer, uint32 nbytes)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(0);
    else
        return(OS_FS_ERROR);
}

int32 OS_readHook(int32  filedes, void *buffer, uint32 nbytes)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(0);
    else
        return(OS_FS_ERR_INVALID_POINTER);
}

os_dirp_t OS_opendirHook (const char *path)
{
	return NULL;
}

os_dirent_t *  OS_readdirHook (os_dirp_t directory)
{
	return NULL;
}

int32 OS_closeHook(int32  filedes)
{
    static uint32 Count = 0;
    if (Count++ % 2 == 0)
        return(0);
    else
        return(OS_FS_ERR_INVALID_FD);
}

int main(void)
{

    int32 status;
    int32 fd, fd2;
    char wbuffer[80];
    char rbuffer[40];
    char fullpathname[128];
    char directoryname[128];
    char filename[40];
    os_fstat_t  fstat_struct;
    os_dirp_t   dir_pointer;
    os_dirent_t *dir_entry;
    OS_FDTableEntry fdEntryInfo;

    /********************************/
    /* Set up output file           */
    /********************************/
    UTF_set_output_filename("utf_test6.ActualOutput");

/* There are two ways to configure the filesystem,
**    1. by making a single call to UTF_add_volume to set everything
**    2. by making a call to UTF_add_volume to set only the device name and physical device and then call OS_mkfs and OS_mount
**       The only time you would want to use option 2 is where the task you are testing calls OS_mkfs and OS_mount.  Most of
**       the time the system will do this for you on startup.
**
**  Option 1
**  - adds the device "/nonvol0"
**  - maps the filesystem to a directory "tmp" in the current working directory
**  - is filesystem based
**  - is not volatile
**  - is not free
**  - is mounted
**  - the volume name is "CF"
**  - the virtual filesystem is "/cf", i.e. references to the virtual directory "/cf" get translated
**    to the physical directory "tmp/nonvol0/".  Note that this directory should already exist.
**  - block size is 0
**
**    UTF_add_volume("/nonvol0", "tmp", FS_BASED, FALSE, FALSE, TRUE, "CF", "/cf", 0);
**
**  Option 2
**  - adds the device "/nonvol0"
**  - maps the filesystem to a directory "tmp" in the current working directory
**  - is filesystem based
**  - is not volatile
**  - is free - needed for OS_mkfs
**  - is not mounted - needed for OS_mount
**  - the volume name is " " - set by OS_mkfs
**  - the virtual filesystem is " ", - set by OS_mount
**  - block size is 0
**
**    UTF_add_volume("/nonvol0", "tmp", FS_BASED, FALSE, TRUE, FALSE, " ", " ", 0);
**    status = OS_mkfs(0, "/nonvol0", "CF", 0, 0);
**    status = OS_mount("/nonvol0", "/cf");
**
*/

    /**************************************************/
    /* Make and mount the filesystem - option 2       */
    /**************************************************/

    /* add the volume "/nonvol0" to the system and set the physical location of the filesystem
       to the "tmp" folder under the current working directory */
    UTF_add_volume("/nonvol0", "tmp", FS_BASED, FALSE, TRUE, FALSE, " ", " ", 0);

    /* make the filesystem */
    status = OS_mkfs(0, "/nonvol0", "CF", 0, 0);

    /* mount the filesystem, references to the virtual directory "/cf" will be translated to the
       physical directory "tmp/nonvol0/" */
    status = OS_mount("/nonvol0", "/cf");

    /* try unmounting the filesystem */
    status = OS_unmount("/cf");
    status = OS_mount("/nonvol0", "/cf");

    UTF_put_text("\n***UTF Version Test***\n");
    UTF_version();

    UTF_put_text("\n#\n# Test OS File System calls\n#\n");
    /**************************************************/
    /* Create a file.                                 */
    /**************************************************/

    UTF_put_text("\n#\n# Create file, 'utf_test6_newfile.dat'\n#\n# ");

    /* Create a file */
    strcpy(directoryname, "/cf");
    strcpy(filename,"utf_test6_file.dat");
    sprintf(fullpathname, "%s/%s",directoryname,filename);
    UTF_put_text("\n***OS_creat Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_CREAT_PROC, OS_FS_ERR_INVALID_POINTER);
    UTF_put_text("OS_Creat Return Code %x\n", OS_creat(fullpathname,OS_READ_WRITE));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_CREAT_PROC);
    
    UTF_put_text("->Call to OS_creat for file <%s>\n", fullpathname);
    UTF_put_text("->Expect non-negative return value\n");
    fd = OS_creat(fullpathname,OS_READ_WRITE);
    if (fd <= -1)
    {
	UTF_put_text("Error return %d from OS_creat for fullpathname= <%s>\n",fd, fullpathname);
	perror("open function error ");
	exit(-1);
    }
    UTF_put_text ("OS_creat returned %d\n", fd);

    /**************************************************/
    /* Write to output file.                          */
    /**************************************************/
    UTF_put_text("\n#\n# Write to file\n#\n# ");
    sprintf(wbuffer,"%s","darling, I love you but give me Park Avenue!");
    UTF_put_text("wbuffer contains [%s]\n", wbuffer);
    UTF_put_text("\n***OS_write Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_WRITE_PROC, OS_FS_ERROR);
    UTF_put_text("OS_write Return Code %x\n", OS_write(fd, wbuffer,strlen(wbuffer)));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_WRITE_PROC);
    
    UTF_put_text("->Call OS_write with wbuffer\n");
    UTF_put_text("->Expect return value of %d\n", strlen(wbuffer));
    status = OS_write(fd, wbuffer,strlen(wbuffer));
    UTF_put_text ("OS_write returned %d\n", status);

    /**************************************************/
    /* Go to the beginning of the file                */
    /* and overwrite 1 chars.                         */
    /**************************************************/
    UTF_put_text("\n#\n# Go  to the beginning of the file and write 'D'.\n#\n# ");
    UTF_put_text("\n***OS_lseek Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_LSEEK_PROC, OS_FS_ERROR);
    UTF_put_text("OS_lseek Return Code %x\n", OS_lseek(fd,0,OS_SEEK_SET));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_LSEEK_PROC);

    UTF_put_text("->Call OS_lseek with OS_SEEK_SET\n");
    UTF_put_text("->Expect 0.\n");
    status = OS_lseek(fd,0,OS_SEEK_SET);
    UTF_put_text ("OS_lseek returned %d\n", status);

    strcpy(wbuffer,"D");
    UTF_put_text("->Call OS_write with <%s>\n", wbuffer);
    UTF_put_text("->Expect 1.\n");
    status = OS_write(fd, wbuffer,strlen(wbuffer));
    UTF_put_text ("OS_write returned %d\n", status);

    /* Close the file */
    UTF_put_text("\n***OS_close Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_CLOSE_PROC, OS_FS_ERROR);
    UTF_put_text("OS_close Return Code %x\n", OS_close(fd));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_CLOSE_PROC);

    UTF_put_text("->Call OS_close\n");
    status = OS_close(fd);
    UTF_put_text ("OS_close returned %d\n", status);

    /**************************************************/
    /* Reopen file  for read + write.                 */
    /**************************************************/
    UTF_put_text("\n#                          #\n");
    UTF_put_text  ("# Open file for r+w.       #\n");
    UTF_put_text  ("#                          #\n");

    /* Open the file for writing */
    UTF_put_text("\n***OS_open Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_OPEN_PROC, OS_FS_ERROR);
    UTF_put_text("OS_open Return Code %x\n", OS_open(fullpathname,OS_READ_WRITE,1));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_OPEN_PROC);

    UTF_put_text("->Call OS_open for fullpathname= <%s> with OS_READ_WRITE\n",
	fullpathname);
    fd = OS_open(fullpathname,OS_READ_WRITE,1);

    if (fd <= -1)
    {
	UTF_put_text("Error return %d from OS_open for fullpathname= <%s>\n",
		fd, fullpathname);
	exit(-1);
    }
    UTF_put_text ("OS_open returned %d\n", fd);

    /* Move to end of file */
    UTF_put_text("->Call OS_lseek with OS_SEEK_END\n");
    status = OS_lseek(fd,0,OS_SEEK_END);
    UTF_put_text ("OS_lseek returned %d\n", status);

    /* Write */
    sprintf(wbuffer,"%s","Oh Danny boy, the pipes, the pipes are calling");
    UTF_put_text("wbuffer contains [%s]\n", wbuffer);
    UTF_put_text("->Call OS_write with wbuffer\n");
    status = OS_write(fd, wbuffer,strlen(wbuffer));
    UTF_put_text ("OS_write returned %d\n", status);

    /**************************************************/
    /* Move to beginning of file, read into buffer,   */
    /* close file.                                    */
    /**************************************************/
    UTF_put_text("\n#                               #\n");
    UTF_put_text  ("# Move to beginning of file,    #\n");
    UTF_put_text  ("# read into buffer, close file. #\n");
    UTF_put_text  ("#                               #\n");

    UTF_put_text("->Call OS_lseek with OS_SEEK_SET\n");
    UTF_put_text("->Expect 0.\n");
    status = OS_lseek(fd,0,OS_SEEK_SET);
    UTF_put_text ("OS_lseek returned %d\n", status);

    UTF_put_text("\n***OS_read Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_READ_PROC, OS_FS_ERR_INVALID_POINTER);
    UTF_put_text("OS_read Return Code %x\n", OS_read(fd, rbuffer, sizeof(rbuffer)-1));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_READ_PROC);

    UTF_put_text("->Call OS_read with rbuffer\n");
    status = OS_read(fd, rbuffer, sizeof(rbuffer) -1);
    rbuffer[39] = '\0';
    UTF_put_text ("OS_read returned %d\n", status);
    UTF_put_text("rbuffer contains [%s]\n", rbuffer);

    UTF_put_text("->Call OS_read with rbuffer\n");
    status = OS_read(fd, rbuffer, sizeof(rbuffer) -1);
    rbuffer[sizeof(rbuffer) -1] = '\0';
    UTF_put_text ("OS_read returned %d\n", status);
    UTF_put_text("rbuffer contains [%s]\n", rbuffer);

    /* Close the file */
    UTF_put_text("->Call OS_close\n");
    status = OS_close(fd);
    UTF_put_text ("OS_close returned %d\n", status);

    /* Get the status of the file */
    UTF_put_text("\n***OS_stat Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_STAT_PROC, OS_FS_ERR_PATH_INVALID);
    UTF_put_text("OS_stat Return Code %x\n", OS_stat(fullpathname, &fstat_struct));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_STAT_PROC);

    UTF_put_text("->Call OS_stat\n");
    status = OS_stat(fullpathname, &fstat_struct);
    UTF_put_text ("OS_stat returned %d\n", status);
    UTF_put_text ("Print out the file size from the stat structure %d\n", fstat_struct.st_size);

    /* Rename the file */
    UTF_put_text("\n***OS_rename Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_RENAME_PROC, OS_FS_ERR_PATH_TOO_LONG);
    UTF_put_text("OS_rename Return Code %x\n", OS_rename(fullpathname, "/cf/utf_test6_ren.dat"));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_RENAME_PROC);

    UTF_put_text("->Call OS_rename %s to '/cf/utf_test6_ren.dat'\n",fullpathname);
    status = OS_rename(fullpathname, "/cf/utf_test6_ren.dat");
    UTF_put_text ("OS_rename returned %d\n", status);

    /* Remove the file */
    UTF_put_text("\n***OS_remove Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_REMOVE_PROC, OS_FS_ERR_INVALID_POINTER);
    UTF_put_text("OS_remove Return Code %x\n", OS_remove("/cf/utf_test6_ren.dat"));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_REMOVE_PROC);

    UTF_put_text("->Call OS_remove with '/cf/utf_test6_ren.dat'\n");
    status = OS_remove("/cf/utf_test6_ren.dat");
    UTF_put_text ("OS_remove returned %d\n", status);

    /* Make a directory */
    UTF_put_text("\n***OS_mkdir Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_MKDIR_PROC, OS_FS_ERROR);
    UTF_put_text("OS_mkdir Return Code %x\n", OS_mkdir("/cf/steve", 0));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_MKDIR_PROC);

    UTF_put_text("->Call OS_mkdir to make a steve directory\n");
    status = OS_mkdir("/cf/steve", 0);
    UTF_put_text ("OS_mkdir returned %d\n", status);

    UTF_put_text("->Create 2 files in the new directory\n");
    fd = OS_creat("/cf/steve/utf_test1.dat", OS_READ_WRITE);
    fd2 = OS_creat("/cf/steve/utf_test2.dat", OS_READ_WRITE);

    /* Call OS_FDGetInfo for each file above */
    UTF_put_text("OS_FDGetInfo Return Code %x\n", OS_FDGetInfo(fd,&fdEntryInfo));
    UTF_put_text("EntryInfo->OSfd = %d\n", fdEntryInfo.OSfd);
    UTF_put_text("EntryInfo->path = %s\n", fdEntryInfo.Path);
    UTF_put_text("EntryInfo->User = %d\n", fdEntryInfo.User);
    if (fdEntryInfo.IsValid == TRUE)
	UTF_put_text("EntryInfo->IsValid = TRUE\n");
    else
	UTF_put_text("EntryInfo->IsValid = FALSE\n");

    UTF_put_text("OS_FDGetInfo Return Code %x\n", OS_FDGetInfo(fd2,&fdEntryInfo));
    UTF_put_text("EntryInfo->OSfd = %d\n", fdEntryInfo.OSfd);
    UTF_put_text("EntryInfo->path = %s\n", fdEntryInfo.Path);
    UTF_put_text("EntryInfo->User = %d\n", fdEntryInfo.User);
    if (fdEntryInfo.IsValid == TRUE)
	UTF_put_text("EntryInfo->IsValid = TRUE\n");
    else
	UTF_put_text("EntryInfo->IsValid = FALSE\n");

    status = OS_close(fd);
    status = OS_close(fd2);

    UTF_put_text("->Call OS_opendir to open the steve directory\n");
    dir_pointer = OS_opendir("/cf/steve");

    UTF_put_text("->Call OS_readdir to read all files in the steve directory\n");
    dir_entry = OS_readdir(dir_pointer);
    while (dir_entry != NULL)
    {
        UTF_put_text ("File in steve directory %s\n", dir_entry->d_name);
        dir_entry = OS_readdir(dir_pointer);
    }

    UTF_put_text("\n***OS_closedir Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_CLOSEDIR_PROC, OS_FS_ERR_INVALID_POINTER);
    UTF_put_text("OS_closedir Return Code %x\n", OS_closedir(dir_pointer));
	UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_CLOSEDIR_PROC);

    UTF_put_text("->Call OS_closedir to close the steve directory\n");
    status = OS_closedir(dir_pointer);
    UTF_put_text ("OS_closedir returned %d\n", status);

    /* Remove the file */
    UTF_put_text("->Call OS_remove to remove the files in the steve directory\n");
    status = OS_remove("/cf/steve/utf_test1.dat");
    UTF_put_text ("OS_remove returned %d for utf_test1.dat\n", status);
    status = OS_remove("/cf/steve/utf_test2.dat");
    UTF_put_text ("OS_remove returned %d for utf_test2.dat\n", status);

    /* Remove the directory */
    UTF_put_text("\n***OS_rmdir Return Code Setting Test ***\n");
    UTF_OSFILEAPI_Set_Api_Return_Code(OS_RMDIR_PROC, OS_FS_ERROR);
    UTF_put_text("OS_rmdir Return Code %x\n", OS_rmdir("/cf/steve"));
    UTF_OSFILEAPI_Use_Default_Api_Return_Code(OS_RMDIR_PROC);

    UTF_put_text("->Call OS_rmdir to remove the steve directory\n");
    status = OS_rmdir("/cf/steve");
    UTF_put_text ("OS_rmdir returned %d\n", status);

    /* Get the Physical Drive name */
    UTF_put_text ("\n**** Testing OS_FS_GetPhysDriveName using '/cf' ****\n");
    status = OS_FS_GetPhysDriveName(fullpathname, "/cf");
    UTF_put_text ("OS_FS_GetPhysDriveName returned %d\n", status);
    if (status == OS_SUCCESS) 
	UTF_put_text ("PhysDriveName returned '%s'\n", fullpathname);

    UTF_put_text ("**** Testing OS_FS_GetPhysDriveName using '/ram' ****\n");
    status = OS_FS_GetPhysDriveName(fullpathname, "/ram");
    UTF_put_text ("OS_FS_GetPhysDriveName returned %d\n", status);
    if (status == OS_SUCCESS)
	UTF_put_text ("PhysDriveName returned '%s'\n", fullpathname);
    else if (status == OS_FS_ERROR)
	UTF_put_text ("'/ram' was not mounted!!!\n");

    UTF_put_text ("**** Testing OS_FS_GetPhysDriveName using an invalid pointer ****\n");
    status = OS_FS_GetPhysDriveName(fullpathname, NULL);
    UTF_put_text ("OS_FS_GetPhysDriveName returned %d\n", status);
    if (status == OS_FS_ERR_INVALID_POINTER)
	UTF_put_text ("Status indicates an invalid pointer was passed\n", fullpathname);

    status = OS_FS_GetPhysDriveName(NULL, "/cf");
    UTF_put_text ("OS_FS_GetPhysDriveName returned %d\n", status);
    if (status == OS_FS_ERR_INVALID_POINTER)
	UTF_put_text ("Status indicates an invalid pointer was passed\n", fullpathname);

    UTF_put_text("\n***Function Hook Tests***\n");
    UTF_OSFILEAPI_set_function_hook(OS_CREAT_HOOK, (void *)&OS_creatHook);
    UTF_put_text("creat Hook Return Code %x\n", OS_creat("/cf/steve/utf_test1.dat", OS_READ_WRITE));  
    UTF_put_text("creat Hook Return Code %x\n", OS_creat("/cf/steve/utf_test1.dat", OS_READ_WRITE));  
    UTF_put_text("creat Hook Return Code %x\n", OS_creat("/cf/steve/utf_test1.dat", OS_READ_WRITE));  
    UTF_put_text("creat Hook Return Code %x\n", OS_creat("/cf/steve/utf_test1.dat", OS_READ_WRITE));

    UTF_OSFILEAPI_set_function_hook(OS_WRITE_HOOK, (void *)&OS_writeHook);
    UTF_put_text("write Hook Return Code %x\n", OS_write(fd, wbuffer,strlen(wbuffer)));  
    UTF_put_text("write Hook Return Code %x\n", OS_write(fd, wbuffer,strlen(wbuffer)));  
    UTF_put_text("write Hook Return Code %x\n", OS_write(fd, wbuffer,strlen(wbuffer)));  
    UTF_put_text("write Hook Return Code %x\n", OS_write(fd, wbuffer,strlen(wbuffer)));  

    UTF_OSFILEAPI_set_function_hook(OS_READ_HOOK, (void *)&OS_readHook);
    UTF_put_text("read Hook Return Code %x\n", OS_read(fd, rbuffer, sizeof(rbuffer)-1));  
    UTF_put_text("read Hook Return Code %x\n", OS_read(fd, rbuffer, sizeof(rbuffer)-1));  
    UTF_put_text("read Hook Return Code %x\n", OS_read(fd, rbuffer, sizeof(rbuffer)-1));  
    UTF_put_text("read Hook Return Code %x\n", OS_read(fd, rbuffer, sizeof(rbuffer)-1));

    UTF_OSFILEAPI_set_function_hook(OS_CLOSE_HOOK, (void *)&OS_closeHook);
    UTF_put_text("close Hook Return Code %x\n", OS_close(fd));  
    UTF_put_text("close Hook Return Code %x\n", OS_close(fd));  
    UTF_put_text("close Hook Return Code %x\n", OS_close(fd));  
    UTF_put_text("close Hook Return Code %x\n", OS_close(fd));

    UTF_OSFILEAPI_set_function_hook(OS_OPENDIR_HOOK, (void *)&OS_opendirHook);
    dir_pointer = OS_opendir("/cf/steve");
    if (dir_pointer == NULL)
	UTF_put_text("opendir Hook function returned NULL as expected\n");
    else
	UTF_put_text("opendir Hook function did not return the expected value\n");

    UTF_OSFILEAPI_set_function_hook(OS_READDIR_HOOK, (void *)&OS_readdirHook);
    dir_entry = OS_readdir(dir_pointer);
    if (dir_entry == NULL)
	UTF_put_text("readdir Hook function returned NULL as expected\n");
    else
	UTF_put_text("readdir Hook function did not return the expected value\n");


    return(1);
}
