import sys
import os
import math
from datetime import datetime, timedelta
from datetime import datetime, timedelta
from satellite_tle import SatelliteTle
from inview_calculator import InviewCalculator
from pytz import UTC

###############################################################################
# Python module to put together SatelliteTle and InviewCalculator functionality
# to produce a report of the angular separation with respect to a ground
# station over time between satellites during inviews for a specified period
# of time.
###############################################################################

class AngularSeparationReportGenerator:
    """Class to create an angular separtion report for a given ground station, a given satellite, and one or more other satellites for a specific time period """
    # Constructor
    def __init__(self, base_output_dir, ground_station, satellite_of_interest_tle, other_satellite_tle_list, tz, \
                 start_day = 0, end_day = 0):
        # days:  0=today, -1=yesterday, 1 = tomorrow, etc.
        """Constructor"""
        self.__base_output_dir = base_output_dir
        self.__ground_station = ground_station
        self.__satellite_of_interest = satellite_of_interest_tle
        self.__other_satellite_tle_list = other_satellite_tle_list
        self.__tz = tz
        self.__report_timezone = tz.tzname(datetime.now())
        self.__start_day = start_day
        self.__end_day = end_day
        self.__debug = False
        self.__trace = False
        self.__out = sys.stdout

    # Member functions
    def generate_report(self):
        """Method to generate the CSV report"""
        
        today = datetime.now()
        directory = "%s/%4.4d-%2.2d-%2.2d" % (self.__base_output_dir, today.year, today.month, today.day)
        #sys.stderr.write("Generating report in directory: %s\n" % directory)
        try:
            os.makedirs(directory)
        except OSError as e:
            if (e.errno != os.errno.EEXIST):
                sys.stderr.write("Error making directory %s: %s" % (directory, e))
                raise e

        filename = "%s/%4.4d-%2.2d-%2.2d-angular-separation.txt" % (directory, today.year, today.month, today.day)
        #sys.stderr.write("Generating report: %s\n" % filename)

        with open(filename, "w") as self.__out:

            for i in range(self.__start_day, self.__end_day+1):
                self.__generate_separations_for_day(i)

    def __generate_separations_for_day(self, day):
        """Method to generate separations for one day"""

        D2R = math.pi / 180.0
        # ALL TIMES ARE IN THE TIMEZONE self.__tz !!
        # Determine the time range for the requested day
        day_date = datetime.now() + timedelta(days=day)
        day_year = day_date.year
        day_month = day_date.month
        day_day = day_date.day
        start_time = self.__tz.localize(
            datetime(day_year, day_month, day_day, 0, 0, 0))
        end_time = self.__tz.localize(
            datetime(day_year, day_month, day_day, 23, 59, 59))
        
        # Get the InviewCalculator and compute the inviews
        base_ic = InviewCalculator(self.__ground_station, self.__satellite_of_interest)
        base_inviews = []
        base_inviews = base_ic.compute_inviews(start_time, end_time)
        if (self.__debug):
            self.__out.write("Inviews for satellite of interest %s\n" % self.__satellite_of_interest.get_satellite_number()) # debug
            base_ic.print_inviews(base_inviews)
        for sat in self.__other_satellite_tle_list:
            other_ic = InviewCalculator(self.__ground_station, sat)
            other_inviews = []
            other_inviews = other_ic.compute_inviews(start_time, end_time)
            if (self.__debug):
                self.__out.write("Inviews for other satellite %s\n" % sat.get_satellite_number()) # debug
                other_ic.print_inviews(other_inviews)

            combined_inviews = self.combine_inviews(base_inviews, other_inviews)
            if (self.__debug):
                self.__out.write("Combined inviews for %s and %s\n" % (self.__satellite_of_interest.get_satellite_number(), sat.get_satellite_number()))
            for civ in combined_inviews:
                if (self.__debug):
                    self.__out.write("Start:  %s, end:  %s\n" % (civ[0], civ[1]))
                base_azel = base_ic.compute_azels(civ[0], civ[1], 15)
                other_azel = other_ic.compute_azels(civ[0], civ[1], 15)
                for i in range(len(base_azel)):
                    base_unit = [math.cos(D2R*base_azel[i][1])*math.cos(D2R*base_azel[i][2]), 
                            math.sin(D2R*base_azel[i][1])*math.cos(D2R*base_azel[i][2]), 
                            math.sin(D2R*base_azel[i][2])]
                    other_unit = [math.cos(D2R*other_azel[i][1])*math.cos(D2R*other_azel[i][2]), 
                            math.sin(D2R*other_azel[i][1])*math.cos(D2R*other_azel[i][2]), 
                            math.sin(D2R*other_azel[i][2])]
                    separation_angle = math.acos(base_unit[0]*other_unit[0] + base_unit[1]*other_unit[1] + base_unit[2]*other_unit[2]) / D2R
                    self.__out.write("%s, %s, %s, %s, %f, %f, %f, %f, %f\n" % 
                            (base_azel[i][0], other_azel[i][0], self.__satellite_of_interest.get_satellite_number(), sat.get_satellite_number(), separation_angle, 
                                base_azel[i][1], base_azel[i][2],
                                other_azel[i][1], other_azel[i][2]))

    def combine_inviews(self, inviews1, inviews2):
        """Method to combine two set of inviews into the overlapping parts + single ends"""
        index1 = 0
        index2 = 0
        combined_inviews = []
        if (self.__trace):
            self.__out.write(inviews1)
            self.__out.write("\n")
            self.__out.write(inviews2)
            self.__out.write("\n")

        while ((index1 < len(inviews1)) and (index2 < len(inviews2))): 
            if (inviews1[index1][0] < inviews2[index2][0]):
                combined_start = inviews1[index1][0]
                starting_number = 1
            else:
                combined_start = inviews2[index2][0]
                starting_number = 2
            if (self.__trace):
                self.__out.write("Combined start beginning at indices %d, %d is number %d at %s\n" % (index1, index2, starting_number, combined_start)) # debug
            if (starting_number == 1):
                if (inviews2[index2][0] <= inviews1[index1][1]):
                    if (self.__trace):
                        self.__out.write("Overlap found for indices %d, %d\n" % (index1, index2)) # debug
                    if (inviews2[index2][1] > inviews1[index1][1]):
                        ending_number = 2
                        combined_end = inviews2[index2][1]
                        if (self.__trace):
                            self.__out.write("Combined inview for indices %d, %d is %s-%s\n" % (index1, index2, combined_start, combined_end)) # debug
                    else:
                        ending_number = 1
                        combined_end = inviews1[index1][1]
                        if (self.__trace):
                            self.__out.write("Combined inview for indices %d, %d is %s-%s\n" % (index1, index2, combined_start, combined_end)) # debug
                    combined_inviews.append((combined_start, combined_end))
                else:
                    if (self.__trace):
                        self.__out.write("No overlap found for indices %d, %d\n" % (index1, index2)) # debug
                    if (inviews1[index1][0] > inviews2[index2][1]):
                            index2 = index2 + 1
                index1 = index1 + 1
            else:
                if (inviews1[index1][0] <= inviews2[index2][1]):
                    if (self.__trace):
                        self.__out.write("Overlap found for indices %d, %d\n" % (index1, index2)) # debug
                    if (inviews2[index2][1] > inviews1[index1][1]):
                        ending_number = 2
                        combined_end = inviews2[index2][1]
                        if (self.__trace):
                            self.__out.write("Combined inview for indices %d, %d is %s-%s\n" % (index1, index2, combined_start, combined_end)) # debug
                    else:
                        ending_number = 1
                        combined_end = inviews1[index1][1]
                        if (self.__trace):
                            self.__out.write("Combined inview for indices %d, %d is %s-%s\n" % (index1, index2, combined_start, combined_end)) # debug
                    combined_inviews.append((combined_start, combined_end))
                else:
                    if (self.__trace):
                        self.__out.write("No overlap found for indices %d, %d\n" % (index1, index2)) # debug
                    if (inviews2[index2][0] > inviews1[index1][1]):
                            index1 = index1 + 1
                index2 = index2 + 1
        return combined_inviews
