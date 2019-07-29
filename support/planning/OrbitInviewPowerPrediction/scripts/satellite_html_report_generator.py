import sys
import os
from datetime import datetime, timedelta
from datetime import datetime, timedelta
from satellite_tle import SatelliteTle
from inview_calculator import InviewCalculator
from az_el_range_report import AzElRangeReportGenerator
from pytz import UTC
from ground_station_tracking_schedule import GroundStationTrackingSchedule

###############################################################################
# Python module to put together SatelliteTle and InviewCalculator functionality
# to produce a nice HTML report that describes events of interest
# for a satellite and ground station for a specified period of time.
###############################################################################

class SatelliteHtmlReportGenerator:
    """Class to create an HTML report for a given satellite and ground station for a specific time period """
    # Constructor
    def __init__(self, base_output_dir, satellite_tle, ground_station_list, tz, \
                 create_inviews = True, create_contacts = False, create_insun = True, aer_days = 0, \
                 start_day = 0, end_day = 0, time_step_seconds = 15):
        # days:  0=today, -1=yesterday, 1 = tomorrow, etc.
        """Constructor"""
        self.__base_output_dir = base_output_dir
        self.__satellite_tle = satellite_tle
        self.__ground_station_list = ground_station_list
        self.__tz = tz
        self.__create_inviews = create_inviews
        self.__create_contacts = create_contacts
        self.__create_insun = create_insun
        self.__aer_days = aer_days
        self.__start_day = start_day
        self.__end_day = end_day
        self.__time_step_seconds = time_step_seconds
        self.__html_out = sys.stdout


    # Member functions
    def generate_report(self):
        """Method to generate the HTML report"""
        # Use Google timeline JavaScript API from:  https://developers.google.com/chart/interactive/docs/gallery/timeline
        
        today = datetime.now()
        directory = "%s/%4.4d-%2.2d-%2.2d" % (self.__base_output_dir, today.year, today.month, today.day)
        #sys.stderr.write("Generating report in directory: %s\n" % directory)
        try:
            os.makedirs(directory)
        except OSError as e:
            if (e.errno != os.errno.EEXIST):
                sys.stderr.write("Error making directory %s: %s" % (directory, e))
                raise e

        filename = "%s/%4.4d-%2.2d-%2.2d.html" % (directory, today.year, today.month, today.day)
        #sys.stderr.write("Generating report: %s\n" % filename)

        with open(filename, "w") as self.__html_out:

            self.__html_out.write("<html>\n")
            self.__generate_html_head()
            self.__html_out.write("  <body>\n")
            self.__generate_html_body_header()

            # Timelines
            unit_height = 60
            standard_row_height = 0
            if (self.__create_insun):
                standard_row_height += unit_height
            if (self.__create_inviews):
                for gs in self.__ground_station_list:
                    standard_row_height += unit_height
                    if (gs.get_show_operations_hours()):
                        standard_row_height += unit_height

            for i in range(self.__start_day, self.__end_day+1):
                row_height = unit_height + standard_row_height
                if (i == 0):
                    row_height += unit_height # space for report generation time bar
                self.__html_out.write("     <hr>\n")
                self.__html_out.write(("    <div id=\"timeline%s\" " + \
                       "style=\"height: %dpx;\"></div>\n") % \
                       (i, row_height))

            self.__html_out.write("  </body>\n")
            self.__html_out.write("</html>\n")

        if (self.__aer_days > 0):
            #print self.__aer_days # debug
            i = 0
            for gs in self.__ground_station_list:
                aerg = AzElRangeReportGenerator(self.__base_output_dir, gs, i, self.__satellite_tle, \
                        self.__tz, self.__aer_days, self.__time_step_seconds)
                aerg.generate_report()
                i = i + 1
        
    def __generate_html_head(self):
        ident = self.__satellite_tle.get_satellite_name()
        title = "Satellite %s Report" % ident
        self.__html_out.write("  <head>\n")
        self.__html_out.write("    <title>%s</title>\n" % title)
        self.__html_out.write("    <script type=\"text/javascript\" " + \
              "src=\"https://www.google.com/jsapi\"></script>\n")
        self.__html_out.write("    <script type=\"text/javascript\">\n")
        self.__html_out.write("      google.load(\"visualization\", \"1\", " + \
              "{packages:[\"timeline\"]});\n")
        self.__html_out.write("      google.setOnLoadCallback(drawCharts);\n")
        self.__html_out.write("      function drawCharts() {\n")
        for i in range(self.__start_day, self.__end_day+1):
            self.__generate_chart_for_day(i)
            pass
        self.__html_out.write("      }\n")
        self.__html_out.write("    </script>\n")
        self.__html_out.write("  </head>\n")

    def __generate_html_body_header(self):
        today = datetime.now()
        yday = datetime.now() + timedelta(days=-1)
        tom = datetime.now() + timedelta(days=+1)
        ident = self.__satellite_tle.get_satellite_name()
        title = "Satellite %s Report for %04d-%02d-%02d\n" % \
                (ident, today.year, today.month, today.day)
        self.__html_out.write("    <h1>%s</h1>\n" % title)
        self.__html_out.write("    <h2>NOTE:  Times displayed on the timeline are " + \
              "for the timezone:  %s</h2>\n" % \
              self.__tz)
        self.__html_out.write("Please click on the inview bars to get azimuth/elevation report information.<br>\n")
        self.__html_out.write("Inview bar color codes:  grey=no schedule information, blue=scheduled contact, purple=no scheduled contact.<br>\n")
        self.__html_out.write("Please hover over any colored bar for detailed time information.\n")
        self.__html_out.write("    <hr>\n")
        self.__html_out.write("    Report from day %s to day %s<br>\n" % \
              (self.__start_day, self.__end_day))
        self.__html_out.write(("    (If they exist: " + \
              "<a href=\"../%04d-%02d-%02d/%04d-%02d-%02d.html\">Previous Report</a> " + \
              "<a href=\"../%04d-%02d-%02d/%04d-%02d-%02d.html\">Next Report</a>)\n") % \
              (yday.year, yday.month, yday.day, \
               yday.year, yday.month, yday.day, \
               tom.year, tom.month, tom.day, \
               tom.year, tom.month, tom.day))
        self.__html_out.write("    <hr>\n")
        epoch = datetime(2000 + \
                         int(self.__satellite_tle.get_epoch_year()), 1, 1) + \
                timedelta(days=float(self.__satellite_tle.get_epoch_day())-1)
        self.__html_out.write("Two Line Element Set for Epoch %s (UTC) :" % epoch)
        if (self.__satellite_tle.get_tle_url() is None):
            self.__html_out.write("<br>\n")
        else:
            self.__html_out.write("  (<a href=\"%s\" target=\"_blank\">TLE Link - CAVEAT:  The data at this URL may have changed since it was retrieved for this report.</a>)<br>\n" % self.__satellite_tle.get_tle_url())
        self.__html_out.write("%s\n" % self.__satellite_tle.pretty_string())
        if (self.__create_inviews):
            for gs in self.__ground_station_list:
                self.__html_out.write("    <hr>\n")
                self.__html_out.write("    With inviews for ground station:  %s (%s)<br>\n" % \
                      (gs.get_name(), \
                       gs.get_address()))
                self.__html_out.write(("    <a href=\"https://www.google.com/maps?q=%f,%f\" target=\"_blank\">(Latitude %s degrees, Longitude %s degrees, Elevation %s meters, " + \
                       "Timezone %s)</a><br>\n") % \
                       (gs.get_latitude(), \
                        gs.get_longitude(), \
                        gs.get_latitude(), \
                        gs.get_longitude(), \
                        gs.get_elevation_in_meters(), \
                        gs.get_tz().tzname(datetime.now())))
                self.__html_out.write("    Minimum elevation above the horizon for inview:  %s degrees (inview start and end times are at this elevation)<br>\n" % \
                      gs.get_minimum_elevation_angle())
                if (gs.get_wx_url() is not None):
                    self.__html_out.write("    <a href=\"%s\" target=\"_blank\">Click for weather information</a><br>\n" % gs.get_wx_url())
                if (gs.get_other_info() is not None):
                    self.__html_out.write("    %s<br>\n" % gs.get_other_info())
                self.__html_out.write("    <hr>\n")

    def __generate_chart_for_day(self, day):
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
        
        self.__html_out.write("        var container = " + \
              "document.getElementById('timeline%s');\n" % \
               day)
        self.__html_out.write("        var chart%d = new google.visualization.Timeline(container);\n" % (day+200)) # 200 is a hack to not have - in names
        self.__html_out.write("        var dataTable = new google.visualization.DataTable();\n")

        self.__html_out.write("        dataTable.addColumn({ type: 'string', id: 'Title' });\n")
        self.__html_out.write("        dataTable.addColumn({ type: 'string', id: 'Barname' });\n")
        self.__html_out.write("        dataTable.addColumn({ type: 'string', role: 'style' });\n")
        self.__html_out.write("        dataTable.addColumn({ type: 'string', role: 'tooltip' });\n")
        self.__html_out.write("        dataTable.addColumn({ type: 'date', id: 'Start' });\n")
        self.__html_out.write("        dataTable.addColumn({ type: 'date', id: 'End' });\n")
        self.__html_out.write("        google.visualization.events.addListener(chart%d, 'select', selectChart%d);\n" % (day+200, day+200)) # 200 is a hack to not have - in names
        self.__generate_wholeday_bar_for_day(day, start_time, end_time)
        iv = []
        row = 1
        if (day == 0):
            self.__generate_report_generation_time_bar()
            row = row + 1
        i = 0
        if (self.__create_inviews):
            for gs in self.__ground_station_list:
                if (gs.get_show_operations_hours()):
                    self.__generate_dayshift_bar_for_day(gs, day, \
                                                         day_date, day_year, \
                                                         day_month, day_day)
                    row = row + 1
                iv.append(row)
                row = row + self.__generate_inview_bars_for_day(gs, day, \
                                                    start_time, end_time)
                i = i + 1
            iv.append(row)
        if (self.__create_insun):
            self.__generate_insun_bars_for_day(start_time, end_time)
        
        self.__html_out.write("        function selectChart%d(e) {\n" % (day+200)) # 200 is a hack to not have - in names
        self.__html_out.write("          var ivrows = [")
        for j in range(0,i+1):
            self.__html_out.write("%d, " % iv[j])
        self.__html_out.write("];\n")
        self.__html_out.write("          var msg = 'Table %d selection: ';\n" % (day))
        self.__html_out.write("          var selection = chart%d.getSelection();\n" % (day+200)) # 200 is a hack to not have - in names
        self.__html_out.write("          for (var i = 0; i < selection.length; i++) {\n")
        self.__html_out.write("            var item = selection[i];\n")
        self.__html_out.write("            if (item.row != null) {;\n")
        self.__html_out.write("              msg += 'Row ' + item.row + ' ';\n")
        self.__html_out.write("              for (var j = 0; j < ivrows.length-1; j++) {\n")
        self.__html_out.write("                if ((ivrows[j] <= item.row) && (ivrows[j+1] > item.row)) {\n")
        #self.__html_out.write("                  alert('Day %d Inview GS ' + j + ' Inview # ' + (item.row-ivrows[j]));\n" % (day))
        self.__html_out.write("                   if ((%d >= 0) && (%d < %d)) {\n" % (day, day, self.__aer_days))
        today = datetime.now()
        self.__html_out.write("                       var win = window.open('../%4.4d-%2.2d-%2.2d/aer-day%d-gs' + j + '-sat%s.html#inview' + (1+item.row-ivrows[j]));\n" % 
                (today.year, today.month, today.day, day, self.__satellite_tle.get_satellite_number()))
        self.__html_out.write("                       win.focus();\n")
        self.__html_out.write("                   }\n")
        self.__html_out.write("                }\n")
        self.__html_out.write("              }\n")
        self.__html_out.write("            }\n")
        self.__html_out.write("          }\n")
        #self.__html_out.write("          alert(msg);\n") # debug
        self.__html_out.write("        }\n")
        self.__html_out.write("        chart%d.draw(dataTable);\n" % (day+200)) # 200 is a hack to not have - in names

    def __generate_report_generation_time_bar(self):
        # Time bar for the minute at which the report was generated
        generation_time = self.__tz.localize(datetime.now())
        self.__html_out.write("        dataTable.addRows([\n")
        self.__html_out.write(("          ['Report Generation Time:', " + \
              "'Report was generated at:  %s', \n") % \
               (generation_time))
        self.__html_out.write("          'red', \n")
        self.__html_out.write("          'Report was generated at:  %s', \n" % \
               (generation_time))
        self.__html_out.write("           new Date(%s, %s, %s, %s, %s, %s), \n" % \
              (generation_time.year, generation_time.month-1, \
               generation_time.day, generation_time.hour, \
               generation_time.minute, generation_time.second))
        self.__html_out.write("           new Date(%s, %s, %s, %s, %s, %s)],\n" % \
              (generation_time.year, generation_time.month-1, \
               generation_time.day, generation_time.hour, \
               generation_time.minute, generation_time.second))
        self.__html_out.write("        ]);\n")

    def __generate_wholeday_bar_for_day(self, day, start_time, end_time):
        # Time bar for the whole day
        if (day < -1):
            dayname = "PAST"
            daycolor = "#888"
        elif (day == -1):
            dayname = "YESTERDAY"
            daycolor = "#888"
        elif (day == 0):
            dayname = "TODAY"
            daycolor = "#0a0"
        elif (0 < day):
            dayname = "FUTURE"
            daycolor = "#dd8"
        self.__html_out.write("        dataTable.addRows([\n")
        self.__html_out.write(("          ['Times Displayed are %s', " + \
              "'%s, Day %s:  %s to %s', \n") % \
               (self.__tz, dayname, day, start_time, end_time))
        self.__html_out.write("           '%s', \n" % (daycolor))
        self.__html_out.write("           '%s', \n" % dayname)
        self.__html_out.write("           new Date(%s, %s, %s, %s, %s, %s), \n" % \
              (start_time.year, start_time.month-1, \
               start_time.day, start_time.hour, \
               start_time.minute, start_time.second))
        self.__html_out.write("           new Date(%s, %s, %s, %s, %s, %s)],\n" % \
              (end_time.year, end_time.month-1, \
               end_time.day, end_time.hour, \
               end_time.minute, end_time.second))
        self.__html_out.write("        ]);\n")
        
    def __generate_dayshift_bar_for_day(self, ground_station, day, day_date, \
                                        day_year, day_month, day_day):
        # Time bar for local day shift
        if (day < 0):
            daycolor = "#888"
        elif (day == 0):
            daycolor = "#0a0"
        elif (0 < day):
            daycolor = "#dd8"
        local_day_start = ground_station.get_tz(). \
                          localize( \
                              datetime(day_year, day_month, day_day,
                                       ground_station.get_operations_start_hour(), ground_station.get_operations_start_minute(), 0)).astimezone(self.__tz)
        local_day_end   = ground_station.get_tz(). \
                          localize( \
                              datetime(day_year, day_month, day_day,
                                       ground_station.get_operations_end_hour(), ground_station.get_operations_end_minute(), 0)).astimezone(self.__tz)
        self.__html_out.write("        dataTable.addRows([\n")
        self.__html_out.write("          ['%s Operating hours', '%s Operating hours %2.2d:%2.2d-%2.2d:%2.2d ground station local time, which is %s', \n" % \
              (ground_station.get_name(), \
               ground_station.get_name(), \
               ground_station.get_operations_start_hour(), ground_station.get_operations_start_minute(), \
               ground_station.get_operations_end_hour(), ground_station.get_operations_end_minute(), \
               ground_station.get_tz().tzname(day_date)))
        self.__html_out.write("           '%s', \n" % (daycolor))
        self.__html_out.write("           '%s Operating hours',\n" % ground_station.get_name())
        self.__html_out.write("           new Date(%s, %s, %s, %s, %s, %s),\n" % \
              (local_day_start.year, local_day_start.month-1, \
               local_day_start.day, local_day_start.hour, \
               local_day_start.minute, local_day_start.second))
        self.__html_out.write("           new Date(%s, %s, %s, %s, %s, %s)],\n" % \
              (local_day_end.year, local_day_end.month-1, \
               local_day_end.day, local_day_end.hour, \
               local_day_end.minute, local_day_end.second))
        self.__html_out.write("        ]);\n")

    def __generate_inview_bars_for_day(self, ground_station, day, \
                                       start_time, end_time):
        # Time bars for inviews
        gsname = ground_station.get_name()        
        helptext = ""
        if ((day >= 0) and (day < self.__aer_days)): 
            helptext = " (CLICK BAR FOR AZ/EL REPORT AND GRAPH)"

        # Get the InviewCalculator and compute the inviews
        ic = InviewCalculator(ground_station, \
                              self.__satellite_tle)
        inviews = []
        inviews = ic.compute_inviews(start_time, end_time)
        contacts = []
        schedname = ""
        if (self.__create_contacts):
            fname = ground_station.get_schedule_directory().get_latest_schedule_full_filename_for_date(start_time.date())
            #print("Schedule filename: %s" % fname)
            if (fname is not None):
                gsts = GroundStationTrackingSchedule(fname)
                contacts = gsts.get_satellite_contacts(self.__satellite_tle.get_satellite_contact_name())
                #print(contacts)
                schedname = " -- (Schedule File: %s)" % ground_station.get_schedule_directory().get_latest_schedule_filename_for_date(start_time.date())

        self.__html_out.write("        dataTable.addRows([\n")
        for i in range(0, len(inviews)):
            (typetext, color) = self.inview_contact_info(contacts, inviews[i])
            riselocal = inviews[i][0].astimezone(self.__tz)
            setlocal = inviews[i][1].astimezone(self.__tz)
            self.__html_out.write(("          ['%s - %s Inviews', ' ', '%s', " + \
                   "'%s %02d:%02d:%02d - %02d:%02d:%02d, Max Elev %02.1f degrees%s %s', " + \
                   "new Date(%s, %s, %s, %s, %s, %s), " + \
                   "new Date(%s, %s, %s, %s, %s, %s)],\n") % \
                   (gsname, self.__satellite_tle.get_satellite_name(), color, typetext, \
                    riselocal.hour, riselocal.minute, riselocal.second, \
                    setlocal.hour, setlocal.minute, setlocal.second, \
                    inviews[i][2], helptext, schedname, \
                    riselocal.year, riselocal.month-1, riselocal.day, \
                    riselocal.hour, riselocal.minute, riselocal.second, \
                    setlocal.year, setlocal.month-1, setlocal.day, \
                    setlocal.hour, setlocal.minute, setlocal.second))
        self.__html_out.write("        ]);\n")
        return len(inviews)

    def inview_contact_info(self, contacts, inview):
        if (len(contacts) == 0):
            # No schedule information
            return ('Inview', 'grey')
        else:
            for c in contacts:
                if ((c[0] <= inview[1]) and (c[1] >= inview[0])):
                    return ('Scheduled Contact', 'blue')
            # No overlap found... not on schedule
            return ('NO Contact', 'purple')

    def __generate_insun_bars_for_day(self, start_time, end_time):
        # Time bars for in sun times

        # Get the SatelliteTle and compute the in sun times
        suntimes = []
        tables = self.__satellite_tle.compute_sun_times(start_time, end_time)

        suntimes = tables[0]
        self.__html_out.write("        dataTable.addRows([\n")
        for i in range(0, len(suntimes)):
            enterlocal = suntimes[i][0].astimezone(self.__tz)
            exitlocal = suntimes[i][1].astimezone(self.__tz)
            self.__html_out.write(("          ['%s In Sunlight Times', ' ', 'yellow', " + \
                   "'Sunlight: %02d:%02d:%02d - %02d:%02d:%02d', " + \
                   "new Date(%s, %s, %s, %s, %s, %s), " + \
                   "new Date(%s, %s, %s, %s, %s, %s)],\n") % \
                   (self.__satellite_tle.get_satellite_name(), \
                    enterlocal.hour, enterlocal.minute, enterlocal.second, \
                    exitlocal.hour, exitlocal.minute, exitlocal.second, \
                    enterlocal.year, enterlocal.month-1, enterlocal.day, \
                    enterlocal.hour, enterlocal.minute, enterlocal.second, \
                    exitlocal.year, exitlocal.month-1, exitlocal.day, \
                    exitlocal.hour, exitlocal.minute, exitlocal.second))
        self.__html_out.write("        ]);\n")
        
        pentimes = tables[1]
        self.__html_out.write("        dataTable.addRows([\n")
        for i in range(0, len(pentimes)):
            enterlocal = pentimes[i][0].astimezone(self.__tz)
            exitlocal = pentimes[i][1].astimezone(self.__tz)
            self.__html_out.write(("          ['%s In Sunlight Times', ' ', 'grey', " + \
                   "'Penumbra: %02d:%02d:%02d - %02d:%02d:%02d', " + \
                   "new Date(%s, %s, %s, %s, %s, %s), " + \
                   "new Date(%s, %s, %s, %s, %s, %s)],\n") % \
                   (self.__satellite_tle.get_satellite_name(), \
                    enterlocal.hour, enterlocal.minute, enterlocal.second, \
                    exitlocal.hour, exitlocal.minute, exitlocal.second, \
                    enterlocal.year, enterlocal.month-1, enterlocal.day, \
                    enterlocal.hour, enterlocal.minute, enterlocal.second, \
                    exitlocal.year, exitlocal.month-1, exitlocal.day, \
                    exitlocal.hour, exitlocal.minute, exitlocal.second))
        self.__html_out.write("        ]);\n")
        
        umbratimes = tables[2]
        self.__html_out.write("        dataTable.addRows([\n")
        for i in range(0, len(umbratimes)):
            enterlocal = umbratimes[i][0].astimezone(self.__tz)
            exitlocal = umbratimes[i][1].astimezone(self.__tz)
            self.__html_out.write(("          ['%s In Sunlight Times', ' ', 'black', " + \
                   "'Umbra: %02d:%02d:%02d - %02d:%02d:%02d', " + \
                   "new Date(%s, %s, %s, %s, %s, %s), " + \
                   "new Date(%s, %s, %s, %s, %s, %s)],\n") % \
                   (self.__satellite_tle.get_satellite_name(), \
                    enterlocal.hour, enterlocal.minute, enterlocal.second, \
                    exitlocal.hour, exitlocal.minute, exitlocal.second, \
                    enterlocal.year, enterlocal.month-1, enterlocal.day, \
                    enterlocal.hour, enterlocal.minute, enterlocal.second, \
                    exitlocal.year, exitlocal.month-1, exitlocal.day, \
                    exitlocal.hour, exitlocal.minute, exitlocal.second))
        self.__html_out.write("        ]);\n")
        
