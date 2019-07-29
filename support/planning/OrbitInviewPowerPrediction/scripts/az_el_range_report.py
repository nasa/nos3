import sys
import os
import math
from datetime import datetime, timedelta
from datetime import datetime, timedelta
from satellite_tle import SatelliteTle
from inview_calculator import InviewCalculator
from pytz import UTC
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

###############################################################################
# Python module to put together SatelliteTle and InviewCalculator functionality
# to produce a report of azimuth, elevation(, and range) with respect to a
# ground station during inviews between the ground station and the satellite
# for a specified period of time.
###############################################################################

class AzElRangeReportGenerator:
    """Class to create an azimuth, elevation(, and range) report for a given ground station and a given satellite for a specific time period """
    # Constructor
    def __init__(self, base_output_dir, ground_station, gs_number, satellite_of_interest_tle, tz, \
                 aer_days = 0, time_step_seconds = 15):
        # days:  0=today, -1=yesterday, 1 = tomorrow, etc.
        """Constructor"""
        self.__base_output_dir = base_output_dir
        self.__ground_station = ground_station
        self.__gs_number = gs_number
        self.__satellite_of_interest = satellite_of_interest_tle
        self.__tz = tz
        self.__report_timezone = tz.tzname(datetime.now())
        self.__aer_days = aer_days
        self.__time_step_seconds = time_step_seconds # depends on the caller to guard from stupidity
        self.__debug = False
        self.__trace = False
        self.__out = sys.stdout
        today = datetime.now()
        self.__datestr = "%4.4d-%2.2d-%2.2d" % (today.year, today.month, today.day)
        self.__directory = "%s/%s" % (self.__base_output_dir, self.__datestr)
        #sys.stderr.write("Generating report in directory: %s\n" % directory)

    # Member functions
    def generate_report(self):
        """Method to generate the report"""
        
        if (self.__aer_days > 0):
            try:
                os.makedirs(self.__directory)
            except OSError as e:
                if (e.errno != os.errno.EEXIST):
                    sys.stderr.write("Error making directory %s: %s" % (directory, e))
                    raise e

            for i in range(0, self.__aer_days):
                filename = "%s/aer-day%d-gs%d-sat%s.html" % (self.__directory, i, self.__gs_number, self.__satellite_of_interest.get_satellite_number())
                #sys.stderr.write("Generating report: %s\n" % filename)
                with open(filename, "w") as self.__out:
                    self.__generate_azelrange_for_day(i)

    def __generate_azelrange_for_day(self, day):
        """Method to generate azimuth, elevation, range report for one day"""

        speed_of_light = 300000 # km/s
        # ALL TIMES ARE IN THE TIMEZONE self.__tz !!
        # Determine the time range for the requested day
        day_date = datetime.now() + timedelta(days=day)
        day_year = day_date.year
        day_month = day_date.month
        day_day = day_date.day
        start_time = self.__tz.localize(
            datetime(day_year, day_month, day_day, 0, 0, 0))
        end_time = self.__tz.localize(
            datetime(day_year, day_month, day_day, 23, 59, 59))
        
        # Get the InviewCalculator and compute the inviews
        base_ic = InviewCalculator(self.__ground_station, self.__satellite_of_interest)
        base_inviews = []
        base_inviews = base_ic.compute_inviews(start_time, end_time)

        if (self.__ground_station.get_name() != ""):
            station_name = self.__ground_station.get_name()
        else:
            station_name = ("Lat:%s Lon:%s" % self.__ground_station.get_latitude(), self.__ground_station.get_longitude())

        self.__out.write("<html>\n")
        self.__out.write("  <head>\n")
        self.__out.write("    <title>AER %s to %s, Day %d</title>\n" % ( \
                station_name, self.__satellite_of_interest.get_satellite_name(), day))
        self.__out.write("  </head>\n")
        self.__out.write("  <body>\n")

        report_date = datetime.now() + timedelta(days=day)
        title = "Azimuth, Elevation, Range Report from Ground Station %s to Satellite %s for Day %s (%04d-%02d-%02d)" % \
                (station_name, self.__satellite_of_interest.get_satellite_name(), day, \
                 report_date.year, report_date.month, report_date.day)
        self.__out.write("    <h1>%s</h1>\n" % title)
        self.__out.write(("    <h2>NOTE:  Times displayed on the timeline are " + \
              "for the timezone:  %s</h2>\n") % \
              self.__report_timezone)

        i = 0
        self.__out.write("    <ul>\n")
        for iv in base_inviews:
            #self.__out.write(iv) # debug
            start_time = iv[0].astimezone(self.__tz)
            end_time = iv[1].astimezone(self.__tz)
            i = i + 1
            self.__out.write("    <li>Goto: <a href=\"#inview%d\">Inview #%d (%s to %s)</a></li>\n" % (i, i, start_time, end_time))
        self.__out.write("    </ul>\n")

        i = 0
        for iv in base_inviews:
            #self.__out.write(iv) # debug
            start_time = iv[0].astimezone(self.__tz)
            end_time = iv[1].astimezone(self.__tz)
            i = i + 1
            sat_string = "%s" % (self.__satellite_of_interest.get_satellite_name())
            rx_freq = self.__satellite_of_interest.get_receive_frequency()
            if (rx_freq is not None):
                sat_string = sat_string + " (Receive Frequency %8.3f)" % (rx_freq)
            tx_freq = self.__satellite_of_interest.get_transmit_frequency()
            if (tx_freq is not None):
                sat_string = sat_string + " (Transmit Frequency %8.3f)" % (tx_freq)
            self.__out.write("    <h3><a name=\"inview%d\">%s to %s inview #%d (%s to %s)</a></h3>\n" % \
                    (i, station_name, sat_string, i, start_time, end_time))
            azels = base_ic.compute_azels(iv[0], iv[1], self.__time_step_seconds)
            self.__out.write("    <table border='1'><tr><td>\n")
            self.__out.write("    <pre><code>\n")
            out_string = "      Time (%-19s, Azimuth, Elevation, Range(km), Rng Rt(km/s)" % (str(self.__tz) + ")")
            if (rx_freq is not None):
                out_string = out_string + ",  Xmit Freq"
            if (tx_freq is not None):
                out_string = out_string + ",  Recv Freq"
            self.__out.write("%s\n" % out_string)
            prev_rng = None
            for azel in azels:
                if (prev_rng is None):
                    rr_string = "--------"
                    if (rx_freq is not None):
                        tx_string = ",   --------"
                    else:
                        tx_string = ""
                    if (tx_freq is not None):
                        rx_string = ",   --------"
                    else:
                        rx_string = ""
                else:
                    range_rate = (azel[3] - prev_rng) / self.__time_step_seconds
                    rr_string = "%8.5f" % range_rate
                    if (rx_freq is not None):
                        freq = rx_freq * (1 + range_rate / speed_of_light)
                        tx_string = ",   %8.3f" % freq
                    else:
                        tx_string = ""
                    if (tx_freq is not None):
                        freq = tx_freq * (1 - range_rate / speed_of_light)
                        rx_string = ",   %8.3f" % freq
                    else:
                        rx_string = ""
                self.__out.write("      %s, %7.2f,    %6.2f,   %7.1f,     %8.8s%s%s\n" % (azel[0].astimezone(self.__tz), azel[1], azel[2], azel[3], rr_string, tx_string, rx_string)) # convert to specified time zone
                prev_rng = azel[3]
            self.__out.write("    </pre></code>\n")
            self.__out.write("    </td><td>")
            self.__out.write("    <b><p id=\"time%d\" align=\"center\"></p></b>\n" % i)
            self.__out.write("    <script>\n")
            self.__out.write("      var temp%d = setInterval(myTimer%d, 1000);\n" % (i, i))
            self.__out.write("      function myTimer%d() {\n" % i)
            self.__out.write("        var d = new Date();\n")
            self.__out.write("        var t = d.toLocaleTimeString();\n")
            self.__out.write("        document.getElementById(\"time%d\").innerHTML = d;\n" % i)
            self.__out.write("      }\n")
            self.__out.write("    </script>\n")
            filename = "aer-day%d-gs%d-sat%s-iv%d.png" % (day, self.__gs_number, self.__satellite_of_interest.get_satellite_number(), i)
            path = "%s/%s" % (self.__directory, filename)
            self.__generate_azelrange_plot(path, azels)
            self.__out.write("    <img src='../%s/%s'>\n" % (self.__datestr, filename))
            self.__out.write("    </td></tr></table>\n")

        self.__out.write("  </body>\n")
        self.__out.write("</html>\n")


    def __generate_azelrange_plot(self, filename, azels):
        """Method to generate azimuth, elevation(, range) plot for a day, ground station, inview"""
        (fig, ax) = self.create_polar_fig_and_ax()
        self.generate_azelrange_plot_groundconstraints(ax)
        self.generate_azelrange_plot_track(ax, self.__satellite_of_interest.get_satellite_name(), azels, 4) # Hardwire every 4
        plt.savefig(filename)
        plt.close(fig)

    def create_polar_fig_and_ax(self):
        plt.rc('grid', color='#000000', linewidth=1, linestyle='-')
        plt.rc('xtick', labelsize=10)
        plt.rc('ytick', labelsize=10)

        # force square figure and square axes looks better for polar, IMO
        fig = plt.figure(figsize=(8, 8))
        ax = fig.add_axes([0.1, 0.1, 0.8, 0.8],
                          projection='polar')
        ax.legend()
        return (fig, ax)

    def generate_azelrange_plot_track(self, ax, name, azels, time_label_step):
        """Method to plot a track on a set of azimuth/elevation axes.  name is used for the label; azels is a list of time, azimuth, elevation lists; time_label_step indicates how often the track should be labeled with the time from the azels"""
        # CAVEAT EMPTOR:  It was easier to work with the azimuth in radians (0 to 2pi) and the elevation in degrees (0 to 90)
        xarr = []
        yarr = []
        xlbl = []
        ylbl = []
        i = 0
        for azel in azels:
            theta = azel[1]*np.pi/180.0
            xarr.append(theta)
            r = 90.0 - azel[2]
            yarr.append(r)
            if ((i % time_label_step) == 0):
                dt = azel[0].astimezone(self.__tz)
                time = "%2.2d:%2.2d:%2.2d" % (dt.hour, dt.minute, dt.second)
                ax.annotate(time, (theta, r))
                xlbl.append(theta)
                ylbl.append(r)
            i = i + 1
        ax.plot(xarr, yarr, color='#000000', lw=3, label=name)
        ax.plot(xlbl, ylbl, color='#000000', lw=3, linestyle='None', marker='o')

    def generate_azelrange_plot_points(self, ax, name, color, azels):
        """Method to plot points on a set of azimuth/elevation axes.  name and color are used for the label; azels is a list of time, azimuth, elevation lists"""
        # CAVEAT EMPTOR:  It was easier to work with the azimuth in radians (0 to 2pi) and the elevation in degrees (0 to 90)
        xarr = []
        yarr = []
        i = 0
        for azel in azels:
            theta = azel[1]*np.pi/180.0
            xarr.append(theta)
            r = 90.0 - azel[2]
            yarr.append(r)
        ax.scatter(xarr, yarr, color=color, lw=3, label=name)

    def generate_azelrange_plot_groundconstraints(self, ax):
        """Method to plot the ground station constraints on azimuth/elevation axes.  Constraints include min elevation, keyhole elevation, good sectors, and bad sectors"""
        # CAVEAT EMPTOR:  It was easier to work with the azimuth in radians (0 to 2pi) and the elevation in degrees (0 to 90)
        ax.set_theta_zero_location("N")
        ax.text(0, 103, "N", fontsize=10)
        ax.text(np.pi/2, 107, "E", fontsize=10)
        ax.text(np.pi, 105, "S", fontsize=10)
        ax.text(3*np.pi/2, 105, "W", fontsize=10)

        min_el =self.__ground_station.get_aer_min_el()
        keyhole_el =self.__ground_station.get_aer_keyhole_el()
        x = np.arange(0, 2*np.pi + 0.1, 0.1)
        ax.fill_between(x, 90 - min_el, 90, color='#ffff00', alpha=0.5) # Min el: 90 - angle, plt has 0 at bullseye
        ax.fill_between(x, 0, 90 - keyhole_el, color='#ffff00', alpha=0.5)  # Keyhole: 90 - angle, plt has 0 at bullseye

        for sector in self.__ground_station.get_good_sectors():
            [minaz, maxaz, minel, maxel] = sector
            if ((minaz < maxaz) and (minel < maxel)):
                #print("Min/Max Az/El: %f %f %f %f" % (minaz, maxaz, minel, maxel)) # debug
                x = np.arange(minaz*np.pi/180.0, maxaz*np.pi/180.0 + 0.01, 0.01)
                ax.fill_between(x, 90 - maxel, 90 - minel, color='#00ff00', alpha=0.2) # 90 - angle, plt 0 at bull

        for sector in self.__ground_station.get_bad_sectors():
            [minaz, maxaz, minel, maxel] = sector
            if ((minaz < maxaz) and (minel < maxel)):
                #print("Min/Max Az/El: %f %f %f %f" % (minaz, maxaz, minel, maxel)) # debug
                x = np.arange(minaz*np.pi/180.0, maxaz*np.pi/180.0 + 0.01, 0.01)
                ax.fill_between(x, 90 - maxel, 90 - minel, color='#ff0000', alpha=0.5) # 90 - angle, plt 0 at bull

        theta = np.arange(0, 2*np.pi + 0.1, 0.1)
        r = theta*0 + 90
        ax.plot(theta, r, color='#ff0000')

        labels = []
        for angle in range(0, 105, 15):
            if (min_el == angle):
                pass
            elif (min_el < angle):
                labels.append(min_el)
            if (keyhole_el == angle):
                pass
            elif ((min_el != keyhole_el) and (keyhole_el < angle)):
                labels.append(keyhole_el)
            labels.append(angle)
        labels.reverse()
        ticks = []
        for label in labels:
            ticks.append(90-label)

        ax.set_yticks(ticks)
        ax.set_yticklabels(map(str, labels))

