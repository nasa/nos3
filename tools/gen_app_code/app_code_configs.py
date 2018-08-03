#========================================================================================
# File:    app_code_configs.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
#========================================================================================

import os, sys, time, datetime, app_utils

#========================================================================================

# Global variables - only global to this file
g_Date    = ""

g_Owner   = "unk"
g_Mission = "unk"

g_OutDir = "."

g_Apps = []
g_Tbls = []

g_Subdirs2 = ['for_build', 'mission_inc', 'platform_inc', 'src', 'tables', 'unit_test']

#========================================================================================

def parse_configs():
    global g_OutDir, g_Mission, g_Date, g_Owner, g_Apps, g_Tbls

    if len(sys.argv) <= 1:
        print "\nUsage:  gen_app_code.py <input-file>\t/* See SAMPLE_APPS.in for example */"
        print "\n        gen_app_code.py [MISSION <mission-name>]    /* No spaces in name */"
        print "                        [OWNER \"<owner-name>\"]\t    \
/* The only option with quoted string; Can have spaces in string */"
        print "                        [OUTDIR <output-dir>]\t    /* No spaces in path */"
        print "                        [APPS <app-list>]\t    /* List of app names, \
separated by spaces; No spaces in name */"
        print "                        [TBLS <key-list>]*\t    /* Optional; \
Keys: [iload, cds] */"
        print "\nEx:  gen_app_code.py SAMPLE_APPS.in"
        print "     gen_app_code.py MISSION myProj OWNER \"myFirst myLast\" \
OUTDIR /home/jdoe/projA APPS app1 app2"
        print "     gen_app_code.py MISSION myProj OWNER \"myFirst myLast\" OUTDIR . \
APPS app3 app4 app5 TBLS iload\n"
        sys.exit()
    else:
        tmpList = []
        tmpList.extend(sys.argv)
        # Remove script name
        tmpList.pop(0)

        curOpt  = ""
        optList = ['MISSION', 'OWNER', 'OUTDIR', 'TBLS', 'APPS']

        for itm in tmpList:
            if itm in optList:
                curOpt = itm
            elif curOpt == "":
                parse_input_file(itm)
                break
            elif curOpt == "MISSION":
                g_Mission = itm
            elif curOpt == "OWNER":
                g_Owner = itm
            elif curOpt == "OUTDIR":
                g_OutDir = itm
            elif curOpt == "TBLS":
                g_Tbls.append(itm)
            elif curOpt == "APPS":
                g_Apps.append(itm)
            else:
                pass

    g_Date = datetime.date.today()

#========================================================================================

def parse_input_file(fileName):
    global g_OutDir, g_Mission, g_Owner, g_Apps, g_Tbls

    # Open file for reading, if exists and is valid; else exit
    iFileHdl = app_utils.open_file_for_reading(fileName)

    # Parse the input file 
    for line in iFileHdl.readlines():
        if not line[0] == '#':
            words = line.split()
            if len(words) > 0:
                key = words[0]
                value = " ".join(words[1:])
                if key == "MISSION":
                    g_Mission = value
                elif key == "OUTDIR":
                    g_OutDir = value
                elif key == "OWNER":
                    g_Owner = value
                elif key == "APPS":
                    g_Apps.extend(value.split())
                elif key == "TBLS":
                    g_Tbls.extend(value.split())
                else:
                    pass

    iFileHdl.close()

#========================================================================================

def print_configs():
    global g_OutDir, g_Mission, g_Date, g_Owner, g_Apps, g_Tbls

    print """
DATE - %s

MISSION - %s

OUTDIR - %s

OWNER - %s

APPS - %s

TBLS - %s
    """ % (g_Date, g_Mission, g_OutDir, g_Owner, g_Apps, g_Tbls)

#========================================================================================

def get_date():
    global g_Date

    return g_Date

#========================================================================================

def get_mission():
    global g_Mission

    return g_Mission

#========================================================================================

def get_outdir():
    global g_OutDir

    return g_OutDir

#========================================================================================

def get_owner():
    global g_Owner

    return g_Owner

#========================================================================================

def get_apps():
    global g_Apps

    return g_Apps

#========================================================================================

def get_tables():
    global g_Tbls

    return g_Tbls

#========================================================================================
# End of app_code_configs.py
#========================================================================================

