#!/usr/bin/env python
import sys
import json
import tempfile
from pytz import timezone
from configuration import Configuration
from satellite_tle import SatelliteTle
from ground_station import GroundStation
from satellite_html_report_generator import SatelliteHtmlReportGenerator
from ground_station_html_report_generator import GroundStationHtmlReportGenerator
from angular_separation_report import AngularSeparationReportGenerator
from az_el_range_report import  AzElRangeReportGenerator
from satellite_overflight_report_generator import SatelliteOverflightReportGenerator
from inview_list_report_generator import InviewListReportGenerator

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    conf_file = "config/sat_html_report.config"
    if (len(sys.argv) > 1):
        conf_file = sys.argv[1]
    #sys.stderr.write("conf_file: %s\n" % conf_file)
    
    with open(conf_file) as json_data_file:
        data = json.load(json_data_file)

    base_output_dir = Configuration.get_config_directory(data.get('base_output_directory',tempfile.gettempdir()))
    tz = Configuration.get_config_timezone(data.get('timezone','US/Eastern'))
    inviews = Configuration.get_config_boolean(data.get('inviews','false'))
    contacts = Configuration.get_config_boolean(data.get('contacts','false'))
    insun = Configuration.get_config_boolean(data.get('insun','false'))
    start_day = Configuration.get_config_int(data.get('start_day','0'), -180, 180, 0)
    end_day = Configuration.get_config_int(data.get('end_day','0'), -180, 180, 0)
    time_step_seconds = Configuration.get_config_float(data.get('time_step_seconds','15'), 1, 600, 15)

    if (data['report_type'] == "Satellite HTML"):
        create_satellite_html_report(base_output_dir, tz, inviews, contacts, insun, start_day, end_day, time_step_seconds, data)
    elif (data['report_type'] == "Ground Station HTML"):
        create_ground_station_html_report(base_output_dir, tz, inviews, start_day, end_day, data)
    elif (data['report_type'] == "Angular Separation CSV"):
        create_angular_separation_report(base_output_dir, tz, start_day, end_day, data)
    elif (data['report_type'] == "AzEl Tabular Text"):
        create_az_el_range_report(base_output_dir, tz, start_day, end_day, data)
    elif (data['report_type'] == "Satellite Overflight"):
        create_satellite_overflight_report(base_output_dir, tz, data)
    elif (data['report_type'] == "Inview List"):
        create_inview_list_report(base_output_dir, tz, start_day, end_day, data)
    else:
        sys.stderr.write("Unsupported report type:  %s.\n" % data['report_type'])
 
def create_satellite_html_report(base_output_dir, tz, inviews, contacts, insun, start_day, end_day, time_step_seconds, data):
    
    sat_tle = SatelliteTle.from_config(data.get('satellite',[]))
        
    ground_station_list = []
    gs_list = data.get('ground_stations',[])
    for gs in gs_list:
        ground_station_list.append(GroundStation.from_config(gs))
        
    aer_days = Configuration.get_config_int(data.get('num_aer_days','0'),0,10,0)

    shrg = SatelliteHtmlReportGenerator(base_output_dir, sat_tle, ground_station_list, tz, inviews, contacts, insun, aer_days, \
            start_day, end_day, time_step_seconds)
    shrg.generate_report()
    
def create_inview_list_report(base_output_dir, tz, start_day, end_day, data):
    sat_tle = SatelliteTle.from_config(data.get('satellite',[]))
    ground_station = GroundStation.from_config(data.get('ground_station',[]))
    ilrg = InviewListReportGenerator(base_output_dir, sat_tle, ground_station, tz, start_day, end_day)
    ilrg.generate_report()

def create_satellite_overflight_report(base_output_dir, tz, data):
    sat_tle = SatelliteTle.from_config(data.get('satellite',[]))
    ground_station = GroundStation.from_config(data.get('ground_station',[]))
    common_sat_name = data.get('common_satellite_name', sat_tle.get_satellite_name())
    common_gs_name = data.get('common_ground_station_name', ground_station.get_name())
    sorg = SatelliteOverflightReportGenerator(base_output_dir, sat_tle, ground_station, tz, common_sat_name, common_gs_name)
    sorg.generate_report()

def create_ground_station_html_report(base_output_dir, tz, inviews, start_day, end_day, data):
    ground_station = GroundStation.from_config(data.get('ground_station',[]))
    
    sat_list = []
    sat_data_list = data.get('satellites',[])
    for sat in sat_data_list:
        sat_list.append(SatelliteTle.from_config(sat))

    aer_days = Configuration.get_config_int(data.get('num_aer_days','0'),0,10,0)

    gshrg = GroundStationHtmlReportGenerator(base_output_dir, ground_station, sat_list, tz, inviews, aer_days, start_day, end_day)
    gshrg.generate_report()

def create_az_el_range_report(base_output_dir, tz, start_day, end_day, data):
    ground_station = GroundStation.from_config(data.get('ground_station',[]))
    sat_of_interest = SatelliteTle.from_config(data.get('satellite',[]))
    # sys.stderr.write(sat_of_interest.__repr__() + "\n") # debug
    time_step_seconds = Configuration.get_config_float(data.get('time_step_seconds','15'), 1, 600, 15)
    aerg = AzElRangeReportGenerator(base_output_dir, ground_station, 0, sat_of_interest, tz, start_day, end_day, time_step_seconds)
    aerg.generate_report()

def create_angular_separation_report(base_output_dir, tz, start_day, end_day, data):
    ground_station = GroundStation.from_config(data.get('ground_station',[]))
    sat_of_interest = SatelliteTle.from_config(data.get('satellite_of_interest',[]))
    
    other_sats = []
    other_sats_data = data.get('other_satellites',[])
    for sat in other_sats_data:
        other_sats.append(SatelliteTle.from_config(sat))

    asrg = AngularSeparationReportGenerator(base_output_dir, ground_station, sat_of_interest, other_sats, tz, start_day, end_day)
    asrg.generate_report()

# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()
