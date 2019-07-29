#!/usr/bin/env python

from satellite_tle import SatelliteTle
from datetime import datetime
from pytz import timezone

###############################################################################
# Script to use the satellite_tle module to compute
# a table of ephemeris points for verification against
# "Revisiting Spacetrack Report #3: Rev 2", AIAA 2006-6753-Rev2,
# Vallado, Crawford, Hujsak, Kelso.
# Get sgp4-ver.tle from https://celestrak.com/software/vallado-sw.asp
###############################################################################

def main():
    """Main function... makes 'forward declarations' of helper functions unnecessary"""
    # Constants
    satnum = "06251" # Delta 1 Deb

    # Times we need
    start = datetime(2006, 06, 25, 19, 46, 43, 980096)
    end = datetime(2006, 06, 26, 19, 46, 43, 980096)
    
    st = SatelliteTle(satnum, tle_file = "../config/sgp4-ver.tle")
    table = st.compute_ephemeris_table(start, end, 7200)
    print_ephemeris_table(st, table)

def print_ephemeris_table(st, table):
    print "Satellite Number: %s" % st.get_satellite_number()
    print "Current Firefly TLE:"
    print st
    print "Time: X/Y/Z in km, VX/VY/VZ in km/s, ECI Coordinates"
    print ""
    for i in range(0, len(table)):
        print "%s: %16.8f/%16.8f/%16.8f %13.9f/%13.9f/%13.9f" % \
              (table[i][0], table[i][1][0], table[i][1][1], table[i][1][2], \
               table[i][2][0], table[i][2][1], table[i][2][2]) 

# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()
