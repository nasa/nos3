#!/usr/bin/env python

import math
from datetime import datetime, timedelta
from pytz import timezone, utc
from ephem import Observer, Sun
from satellite_tle import SatelliteTle
from ground_station import GroundStation
from inview_calculator import InviewCalculator
from astropy.time import Time

###############################################################################

# Brightness from www.n2yo.com
SATS =[
[25544,-0.5],
[20580,3.0],
[19120,3.5],
[19650,3.5],
[20625,3.5],
[22220,3.5],
[22285,3.5],
[22566,3.5],
[22803,3.5],
[23088,3.5],
[23343,3.5],
[23405,3.5],
[23705,3.5],
[24298,3.5],
[25400,3.5],
[25407,3.5],
[25861,3.5],
[10967,3.5],
[16182,3.5],
[17590,3.5],
[10967,4.0],
[16182,4.0],
[17590,4.0],
[19210,4.5],
[21610,4.5],
[23560,4.5],
[23561,4.5],
[25860,4.5],
[27386,4.5],
[27422,4.5],
]
a = [
]

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    [gs, gs_observer, gs_tz, gs_start, gs_end] = init_groundstation()
    [civil_night, nautical_night] = determine_nighttime(gs_observer)

    print("<html><head><title>Visible Satellites Today</title></head>")
    print("<body>")
    print("<a href=http://www.cleardarksky.com/c/FairmontWVkey.html?date=%s>" % gs_start.isoformat())
    print("<img src=\"http://www.cleardarksky.com/c/FairmontWVcsk.gif?c=554409&date=%s\"></a>" % gs_start.isoformat())
    print("<hr>\nTimes when it is night at the ground station and a satellite is inview and in the sun:")
    print("<hr>Civil night: ")
    print_span(gs_tz, civil_night)
    print(", nautical night: ")
    print_span(gs_tz, nautical_night)
    visibilities = []
    for sat in SATS:
        visibilities.extend(determine_satellite_visibility(sat, gs, gs_tz, gs_start, gs_end, civil_night))

    visibilities.sort(key=lambda entry : entry[1][0])
    print_timespans(gs_tz, visibilities)
   
    print("</body></html>")

def determine_satellite_visibility(sat, gs, gs_tz, gs_start, gs_end, night):
    satnum = sat[0]
    saturl = "http://www.celestrak.com/cgi-bin/TLE.pl?CATNR=%s" % satnum
    st = SatelliteTle(satnum, tle_url=saturl)

    [suntimes, pentimes, umtimes] = st.compute_sun_times(gs_start, gs_end)

    ic = InviewCalculator(gs, st)
    inviews = ic.compute_inviews(gs_start, gs_end)

    intersections = []
    for inview in inviews:
        i1 = intersect_times(night, inview)
        if (i1 is not None):
            #print_span(gs_tz, i1)
            for suntime in suntimes:
                i2 = intersect_times(i1, suntime)
                if (i2 is not None):
                    #print_span(gs_tz, i2)
                    i2startmjd = Time(i2[0]).mjd
                    href = "http://www.heavens-above.com/passdetails.aspx?lat=%f&lng=%f&loc=%s&alt=%f&tz=%s&satid=%d&mjd=%f" % \
                        (gs.get_latitude(), gs.get_longitude(), gs.get_name(), gs.get_elevation_in_meters(), gs.get_tz().localize(datetime(i2[0].year, i2[0].month, i2[0].day)).tzname(), satnum, i2startmjd)
                    intersections.append(["<a href=%s>%s (mag:%s) %s %s-%s%s</a>" % 
                        (href, satnum, sat[1], 
                         st.get_satellite_name(), st.get_launch_year(), st.get_launch_year_number(), st.get_launch_piece()),i2])

    #print_debug_times(gs_tz, satnum, night, suntimes, inviews)
    return intersections

def print_debug_times(gs_tz, satnum, night, suntimes, inviews):
    print("<hr>\nNight time:")
    print_span(gs_tz, night)
    print("Times when %s is in the sun" % satnum)
    print_timespans(gs_tz, suntimes)
    print("<hr>\nTimes when %s is inview of the ground station" % satnum)
    print_timespans(gs_tz, inviews)

def print_timespans(gs_tz, spans):
    for span in spans:
        print("<hr>Satellite %s is visible:<br>" % span[0])
        print_span(gs_tz, span[1])

def print_span(gs_tz, span):
    print("%s --to-- %s" % (time_to_string(span[0].astimezone(gs_tz)), time_to_string(span[1].astimezone(gs_tz))))

def time_to_string(time):
    return "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d" %(time.year, time.month, time.day, time.hour, time.minute, time.second)

def init_groundstation():
    # Ground station stuff
    gs_lat = 39.50417 
    gs_lon = -80.218615 
    gs_el_meters = 359.664 
    gs_tzname = 'US/Eastern'
    groundstation_name = 'Fairmont'
    gs_minimum_elevation_angle = 10.0
    gs_observer = Observer()
    gs_observer.lat = gs_lat * math.pi / 180.0
    gs_observer.lon = gs_lon * math.pi / 180.0
    gs_observer.elevation = gs_el_meters

    # Times we need
    now = datetime.now()
    tomorrow = now + timedelta(1)
    gs_observer.date = now
    #now += timedelta(hours=14) # testing
    #tomorrow = now + timedelta(hours=4) # testing
    gs_tz = timezone(gs_tzname)
    gs_start = gs_tz.localize(now)
    gs_end = gs_tz.localize(tomorrow)
    #print(gs_start)
    #print(gs_end)

    gs = GroundStation.from_location(gs_lat, gs_lon, \
                                     gs_el_meters, \
                                     gs_tzname, \
                                     groundstation_name, \
                                     gs_minimum_elevation_angle)

    return [gs, gs_observer,gs_tz, gs_start, gs_end]

def determine_nighttime(gs_observer):
    sun = Sun()
    gs_observer.horizon = '-6' # Civil twilight, stars should start appearing rapidly
    next_set = utc.localize(gs_observer.next_setting(sun, use_center=True).datetime())
    following_rise = utc.localize(gs_observer.next_rising(sun, use_center=True, start=next_set).datetime())
    civil_night = [next_set, following_rise]
    #print(civil_night)
    gs_observer.horizon = '-12' # Nautical twilight, it's dark
    next_set = utc.localize(gs_observer.next_setting(sun, use_center=True).datetime())
    following_rise = utc.localize(gs_observer.next_rising(sun, use_center=True, start=next_set).datetime())
    nautical_night = [next_set, following_rise]
    #print(nautical_night)
    return [civil_night, nautical_night]

def intersect_times(first, second):
    intersection = [max(first[0], second[0]), min(first[1], second[1])]
    if (intersection[0] < intersection[1]):
        return intersection
    else:
        return None

# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()

