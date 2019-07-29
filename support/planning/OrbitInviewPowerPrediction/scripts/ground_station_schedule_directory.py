from os import listdir
from os.path import isfile, join
import ground_station_tracking_schedule
from datetime import timedelta

###############################################################################
# Python module to make it easy to work with a directory of 
# ground station tracking files
###############################################################################

class GroundStationScheduleDirectory:
    # Caches as much as possible to speed it up... but then does not reread directories, files, etc.
    def __init__(self, dir_name="C:\Users\msuder\Google Drive\STF1\WFF-Schedule"):
        self.__dir_name = dir_name
        self.__files = None
        self.__latest_files = None
    
    def get_dir_name(self):
        return self.__dir_name
    
    def get_schedule_file_list(self):
        if (self.__files is None):
            self.__files = []
            try:
                for f in listdir(self.__dir_name):
                    fname = join(self.__dir_name, f)
                    suffix = f[-5:]
                    prefix = f[:1]
                    if (isfile(fname) and (suffix == ".xlsm") and (prefix != "~")): # only care about files that end in .xlsm and are not backup (~) files
                        self.__files.append(f)
            except:
                pass # Just return an empty list of files
        return self.__files

    def get_latest_schedule_full_filename_for_date(self, dt):
        fname = self.get_latest_schedule_filename_for_date(dt)
        if (fname is None):
            return None
        else:
            return join(self.__dir_name, self.get_latest_schedule_filename_for_date(dt))

    def get_latest_schedule_filename_for_date(self, dt):
        zerodays = timedelta(days = 0)
        sevendays = timedelta(days = 7)

        for l in self.get_latest_files():
            span = dt - l[0]
            if ((zerodays <= span) and (span < sevendays)):
                return l[2]

        return None

    def get_latest_files(self):
        if (self.__latest_files is None):
            self.__process_all_files()
        return self.__latest_files

    def __process_all_files(self):
        self.__latest_files = [] 
        for f in self.get_schedule_file_list():
            gsts = ground_station_tracking_schedule.GroundStationTrackingSchedule(join(self.__dir_name, f))
            week = gsts.get_week()
            #print("Week:  %s, 0 processing file: %s" % (week, f))
            l = self.find_entry_for_date(week)
            if (l is None):
                self.__latest_files.append([week, gsts.get_revision(), f])
                #print("Week:  %s, 1 adding file    : %s, rev: %s" % (week, f, gsts.get_revision()))
            else:
                frev = gsts.get_revision()
                if (gsts.compare_revisions(l[1], frev) < 0):
                    #print("Week:  %s, 2 replacing file : %s, rev: %s, with file: %s, rev: %s" % (week, l[2], l[1], f, frev))
                    l[1] = frev
                    l[2] = f

    def find_entry_for_date(self, dt):
        for l in self.get_latest_files():
            if (l[0] == dt):
                return l

        return None

    def print_all_latest(self):
        for l in self.get_latest_files():
            print("Week: %s, Latest Rev: %s, Filename: %s" % (l[0], l[1], l[2]))

