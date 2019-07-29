from urllib import urlopen
from fileinput import input
from pyorbital.orbital import Orbital
from pytz import UTC
from datetime import datetime, timedelta
from math import sqrt, sin, cos, asin, acos, pi
from configuration import Configuration

###############################################################################
# Python module to make it easy to retrieve and use a satellite two
# line element set for a given satellite.  This module downloads
# the spacecraft TLE from Celestrak.
#
# Obviously uses lots of other libraries to do the heavy lifting.
#
# Here are some reference URLs:
# http://www.celestrak.com/columns/v04n03/
# http://www.celestrak.com/columns/v04n05/
# https://docs.python.org/2/install/
# http://stackoverflow.com/questions/15138614/how-can-i-read-the-contents-of-an-url-with-python
# https://docs.python.org/2/library/datetime.html
###############################################################################

class SatelliteTleException:
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class SatelliteTle:
    """Class to retrieve and use a two line element set for a given satellite """
    # Constructor
    def __init__(self, satellite_number, satellite_name = None, satellite_contact_name = None, tle_url = "http://www.celestrak.com/NORAD/elements/cubesat.txt", tle_file = None, 
            rx_freq = None, tx_freq = None):
        """Constructor:  satellite number according to NORAD"""
        self.__satellite_number = satellite_number
        self.__tle_url = tle_url
        self.__tle_file = tle_file # Like "C:\Users\msuder\Desktop\STF1-TLE.txt"
        self.__satellite_name = satellite_name
        self.__satellite_contact_name = satellite_contact_name
        self.__receive_frequency = rx_freq
        self.__transmit_frequency = tx_freq
        self.__line1 = None
        self.__line2 = None
        self.__get_tle()

    # Class Methods to construct in alternative ways
    @classmethod
    def from_config(cls, sat_data):
        # sys.stderr.write(sat_data.__repr__() + "\n") # debug
        sat_num = Configuration.get_config_int(sat_data.get('number','1'), 0, 1000000, 0)
        sat_name = sat_data.get('name', None)
        sat_contact_name = sat_data.get('contact_name', None)
        sat_url = sat_data.get('url', 'http://www.celestrak.com/NORAD/elements/cubesat.txt')
        sat_rx_freq = Configuration.get_config_float(sat_data.get('receive_frequency', None), 0, 9999, None)
        sat_tx_freq = Configuration.get_config_float(sat_data.get('transmit_frequency', None), 0, 9999, None)
        return cls(sat_num, satellite_name=sat_name, satellite_contact_name=sat_contact_name, tle_url=sat_url, rx_freq=sat_rx_freq, tx_freq=sat_tx_freq)
    
    # Member functions
    def refetch_tle(self):
        """Method the class user can call to fetch the TLE info again, to make sure it is up to date"""
        self.__get_tle()

    def compute_ephemeris_point(self, in_time):
        """Method to compute an ephemeris point for a given time"""
        time = self.__time_to_naiveUTC(in_time)
        (pos, vel) = self.__orbit.get_position(time, normalize=False)
        return (in_time, pos, vel)

    def compute_ephemeris_table(self, in_start_time, in_end_time, time_step_seconds):
        """Method to compute a table of ephemerides for a given time span at a given time step"""
        time = in_start_time
        try:
            delta = timedelta(seconds=time_step_seconds)
        except:
            delta = timedelta(seconds=60)
        ephem_tbl = []
        while (time < in_end_time + delta):
            ephem_tbl.append(self.compute_ephemeris_point(time))
            time += delta

        return ephem_tbl

    def compute_lonlatalt_point(self, in_time):
        """Method to compute an ephemeris point for a given time"""
        time = self.__time_to_naiveUTC(in_time)
        (lon, lat, alt) = self.__orbit.get_lonlatalt(time)
        return (in_time, lon, lat, alt)

    def compute_lonlatalt_table(self, in_start_time, in_end_time, time_step_seconds):
        """Method to compute a table of lon/lat/alt for a given time span at a given time step"""
        time = in_start_time
        try:
            delta = timedelta(seconds=time_step_seconds)
        except:
            delta = timedelta(seconds=60)
        lla_tbl = []
        while (time < in_end_time + delta):
            lla_tbl.append(self.compute_lonlatalt_point(time))
            time += delta

        return lla_tbl

    def compute_sun_times(self, in_start_time, in_end_time):
        """Method to compute in sun times (in UTC) for the satellite over a specified time period.  Returns a list of sun entrance/exit times, accurate to the nearest second and using the latest available TLE when the method is called."""
        start_time = self.__time_to_naiveUTC(in_start_time)
        end_time = self.__time_to_naiveUTC(in_end_time)
        # start_time, end_time are now naive
        suntimes = []
        pentimes = []
        umtimes = []
        time = start_time
        state = self.get_satellite_sun_state(time)
        if (state == self.InSun):
            sunenter = time
        elif (state == self.InPenumbra):
            penenter = time
        else:
            umenter = time
        # Step through time, looking for transitions from one state to another
        while (time < end_time):
            now_state = self.get_satellite_sun_state(time)
            if (now_state != state):
                if (state == self.InSun):
                    suntimes.append((sunenter.replace(tzinfo=UTC), time.replace(tzinfo=UTC)))
                elif (state == self.InPenumbra):
                    pentimes.append((penenter.replace(tzinfo=UTC), time.replace(tzinfo=UTC)))
                else:
                    umtimes.append((umenter.replace(tzinfo=UTC), time.replace(tzinfo=UTC)))
                if (now_state == self.InSun):
                    sunenter = time
                elif (now_state == self.InPenumbra):
                    penenter = time
                else:
                    umenter = time
                state = now_state
            time += self.__onesecond
            
        if (state == self.InSun):
            suntimes.append((sunenter.replace(tzinfo=UTC), time.replace(tzinfo=UTC)))
        elif (state == self.InPenumbra):
            pentimes.append((penenter.replace(tzinfo=UTC), time.replace(tzinfo=UTC)))
        else:
            umtimes.append((umenter.replace(tzinfo=UTC), time.replace(tzinfo=UTC)))

        return [suntimes, pentimes, umtimes]            

    def get_satellite_sun_state(self, in_time):
        """Method to determine if the satellite is in sun, penumbra, or umbra at the given time"""
        # https://celestrak.com/columns/v03n01/
        earth_sun = self.get_sun_vector(in_time)
        ephemeris = self.compute_ephemeris_point(in_time)
        earth_sat = ephemeris[1]
        sat_sun = [earth_sun[0]-earth_sat[0], earth_sun[1]-earth_sat[1], earth_sun[2]-earth_sat[2]]

        rho_e = sqrt(earth_sat[0]*earth_sat[0] + earth_sat[1]*earth_sat[1] + earth_sat[2]*earth_sat[2])        
        rho_s = sqrt(sat_sun[0]*sat_sun[0] + sat_sun[1]*sat_sun[1] + sat_sun[2]*sat_sun[2])
        
        theta_e = asin(self.__earthradius / rho_e)
        theta_s = asin(self.__sunradius / rho_s)
        theta = acos(-1 * (earth_sat[0] * sat_sun[0] + earth_sat[1] * sat_sun[1] + earth_sat[2] * sat_sun[2]) / (rho_e * rho_s))

        if (theta > theta_e + theta_s):
            return self.InSun
        elif ((theta_e > theta_s) and (theta < theta_e - theta_s)):
            return self.InUmbra
        else:
            return self.InPenumbra
        
    def get_sun_vector(self, in_time):
        """Method to determine the sun vector at a specific time."""
        # Astronomical Algorithms, 2nd ed., Jean Meeus, Willman-Bell, 1998
        dt = self.__time_to_naiveUTC(in_time) - datetime(2000, 1, 1, 12, 0)
        T = (dt.days + (dt.seconds + dt.microseconds / (1000000.0)) / (24 * 3600.0)) / 36525.0 # (25.1)
        epsilon_0 = (23.0 + 26.0/60.0 + 21.448/3600.0 - (46.8150*T + 0.00059*T*T - 0.001813*T*T*T) / 3600) * pi / 180.0 # (22.2)
        L_0 = 280.46646 + 36000.76983*T + 0.0003032*T*T # (25.2)
        M = (357.52911 + 35999.05029*T - 0.0001537*T*T) * pi / 180.0 # (25.3)
        e = 0.016708634 - 0.000042037*T - 0.0000001267*T*T # (25.4)
        C = ((1.914602 - 0.004817*T - 0.000014*T*T)*sin(M) + (0.019993 - 0.000101*T)*sin(2*M) + 0.000289*sin(3*M)) # p. 164
        true_longitude = (L_0 + C) * pi / 180.0 # p. 164
        nu = M + C # p. 164
        R = (1.000001018 * (1 - e*e))/(1 + e * cos(nu)) # (25.5)
        x = cos(true_longitude) # Set x = cos(true longitude), see (25.6)
        y = cos(epsilon_0) * sin(true_longitude) # Then y comes from this by (25.6)
        z = sin(epsilon_0) * sin(true_longitude) # And z comes from this by (25.7)
        x = x * R * self.__astronomical_unit # Scale unit vector to earth/sun distance in km
        y = y * R * self.__astronomical_unit # Scale unit vector to earth/sun distance in km
        z = z * R * self.__astronomical_unit # Scale unit vector to earth/sun distance in km

        return [x, y, z]
    
    # Getters
    def get_tle_url(self):
        return self.__tle_url
    
    def get_line1(self):
        return self.__line1
    
    def get_line2(self):
        return self.__line2

    def get_satellite_name(self):
        if (self.__satellite_name is None):
            return self.get_satellite_number()
        else:
            return self.__satellite_name

    def get_satellite_contact_name(self):
        if (self.__satellite_contact_name is None):
            return self.get_satellite_name()
        else:
            return self.__satellite_contact_name
    
    def get_receive_frequency(self):
        return self.__receive_frequency

    def get_transmit_frequency(self):
        return self.__transmit_frequency

    def get_satellite_number(self):
        return self.__line1[2:7]
    
    def get_launch_year(self): # last two digits of launch year
        return self.__line1[9:11]
    
    def get_launch_year_number(self): # launch number of the year
        return self.__line1[11:14]
    
    def get_launch_piece(self): # piece of the launch
        return self.__line1[14:17]
    
    def get_epoch_year(self): # last two digits of year
        return self.__line1[18:20]
    
    def get_epoch_day(self): # day of the year and fractional part of the day
        return self.__line1[20:32]
    
    def get_mean_motion_dot(self): # first time derivative of the mean motion
        return self.__line1[33:43]
    
    def get_mean_motion_doubledot(self): # second time derivative of the mean motion, decimal point assumed
        return self.__line1[44:52]
    
    def get_bstar(self): # decimal point assumed
        return self.__line1[53:61]
    
    def get_element_number(self):
        return self.__line1[64:68]
    
    def get_inclination(self): # degrees
        return self.__line2[8:16]
    
    def get_raan(self): # degrees
        return self.__line2[17:25]
    
    def get_eccentricity(self): # decimal point assumed
        return self.__line2[26:33]
    
    def get_arg_perigee(self): # degrees
        return self.__line2[34:42]
    
    def get_mean_anomaly(self): # degrees
        return self.__line2[43:51]
    
    def get_mean_motion(self): # revs per day
        return self.__line2[52:63]
    
    def get_rev_at_epoch(self): # revs
        return self.__line2[63:68]

    # Other methods
    def __repr__(self):
        """Returns a string representing an instance of this class."""
        hdr0 = "Sat_Name________________\n"
        out0 = ('%s\n') % (self.get_satellite_name())
        hdr1 = "L_SatnmU_LyLnmLp__EyEdd.dddddddd__MeanMoDot__MMDbDot__-BSTAR-_0_ElNmX\n"
        out1 = ('%s\n') % (self.__line1)
        hdr2 = "L_Satnm__Inclina_RtAscANd_Eccentr_ArgPerig_MeanAnom_MeanMotion-RevNmX\n"
        out2 = ('%s') % (self.__line2)
        return hdr1 + out1 + hdr2 + out2

    def raw_string(self):
        out = ""
        if (self.__satellite_name is not None):
            out += ("%s\n") % self.__satellite_name
        out += ('%s\n%s') % (self.__line1, self.__line2)
        return out
        
    def pretty_string(self):
        rx_freq = ""
        if (self.__receive_frequency is not None):
            rx_freq = ", Receive Frequency %s" % self.__receive_frequency
        tx_freq = ""
        if (self.__transmit_frequency is not None):
            tx_freq = ", Transmit Frequency %s" % self.__transmit_frequency
        out0 = ('Satellite Name=%s%s%s\n') % \
               (self.get_satellite_name(), rx_freq, tx_freq)
        out1 = ('Satellite Number=%s, Launch Year=%s, Launch Day=%s, Launch Piece=%s\n') % \
               (self.get_satellite_number(), self.get_launch_year(), \
                self.get_launch_year_number(), self.get_launch_piece())
        out2 = ('Epoch Year=%s, Epoch Day=%s, Mean Motion Dot=%s\n') % \
               (self.get_epoch_year(), self.get_epoch_day(), self.get_mean_motion_dot())
        out3 = ('Mean Motion Double Dot=0.%s, BSTAR=0.%s, Element Number=%s\n') % \
               (self.get_mean_motion_doubledot(), self.get_bstar(), self.get_element_number())
        out4 = ('Inclination=%s, RAAN=%s, Eccentricity=0.%s\n') % \
               (self.get_inclination(), self.get_raan(), self.get_eccentricity())
        out5 = ('Argument of Perigee=%s, Mean Anomaly=%s\n') % \
               (self.get_arg_perigee(), self.get_mean_anomaly())
        out6 = ('Mean Motion=%s, Rev at Epoch=%s') % \
               (self.get_mean_motion(), self.get_rev_at_epoch())
        return out0 + out1 + out2 + out3 + out4 + out5 + out6

    # Private method to fetch the TLE
    def __get_tle(self):
        """Method to retrieve the TLE for the initialized satellite number, usually from Celestrak (could be from a hardwired file).  No return value."""
        if (self.__tle_file is not None):
            lines = input(self.__tle_file)
        else:
            try:
                lines = urlopen(self.__tle_url)
            except:
                default_file = "C:\Users\msuder\Desktop\cubesat.txt"
                lines = input(default_file)

        last_name = ""
        for line in lines:
            if "1 %s" % self.__satellite_number in line:
                self.__line1 = line[0:69]
                if (self.__satellite_name is None):
                    self.__satellite_name = last_name
            if "2 %s" % self.__satellite_number in line:
                self.__line2 = line[0:69]
            if ("1 " != line[0:2]) and ("2 " != line[0:2]):
                last_name = line.rstrip()
            else:
                last_name = None
                
        if ((self.__line1 is None) or (self.__line2 is None)):
            if (self.__tle_file is not None):
                raise SatelliteTleException('Could not find TLE for satellite %s in file %s' % \
                    (self.__satellite_number, self.__tle_file))
            else:
                raise SatelliteTleException('Could not find TLE for satellite %s at URL %s' % \
                    (self.__satellite_number, self.__tle_url))
        else:
            self.__orbit = Orbital(str(self.__satellite_number), line1=self.__line1, \
                line2=self.__line2)

    def __time_to_naiveUTC(self, in_time):
        """Private method to convert (if necessary) a time (potentially with timezone) to a naive time that is UTC."""
        # NOTE:  pyorbital EXPECTS naive date/times and interprets them
        # as UTC... so we need to satisfy it; however, we are making this
        # module DATETIME AWARE, so RETURN VALUES are DATETIME AWARE!!
        # Also, all naive inputs are assumed to be UTC and all aware inputs
        # are converted to UTC (and then made naive)
        if (in_time.tzinfo is not None):
            temp = in_time.astimezone(UTC)
            time = datetime(temp.year, temp.month, temp.day, \
                                  temp.hour, temp.minute, temp.second)
        else:
            time = in_time
        return time

        
    # Class member constants
    __onesecond = timedelta(seconds=1)
    __oneminute = timedelta(minutes=1)
    __sunradius = 695700 # km, https://www.iau.org/static/resolutions/IAU2015_English.pdf
    __earthradius = 6378.137 # km, http://earth-info.nga.mil/GandG/publications/tr8350.2/wgs84fin.pdf
    __astronomical_unit = 149597870.700 # km, https://www.iau.org/static/resolutions/IAU2012_English.pdf
    __dp_over_dsplusdp = 2 * __earthradius / (2 * __sunradius + 2 * __earthradius)
    InSun = 0
    InPenumbra = 1
    InUmbra = 2
