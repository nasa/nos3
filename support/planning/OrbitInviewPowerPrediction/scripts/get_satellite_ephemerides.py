#!/usr/bin/env python

import argparse
from argvalidator import ArgValidator
import math
from satellite_tle import SatelliteTle
from ground_station import GroundStation
from inview_calculator import InviewCalculator
from datetime import datetime
from pytz import timezone
from pyorbital.orbital import astronomy
import geomag
import aacgmv2

###############################################################################
# Script to use the satellite_tle module to compute
# an ephemeris point at a time (default is now) for a satellite number
# (default is 43852, which is STF-1)
###############################################################################

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--satnum", help="Specify satellite number (e.g. 25544=ISS, 43852=STF-1)", \
            type=int, metavar="[1-99999]", choices=range(1,99999), default=43852)
    parser.add_argument("-t", "--time", help="Specify date/time (UTC)", type=ArgValidator.validate_datetime, metavar="YYYY-MM-DDTHH:MM:SS.s", default=datetime.now())
    parser.add_argument("-r", "--endtime", help="Specify date time range with this end date/time (UTC)", \
            type=ArgValidator.validate_datetime, metavar="YYYY-MM-DDTHH:MM:SS.s", default=None)
    parser.add_argument("-d", "--timestep", help="Specify time step (delta) in seconds for tabular data", \
            type=int, metavar="[1-9999]", choices=range(1,3600), default=60)
    parser.add_argument("-p", "--tle", help="Print TLE information", action="store_true")
    parser.add_argument("-u", "--sun", help="Print sun/umbra/penumbra information", action="store_true")
    parser.add_argument("-e", "--ecef", help="Print ECEF ephemeris", action="store_true")
    parser.add_argument("-i", "--eci", help="Print ECI ephemeris", action="store_true")
    parser.add_argument("-l", "--lla", help="Print latitude, longitude, altitude (geodetic degrees, altitude in km above WGS-84 ellipsoid) ephemeris", action="store_true")
    parser.add_argument("-f", "--file", help="TLE file to use (instead of looking up the TLE on CelesTrak)", type=ArgValidator.validate_file, default=None)
    parser.add_argument("-m", "--mag", help="Print geomagnetic data", action="store_true")
    parser.add_argument("-a", "--aer", help="Print az/el/range from ground station", action="store_true")
    parser.add_argument("-x", "--longitude", help="Specify longitude (degrees) of ground station", \
            type=float, default=-79.825518056)
    parser.add_argument("-y", "--latitude", help="Specify latitude (degrees) of ground station", \
            type=float, default=38.43685028)
    parser.add_argument("-z", "--elevation", help="Specify elevation (meters) of ground station", \
            type=float, default=842.0)
    args = parser.parse_args()

    if (args.file is not None):
        st = SatelliteTle(args.satnum, tle_file=args.file)
    else:
        saturl = "http://www.celestrak.com/cgi-bin/TLE.pl?CATNR=%s" % args.satnum
        st = SatelliteTle(args.satnum, tle_url=saturl)

    if (args.tle):
        print_tle_data(st)

    if (args.eci):
        if (args.endtime is None):
            point = st.compute_ephemeris_point(args.time)
            print_ephemeris_point(st, point)
        else:
            table = st.compute_ephemeris_table(args.time, args.endtime, args.timestep)
            print_ephemeris_table(st, table)

    if (args.ecef):
        #print "ECEF is not implemented"
        if (args.endtime is None):
            point = st.compute_ephemeris_point(args.time)
            print_ephemeris_point(st, point, False)
        else:
            table = st.compute_ephemeris_table(args.time, args.endtime, args.timestep)
            print_ephemeris_table(st, table, False)


    if (args.lla):
        if (args.endtime is None):
            llap = st.compute_lonlatalt_point(args.time)
            print_lonlatalt(st, llap)
        else:
            table = st.compute_lonlatalt_table(args.time, args.endtime, args.timestep)
            print_lonlatalt_table(st, table)

    if (args.mag):
        if (args.endtime is None):
            llap = st.compute_lonlatalt_point(args.time)
            print_mag(st, llap)
        else:
            table = st.compute_lonlatalt_table(args.time, args.endtime, args.timestep)
            print_mag_table(st, table)

    if (args.aer):
        gs = GroundStation(lat=args.latitude, lon=args.longitude, el_meters=args.elevation)
        ic = InviewCalculator(gs, st)
        if (args.endtime is None):
	    args.endtime = args.time
        azels = ic.compute_azels(args.time, args.endtime, args.timestep)
        print_azelrange_table(azels)

    if (args.sun):
        if (args.endtime is None):
            sun_state = st.get_satellite_sun_state(args.time)
            if (sun_state == st.InSun):
                in_sun = "in sun"
            elif (sun_state == st.InPenumbra):
                in_sun = "in penumbra"
            else:
                in_sun = "in umbra"
            print "===== SUN ====="
            print "Satellite is %s" % in_sun
        else:
            tables = st.compute_sun_times(args.time, args.endtime)
            print_sun_times_table(st, args.time, args.endtime, tables)

