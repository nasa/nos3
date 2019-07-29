#!/usr/bin/env python

from satellite_tle import SatelliteTle
from datetime import datetime
from pytz import timezone

###############################################################################
# Script to use the satellite_tle module to compute
# a table of sun entrance/exit times for (local) today for ISS
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
    our_today_start = our_tz.localize(datetime(now.year, now.month, now.day, \
                                               0, 0, 0))
    our_today_end = our_tz.localize(datetime(now.year, now.month, now.day, \
                                             23, 59, 59))

    st = SatelliteTle(satnum, tle_url=saturl)
    tables = st.compute_sun_times(our_today_start, our_today_end)
    print_sun_times_table(st, our_today_start, our_today_end, tables)

def print_sun_times_table(st, our_today_start, our_today_end, tables):
    print "Satellite Number: %s" % st.get_satellite_number()
    print "Current ISS TLE:"
    print st
    print ""
    table = tables[0]
    print "In sun times from %s to %s" % (our_today_start, our_today_end)
    print "        Enter Sun                Exit Sun           (Len )"
    for i in range(0, len(table)):
        delta = table[i][1] - table[i][0]
        print "%s %s (%s)" % (table[i][0].isoformat(), table[i][1].isoformat(), \
                              delta.seconds)
    table = tables[1]
    print ""
    print "In penumbra times from %s to %s" % (our_today_start, our_today_end)
    print "        Enter Penumbra           Exit Penumbra       (Len )"
    for i in range(0, len(table)):
        delta = table[i][1] - table[i][0]
        print "%s %s (%s)" % (table[i][0].isoformat(), table[i][1].isoformat(), \
                              delta.seconds)
    table = tables[2]
    print ""
    print "In umbra times from %s to %s" % (our_today_start, our_today_end)
    print "        Enter Umbra              Exit Umbra         (Len )"
    for i in range(0, len(table)):
        delta = table[i][1] - table[i][0]
        print "%s %s (%s)" % (table[i][0].isoformat(), table[i][1].isoformat(), \
                              delta.seconds)

# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()
