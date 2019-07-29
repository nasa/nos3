#!/usr/bin/env python
import argparse
from os.path import join
from argvalidator import ArgValidator
from datetime import datetime
from pytz import timezone
import ground_station_tracking_schedule
import ground_station_schedule_directory

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", "--dir", help="Location of directory with ground station tracking schedule xlsm files", type=ArgValidator.validate_directory, default=None)
    parser.add_argument("-f", "--file", help="ground station tracking schedule xlsm file to parse", type=ArgValidator.validate_file, default=None)
    parser.add_argument("-d", "--date", help="Date to find schedule for", type=ArgValidator.validate_datetime, default=datetime.now())
    parser.add_argument("-s", "--satellite", help="Satellite to find schedule for", default="stf1")
    args = parser.parse_args()

    tz = timezone('US/Eastern')
    gssd = None
    if (args.dir is None):
        gssd = ground_station_schedule_directory.GroundStationScheduleDirectory()
    else:
        gssd = ground_station_schedule_directory.GroundStationScheduleDirectory(args.dir)

    thedate = None
    if (args.file is None):
        thedate = args.date.date()
        fname = gssd.get_latest_schedule_full_filename_for_date(thedate)
    else:
        fname = args.file

    print("Arguments:  ")
    print("  Dir:        %s" % args.dir)
    print("  File:       %s" % args.file)
    print("  Date:       %s" % args.date)
    print("  Satellite:  %s" % args.satellite)

    gsts = ground_station_tracking_schedule.GroundStationTrackingSchedule(join(gssd.get_dir_name(),fname))
    if (thedate is None):
        thedate = gsts.get_week()

    print("Computed:  ")
    print("  Dir:        %s" % gssd.get_dir_name())
    print("  File:       %s" % fname)
    print("  Date:       %s" % thedate)

    contacts = gsts.get_satellite_contacts(args.satellite)

    print("Schedule for satellite %s containing date %s found." % (args.satellite, thedate))
    print("Week beginning is %s; revision is %s." %(gsts.get_week(), gsts.get_revision()))

    for c in contacts:
        print("%s contact: %s to %s, max el %f" % (args.satellite, c[0].astimezone(tz), c[1].astimezone(tz), c[2])) 



# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()