def print_tle_data(st):
    print "===== TLE ====="
    print "Column Headers:"
    print st
    print "Raw:"
    print st.raw_string()
    print "Pretty:"
    print st.pretty_string()

def print_ephemeris_point(st, point, inertial=True):
    #print point
    if (inertial):
        coords = "ECI"
    else:
        coords = "ECEF"
    print "===== %s =====" % coords

    if (not inertial):
        gmst_radians = astronomy.gmst(point[0])
    print "Date/Time: %s  Satellite Number: %s" % \
          (point[0], st.get_satellite_number())
    (x, y, z) = (point[1][0], point[1][1], point[1][2])
    if (not inertial):
        r = math.sqrt(x*x + y*y)
        x = r*math.cos(-1.0*gmst_radians)
        y = r*math.sin(-1.0*gmst_radians)
    print "Position (km,   x/y/z %s): %s/%s/%s" % (coords, x, y, z)
    (x, y, z) = (point[2][0], point[2][1], point[2][2])
    if (not inertial):
        r = math.sqrt(x*x + y*y)
        x = r*math.cos(-1.0*gmst_radians)
        y = r*math.sin(-1.0*gmst_radians)
    print "Velocity (km/s, x/y/z %s): %s/%s/%s" % (coords, x, y, z)

def print_lonlatalt(st, llap):
    print "===== LLA ====="
    print "Date/Time: %s  Satellite Number: %s" % \
          (llap[0], st.get_satellite_number())
    print "Position (lat/lon/alt in geodetic degrees and km above WGS-84 ellipsoid): %s/%s/%s" % \
          (llap[2], llap[1], llap[3])

KM_TO_FEET=3280.84
def print_mag(st, llap):
    print "===== MAG ====="
    print "Date/Time: %s  Satellite Number: %s" % \
          (llap[0], st.get_satellite_number())
    d = datetime.date(llap[0])
    gm = geomag.geomag.GeoMag()
    mag = gm.GeoMag(llap[2], llap[1], llap[3]*KM_TO_FEET, d)
    aacgm = aacgmv2.get_aacgm_coord(llap[2], llap[1], llap[3], llap[0])
    print "Geodetic Latitude (degrees)/Geodetic Longitude (degrees)/Altitude (km above WGS-84 ellipsoid)/Declination (degrees)/Inclination (degrees)/Total Intensity (nT)/Horizontal (nT)/North (nT)/East (nT)/Vertical (nT)/Magnetic Latitude (degrees)/Magnetic Longitude(degrees)/Magnetic Local Time (hours):  %s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s" % \
    	(llap[2], llap[1], llap[3], mag.dec, mag.dip, mag.ti, mag.bh, mag.bx, mag.by, mag.bz, aacgm[0], aacgm[1], aacgm[2])

