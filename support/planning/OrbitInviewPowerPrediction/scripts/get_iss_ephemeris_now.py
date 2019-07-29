#!/usr/bin/env python

from satellite_tle import SatelliteTle
from datetime import datetime
from pytz import timezone

###############################################################################
# Script to use the satellite_tle module to compute
# the current ephemeris point for ISS
###############################################################################

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    # Constants
    satnum = 25544 # ISS = 25544
    saturl="http://www.celestrak.com/NORAD/elements/stations.txt"
    our_tzname = 'US/Eastern'

    # Times we need
    now = datetime.now()
    our_tz = timezone(our_tzname)
    our_now = our_tz.localize(datetime(now.year, now.month, now.day, \
                                       now.hour, now.minute, now.second))

    st = SatelliteTle(satnum, tle_url=saturl)
    print_tle_data(st)

    point = st.compute_ephemeris_point(our_now)
    print_ephemeris_point_now(st, point)

    llap = st.compute_lonlatalt_point(our_now)
    print_lonlatalt_now(st, llap)

    sun_state = st.get_satellite_sun_state(our_now)
    if (sun_state == st.InSun):
        in_sun = "in sun"
    elif (sun_state == st.InPenumbra):
        in_sun = "in penumbra"
    else:
        in_sun = "in umbra"
        
    print "Satellite is %s" % in_sun

def print_tle_data(st):
    print "Current ISS TLE:"
    print "Column Headers:"
    print st
    print "Raw:"
    print st.raw_string()
    print "Pretty:"
    print st.pretty_string()
    print ""

def print_ephemeris_point_now(st, point):
    #print point
    print "Date/Time: %s  Satellite Number: %s" % \
          (point[0], st.get_satellite_number())
    print "Position (km,   x/y/z ECI): %s/%s/%s" % \
          (point[1][0], point[1][1], point[1][2])
    print "Velocity (km/s, x/y/z ECI): %s/%s/%s" % \
          (point[2][0], point[2][1], point[2][2])
    print ""

def print_lonlatalt_now(st, llap):
    print "Date/Time: %s  Satellite Number: %s" % \
          (llap[0], st.get_satellite_number())
    print "Position (lat/lon/alt km): %s/%s/%s" % \
          (llap[2], llap[1], llap[3])

# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()
