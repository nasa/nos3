import sys
import os
from datetime import datetime, timedelta
from datetime import datetime, timedelta
from satellite_tle import SatelliteTle
from inview_calculator import InviewCalculator
from az_el_range_report import AzElRangeReportGenerator
from pytz import UTC

###############################################################################
# Python module to put together SatelliteTle and InviewCalculator functionality
# to produce a nice HTML report that describes events of interest
# for a satellite and ground station for a specified period of time.
###############################################################################

class SatelliteOverflightReportGenerator:
    """Class to create an HTML report for a given satellite and ground station for a specific time period """
    # Constructor
    def __init__(self, base_output_dir, satellite_tle, ground_station, tz, common_satellite_name, common_ground_station_name):
        # days:  0=today, -1=yesterday, 1 = tomorrow, etc.
        """Constructor"""
        self.__base_output_dir = base_output_dir
        self.__satellite_tle = satellite_tle
        self.__ground_station = ground_station
        self.__tz = tz
        self.__common_satellite_name = common_satellite_name
        self.__common_ground_station_name = common_ground_station_name
        self.__report_timezone = tz.tzname(datetime.now())
        self.__html_out = sys.stdout

    # Member functions
    def generate_report(self):
        """Method to generate the HTML report"""
        # Use Google timeline JavaScript API from:  https://developers.google.com/chart/interactive/docs/gallery/timeline
        
        months = ['zero', 'January', 'February', 'March', 'April', 'May', 'June', \
                'July', 'August', 'September', 'October', 'November', 'Decemter']
        weekdays = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday']
        today = datetime.now()
        start_time = self.__tz.localize(
            datetime(today.year, today.month, today.day, 0, 0, 0))
        end_time = self.__tz.localize(
            datetime(today.year, today.month, today.day, 23, 59, 59))

        filename = "%s/satellite-overflight.html" % (self.__base_output_dir)
        #sys.stderr.write("Generating report: %s\n" % filename)

        with open(filename, "w") as self.__html_out:
            self.__html_out.write("<html>\n")
            self.__html_out.write("  <head>\n")
            self.__html_out.write("  </head>\n")
            self.__html_out.write("    <title>Satellite %s Over %s</title>\n" % (self.__common_satellite_name, self.__common_ground_station_name))
            self.__html_out.write("  <body>\n")
            self.__html_out.write("    <p>On %s, %s %d, %4.4d, %s is over %s during:</p>\n" % \
                    (weekdays[today.weekday()], months[today.month], today.day, today.year, self.__common_satellite_name, self.__common_ground_station_name))

            ic = InviewCalculator(self.__ground_station, \
                                  self.__satellite_tle)
            inviews = []
            inviews = ic.compute_inviews(start_time, end_time)
            self.__html_out.write("    <ul>\n")
            for iv in inviews:
                riselocal = iv[0].astimezone(self.__tz)
                setlocal  = iv[1].astimezone(self.__tz)
                self.__html_out.write("      <li>%2.2d:%2.2d to %2.2d:%2.2d %s time</li>\n" % 
                        (riselocal.hour, riselocal.minute, setlocal.hour, setlocal.minute, self.__tz))
            self.__html_out.write("    </ul>\n")

            self.__html_out.write("  </body>\n")
            self.__html_out.write("</html>\n")

        
