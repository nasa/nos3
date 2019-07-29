#!/usr/bin/env python
import sys
import tempfile
from pytz import timezone

class Configuration:

    def __init__(self):
        pass
    
    @staticmethod
    def get_config_sectors(sector_list):
        sectors = []
        for sector in sector_list:
            sectors.append(Configuration.get_config_sector(sector))
        return sectors
    
    @staticmethod
    def get_config_sector(sector_data):
        sector = []
        sector.append(Configuration.get_config_float(sector_data.get('min_az','0'),0,360,0))
        sector.append(Configuration.get_config_float(sector_data.get('max_az','0'),0,360,0))
        sector.append(Configuration.get_config_float(sector_data.get('min_el','0'),0,90,0))
        sector.append(Configuration.get_config_float(sector_data.get('max_el','0'),0,90,0))
        return sector
        
    @staticmethod
    def get_config_int(input, min, max, default):
        try:
            t = int(input)
            if ((t >= min) and (t <= max)):
                output = t
            else:
                output = default
        except:
            output = default
            
        return output
    
    @staticmethod
    def get_config_float(input, min, max, default):
        try:
            t = float(input)
            if ((t >= min) and (t <= max)):
                output = t
            else:
                output = default
        except:
            output = default
            
        return output
    
    @staticmethod
    def get_config_timezone(input):
        try:
            tz = timezone(data['timezone'])
        except:
            tz = timezone('US/Eastern')
        return tz
    
    @staticmethod
    def get_config_boolean(input):
        if (input.lower() == "true"):
            return True
        else:
            return False
    
    @staticmethod
    def get_config_directory(input):
        directory = input
        try:
            #sys.stderr.write("Trying to see if directory %s is writable\n" % directory)
            with tempfile.TemporaryFile(dir = directory) as testfile:
              testfile.close
        except:
            sys.stderr.write("Error getting writable base_output_directory %s\n" % (directory))
            directory = tempfile.gettempdir()
        return directory
    
