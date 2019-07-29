CAVEAT EMPTOR:  This readme is by now getting really old and hysterical (historical?)... the general ideas are here and some of the prerequisite packages needed, but for the latest on what OIPP does and how to run parts of it, you will need to read the code :-(

Quick Start:  
============
Run "scripts/generate_html_report.py".

More details:
========================================================================================
This directory contains a collection of Python scripts and config files (and example outputs) to support Orbit, Inview, and Power Prediction processing.

Basically, this just means they can produce information for a satellite and one or more ground stations or a ground station and one or more satellites that describes when the satellite is inview of the ground station and when the satellite is in sunlight and in shadow.

Prerequisites:
--------------
To run these scripts, you need:
- Python 2
- Additional Python Modules:
-- Geocoder - https://pypi.python.org/pypi/geocoder
-- Numpy - http://www.numpy.org/ (used by Pyorbital)
-- Pyorbital - https://pypi.python.org/pypi/pyorbital
-- Pytz - http://pytz.sourceforge.net/

Running:
--------
-Example output file from a previous run of "scripts/generate_html_report.py" is at "example_outputs/sat_html_report.html"
-Example output file from a previous run of "scripts/generate_html_report.py config/wallops_html_report.config" is at "example_outputs/wallops_html_report.html"

If all is well, to run just fire up Python and try running:
    
    - "scripts/generate_html_report.py"  <==== IMPORTANT LINE HERE
    or
    - "scripts/generate_html_report.py config/wallops_html_report.config"  <==== IMPORTANT LINE HERE (or specify any other config file in the config directory)
    
    - You should get HTML text written to standard output... you can copy or redirect this to a text file... slap the extension ".html" on the end, and if you look at it in a web browser it should look something like the example HTML output files that are in the example_outputs directory.
Else
    - Let me know... I'll figure out what I have wrong
    
- If you want to automate this on a Linux/Unix system, see the "crontab" file in the current directory; I have been using that "crontab" on a machine for over a year to generate daily reports (as of 11/27/2016)... you will need to know a little about cron and Apache to make this all work, but it's not too bad.

Configuring:
------------
Satellite HTML report: (one satellite, multiple ground stations, satellite sun times can be shown)
----------------------
Copy "config/sat_html_report.config" to another config file name
Open your new config file.
- Make sure the "report_type" is "Satellite HTML"
- Alter the "timezone" (timezone for the report bars), "inviews" (True is generate them, False is do not generate them), "insun" (similar to "inviews"), "start_day" (0 means today, -1 means yesterday, 1 means tomorrow, 7 means seven days from now, etc.), "end_day" (similar to "start_day").
- Alter the satellite information.
  -- Change the satellite "number" to the NORAD Spacecraft Number of your favorite satellite to get info for a different satellite.  See http://celestrak.com/ for information on NORAD Spacecraft Numbers, Two Line Element Sets (TLEs), etc.
  -- The "number" listed must exist in the file at the "url" listed... Celestrak has other TLE files such as "cubesat.txt", "tle_new.txt", and "stations.txt".
  -- List a "name" if you like... if none is specified, the program will attempt to retrieve the name from the TLE file.
- Alter the ground stations list.  You can add or take out {}, blocks as desired to add or remove ground stations from the list.
  -- There are several predefined ground stations, see the file "config/sat_html_report.config" for which ones (search for "predefined")
  -- Otherwise, you can specify latitude, longitude, elevation, name, time zone, min elevation angle for inview, and operation times (relative to the specified time zone)

Ground Station HTML report: (one ground station, multiple satellites)
---------------------------
Copy "config/wallops_html_report.config" to another config file name
Open your new config file 
- Make sure the "report_type" is "Ground Station HTML"
- Alterations similar to those for "Satellite HTML report" can be made... only now there is only one ground station and multiple satellites
  
Caveats:
--------
1.  Satellite ephemeris is generated using Two Line Element sets (TLEs) and SGP4.  The outputs will only be as good as those algorithms and input data.  You can ask for data from start day = -180 to end day = +365, but you may not want to trust it :-)
2.  This program may not work or may not work correctly.  I did my best and want to improve it.  I have checked Firefly inviews to Wallops against results from AGI's Systems Toolkit (STK) a couple of times and the agreement is very good (to the second), I have also checked Wallops to IceCube, Dellingr, and Mirata and the agreement is pretty good (couple of seconds).  I have checked the sun times against AGI's STK a couple of times and the agreement is so-so.
3.  Those are pretty much the major caveats.  If you have any other feedback, please let me know.

