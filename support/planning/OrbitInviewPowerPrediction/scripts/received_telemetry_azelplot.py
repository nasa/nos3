#!/usr/bin/env python
#
# Quick script to graph where STF-1 would be in azimuth and elevation relative to Wallops at times
# specified in an input file... if these files represent when telemetry was received, then the 
# plot indicates where STF-1 was when the telemetry was received.  The location computations are
# based on TLEs.
#
# Syntax:  received_telemetry_azelplot.py <file of times, format YYYY-MM-DD HH:MM:SS>
#

import argparse
import json
#import tempfile
from argvalidator import ArgValidator
import os
#import sys
import glob
import re
from configuration import Configuration
from pyorbital.orbital import Orbital
from satellite_tle import SatelliteTle
from ground_station import GroundStation
from inview_calculator import InviewCalculator
from az_el_range_report import AzElRangeReportGenerator
from datetime import datetime
from pytz import timezone, UTC
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("-c", "--config", help="OIPP config file, used to get satellite number and ground station properties", type=ArgValidator.validate_file, default=None)
  parser.add_argument("-t", "--tlmfile", help="File of telemetry", type=ArgValidator.validate_file, default=None)
  parser.add_argument("-x", "--cmdfile", help="File of commands", type=ArgValidator.validate_file, default=None)
  parser.add_argument("-p", "--pngfile", help="PNG filename to output", default="plot.png")
  parser.add_argument("-d", "--directory", \
      help="Directory of dated directories containing TLE files (*.tle), assumed format of dated directory is YYYY-MM-DD", \
      type=ArgValidator.validate_directory, default="/home/itc/Desktop/oipp-data/stf1/")
  args = parser.parse_args()

  with open(args.config) as json_data_file:
      data = json.load(json_data_file)

  tz = Configuration.get_config_timezone(data.get('timezone','US/Eastern'))

  if (data['report_type'] == "AzElCmdTelem"):
      create_azel_cmd_telemetry_report(tz, args.tlmfile, args.cmdfile, args.pngfile, data)

def create_azel_cmd_telemetry_report(tz, tlmfile, cmdfile, pngfile, data):
  satnum = data.get('satellite',[])['number']
  ground_station = GroundStation.from_config(data.get('ground_station',[]))
  tle = SatelliteTle.from_config(data.get('satellite',[]))
  show_track = Configuration.get_config_boolean(data.get('show_track','false'))

  aer = AzElRangeReportGenerator("", ground_station, 1, tle, tz, 0, time_step_seconds = 15)
  (fig, ax) = aer.create_polar_fig_and_ax()
  aer.generate_azelrange_plot_groundconstraints(ax)

  satdata = {}
  tlmdata = {}
  if (tlmfile is not None):
    process_telem_file(tlmfile, tz, satnum, ground_station, data, satdata, tlmdata)

  hidata = {}
  lowdata = {}
  cleardata = {}
  nre = {}
  if (cmdfile is not None):
    hidata = process_cmd_file(cmdfile, tz, satnum, ground_station, data, satdata,    re.compile("CADET_FIFO_REQUEST.*FLAGS [1H]"), True)
    lowdata = process_cmd_file(cmdfile, tz, satnum, ground_station, data, satdata,   re.compile("CADET_FIFO_REQUEST.*FLAGS [^1H]"), True)
    cleardata = process_cmd_file(cmdfile, tz, satnum, ground_station, data, satdata, re.compile("CADET_FIFO_CLEAR"), True)
    nre = process_cmd_file(cmdfile, tz, satnum, ground_station, data, satdata,       re.compile("CADET_FIFO"), False)

  if (show_track):
    for j in satdata.keys():
      d = satdata[j]
      for i in range(len(d.plotinviews)):
        if (d.plotinviews[i]):
          icazels = d.inview_computer.compute_azels(d.inviews[i][0], d.inviews[i][1], 15)
          aer.generate_azelrange_plot_track(ax, "%s %s" % (tle.get_satellite_name(), j), icazels, 4) # Hardwire every 4

  for i in tlmdata.keys():
    aer.generate_azelrange_plot_points(ax, "Telemetry", "#00FF00", tlmdata[i])
  for i in cleardata.keys():
    aer.generate_azelrange_plot_points(ax, "Clear Data Cmd", "#FF0000", cleardata[i])
  for i in hidata.keys():
    aer.generate_azelrange_plot_points(ax, "High Data Request", "#0000FF", hidata[i])
  for i in lowdata.keys():
    aer.generate_azelrange_plot_points(ax, "Low Data Request", "#9900FF", lowdata[i])
  for i in nre.keys():
    aer.generate_azelrange_plot_points(ax, "NRE Cmd", "#FF6600", nre[i])

  ax.legend(loc=1, bbox_to_anchor=(1.12, 1.12))
  plt.figure(1)
  plt.savefig(pngfile)

