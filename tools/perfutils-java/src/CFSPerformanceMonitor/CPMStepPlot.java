/**
 * CFS Performance Monitor Step Plot
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.ENTRY;
import static CFSPerformanceMonitor.CPMConstants.EXIT;
import static CFSPerformanceMonitor.CPMConstants.MIN_WINDOW_WIDTH;
import static CFSPerformanceMonitor.CPMConstants.PLOT_LABEL_FONT;
import static CFSPerformanceMonitor.CPMConstants.PLOT_TICK_FONT;
import static CFSPerformanceMonitor.CPMConstants.STEP_PLOT;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.geom.Point2D;

import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.SymbolAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.CombinedDomainXYPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.PlotRenderingInfo;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;
import CFSPerformanceMonitor.CPMLogHandler.PerfLog;

/******************************************************************************
 * CFS Performance Monitor step plot class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMStepPlot extends CPMXYPlotHandler
{
    // Set to true if the CPU activity line is plotted
    private Boolean hasActivityLine;

    /**************************************************************************
     * Step plot class constructor
     * 
     * @param perfLog
     *            performance log
     * 
     * @param perfIDs
     *            performance IDs
     * 
     * @param perfMain
     *            main class
     * 
     * @param plotPrefs
     *            plot preferences
     *************************************************************************/
    public CPMStepPlot(CPMLogHandler perfLog,
                       CPMIDHandler perfIDs,
                       CPMMain perfMain,
                       CPMPreferencesHandler plotPrefs)
    {
        super(STEP_PLOT);

        this.perfLog = perfLog;
        this.perfIDs = perfIDs;
        this.perfMain = perfMain;
        this.plotPrefs = plotPrefs;

        // Create and display the step plot
        createPlot(perfMain.getStepPlotPanel(), false);
    }

    /**************************************************************************
     * Return the flag indicating if the CPU activity line is displayed
     * 
     * @return true if the CPU activity line is displayed
     *************************************************************************/
    protected boolean isHasActivityLine()
    {
        return hasActivityLine;
    }

    /**************************************************************************
     * Adjust the height and spacing of the subplots
     *************************************************************************/
    @Override
    protected void adjustPlotHeightAndSpacing()
    {
        // Set the chart size based on the number of plots, which in turns sets
        // the subplot size. A value is subtracted from the width to make room
        // for the vertical scroll bar (the actual value used makes no
        // difference so long as it's larger than what's needed to display the
        // scroll bar)
        getPlotChartPanel().setPreferredSize(new Dimension(MIN_WINDOW_WIDTH
                                                           - 100,
                                                           getPlotSeries().size()
                                                               * plotPrefs.getStepPlotHeight(false)));

        // Adjust the gap between the subplots. 1 is subtracted so that a zero
        // gap value actually equates to -1, which is the value required to
        // reduce the gap to its smallest value
        ((CombinedDomainXYPlot) getPlotChart().getPlot()).setGap(plotPrefs.getStepPlotSpacing(false) - 1);

        // Step through the subplots
        for (XYPlot plot : getPlots())
        {
            // Turn on/off subplot y-axis tick labels based on the user-
            // selectable flag
            plot.getRangeAxis(0).setTickLabelsVisible(plotPrefs.isStepPlotShowLabels(false));
        }

        // Enable the chart progress listener so that the spacing for the
        // start/end/delta fields and the x-axis scroll bar can be adjusted
        setChartProgressListenerEnable(true);
    }

    /**************************************************************************
     * Extend CombinedDomainXYPlot to override its panning methods so that the
     * y-axis can't be panned, and so that the label plot x-axis and scroll bar
     * follow the combined plot's x-axis panning
     *************************************************************************/
    private class CPMCombinedDomainXYPlot extends CombinedDomainXYPlot
    {
        /**********************************************************************
         * CPMCombinedDomainXYPlot constructor
         *********************************************************************/
        private CPMCombinedDomainXYPlot(ValueAxis domainAxis)
        {
            super(domainAxis);
        }

        /**********************************************************************
         * Pass the domain (x-axis) panning event through to its normal
         * handler, then adjust the x-axis scroll bar
         *********************************************************************/
        @Override
        public void panDomainAxes(double percent,
                                  PlotRenderingInfo info,
                                  Point2D source)
        {
            super.panDomainAxes(percent, info, source);

            // Pan the label plot's x-axis to match the combined plot's pan
            getLabelPlot().panDomainAxes(percent, info, source);

            // Adjust the x-axis scroll bar thumb size, position, and
            // increments
            adjustScrollBarIncrements();
        }

        /**********************************************************************
         * Override the normal range (y-axis) panning handler to prevent
         * panning of the y-axis
         *********************************************************************/
        @Override
        public void panRangeAxes(double panRange,
                                 PlotRenderingInfo info,
                                 Point2D source)
        {
        }
    }

    /**************************************************************************
     * Create the step combined XYPlot chart
     * 
     * @return Step plot chart
     *************************************************************************/
    @Override
    protected JFreeChart createPlotChart()
    {
        // Plot line and fill color
        Color color;

        // Set the x- and y-axis boundary values
        setXAxisMinMax();
        setYAxisMinMax();

        // Create a combined plot
        CPMCombinedDomainXYPlot combinedPlot = new CPMCombinedDomainXYPlot(getPlotDomainAxis());

        // Stack the subplots vertically
        combinedPlot.setOrientation(PlotOrientation.VERTICAL);

        // Allow panning of the combined plot's x-axis
        combinedPlot.setDomainPannable(true);

        // Create the renderers and data sets
        CPMXYStepRenderer rend[] = new CPMXYStepRenderer[getPlotSeries().size()];
        XYSeriesCollection dataset[] = new XYSeriesCollection[getPlotSeries().size()];

        // For each plot window
        for (int index = 0; index < getPlotSeries().size(); index++)
        {
            // Point to performance log data
            dataset[index] = new XYSeriesCollection();
            dataset[index].addSeries(getPlotSeries().get(index));

            // Draw the plot lines in steps (vs. point-to-point)
            rend[index] = new CPMXYStepRenderer(perfLog, plotPrefs);

            // Associate the tool tip generator with this subplot's renderer
            rend[index].setBaseToolTipGenerator(getToolTipGenerator());

            // Set the color to the ID color or the CPU activity line color, if
            // this line is present
            color = getLineColor().get(index);

            // Set the line color
            rend[index].setSeriesPaint(0, color);

            // Set the fill color by making the line color darker and the color
            // transparent (the outline color is used for filling)
            color = color.darker().darker().darker();
            color = new Color(color.getRed(),
                              color.getGreen(),
                              color.getBlue(),
                              170);
            rend[index].setSeriesOutlinePaint(0, color);

            // W/o this the plot window background is gray on the lower half
            getSymbolAxis().get(index).setGridBandsVisible(false);

            // Reorient y-axis labels to horizontal (angle is in radians)
            getSymbolAxis().get(index).setLabelAngle(Math.PI / 2.0);

            // Set y-axis label fonts
            getSymbolAxis().get(index).setLabelFont(PLOT_LABEL_FONT);
            getSymbolAxis().get(index).setTickLabelFont(PLOT_TICK_FONT);

            // Create the subplot
            getPlots().add(new XYPlot(dataset[index],
                                      null,
                                      getSymbolAxis().get(index),
                                      rend[index]));

            // Set the plot grid line visibility
            getPlots().get(index).setRangeGridlinesVisible(false);

            // Add the subplot to the combined plot stack
            combinedPlot.add(getPlots().get(index), 1);
        }

        // Set the plot window background and grid line colors
        setPlotColors(hasActivityLine);

        // Draw or hide the vertical grid lines
        setVerticalGridLines();

        // Initialize the x-axis boundary values used by the horizontal scroll
        // bar
        initXAxisMinMax();

        // Create the top level JFreeChart
        JFreeChart chart = new JFreeChart(combinedPlot);
        chart.removeLegend();

        // Set the background color to match the application's color scheme
        chart.setBackgroundPaint(perfMain.getStepPlotPanel().getBackground());

        return chart;
    }

    /**************************************************************************
     * Copy the log data to a XY series for plotting the individual ID events
     *************************************************************************/
    @Override
    protected void convertLogDataToXYSeries()
    {
        int seriesCount = 0;
        String[] yAxisLabel = new String[] {"Exit", "Entry"};

        // Remove the list arrays, if present
        getLineColor().removeAll(getLineColor());
        getPlotSeries().removeAll(getPlotSeries());
        getSymbolAxis().removeAll(getSymbolAxis());

        // Set the flag indicating that the CPU activity line should not be
        // shown
        hasActivityLine = false;

        // Create a series for each performance ID
        for (PerformanceID pid : perfIDs.getPerfIDList())
        {
            // Check if plotting flag is enabled for this ID
            if (pid.isPlotPerfID())
            {
                getPlotSeries().add(new XYSeries(pid.getPerfIDName(true)));

                // Set plot's y-axis label (use the ID if no ID name exists)
                // and the min/max tick labels
                getSymbolAxis().add(new SymbolAxis(pid.getPerfIDName(true),
                                                   yAxisLabel));

                // Add the ID's color to the line color array
                getLineColor().add(pid.getPerfIDColor());

                // Set up a temporary variable to shorten the subsequent
                // method calls
                XYSeries ps = getPlotSeries().get(seriesCount);

                int gapIndex = 0;
                boolean firstPoint = true;

                // Collect data for this ID from the log
                for (PerfLog log : perfLog.getPerfLogData())
                {
                    // Check if this is the first data point following a log
                    // data gap for the current ID
                    if (gapIndex < perfLog.getDataGapIndex().size()
                        && log.equals(perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(gapIndex))))
                    {
                        // Set the first point flag so that the next plotted
                        // point (is treated like an initial point
                        firstPoint = true;
                    }

                    // Check if the logged ID matches the series ID
                    if (log.getPerfID() == pid.getPerfID())
                    {
                        // Check if this is the first point for this ID or the
                        // first point following a gap between two logs
                        if (firstPoint)
                        {
                            // Reset the flag
                            firstPoint = false;

                            // Add a starting point at initial time stamp for
                            // the current log file's data. This is so that the
                            // plot line starts at the beginning of the x-axis
                            // for the first file and that the IDs' entry/exit
                            // statuses are reset following a data gap. It's
                            // assumed that an ID's initial state is 'entry' if
                            // its first logged point is 'exit', and vice versa
                            ps.add(ps.getItemCount() == 0
                                                         ? 0.0
                                                         : perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(gapIndex)).getTimeStamp(),
                                   log.isEntryEvent()
                                                     ? EXIT
                                                     : ENTRY,
                                   false);
                        }

                        // Add the data point to this ID's plot
                        ps.add(log.getTimeStamp(),
                               log.isEntryEvent()
                                                 ? ENTRY
                                                 : EXIT,
                               false);
                    }
                }

                // Check if data was found in the log for this ID
                if (ps.getItemCount() != 0)
                {
                    // Add an end point at the maximum time stamp so that the
                    // plot line ends at the end of the x-axis. The ID value
                    // used is the same as its last recorded value
                    ps.add(perfLog.getMaximumTimeStamp(),
                           ps.getY(ps.getItemCount() - 1).doubleValue());
                }

                seriesCount++;
            }
        }

        // Add the CPU activity subplot if more than one series is selected
        // for display (with only a single series displayed, the CPU plot is
        // identical to the displayed ID's plot), or if no IDs are selected
        // (so that a flat line can be drawn showing the CPU idle)
        if (seriesCount != 1)
        {
            // Create the CPU activity subplot line. This series checks the
            // displayed IDs' at each log entry and, if all are inactive (i.e.,
            // between an exit and entry event) then the CPU is shown as idle;
            // otherwise the CPU is shown as busy
            XYSeries cpuActivitySeries = new XYSeries("CPU Activity");

            // Set the CPU activity line color
            getLineColor().add(0, Color.WHITE);

            // Check that some IDs are selected to plot
            if (seriesCount != 0)
            {
                // Set the flag indicating that the CPU activity line should be
                // shown
                hasActivityLine = true;

                // Of the IDs to be plotted, determine the number of those that
                // are initially active (i.e., the ID's first event is an exit)
                int activeIDCount = perfLog.getActiveIDCount(-1);

                // Add the start point. If any IDs are active at the beginning
                // then start with the CPU showing busy; otherwise assume the
                // CPU is idle initially
                cpuActivitySeries.add(0.0,
                                      activeIDCount == 0
                                                        ? EXIT
                                                        : ENTRY,
                                      false);

                // Set the index into the data gap list to the first gap. If
                // only one log file is loaded then this is unused
                int gapIndex = 0;

                // Step through the log data
                for (PerfLog log : perfLog.getPerfLogData())
                {
                    // Get the performance ID associated with this log ID
                    PerformanceID pid = perfIDs.getPerfID(log.getPerfID());

                    // Check if this log entry is the first following a data
                    // gap between two logs
                    if (gapIndex < perfLog.getDataGapIndex().size()
                        && log.equals(perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(gapIndex))))
                    {
                        // Determine the number of IDs that are active at the
                        // beginning of this gap
                        activeIDCount = perfLog.getActiveIDCount(gapIndex);

                        gapIndex++;
                    }

                    // Check if this event's ID is set to be plotted
                    if (pid.isPlotPerfID())
                    {
                        // Adjust the active ID counter based on the event type
                        activeIDCount += log.isEntryEvent()
                                                           ? 1
                                                           : -1;

                        // Check if all events are off
                        if (activeIDCount == 0)
                        {
                            // Add a point to the CPU activity plot showing
                            // that the CPU is idle
                            cpuActivitySeries.add(log.getTimeStamp(),
                                                  EXIT,
                                                  false);
                        }
                        // Check if the active ID count went from 0 to 1, which
                        // implies that the CPU just became active
                        else if (activeIDCount == 1 && log.isEntryEvent())
                        {
                            // Add a point to the CPU activity plot showing
                            // that the CPU is active
                            cpuActivitySeries.add(log.getTimeStamp(),
                                                  ENTRY,
                                                  false);
                        }
                    }
                }
            }
            // No IDs are selected to plot
            else
            {
                // Plot the initial point showing the CPU is inactive. The end
                // point is handled below
                cpuActivitySeries.add(0.0, EXIT, false);
            }

            // Check that at least one data point has been added to the plot
            // line
            if (cpuActivitySeries.getItemCount() > 0)
            {
                // Add an end point so that the plot line fills the x-axis
                cpuActivitySeries.addOrUpdate(perfLog.getMaximumTimeStamp(),
                                              cpuActivitySeries.getY(cpuActivitySeries.getItemCount() - 1));

                // Insert the CPU activity series as the first one shown in the
                // combined plot
                getPlotSeries().add(0, cpuActivitySeries);
                getSymbolAxis().add(0, new SymbolAxis("CPU Activity ",
                                                      new String[] {"Idle", "Busy"}));
            }
        }

        // Set the amount to add to either side of the logged data minimum and
        // maximum x-axis values to provide some blank space between the edges
        // of the plot and the first/last data points
        setXPadding(perfLog.getMaximumTimeStamp());
    }
}
