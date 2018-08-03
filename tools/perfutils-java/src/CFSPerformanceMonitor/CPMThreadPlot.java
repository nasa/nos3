/**
 * CFS Performance Monitor Thread of Execution Plot
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.PLOT_LABEL_FONT;
import static CFSPerformanceMonitor.CPMConstants.THREAD_PLOT;
import static CFSPerformanceMonitor.CPMConstants.THREAD_POINT_SIZE;

import java.awt.Shape;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Point2D;
import java.util.ArrayList;
import java.util.List;

import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.SymbolAxis;
import org.jfree.chart.plot.DatasetRenderingOrder;
import org.jfree.chart.plot.PlotRenderingInfo;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;
import CFSPerformanceMonitor.CPMLogHandler.PerfLog;

/******************************************************************************
 * CFS Performance Monitor thread of execution plot class
 *****************************************************************************/
public class CPMThreadPlot extends CPMXYPlotHandler
{
    // List of plot y-axis labels
    private final List<String> yAxisLabel;

    // Thread of execution plot component
    private XYPlot plot;

    /**************************************************************************
     * Thread of execution plot class constructor
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
    public CPMThreadPlot(CPMLogHandler perfLog,
                         CPMIDHandler perfIDs,
                         CPMMain perfMain,
                         CPMPreferencesHandler plotPrefs)
    {
        super(THREAD_PLOT);

        this.perfLog = perfLog;
        this.perfIDs = perfIDs;
        this.perfMain = perfMain;
        this.plotPrefs = plotPrefs;

        // Create a list to store the plot y-axis labels
        yAxisLabel = new ArrayList<String>();

        // Create and display the thread of execution plot
        createPlot(perfMain.getThreadPlotPanel(), true);
    }

    /**************************************************************************
     * Return the list of y-axis labels
     * 
     * @return Y-axis label list
     *************************************************************************/
    protected List<String> getYAxisLabel()
    {
        return yAxisLabel;
    }

    /**************************************************************************
     * Show or hide the horizontal grid lines based on the user's selection
     *************************************************************************/
    protected void setHorizontalGridLines()
    {
        // Draw the vertical grid lines
        plot.setRangeMinorGridlinesVisible(plotPrefs.isShowPlotHorizontalGrid(false));
        plot.setRangeGridlinesVisible(plotPrefs.isShowPlotHorizontalGrid(false));
    }

    /**************************************************************************
     * Extend XYPlot to override its panning methods so that the x- and y-axes
     * scroll bars follow the plot's x- and y-axes panning
     *************************************************************************/
    @SuppressWarnings("serial")
    private class CPMXYPlot extends XYPlot
    {
        /**********************************************************************
         * CPMXYPlot constructor
         *********************************************************************/
        private CPMXYPlot()
        {
            super();
        }

        /**********************************************************************
         * Pass the domain (x-axis) panning event through to its normal
         * handler, then adjust the x-axis label plot and scroll bar
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
         * Pass the range (y-axis) panning event through to its normal handler,
         * then adjust the y-axis scroll bar
         *********************************************************************/
        @Override
        public void panRangeAxes(double panRange,
                                 PlotRenderingInfo info,
                                 Point2D source)
        {
            super.panRangeAxes(panRange, info, source);

            // Adjust the y-axis scroll bar thumb size, position, and
            // increments
            adjustScrollBarIncrements();
        }
    }

