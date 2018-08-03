#========================================================================================
# File:    app_utils.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
#========================================================================================

import os, sys, shutil, app_code_configs

#========================================================================================

def create_a_dir(dirName):
    if not os.path.exists(dirName):
        os.mkdir(dirName, 0755)
        print "Created directory ", dirName

#========================================================================================

def create_cfs_dirs(tgtApp):
    OutputDir = app_code_configs.get_outdir()
    Tables = app_code_configs.get_tables()

    # Create the app's top-level directory
    level0 = os.path.join(OutputDir, tgtApp.lower())
    create_a_dir(level0)

    # Create "fsw" sub-directory
    level1 = os.path.join(level0, "fsw")
    create_a_dir(level1)

    # Create sub-directories under "fsw/"
    subdirs = ['for_build', 'mission_inc', 'platform_inc', 'src', 'unit_test']
    if "none" in Tables:
        # Remove directory tree
        level2 = os.path.join(level1, "tables")
        remove_dir_tree(level2)
    elif "iload" in Tables or "cds" in Tables:
        subdirs.append("tables")
    else:
        pass

    for item in subdirs:
        level2 = os.path.join(level1, item)
        create_a_dir(level2)

#========================================================================================

def remove_dir_tree(dirName):
    if os.path.exists(dirName):
        shutil.rmtree(dirName)
        print "Removed   ", dirName, "and its sub-directories"

#========================================================================================

def open_file_for_reading(fileName):
    # First, check for existence of input; else exit
    if not os.path.exists(fileName):
        print "\nError:", fileName, "does not exist!\n"
        sys.exit()

    # Second, check for valid file type; else exit
    if os.path.isdir(fileName):
        print "\nError:", fileName, " is a directory!\n"
        sys.exit()

    # Third, check for file's read access; else exit
    if not os.access(fileName, os.R_OK):
        print "\nError:", fileName, "is not readable - check file permission!\n"
        sys.exit()

    # When all checks are valid, open file for reading; return file handle
    return open(fileName, 'r')

#========================================================================================

def open_file_for_writing(fileName, mustExistFlg):
    if mustExistFlg:
        # First, check for existence of input; else exit
        if not os.path.exists(fileName):
            print "\nError:", fileName, "does not exist!\n"
            sys.exit()

        # Second, check for valid file type; else exit
        if os.path.isdir(fileName):
            print "\nError:", fileName, "is a directory!\n"
            sys.exit()

        # Third, check for file's write access; else exit
        if not os.access(fileName, os.W_OK):
            print "\nError:", fileName, " not writable - check file permission!\n"
            sys.exit()

    # Open file for writing; return file handle
    return open(fileName, 'w')

#========================================================================================

def close_file_from_writing(fileHdl, fileName, fileExistFlg):
    fileHdl.close()
    if fileExistFlg:
        print "Over-written  ", fileName
    else:
        print "Created  ", fileName

#========================================================================================

def remove_file(fileName):
    if os.path.exists(fileName):
        os.remove(fileName)
        print "Removed  ", fileName

#========================================================================================
# End of app_utils.py
#========================================================================================