def print_ephemeris_table(st, table, inertial=True):
    if (inertial):
        coords = "ECI"
    else:
        coords = "ECEF"
    print "===== %s =====" % coords

    print "Time, X,Y,Z in km, VX,VY,VZ in km/s (%s Coordinates)" % coords
    for i in range(0, len(table)):
        ( x,  y,  z) = (table[i][1][0], table[i][1][1], table[i][1][2])
        (vx, vy, vz) = (table[i][2][0], table[i][2][1], table[i][2][2])
        if (not inertial):
            gmst_radians = astronomy.gmst(table[i][0])
            r = math.sqrt(x*x + y*y)
            x = r*math.cos(-1.0*gmst_radians)
            y = r*math.sin(-1.0*gmst_radians)
            vr = math.sqrt(vx*vx + vy*vy)
            vx = r*math.cos(-1.0*gmst_radians)
            vy = r*math.sin(-1.0*gmst_radians)
        print "%s, %16.8f,%16.8f,%16.8f, %13.9f,%13.9f,%13.9f" % \
              (table[i][0], x, y, z, vx, vy, vz)

def print_azelrange_table(table):
    print "===== AER ====="
    print "Time, azimuth (degrees), elevation (degrees), range (km)"
    for i in range(0, len(table)):
        print "%s, %6.2f,%8.2f,%9.2f" % \
              (table[i][0], table[i][2], table[i][1], table[i][3]) 

def print_lonlatalt_table(st, table):
    print "===== LLA ====="
    print "Time, lat,lon,alt (geodetic degrees, km above WGS-84 ellipsoid)"
    for i in range(0, len(table)):
        print "%s, %6.2f,%8.2f,%9.2f" % \
              (table[i][0], table[i][2], table[i][1], table[i][3]) 

def print_mag_table(st, table):
    gm = geomag.geomag.GeoMag()
    print "===== MAG ====="
    print "Time (UTC), geodetic latitude (degrees), geodetic longitude (degrees), alt (km above WGS-84 ellipsoid), magnetic declination (degrees), inclination (degrees), total intensity (nT), horizontal (nT), north (nT), east (nT), vertical (nT), magnetic latitude (degrees), magnetic longitude (degrees), magnetic local time (hours)"
    for i in range(0, len(table)):
        mag = gm.GeoMag(table[i][2], table[i][1], table[i][3]*KM_TO_FEET, datetime.date(table[i][0]))
        aacgm = aacgmv2.get_aacgm_coord(table[i][2], table[i][1], table[i][3], table[i][0])
        print "%s, %6.2f, %8.2f, %9.2f, %6.2f, %8.2f, %7.1f, %7.1f, %7.1f, %7.1f, %7.1f, %6.2f, %8.2f, %5.2f" % \
            (table[i][0], table[i][2], table[i][1], table[i][3], mag.dec, mag.dip, mag.ti, mag.bh, mag.bx, mag.by, mag.bz, aacgm[0], aacgm[1], aacgm[2])

def print_sun_times_table(st, start, end, tables):
    print "===== SUN ====="
    table = tables[0]
    print "In sun times from %s to %s" % (start, end)
    print "        Enter Sun                Exit Sun           (Len )"
    for i in range(0, len(table)):
        delta = table[i][1] - table[i][0]
        print "%s %s (%s)" % (table[i][0].isoformat(), table[i][1].isoformat(), \
                              delta.seconds)
    table = tables[1]
    print ""
    print "In penumbra times from %s to %s" % (start, end)
    print "        Enter Penumbra           Exit Penumbra       (Len )"
    for i in range(0, len(table)):
        delta = table[i][1] - table[i][0]
        print "%s %s (%s)" % (table[i][0].isoformat(), table[i][1].isoformat(), \
                              delta.seconds)
    table = tables[2]
    print ""
    print "In umbra times from %s to %s" % (start, end)
    print "        Enter Umbra              Exit Umbra         (Len )"
    for i in range(0, len(table)):
        delta = table[i][1] - table[i][0]
        print "%s %s (%s)" % (table[i][0].isoformat(), table[i][1].isoformat(), \
                              delta.seconds)

# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()
