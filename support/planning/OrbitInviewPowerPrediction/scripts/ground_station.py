import geocoder
from datetime import datetime, timedelta
from pytz import timezone
from configuration import Configuration
from ground_station_schedule_directory import GroundStationScheduleDirectory

###############################################################################
# Python module to make it easy to work with a ground station, its location,
# and its time zone, determined either by specific lat, lon, elev, tz, etc.
# or determined using the Google geocoder
###############################################################################

class GroundStation:
    """Class to represent the location and timezone of a particular ground station """
    # Constructor
    def __init__(self, name="", address="", lat="", lon="", el_meters="", \
                 tz=None, wx_url=None, other_info=None, minimum_elevation_angle=0.0, \
                 show_operations_hours=True, \
                 operations_start_hour=8, operations_start_minute=0, operations_end_hour=16, operations_end_minute=30, \
                 aer_min_el=0, aer_keyhole_el=90, good_sectors=[], bad_sectors=[], contact_schedule_directory=None):
        self.__name = name
        self.__address = address
        self.__latitude = lat
        self.__longitude = lon
        self.__elevation_in_meters = el_meters
        self.__tz = tz
        self.__wx_url = wx_url
        self.__other_info = other_info
        self.__show_operations_hours = show_operations_hours
        self.__minimum_elevation_angle = minimum_elevation_angle
        self.__operations_start_hour = operations_start_hour
        self.__operations_start_minute = operations_start_minute
        self.__operations_end_hour = operations_end_hour
        self.__operations_end_minute = operations_end_minute
        self.__aer_min_el = aer_min_el
        self.__aer_keyhole_el = aer_keyhole_el
        self.__good_sectors = good_sectors
        self.__bad_sectors = bad_sectors
        if (contact_schedule_directory is None):
            self.__schedule_directory = GroundStationScheduleDirectory()
        else:
            self.__schedule_directory = GroundStationScheduleDirectory(contact_schedule_directory)
        
    # Class Methods to construct in alternative ways
    @classmethod
    def from_config(cls, gs_data):
        if (gs_data.get('predefined','').lower() == 'wallops'):
                gs = GroundStation.create_wallops()
        elif (gs_data.get('predefined','').lower() == 'morehead'):
                gs = GroundStation.create_morehead()
        elif (gs_data.get('predefined','').lower() == 'sri_paloalto'):
                gs = GroundStation.create_sri_paloalto()
        else:
            name = gs_data.get('name','')
            lat = Configuration.get_config_float(gs_data.get('lat','0'), -90, 90, 0)
            lon = Configuration.get_config_float(gs_data.get('lon','0'), -180, 180, 0)
            el_meters = Configuration.get_config_float(gs_data.get('el_meters','0'), -1000, 10000, 0)
            tz = Configuration.get_config_timezone('tz')
            wx_url = gs_data.get('weather_url', None)
            other_info = gs_data.get('other_info', None)
            minimum_elevation_angle = Configuration.get_config_float(gs_data.get('minimum_elevation_angle','0'), 0, 90, 0)
            show_operations_hours = Configuration.get_config_boolean(gs_data.get('show_operations_hours', 'true'))
            operations_start_hour = Configuration.get_config_int(gs_data.get('operations_start_hour','8'), 0, 23, 8)
            operations_start_minute = Configuration.get_config_int(gs_data.get('operations_start_minute','0'), 0, 59, 0)
            operations_end_hour = Configuration.get_config_int(gs_data.get('operations_end_hour','16'), 0, 23, 16)
            operations_end_minute = Configuration.get_config_int(gs_data.get('operations_end_minute','30'), 0, 59, 30)
            aer_min_el = Configuration.get_config_float(gs_data.get('aer_min_el','0'),0,90,0)
            aer_keyhole_el = Configuration.get_config_float(gs_data.get('aer_keyhole_el','90'),0,90,90)
            good_sectors = Configuration.get_config_sectors(gs_data.get('good_sectors',[]))
            bad_sectors = Configuration.get_config_sectors(gs_data.get('bad_sectors',[]))
            contact_schedule_directory = gs_data.get('contact_schedule_directory', None)
            gs = cls(name, '', lat, lon, el_meters, tz, wx_url, other_info, \
                    minimum_elevation_angle, show_operations_hours, operations_start_hour, \
                    operations_start_minute, operations_end_hour, operations_end_minute, \
                    aer_min_el, aer_keyhole_el, good_sectors, bad_sectors, contact_schedule_directory)
    
        return gs
     
    @classmethod
    def from_address(cls, address, name="", \
        minimum_elevation_angle=0.0, show_operations_hours=True, operations_start_hour=8, operations_start_minute=0, operations_end_hour=16, operations_end_minute=30):
        gc = geocoder.google(address)
        [lon, lat] = gc.geometry['coordinates']
        el = geocoder.google([lat, lon], method='elevation')
        el_meters = el.meters
        gtz = geocoder.google([lat, lon], method='timezone')
        tz_offset_seconds = gtz.dstOffset + gtz.rawOffset
        tz = timezone(gtz.timeZoneId)
        obj = cls(name, address, lat, lon, el_meters, tz, minimum_elevation_angle, \
                  show_operations_hours, operations_start_hour, operations_start_minute, operations_end_hour, operations_end_minute)
        return obj
    
    @classmethod
    def from_location(cls, lat, lon, el_meters, tzname, name="", minimum_elevation_angle=0.0, \
                      show_operations_hours=True, operations_start_hour=8, operations_start_minute=0, operations_end_hour=16, operations_end_minute=30):
        tz = timezone(tzname)
        obj = cls(name, "", lat, lon, el_meters, tz, minimum_elevation_angle, \
                  show_operations_hours, operations_start_hour, operations_start_minute, operations_end_hour, operations_end_minute)
        return obj

    # And a few static factory methods for specific locations
    @staticmethod
    def create_wallops():
        groundstation_name = 'Wallops Antenna'
        groundstation_address = 'Radar Road, Temperanceville, VA  23442'
        gs_minimum_elevation_angle = 0.0
        gs_show_operations_hours = True
        gs_operations_start_hour = 8
        gs_operations_start_minute = 30
        gs_operations_end_hour = 23
        gs_operations_end_minute = 30

        # Alternate constants
        gs_alt_lat = 37.854886 # Only needed if address not found
        gs_alt_lon = -75.512936 # Ditto
        gs_alt_el_meters = 3.8 # Ditto
        gs_alt_tzname = 'US/Eastern' # Ditto

        # Otherwise, use explicit location data...
        gs = GroundStation.from_location(gs_alt_lat, gs_alt_lon, \
                                         gs_alt_el_meters, \
                                         gs_alt_tzname, \
                                         groundstation_name, \
                                         gs_minimum_elevation_angle, \
                                         gs_show_operations_hours, gs_operations_start_hour, gs_operations_start_minute, gs_operations_end_hour, gs_operations_end_minute)
        return gs
    
    @staticmethod
    def create_morehead():
        groundstation_name = 'Morehead Antenna'
        gs_minimum_elevation_angle = 10.0
        gs_alt_lat = 38.191834
        gs_alt_lon = -83.438841
        gs_alt_el_meters = 353
        gs_alt_tzname = 'US/Eastern'

        # Use explicit location data...
        gs = GroundStation.from_location(gs_alt_lat, gs_alt_lon, \
                                         gs_alt_el_meters, \
                                         gs_alt_tzname, \
                                         groundstation_name, \
                                         gs_minimum_elevation_angle)
        return gs    

    @staticmethod
    def create_sri_paloalto():
        groundstation_name = 'SRI Palo Alto Antenna'
        gs_minimum_elevation_angle = 10.0
        gs_alt_lat = 37.40303
        gs_alt_lon = -122.17423
        gs_alt_el_meters = 156.47
        gs_alt_tzname = 'US/Pacific'

        # Use explicit location data...
        gs = GroundStation.from_location(gs_alt_lat, gs_alt_lon, \
                                         gs_alt_el_meters, \
                                         gs_alt_tzname, \
                                         groundstation_name, \
                                         gs_minimum_elevation_angle)
        return gs
    
    def get_name(self):
        return self.__name

    def get_address(self):
        return self.__address

    def get_latitude(self):
        return self.__latitude

    def get_longitude(self):
        return self.__longitude

    def get_elevation_in_meters(self):
        return self.__elevation_in_meters

    def get_tz(self):
        return self.__tz
    
    def get_wx_url(self):
        return self.__wx_url
    
    def get_other_info(self):
        return self.__other_info
    
    def get_utcoffset_ondate(self, year, month, day):
        date = datetime(year, month, day)
        return self.__tz.utcoffset(date)

    def get_utcoffset_hours_ondate(self, year, month, day):
        td = self.get_utcoffset_ondate(year, month, day)
        return (td.days * 86400 + td.seconds) / 3600

    def get_minimum_elevation_angle(self):
        return self.__minimum_elevation_angle

    def get_show_operations_hours(self):
        return self.__show_operations_hours

    def get_operations_start_hour(self):
        return self.__operations_start_hour

    def get_operations_start_minute(self):
        return self.__operations_start_minute

    def get_operations_end_hour(self):
        return self.__operations_end_hour

    def get_operations_end_minute(self):
        return self.__operations_end_minute

    def get_aer_min_el(self):
        return self.__aer_min_el

    def get_aer_keyhole_el(self):
        return self.__aer_keyhole_el

    def get_good_sectors(self):
        return self.__good_sectors

    def get_bad_sectors(self):
        return self.__bad_sectors

    def get_schedule_directory(self):
        return self.__schedule_directory

    # Other methods
    def __repr__(self):
        """Returns a string representing an instance of this class."""
        out1 = ('Name: %s Address: %s Timezone: %s\n') % (self.__name, self.__address, self.__tz)
        out2 = ('Lat: %s Lon: %s El(m): %s\n') % (self.__latitude, self.__longitude, self.__elevation_in_meters)
        out3 = ('Min El: %s Operating times: %2.2d:%2.2d to %2.2d:%2.2d, Show times: %s\n') % \
            (self.__minimum_elevation_angle, \
             self.__operations_start_hour, self.__operations_start_minute, self.__operations_end_hour, self.__operations_end_minute, self.__show_operations_hours)
        return out1 + out2 + out3
