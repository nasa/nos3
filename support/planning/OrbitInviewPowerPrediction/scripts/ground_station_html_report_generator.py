import sys
import os
from datetime import datetime, timedelta
from datetime import datetime, timedelta
from satellite_tle import SatelliteTle
from inview_calculator import InviewCalculator
from az_el_range_report import AzElRangeReportGenerator
from pytz import UTC

###############################################################################
# Python module to put together SatelliteTle and InviewCalculator functionality
# to produce a nice HTML report that describes events of interest
# for a ground station and one or more satellites for a specified period of time.
###############################################################################

class GroundStationHtmlReportGenerator:
    """Class to create an HTML report for a given ground station and one or more satellites for a specific time period """
    # Constructor
    def __init__(self, base_output_dir, ground_station, satellite_tle_list, tz, \
                 create_inviews = True, aer_days = 0,\
                 start_day = 0, end_day = 0):
        # days:  0=today, -1=yesterday, 1 = tomorrow, etc.
        """Constructor"""
        self.__base_output_dir = base_output_dir
        self.__ground_station = ground_station
        self.__satellite_tle_list = satellite_tle_list
        self.__tz = tz
        self.__create_inviews = create_inviews
        self.__aer_days = aer_days
        self.__start_day = start_day
        self.__end_day = end_day
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
            unit_height = 55
            standard_row_height = unit_height + unit_height * len(self.__satellite_tle_list)
            if (self.__ground_station.get_show_operations_hours()):
                standard_row_height += unit_height
            for i in range(self.__start_day, self.__end_day+1):
                row_height = standard_row_height
                if (i == 0):
                    row_height += unit_height # space for report generation time bar
                self.__html_out.write("    <hr>\n")
                self.__html_out.write(("    <div id=\"timeline%s\" " + \
                       "style=\"height: %dpx;\"></div>\n") % \
                       (i, row_height))

            self.__html_out.write("  </body>\n")
            self.__html_out.write("</html>\n")

        if (self.__aer_days > 0):
            #print self.__aer_days # debug
            for sat in self.__satellite_tle_list:
                aerg = AzElRangeReportGenerator(self.__base_output_dir, self.__ground_station, 0, sat, \
                        self.__tz, self.__aer_days)
                aerg.generate_report()
        
    def __generate_html_head(self):
        if (self.__ground_station.get_name() != ""):
            ident = self.__ground_station.get_name()
        else:
            ident = ("Lat:%s Lon:%s" % self.__ground_station.get_latitude(), self.__ground_station.get_longitude())
        title = "Ground Station %s Report" % ident
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
        if (self.__ground_station.get_name() != ""):
            ident = self.__ground_station.get_name()
        else:
            ident = ("Lat:%s Lon:%s" % self.__ground_station.get_latitude(), self.__ground_station.get_longitude())
        title = "Ground Station %s Report for %04d-%02d-%02d" % \
                (ident, today.year, today.month, today.day)
        self.__html_out.write("    <h1>%s</h1>\n" % title)
        self.__html_out.write(("    <h2>NOTE:  Times displayed on the timeline are " + \
              "for the timezone:  %s</h2>\n") % \
              self.__tz)
        self.__html_out.write("Please click on the inview bars to get azimuth/elevation report information.<br>\n")
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
        self.__html_out.write("    Ground Station:  %s (%s)<br>\n" % \
              (self.__ground_station.get_name(), \
               self.__ground_station.get_address()))
        self.__html_out.write(("    (Latitude %s degrees, Longitude %s degrees, Elevation %s meters, " + \
               "Timezone %s)<br>\n") % \
               (self.__ground_station.get_latitude(), \
                self.__ground_station.get_longitude(), \
                self.__ground_station.get_elevation_in_meters(), \
                self.__ground_station.get_tz().tzname(datetime.now())))
        self.__html_out.write("    Minimum elevation above the horizon for inview:  %s degrees (inview start and end times are at this elevation)\n" % \
              self.__ground_station.get_minimum_elevation_angle())
        self.__html_out.write("    <hr>\n")
        
        if (self.__create_inviews):
            for sat in self.__satellite_tle_list:
                self.__html_out.write("    <hr>\n")
                self.__html_out.write("    With inviews for satellite:  %s (%s)<br>\n" % \
                      (sat.get_satellite_name(), sat.get_satellite_number()))
                epoch = datetime(2000 + \
                                 int(sat.get_epoch_year()), 1, 1) + \
                        timedelta(days=float(sat.get_epoch_day())-1)
                self.__html_out.write("    Two Line Element Set for Epoch %s (UTC) :<br>\n" % epoch)
                self.__html_out.write("%s\n" % sat.pretty_string())
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
        
        self.__html_out.write(("        var container = " + \
              "document.getElementById('timeline%s');\n") % \
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
        satnums = []
        iv = []
        row = 1
        if (day == 0):
            self.__generate_report_generation_time_bar()
            row = row + 1
        if (self.__ground_station.get_show_operations_hours()):
            self.__generate_operations_bar_for_day(self.__ground_station, day, \
                                                   day_date, day_year, \
                                                   day_month, day_day)
            row = row + 1
        i = 0
        if (self.__create_inviews):
            for sat in self.__satellite_tle_list:
                satnums.append(sat.get_satellite_number())
                iv.append(row)
                row = row + self.__generate_inview_bars_for_day(sat, day, start_time, end_time)
                i = i + 1
            iv.append(row)
        self.__html_out.write("        function selectChart%d(e) {\n" % (day+200)) # 200 is a hack to not have - in names
        self.__html_out.write("          var satnums = [")
        for j in range(0,i):
            self.__html_out.write("%s, " % satnums[j])
        self.__html_out.write("];\n")
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
        #self.__html_out.write("                  alert('Day %d Inview Sat ' + j + ' number: ' + satnums[j] + ' Inview # ' + (item.row-ivrows[j]));\n" % (day))
        self.__html_out.write("                  if ((%d >= 0) && (%d < %d)) {\n" % (day, day, self.__aer_days))
        today = datetime.now()
        self.__html_out.write("                    var win = window.open('../%4.4d-%2.2d-%2.2d/aer-day%d-gs0-sat' + satnums[j] + '.html#inview' + (1+item.row-ivrows[j]));\n" % 
                (today.year, today.month, today.day, day))
        self.__html_out.write("                    win.focus();\n")
        self.__html_out.write("                  }\n")
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
        self.__html_out.write(("          'red', \n"))
        self.__html_out.write(("          'Report was generated at:  %s', \n") % \
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
        
    def __generate_operations_bar_for_day(self, ground_station, day, day_date, \
                                        day_year, day_month, day_day):
        # Time bar for local operations
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
               ground_station.get_tz()))
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

    def __generate_inview_bars_for_day(self, sat, day, \
                                       start_time, end_time):
        # Time bars for inviews
        gsname = self.__ground_station.get_name()        
        helptext = ""
        if ((day >= 0) and (day < self.__aer_days)): 
            helptext = " (CLICK BAR FOR AZ/EL REPORT AND GRAPH)"

        # Get the InviewCalculator and compute the inviews
        ic = InviewCalculator(self.__ground_station, \
                              sat)
        inviews = []
        inviews = ic.compute_inviews(start_time, end_time)

        self.__html_out.write("        dataTable.addRows([\n")
        for i in range(0, len(inviews)):
            riselocal = inviews[i][0].astimezone(self.__tz)
            setlocal = inviews[i][1].astimezone(self.__tz)
            self.__html_out.write(("          ['%s - S/C %s Inviews', ' ', 'blue', " + \
                   "'%02d:%02d:%02d - %02d:%02d:%02d, Max Elev %02.1f degrees%s', " + \
                   "new Date(%s, %s, %s, %s, %s, %s), " + \
                   "new Date(%s, %s, %s, %s, %s, %s)],\n") % \
                   (gsname, sat.get_satellite_name(), \
                    riselocal.hour, riselocal.minute, riselocal.second, \
                    setlocal.hour, setlocal.minute, setlocal.second, \
                    inviews[i][2], helptext, \
                    riselocal.year, riselocal.month-1, riselocal.day, \
                    riselocal.hour, riselocal.minute, riselocal.second, \
                    setlocal.year, setlocal.month-1, setlocal.day, \
                    setlocal.hour, setlocal.minute, setlocal.second))
        self.__html_out.write("        ]);\n")
        return len(inviews)

