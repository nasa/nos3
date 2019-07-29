#!/bin/bash
# Quick script to automatically pull TLEs for a specific list of TLE numbers.
# Credit:  Jim Lux, JPL; and the Space-Track website
# Usage:  retrieve_spacetrack_tles.sh <file to write> <Space-Track username> <Space-Track password> <beginning sat number> <ending sat number>
# e.g.: retrieve_spacetrack_tles.sh electrontles.tle username password 43849 43862

export spacetrackuser=$2
export spacetrackpass=$3

# originally: curl -c cookies.txt -b cookies.txt https://www.space-track.org/ajaxauth/login -d "identity=$spacetrackuser&password=$spacetrackpass"
curl -s -S -c /tmp/cookies.txt -b /tmp/cookies.txt https://www.space-track.org/ajaxauth/login -d "identity=$spacetrackuser&password=$spacetrackpass" > /dev/null
# originally: curl --limit-rate 100K --cookie cookies.txt https://www.space-track.org/basicspacedata/query/class/tle_latest/ORDINAL/1/NORAD_CAT_ID/43849--43862/orderby/TLE_LINE1%20ASC/format/tle > electrontle$1.txt
curl -s -S --limit-rate 100K --cookie /tmp/cookies.txt https://www.space-track.org/basicspacedata/query/class/tle_latest/ORDINAL/1/NORAD_CAT_ID/$4--$5/orderby/TLE_LINE1%20ASC/format/tle > $1