def process_telem_file(filename, tzone, satnum, gs, data, satdata, telemdata):
  tle_dir = data.get('tle_dir', "")
  lyear = lmonth = lday = 0

  timeline = re.compile("STF1_TLM\t\w+\t(\d{4})/(\d{2})/(\d{2}) (\d{2}):(\d{2}):(\d{2})(.*)")
  azels = []
  with open(filename, "r") as f:
    for line in f:
      match = timeline.match(line)
      if match:
        (year, month, day, hour, minute, second, text) = assign_group(match.group)
        if ((lyear != year) or (lmonth != month) or (lday != day)):
          date = "%s-%s-%s" % (lyear, lmonth, lday)
          if (len(azels) > 0):
            telemdata[date] = azels
          azels = []
          date = "%s-%s-%s" % (year, month, day)
          (inviews, plotinviews, orb) = add_sat_data(satdata, tle_dir, date, year, month, day, tzone, satnum, gs) 

        instant = datetime(int(year), int(month), int(day), int(hour), int(minute), int(second))
        temp = tzone.localize(instant).astimezone(UTC)
        time = datetime(temp.year, temp.month, temp.day, temp.hour, temp.minute, temp.second)
        (az, el) = orb.get_observer_look(time, gs.get_longitude(), gs.get_latitude(), gs.get_elevation_in_meters())
        azels.append((time, az, el))
        set_plot_inviews(temp, inviews, plotinviews)

        lyear = year
        lmonth = month
        lday = day

    date = "%s-%s-%s" % (lyear, lmonth, lday)
    if (len(azels) > 0):
      telemdata[date] = azels

def process_cmd_file(filename, tzone, satnum, gs, data, satdata, cmdtofind, positivematch):
  tle_dir = data.get('tle_dir', "")
  lyear = lmonth = lday = 0
  cmddata = {}

  timeline = re.compile("(\d{4})/(\d{2})/(\d{2}) (\d{2}):(\d{2}):(\d{2})(.*)")
  azels = []
  with open(filename, "r") as f:
    for line in f:
      match = timeline.match(line)
      if match:
        (year, month, day, hour, minute, second, text) = assign_group(match.group)
        if ((lyear != year) or (lmonth != month) or (lday != day)):
          date = "%s-%s-%s" % (lyear, lmonth, lday)
          if (len(azels) > 0):
            cmddata[date] = azels
          azels = []
          date = "%s-%s-%s" % (year, month, day)
          (inviews, plotinviews, orb) = add_sat_data(satdata, tle_dir, date, year, month, day, tzone, satnum, gs) 

        instant = datetime(int(year), int(month), int(day), int(hour), int(minute), int(second))
        temp = tzone.localize(instant).astimezone(UTC)
        time = datetime(temp.year, temp.month, temp.day, temp.hour, temp.minute, temp.second)
        (az, el) = orb.get_observer_look(time, gs.get_longitude(), gs.get_latitude(), gs.get_elevation_in_meters())
        if (cmdtofind.search(text)): # found it
          if (positivematch): # want a match
            azels.append((time, az, el))
        else: # did not find it
          if (not positivematch): # do NOT want a match
            azels.append((time, az, el))
        set_plot_inviews(temp, inviews, plotinviews)

        lyear = year
        lmonth = month
        lday = day

    date = "%s-%s-%s" % (lyear, lmonth, lday)
    if (len(azels) > 0):
      cmddata[date] = azels

  return cmddata

class SatData:
  def __init__(self):
    self.inviews = []
    self.plotinviews = []
    self.inview_computer = None
    self.orb = None

def add_sat_data(satdata, tle_dir, date, year, month, day, tzone, satnum, gs): 
  #print("adding sat on %s" % date)
  if (date not in satdata):
    elfile = glob.glob(tle_dir + "%s-%s-%s/*.tle" % (year, month, day))
    tle = SatelliteTle(satnum, tle_file=elfile[0])
    orb = Orbital(str(tle.get_satellite_number()), line1=tle.get_line1(), line2=tle.get_line2())
    ic = InviewCalculator(gs, tle)
    inviews = ic.compute_inviews(tzone.localize(datetime(int(year), int(month), int(day), 0, 0, 0)), \
	tzone.localize(datetime(int(year), int(month), int(day), 23, 59, 59)))
    plotinviews = []
    for i in range(len(inviews)):
      plotinviews.append(False)
    p = SatData()
    p.inviews = inviews
    p.plotinviews = plotinviews
    p.inview_computer = ic
    p.orb = orb
    satdata[date] = p
  else:
    p = satdata[date]

  return (p.inviews, p.plotinviews, p.orb)

def assign_group(group):
  year = group(1)
  month = group(2)
  day = group(3)
  hour = group(4)
  minute = group(5)
  second = group(6)
  text = group(7)
  return (year, month, day, hour, minute, second, text)

def set_plot_inviews(time, inviews, plotinviews):
  i = 0
  for iv in inviews:
    if ((iv[0] <= time) and (iv[1] >= time)):
      plotinviews[i] = True
    i = i + 1

# Python idiom to eliminate the need for forward declarations
if __name__=="__main__":
   main()

