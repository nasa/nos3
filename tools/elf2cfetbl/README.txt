This README file provides basic information on how to use the elf2cfetbl utility.

SUMMARY:

   The elf2cfetbl utility provides a method of converting an object file containing
   the desired contents of a cFE Application's Table Image into a binary file that
   is compatible with the cFE Table Services for loading the image.

UTILITY SOURCE FILES:

   In the elf2cfetbl utility directory, one should find the following files: 

      cfe_tbl_filedef.h - a header file that should be placed into the fsw/cfe-core/inc
                          directory. This file should only need to be #include'd by those 
                          source files that define the initial contents of a table.

      SampleTblImg.c    - a sample .c file that defines the default contents of a 
                          table image.

      ELF_Structures.h, elf2cfetbl.c - source files for the elf2cfetbl utility.

CREATING THE UTILITY:

   The elf2cfetbl utility and the tables it converts depend on processor
   specific configuration paramters that are defined in cfe_platform_cfg.h.
   Because of this, it is recommended that the elf2cfetbl utility is compiled
   and used for each processor in a cFE/CFS mission directory structure.
   Compiling the utility in this source directory and copying it to a 
   common directory in the executable path on the development host can cause errors
   when creating the flight software tables. 
   If the elf2cfetbl utility is compiled and used in the build/<cpu> tree,
   then the tables should always use the correct parameters, and the tool 
   does not have to be installed on the development workstation. 

   In order to setup the tool to build, check the build/<cpu> directory to see
   if the "elf2cfetbl" directory and corresponding "elf2cfetbl/Makefile"
   exists. If they are present, then the utility can be compiled by executing
   the makefile from that directory:
   $ cd build/cpu1/elf2cfetbl
   $ make

   If the elf2cfetbl directory and Makefile are not present in the build 
   directory, the directory can be created, and the Makefile can be copied
   from the "for_build" directory here. 

   $ mkdir <path-to-build>/build/cpu1/elf2cfetbl
   $ cp for-build/Makefile <path-to-build>/build/cpu1/elf2cfetbl

   Once the directory is setup, the build for the CPU should use this utility.
   If for some reason the build fails because the elf2cfetbl utility cannot be
   found, then it is possible the build makefiles or PSP configuration has not 
   been updated to look for the elf2cfetbl utility in the correct place. 
   In this case, check the file:
   <path-to-cfe>/cfe/fsw/cfe-core/src/make/table-rules.mak
   In this file, make sure the table rule looke like this:
   #
   # Default table rule
   #
   .o.tbl:$(OBJS)
          ../elf2cfetbl/$(TABLE_BIN) $<
  
PREPARING A SOURCE FILE FOR USE WITH THE UTILITY:

   Preparing a .c file for use with the utility requires the use of a special macro.
   At the bottom of the .c file, after defining the default contents of a table, the 
   developer should insert the following macro:

   CFE_TBL_FILEDEF(ObjName, TblName, Desc, Filename)

   where:

      ObjName is the name of the variable previously identified in the file for the 
              instance of the table. Example: MyTblStruct
   
      TblName is the FULL name of the table including the owning application.  
              Example: MyApp.TableName
           
      Desc is a 32 character or less description of the table image.  
           (NOTE: Description cannot include commas) Example: Default Table Image
        
      Filename is the default filename that the application is expecting to load 
               upon initialization.  Example: MyTblDefault.bin

      An example of the usage of this macro is in the SampleTblImg.c file.

UTILITY COMMAND LINE OPTIONS:

   The command line format for the utility is as follows:
    elf2cfetbl [-tTblName] [-d"Description"] [-h] [-v] [-V] [-s#] [-p#] [-n]
               [-T] [-eYYYY:MM:DD:hh:mm:ss] [-fYYYY:MM:DD:hh:mm:ss] SrcFilename [DestDirectory]
    where:
       -tTblName             replaces the table name specified in the object file with 'TblName'
       -d"Description"       replaces the description specified in the object file with 'Description'
       -h                    produces this output
       -v                    produces verbose output showing the breakdown of the object file in detail
       -V                    shows the version of this utility
       -s#                   specifies a Spacecraft ID to be put into file header.
                             # can be specified as decimal, octal (starting with a zero), or hex (starting with '0x')
       -p#                   specifies a Processor ID to be put into file header.
                             # can be specified as decimal, octal (starting with a zero), or hex (starting with '0x')
       -a#                   specifies an Application ID to be put into file header.
                             # can be specified as decimal, octal (starting with a zero), or hex (starting with '0x')
       -n                    specifies that output should NOT byte align FS header and secondary table header to nearest
                             4-byte boundary. The default assumes a 4-byte alignment on both structures.
       -T                    enables insertion of the SrcFilename's file creation time into the standard cFE File Header.
                             This option must be specified for either the '-e' and/or '-f' options below to have any effect.
                             By default, the time tag fields are set to zero.
       -eYYYY:MM:DD:hh:mm:ss specifies the spacecraft epoch time.  The SrcFilename's file creation time will be converted to
                             seconds since the specified epoch time and stored in the standard cFE File Header.
                             where:   YYYY=year, MM=month (01-12), DD=day (01-31),
                                      hh=hour (00-23), mm=minute (00-59), ss=seconds (00-59)
                             If no epoch is specified, the default epoch is either 1970:01:01:00:00:00 or the epoch specified
                             by the user using the '-f' option described below
                             This option requires the '-T' option, defined above, to be specified to have any effect
       -fYYYY:MM:DD:hh:mm:ss specifies the file system epoch time.  The SrcFilename's file creation time is obtained from the
                             file system as seconds since an epoch.  On most systems the file system epoch is defined as
                             1970:01:01:00:00:00.  If the user is running this application on a machine with a different epoch,
                             then the file system epoch should be defined with this option.
                             where:   YYYY=year, MM=month (01-12), DD=day (01-31), 
                                      hh=hour (00-23), mm=minute (00-59), ss=seconds (00-59)
                             If no epoch is specified, the default epoch is 1970:01:01:00:00:00
                             This option requires the '-T' option, defined above, to be specified to have any effect
       SrcFilename           specifies the object file to be converted
       DestDirectory         specifies the directory in which the cFE Table Image file is to be created.
                             If a directory is not specified './' is assumed.
    
    EXAMPLES:
       elf2cfetbl MyObjectFile ../../TblDefaultImgDir/
       elf2cfetbl -s12 -p0x0D -a016 -e2000:01:01:00:00:00 MyObjectFile ../../TblDefaultImgDir/
    
    NOTE: The name of the target file is specified within the source file as part of the CFE_TBL_FILEDEF macro.
          If the macro has not been included in the source file, the utility will fail to convert the object file.

SPECIAL NOTES: 

1) If one wishes to use a table image file under cygwin, the cygwin object file needs to 
   be converted from the COFF format to the ELF format before this utility can process it.  
   Cygwin provides a utility to make this conversion, it is called objcopy. To perform the 
   conversion, the user should issue a command similar to the following:

   $ objcopy -O elf32-little MyObjFilenameInCoffFormat.o MyObjFilenameInElfFormat.o

   where the first specified filename is the original object file and the second is the 
   elf format of the same file.  The resulting elf format file should then be compatible
   with the elf2cfetbl utility.