More Description:
-----------------
Heavy Lifting::::
Most of the heavy lifting (real calculations) in this script are done by packages that lots of great people have already put together.

In particular, Pyorbital is providing the SGP4 ephemeris propagation of TLE data for the requested satellite.  They are also providing much of the inview calculations.  Speaking of Two Line Element sets, they are automatically retrieved over the internet (if possible... there is a file input capability) from http://celestrak.com/.  That ensures that the most recent TLE available (on CelesTrak) is retrieved.

Geocoder and Pytz are used to do some nifty things on the ground like figure out the latitude and longitude of a particular address, what time zone things are in, and how to convert between time zones.

Pyorbital and reused code from it provides sun position in a usable coordinate frame.  You can read the code if you are interested.

Numpy:  Pyorbital relies on this and I used it one place where I reused some Pyorbital code.

Other Main Scripts::::
generate_html_report.py (already described)
generate_stf1_html_report.py (shows how to use a file instead of a URL for the TLEs)
get_iss_ephemeris_now.py
get_iss_ephemeris_today.py
get_iss_lonlatalt_today.py
get_iss_suntimes_today.py
get_icecube_lonlatalt_today.py
get_stf1-like_lonlatalt_today.py
iss_inviews.py

Helper Scripts:::
ground_station.py - Create ground station objects based on either an address or a lat/lon/alt/time zone/minimum elevation angle, and pass back data to users of a ground station.  Also create a few specific ground stations.
satellite_tle.py - Create a satellite object based on a NORAD Satellite Number.  Looks up the TLE automatically from CelesTrak if possible.  Performs propagation and inview determination as needed.
inview_calculator.py - Puts together data from a ground station and a satellite to get inviews.
satellite_html_report_generator.py - Takes some configuration info regarding what satellite, what ground stations, what time period, and what output timelines are desired.  Uses the above helper scripts to do the computations.  Then uses the Google timeline JavaScript API to generate the timelines that are displayed.
ground_station_html_report_generator.py - Takes some configuration info regarding what groud station, what satellites, what time period, and what output timelines are desired.  Uses the above helper scripts to do the computations.  Then uses the Google timeline JavaScript API to generate the timelines that are displayed.

Improvements:::
Inview and sun computations are both done by stepping through the ephemeris minute by minute looking for transitions... which are then refined by stepping backwards second by second to find the exact second of transition.  This is easy to code; but has at least two downfalls:  1. it is not as efficient as it could be... binary searching for the seconds would be quicker; 2.  it could miss transitions that occur within a minute... I don't think I'll worry about that for now based on our typical use cases.

Probably lots more I have not thought of!

Prereqs on Ubuntu Trusty 64 (14.04 LTS) (One way to do it):
-----------------------------------------------------------
sudo apt-get update
sudo apt-get install -y python-numpy
sudo apt-get install -y python-setuptools
sudo easy_install --upgrade pyorbital
sudo easy_install --upgrade geocoder
sudo easy_install --upgrade pytz

Cron and Apache on Ubuntu Trusty 64 (14.04 LTS) (One way to do it):
-------------------------------------------------------------------
sudo apt-get install apache2
sudo tar -xzvf <dir>/OrbitInviewPowerPrediction.tgz --strip 1 -C /usr/lib/cgi-bin # fill in <dir> with where to find the .tgz file
sudo mkdir -p /var/www/html/icecube/icecube
sudo mkdir -p /var/www/html/dellingr/dellingr
sudo mkdir -p /var/www/html/iss/iss
sudo mkdir -p /var/www/html/stf1-like/stf1-like
sudo mkdir -p /var/www/html/wallops/wallops
sudo chown -R <user>:<group> /var/www/html/* # fill in the user/group that you run the crontab command below as
crontab /usr/lib/cgi-bin/crontab