    /**************************************************************************
     * Create the thread of execution XYPlot chart
     * 
     * @return Thread of execution plot chart
     *************************************************************************/
    @Override
    protected JFreeChart createPlotChart()
    {
        // Set the x- and y-axis boundary values
        setXAxisMinMax();
        setYAxisMinMax();

        // Create the point data sets and renderers
        XYSeriesCollection dataset = new XYSeriesCollection();
        CPMXYLineAndShapeRenderer pointRend = new CPMXYLineAndShapeRenderer(false,
                                                                            true);
        CPMXYLineAndShapeRenderer lineRend = new CPMXYLineAndShapeRenderer(true,
                                                                           false);

        // Associate a tool tip generator with the point plotting renderer
        pointRend.setBaseToolTipGenerator(getToolTipGenerator());

        // Create the shape for the plotted data points
        Shape pointShape = new Ellipse2D.Double(-THREAD_POINT_SIZE / 2,
                                                -THREAD_POINT_SIZE / 2,
                                                THREAD_POINT_SIZE,
                                                THREAD_POINT_SIZE);

        // Create the plot. Even though there is a single thread plot, List is
        // needed so that the CPMXYPlotHandler methods, which require a List
        // object, can be used
        getPlots().add(new CPMXYPlot());
        plot = getPlots().get(0);

        // Create a data set for the thread line
        XYSeriesCollection threadDataset = new XYSeriesCollection();

        // Point to the thread of execution data
        threadDataset.addSeries(getPlotSeries().get(0));

        // Assign the thread line renderer
        plot.setRenderer(0, lineRend);

        // For each plotted performance ID
        for (int index = 1; index < getPlotSeries().size(); index++)
        {
            // Create a data set for the end points
            dataset.addSeries(getPlotSeries().get(index));

            // Assign the shape to the data point
            pointRend.setSeriesShape(index - 1, pointShape);

            // Set the point color using the ID color and assign the renderer
            pointRend.setSeriesPaint(index - 1, getLineColor().get(index - 1));
            plot.setRenderer(index, pointRend);
        }

        // W/o this the plot window background is gray on the lower half
        getSymbolAxis().get(0).setGridBandsVisible(false);

        // Set y-axis tick label font
        getSymbolAxis().get(0).setTickLabelFont(PLOT_LABEL_FONT);

        // Allow panning of the plot's x- and y-axes
        plot.setDomainPannable(true);
        plot.setRangePannable(true);

        // Assign the data and axes to the plot
        plot.setDataset(0, threadDataset);
        plot.setDataset(1, dataset);
        plot.setDomainAxis(getPlotDomainAxis());
        plot.setRangeAxis(getSymbolAxis().get(0));

        // Set the data set rendering order so that the ID plot points are
        // drawn over the thread line
        plot.setDatasetRenderingOrder(DatasetRenderingOrder.FORWARD);

        // Set the plot window background, grid line, and thread line colors
        setPlotColors(true);

        // Draw the horizontal and vertical grid lines based on the user's
        // selection
        setHorizontalGridLines();
        setVerticalGridLines();

        // Initialize the x-axis boundary values used by the horizontal scroll
        // bar
        initXAxisMinMax();

        // Initialize the y-axis boundary values used by the vertical scroll
        // bar
        initYAxisMinMax();

        // Create the top level JFreeChart
        JFreeChart chart = new JFreeChart(plot);
        chart.removeLegend();

        // Set the background color to match the application's color scheme
        chart.setBackgroundPaint(perfMain.getThreadPlotPanel().getBackground());

        return chart;
    }

    /**************************************************************************
     * Copy the log data to a XY series for plotting the thread of execution
     *************************************************************************/
    @Override
    protected void convertLogDataToXYSeries()
    {
        // Remove the list arrays, if present
        getLineColor().removeAll(getLineColor());
        yAxisLabel.removeAll(yAxisLabel);
        getPlotSeries().removeAll(getPlotSeries());

        // Create a series for the thread line
        getPlotSeries().add(new XYSeries(0));

        // Make a pass through the IDs to save the ID colors, set the label
        // names, and create the plot series in the order specified by the ID
        // list
        for (PerformanceID pid : perfIDs.getPerfIDList())
        {
            // Check if the plotting flag is enabled for this ID
            if (pid.isPlotPerfID())
            {
                // Add the ID's color to the line color array
                getLineColor().add(pid.getPerfIDColor());

                // Use the ID name (or ID, if the name is unknown) for the
                // y-axis label
                yAxisLabel.add(pid.getPerfIDName(true));

                // Create a series for each performance ID
                getPlotSeries().add(new XYSeries(pid.getPerfIDName(true)));
            }
        }

        // Check if at least one ID is to be plotted
        if (getPlotSeries().size() > 1)
        {
            // Set the plot's y-axis tick labels (first converting the List to
            // an array). There's only a single symbol axis for the thread
            // plot, but a List is used so that the CPMXYPlotHandler methods
            // can be used
            getSymbolAxis().add(new SymbolAxis(null,
                                               yAxisLabel.toArray(new String[yAxisLabel.size()])));

            // Collect data from the log to populate the plot series for each
            // individual ID (in order to plot the end points) and for the
            // thread of execution line
            for (PerfLog log : perfLog.getPerfLogData())
            {
                int idIndex = 0;

                // Find this ID in the ID list
                for (PerformanceID pid : perfIDs.getPerfIDList())
                {
                    // Check if the plotting flag is enabled for this ID
                    if (pid.isPlotPerfID())
                    {
                        // Check if the log ID matches one in the ID list
                        if (log.getPerfID() == pid.getPerfID())
                        {
                            // Base the y-value on the ID index (to match up
                            // with the y-axis tick labels). Bias up or down
                            // based on if this is an entry or exit event
                            double yValue = log.isEntryEvent()
                                                              ? idIndex + 0.25
                                                              : idIndex - 0.25;

                            // Add the data point to this ID's plot line and
                            // the thread plot line
                            getPlotSeries().get(idIndex + 1).add(log.getTimeStamp(),
                                                                 yValue,
                                                                 false);
                            getPlotSeries().get(0).add(log.getTimeStamp(),
                                                       yValue,
                                                       false);

                            // Since the ID matched, quit looking through the
                            // ID list and go to the next log entry
                            break;
                        }

                        // Increment the ID index
                        idIndex++;
                    }
                }
            }
        }
        // No IDs are selected to plot
        else
        {
            // Create a dummy symbol axis so that the plot won't generate an
            // exception when displayed
            getSymbolAxis().add(new SymbolAxis(null, new String[] {""}));
        }

        // Set the amount to add to either side of the logged data minimum and
        // maximum x-axis values to provide some blank space between the edges
        // of the plot and the first/last data points
        setXPadding(perfLog.getMaximumTimeStamp());
    }
}
