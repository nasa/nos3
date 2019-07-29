import sys
import math
from satellite_tle import SatelliteTle
from datetime import datetime, timedelta
from pytz import UTC
from pyorbital.orbital import Orbital, astronomy
import numpy as np

###############################################################################
# Python module to make it easy to compute the inview times (above a
# certain elevation angle) from a location on earth (latitude,
# longitude, elevation) to a spacecraft.  This module downloads
# the spacecraft TLE from Celestrak
#
# Obviously uses lots of other libraries to do the heavy lifting.
#
# Here are some reference URLs:
# http://www.celestrak.com/columns/v04n03/
# http://www.celestrak.com/columns/v04n05/
# https://docs.python.org/2/install/
# https://docs.python.org/2/library/datetime.html
# http://pytroll.org/
###############################################################################

class InviewCalculator:
    """Class to compute inviews above a specified elevation angle from a latitude, longitude, elevation to a satellite (number) """
    # Constructor
    def __init__(self, ground_station, satellite_tle):
        """Constructor:  ground station as GroundStation, satellite TLE as TleManipulator"""
        self.__ground_station = ground_station
        self.__satellite_tle = satellite_tle
        self.__orb = Orbital(str(self.__satellite_tle.get_satellite_number()), \
                             line1=self.__satellite_tle.get_line1(), \
                             line2=self.__satellite_tle.get_line2())

    # Member functions
        
    def __repr__(self):
        """Returns a string representing an instance of this class."""
        out = 'Inview Calculator:\n' \
              'latitude=%f, longitude=%f, elevation=%f, ' \
              'minimum elevation angle=%s, satellite TLE=\n%s' % \
              (self.__ground_station.get_latitude(), self.__ground_station.get_longitude(), self.__ground_station.get_minimum_elevation_angle(), \
               self.__ground_station.get_minimum_elevation_angle(), self.__satellite_tle)
        return out

    def compute_inviews(self, in_start_time, in_end_time):
        """Method to compute inviews (in UTC) for the initialized location, elevation angle, and satellite over a specified time period.  Returns a list of inview start/stop times, accurate to the nearest second and using the latest available TLE when the method is called."""
        # NOTE:  pyorbital EXPECTS naive date/times and interprets them
        # as UTC... so we need to satisfy it; however, we are making this
        # module DATETIME AWARE, so RETURN VALUES are DATETIME AWARE!!
        # Also, all naive inputs are assumed to be UTC and all aware inputs
        # are converted to UTC (and then made naive)
        if (in_start_time.tzinfo is not None):
            temp = in_start_time.astimezone(UTC)
            start_time = datetime(temp.year, temp.month, temp.day, \
                                  temp.hour, temp.minute, temp.second)
        else:
            start_time = in_start_time
        if (in_end_time.tzinfo is not None):
            temp = in_end_time.astimezone(UTC)
            end_time = datetime(temp.year, temp.month, temp.day, \
                                temp.hour, temp.minute, temp.second)
        else:
            end_time = in_end_time
        # start_time, end_time are now naive
        inviews = []
        time = start_time
        up = 0
        el_increasing = 0
        maxel = 0
        try:
            (az, el) = self.__orb.get_observer_look(time, self.__ground_station.get_longitude(), \
                                                    self.__ground_station.get_latitude(), \
                                                    self.__ground_station.get_minimum_elevation_angle())
            if (el > self.__ground_station.get_minimum_elevation_angle()):
                # start the first inview at the input start_time
                up = 1
                el_increasing = 1
                rising = time
            # Step through time, looking for inview starts and ends
            while (time < end_time):
                (az, el) = self.__orb.get_observer_look(time, self.__ground_station.get_longitude(), \
                                                        self.__ground_station.get_latitude(), \
                                                        self.__ground_station.get_minimum_elevation_angle())
                if (el > self.__ground_station.get_minimum_elevation_angle()) and (up == 0):
                    rising = self.__find_exact_crossing(time, up)
                    el_increasing = 1
                    up = 1
                if (el < self.__ground_station.get_minimum_elevation_angle()) and (up == 1):
                    # make sure to append AWARE datetimes
                    crossing = self.__find_exact_crossing(time, up)
                    inviews.append((rising.replace(tzinfo=UTC), \
                                    crossing.replace(tzinfo=UTC), \
                                    maxel))
                    el_increasing = 0
                    up = 0
                    maxel = 0
                if (el > self.__ground_station.get_minimum_elevation_angle()):
                    if (el > maxel):
                        maxel = el
                    elif (el_increasing): # First point after el starts decreasing... find the exact max el
                        el_increasing = 0
                        maxel = self.__find_exact_maxel(time, maxel)
                time += self.__oneminute
            if (up == 1):
                # end the last inview at the input end_time
                # make sure to append AWARE datetimes
                inviews.append((rising.replace(tzinfo=UTC), \
                                end_time.replace(tzinfo=UTC), \
                                maxel))

            return inviews
        except NotImplementedError: # Does not seem to work?
            print("NotImplementedError computing inviews.  Date/time = %s-%s-%sT%s:%s:%s, satellite number = %s" % \
                (time.year, time.month, time.day, time.hour, time.minute, time.second, self.__satellite_tle.get_satellite_number()))
            sys.stdout.flush()
            raise
        except: # Does not seem to work?
            print("Unknown exception computing inviews.  Date/time = %s-%s-%sT%s:%s:%s, satellite number = %s" % \
                (time.year, time.month, time.day, time.hour, time.minute, time.second, self.__satellite_tle.get_satellite_number()))
            sys.stdout.flush()
            raise
    
    def print_inviews(self, inviews):
        """Method to print a table of inviews... assumes that inviews contains the data for such a table"""
        for iv in inviews:
            print("Rise: %s, Set: %s, Maximum Elevation: %f" % (iv[0], iv[1], iv[2]))

    def compute_azels(self, in_start_time, in_end_time, time_step_seconds):
        """Method to compute az/el angles at time_step intervals during the input time period, INDEPENDENT of whether the satellite is actually in view """
        # NOTE:  pyorbital EXPECTS naive date/times and interprets them
        # as UTC... so we need to satisfy it; however, we are making this
        # module DATETIME AWARE, so RETURN VALUES are DATETIME AWARE!!
        # Also, all naive inputs are assumed to be UTC and all aware inputs
        # are converted to UTC (and then made naive)
        if (in_start_time.tzinfo is not None):
            temp = in_start_time.astimezone(UTC)
            start_time = datetime(temp.year, temp.month, temp.day, \
                                  temp.hour, temp.minute, temp.second)
        else:
            start_time = in_start_time
        if (in_end_time.tzinfo is not None):
            temp = in_end_time.astimezone(UTC)
            end_time = datetime(temp.year, temp.month, temp.day, \
                                temp.hour, temp.minute, temp.second)
        else:
            end_time = in_end_time
        # start_time, end_time are now naive
        try:
            delta = timedelta(seconds=time_step_seconds)
        except:
            delta = timedelta(seconds=60)
        azels = []
        time = start_time
        # Naively compute the table... i.e. compute time, az, el for the
        # input duration at each time step... no matter whether the satellite
        # is really inview or not!
        while (time < end_time + delta):
            (az, el) = self.__orb.get_observer_look(time, self.__ground_station.get_longitude(), \
                                                    self.__ground_station.get_latitude(), \
                                                    self.__ground_station.get_minimum_elevation_angle())
            range_km = self.__compute_range(time)
            outtime = time.replace(tzinfo=UTC) # time is unmodified!
            azels.append((outtime, az, el, range_km))
            time += delta
            
        return azels

    def __compute_range(self, utc_time):
        """Method to compute range in kilometers from observer to satellite at *naive* UTC time utc_time"""
        # N.B. pyorbital works in kilometers
        (pos_x, pos_y, pos_z), (vel_x, vel_y, vel_z) = self.__orb.get_position(utc_time, normalize=False)
        (opos_x, opos_y, opos_z), (ovel_x, ovel_y, ovel_z) = astronomy.observer_position(utc_time, \
                self.__ground_station.get_longitude(), self.__ground_station.get_latitude(), self.__ground_station.get_elevation_in_meters()/1000.0)
        dx = pos_x - opos_x
        dy = pos_y - opos_y
        dz = pos_z - opos_z
        return math.sqrt(dx*dx + dy*dy + dz*dz) # km

    # up is what it is **before** the crossing
    def __find_exact_crossing(self, time, up): 
        """Private method to refine an in view/out of view crossing time from the nearest minute to the nearest second."""
        exacttime = time
        # The crossing occurred in the minute before now... search backwards
        # for the exact second of crossing
        for j in range(0, 60):
            exacttime -= self.__onesecond
            (az, el) = self.__orb.get_observer_look(exacttime, \
                                                    self.__ground_station.get_longitude(), \
                                                    self.__ground_station.get_latitude(), \
                                                    self.__ground_station.get_minimum_elevation_angle())
            if ((el > self.__ground_station.get_minimum_elevation_angle()) and (up == 1)) or \
               ((el < self.__ground_station.get_minimum_elevation_angle()) and (up == 0)):
                break
        return exacttime

    def __find_exact_maxel(self, time, maxel):
        """Private method to refine the maximum elevation from occuring at the nearest minute to the nearest second."""
        exacttime = time
        # The maximum elevation occurred in the **two** minutes before now... search backwards
        # for the exact second of maximum elevation
        for j in range(0, 120):
            exacttime -= self.__onesecond
            (az, el) = self.__orb.get_observer_look(exacttime, \
                                                    self.__ground_station.get_longitude(), \
                                                    self.__ground_station.get_latitude(), \
                                                    self.__ground_station.get_minimum_elevation_angle())
            if (el > maxel):
                maxel = el
        return maxel        
    
    # Class member constants
    __onesecond = timedelta(seconds=1)
    __oneminute = timedelta(minutes=1)
