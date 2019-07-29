import sys
#import os
from datetime import datetime, timedelta
#from datetime import datetime, timedelta
from satellite_tle import SatelliteTle
from inview_calculator import InviewCalculator
#from az_el_range_report import AzElRangeReportGenerator
#from pytz import UTC

###############################################################################
# Python module to put together SatelliteTle and InviewCalculator functionality
# to produce a text list of inviews from the ground station to the satellite 
# for a specified period of time.
###############################################################################

class InviewListReportGenerator:
    """Class to create a list of inviews report for a given satellite and ground station for a specific time period """
    # Constructor
    def __init__(self, base_output_dir, satellite_tle, ground_station, tz, \
                 start_day = 0, end_day = 0):
        # days:  0=today, -1=yesterday, 1 = tomorrow, etc.
        """Constructor"""
        self.__base_output_dir = base_output_dir
        self.__satellite_tle = satellite_tle
        self.__ground_station = ground_station
        self.__tz = tz
        self.__report_timezone = tz.tzname(datetime.now())
        self.__start_day = start_day
        self.__end_day = end_day
        self.__out = sys.stdout

    # Member functions
    def generate_report(self):
        """Method to generate the inview report"""
        
        end = start = datetime.now()
        start = start + timedelta(days=self.__start_day)
        end = end + timedelta(days=self.__end_day)
        start_time = self.__tz.localize(
            datetime(start.year, start.month, start.day, 0, 0, 0))
        end_time = self.__tz.localize(
            datetime(end.year, end.month, end.day, 23, 59, 59))

        filename = "%s/inviews.txt" % (self.__base_output_dir)
        #sys.stderr.write("Generating report: %s\n" % filename)

        with open(filename, "w") as self.__out:
            ic = InviewCalculator(self.__ground_station, \
                                  self.__satellite_tle)
            inviews = []
            inviews = ic.compute_inviews(start_time, end_time)
            for iv in inviews:
                riselocal = iv[0].astimezone(self.__tz)
                setlocal  = iv[1].astimezone(self.__tz)
                self.__out.write("%s to %s\n" % (riselocal, setlocal))

        
