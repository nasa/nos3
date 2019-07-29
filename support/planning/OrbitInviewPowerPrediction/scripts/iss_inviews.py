#!/usr/bin/env python

from satellite_tle import SatelliteTle
from inview_calculator import InviewCalculator
from datetime import datetime, timedelta
from pytz import timezone
from satellite_tle import SatelliteTle
from ground_station import GroundStation

###############################################################################
# Script to use the inview_calculator module to compute
# inviews for ISS
###############################################################################

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    # Constants
    groundstation_name = 'Wallops Antenna'
    groundstation_address = 'Radar Road, Temperanceville, VA  23442'
    satnum = 25544 # ISS = 25544
    saturl="http://www.celestrak.com/NORAD/elements/stations.txt"
    gs_minimum_elevation_angle = 10.0

    # Alternate constants
    gs_alt_lat = 37.854886 # Only needed if address not found
    gs_alt_lon = -75.512936 # Ditto
    gs_alt_el_meters = 3.8 # Ditto
    gs_alt_tz_offset_seconds = -18000.0 # Ditto
    gs_tzname = 'US/Eastern'

    # Construct the ground station info
    try:
        # Try to use the address...
        gs = GroundStation.from_address(groundstation_address, \
                                        groundstation_name, \
                                        gs_minimum_elevation_angle)
    except:
        # Otherwise, use explicit location data...
        gs = GroundStation.from_location(gs_alt_lat, gs_alt_lon, \
                                         gs_alt_el_meters, \
                                         gs_tzname, \
                                         groundstation_name, \
                                         gs_minimum_elevation_angle)

    # Times we need
    now = datetime.now()
    gs_today = gs.get_tz().localize(datetime(now.year, now.month, now.day))
    gs_today_start = gs.get_tz().localize(datetime(now.year, now.month, now.day, \
                                              0, 0, 0))    
    gs_today_end = gs.get_tz().localize(datetime(now.year, now.month, now.day, \
                                            23, 59, 59))

    # Get the InviewCalculator and compute the inviews
    st = SatelliteTle(satnum, tle_url=saturl)
    ic = InviewCalculator(gs, st)
    inviews = ic.compute_inviews(gs_today_start, gs_today_end)

    # Print the results
    print_satellite_header(st)
    print_inview_header(gs.get_minimum_elevation_angle(), gs_today, gs)
    print_inviews(gs, inviews)
    print_azeltables(inviews, ic)

# Convenience print functions
def print_satellite_header(st):
    """Function to print a header with satellite info for the satellite number"""
    # Retrieve TLE data
    print "Satellite Number/Launch Year/Launch Number of Year: %s/20%s/%s" % \
          (st.get_satellite_number(), st.get_launch_year(), \
           st.get_launch_year_number())
    year = 2000 + int(st.get_epoch_year())
    fracyear = timedelta(float(st.get_epoch_day()))
    time = datetime(year, 1, 1) + fracyear - timedelta(1)
    print "Epoch Date Time/Rev At Epoch: %s/%s" % \
          (time, st.get_rev_at_epoch())
    print "Inclination/Eccentricity/Average Revs Per Day: %s/0.%s/%s" % \
          (st.get_inclination(), st.get_eccentricity(), st.get_mean_motion())
    print ""

def print_inview_header(minimum_elevation_angle, now, gs):
    """Function to print a header for the inview info"""
    print "Inviews (above %s degrees) on %s-%s-%s" % \
          (minimum_elevation_angle, now.year, now.month, now.day)
    print "At %s:  Lat/Lon/El: %s/%s/%s" % \
          (gs.get_name(), gs.get_latitude(), gs.get_longitude(),
           gs.get_elevation_in_meters())
    print "where local time is UTC%+s hours" % \
          (gs.get_utcoffset_hours_ondate(now.year, now.month, now.day))
    print "  Rise   (UTC)  Set     ( Duration  )    Rise  (UTC%+s) Set" % \
          (gs.get_utcoffset_hours_ondate(now.year, now.month, now.day))

def print_inviews(gs, inviews):
    """Function to print the inviews"""
    #print "Number of inviews from %s to %s:  %d" % \
    #      (today_start.isoformat(), today_end.isoformat(),len(inviews))

    for i in range(0, len(inviews)):
        #print "%s to %s" % (inviews[i][0].isoformat(), inviews[i][1].isoformat())
        print_inview(inviews[i][0], inviews[i][1], gs)

def print_azeltables(inviews, ic):
    """Function to print a table of time, azimuth, elevation for each inview"""
    for i in range(0, len(inviews)):
        print " "
        print "Az/El for inview %s to %s" % (inviews[i][0], inviews[i][1])
        azels = ic.compute_azels(inviews[i][0], inviews[i][1], 15)
        for j in range(0, len(azels)):
            print "At %s, azimuth=%8.2f, elevation=%8.2f" % \
                  (azels[j][0], azels[j][1], azels[j][2])

def print_inview(rise, set, gs):
    """Function to print a single inview"""
    riselocal = rise + gs.get_utcoffset_ondate(rise.year, rise.month, rise.day)
    setlocal = set + gs.get_utcoffset_ondate(set.year, set.month, set.day)
    delta = set - rise
    print "%2d:%02d:%02d  to  %2d:%02d:%02d  (%3d seconds)  %2d:%02d:%02d  to  %2d:%02d:%02d" % \
        (rise.hour, rise.minute, rise.second, set.hour, set.minute, set.second, delta.seconds,
         riselocal.hour, riselocal.minute, riselocal.second, setlocal.hour, setlocal.minute, setlocal.second)
    return

# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()
