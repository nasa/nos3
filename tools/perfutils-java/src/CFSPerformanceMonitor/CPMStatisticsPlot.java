/**
 * CFS Performance Monitor Statistics Plot
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.BAR;
import static CFSPerformanceMonitor.CPMConstants.BY_NAME;
import static CFSPerformanceMonitor.CPMConstants.BY_VALUE;
import static CFSPerformanceMonitor.CPMConstants.CELL_FONT;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LABEL_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LABEL_VERTICAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.MINIMUM_TIME_DELTA;
import static CFSPerformanceMonitor.CPMConstants.PIE;
import static CFSPerformanceMonitor.CPMConstants.PLOT_LABEL_FONT;
import static CFSPerformanceMonitor.CPMConstants.PLOT_SCROLL_BAR_PAD;
import static CFSPerformanceMonitor.CPMConstants.STATS_Y_LABEL_FONT;
import static CFSPerformanceMonitor.CPMConstants.TIME_PRECISION;
import static CFSPerformanceMonitor.CPMConstants.X_AXIS_LABEL_FORMAT;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Paint;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollBar;
import javax.swing.JSeparator;
import javax.swing.SwingConstants;
import javax.swing.border.BevelBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.AxisLocation;
import org.jfree.chart.axis.CategoryAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.event.ChartProgressEvent;
import org.jfree.chart.event.ChartProgressListener;
import org.jfree.chart.labels.StandardCategoryToolTipGenerator;
import org.jfree.chart.labels.StandardPieToolTipGenerator;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PiePlot3D;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.PlotRenderingInfo;
import org.jfree.chart.renderer.category.BarRenderer;
import org.jfree.chart.renderer.category.CategoryItemRenderer;
import org.jfree.chart.renderer.category.GradientBarPainter;
import org.jfree.data.category.CategoryDataset;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.category.SlidingCategoryDataset;
import org.jfree.data.general.DefaultPieDataset;
import org.jfree.util.SortOrder;

import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;

/******************************************************************************
 * CFS Performance Monitor statistics plot class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMStatisticsPlot extends JFrame
{
    private final CPMLogHandler perfLog;
    private final CPMIDHandler perfIDs;
    private final CPMMain perfMain;
    private final CPMPreferencesHandler plotPrefs;

    // Components that need to be accessed by other classes or listeners
    private JComboBox statsComboBox;
    private JCheckBox cpuIdleCheckbox;
    private JRadioButton sortNameButton;
    private JRadioButton sortValueButton;
    private JRadioButton chartPieButton;
    private JRadioButton chartBarButton;
    private PiePlot3D piePlot;
    private CPMChartPanel pieChartPanel;
    private CPMChartPanel barChartPanel;
    private JPanel barPanel;
    private CPMCategoryPlot barPlot;
    private JScrollBar barScrollBar;
    private JPanel controlAndChartPanel;

    // Bar chart height, pixels
    private int barChartHeight;

    // Number formats for the plot tool tips
    private NumberFormat numFormat;
    private NumberFormat pctFormat;

    // Array of displayed performance ID colors
    private final List<Paint> idColors;

    // Maximum value of the selected statistic, used for setting the bar chart
    // range axis maximum
    private Double maxXAxisValue;

    // Data storage for the statistics pie charts
    private final DefaultPieDataset pieDataset;

    // Data storage for the statistics bar charts
    private final DefaultCategoryDataset barDataset;

    // Statistics plot currently selected by the user from the Plots |
    // Statistics sub-menu or combo box
    private static int selectedStat = 0;

    // Flag that is set to true to ignore the statistics plot type combo box
    // action listener
    private boolean ignoreEvent;

    // Statistics plot names. This array is defined as static so that the
    // Preferences dialog can use the same values to create its plot selection
    // combo box
    protected static final String[] PLOT_NAMES = new String[]
    {
     "ID Time Active",
     "ID Entry Events",
     "Avg Frequency",
     "Min Time Active",
     "Max Time Active",
     "Min Interval",
     "Max Interval",
     "Min Overruns",
     "Max Overruns"
    };

    // Statistics plot units
    private final String[] PLOT_UNITS = new String[]
    {
     "sec",
     "evts",
     "evt/sec",
     "sec",
     "sec",
     "sec",
     "sec",
     "sec",
     "sec"
    };

    // Precision of data values for formatting tool tips
    private final int[] PLOT_FRAC = new int[]
    {
     TIME_PRECISION,
     0,
     2,
     TIME_PRECISION,
     TIME_PRECISION,
     TIME_PRECISION,
     TIME_PRECISION,
     TIME_PRECISION,
     TIME_PRECISION
    };

    // Plot identifiers
    private final int TIME_ON = 0;
    private final int EVENT_ON = 1;
    private final int AVG_FREQ = 2;
    private final int MIN_TIME_ON = 3;
    private final int MAX_TIME_ON = 4;
    private final int MIN_INTERVAL = 5;
    private final int MAX_INTERVAL = 6;
    private final int MIN_OVERRUN = 7;
    private final int MAX_OVERRUN = 8;
    protected static final int NUM_PLOTS = 9;

    /**************************************************************************
     * This class is used to override the BarRenderer bar display color so that
     * the bars are displayed in the user-selected ID colors
     *************************************************************************/
    private class CPMBarRenderer extends BarRenderer
    {
        /**********************************************************************
         * Get the color for the specified item (performance ID)
         *********************************************************************/
        @Override
        public Paint getItemPaint(int row, int column)
        {
            // Get the ID color for the given bar. The scroll bar position
            // determines the starting index into the ID color array, and the
            // column value provides the offset from the uppermost bar (ID)
            // visible
            return idColors.get(barScrollBar.getValue() + column);
        }
    }

    /**************************************************************************
     * Statistics plot class constructor
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
    protected CPMStatisticsPlot(CPMLogHandler perfLog,
                                CPMIDHandler perfIDs,
                                CPMMain perfMain,
                                CPMPreferencesHandler plotPrefs)
    {
        this.perfLog = perfLog;
        this.perfIDs = perfIDs;
        this.perfMain = perfMain;
        this.plotPrefs = plotPrefs;

        pieDataset = new DefaultPieDataset();
        barDataset = new DefaultCategoryDataset();
        idColors = new ArrayList<Paint>();
        barChartHeight = 0;
        ignoreEvent = false;

        // Initialize the plot window
        createPlots();
    }

    /**************************************************************************
     * Get the currently selected statistics plot
     * 
     * @return Currently selected statistics plot
     *************************************************************************/
    protected static int getSelectedPlot()
    {
        return selectedStat;
    }

    /**************************************************************************
     * Set the currently selected statistics plot
     * 
     * @param Selected
     *            statistics plot
     *************************************************************************/
    protected static void setSelectedPlot(int plot)
    {
        selectedStat = plot;
    }

    /**************************************************************************
     * Create the statistics pie and bar chart plots
     *************************************************************************/
    private void createPlots()
    {
        // Since this could take a while, show the "wait" mouse pointer to
        // alert the user. The pointer is restored to the default when the plot
        // display completes
        perfMain.setMainCursor(Cursor.WAIT_CURSOR);

        // Create and arrange the plot window components
        createPlotWindowComponents();

        // Create the statistics pie chart
        createPieChart();

        // Create the statistics bar chart
        createBarChart();

        // Sort the IDs by name or value based on user-selected preference
        setSortOrder(plotPrefs.getSortOrder(false));

        // Select the statistics plot to display. This method also creates the
        // pie and bar chart data sets
        selectStatsPlot(selectedStat);

        // Select the type of plot for displaying the statistics data
        selectStatsChartType(chartPieButton.isSelected()
                                                        ? PIE
                                                        : BAR);

        // Place the panel that contains the controls and chart into the main
        // window frame
        perfMain.getStatsPlotPanel().add(controlAndChartPanel);

        // Set the size of the x-axis label plot pane. Validate causes the
        // frame to determine the sizes & positions of its components before
        // they're displayed
        perfMain.getStatsPlotPanel().validate();

        // Draw the plots, etc. in the main application window
        perfMain.getStatsPlotPanel().setVisible(true);
    }

    /**************************************************************************
     * Create the statistics pie chart
     *************************************************************************/
    private void createPieChart()
    {
        // Create the pie chart. Default to the first data set
        JFreeChart pieChart = ChartFactory.createPieChart3D(null,
                                                            pieDataset,
                                                            false,
                                                            true,
                                                            false);

        // Set pie chart characteristics and create the chart panel
        pieChartPanel = setChartCharacteristics(pieChart);

        // Set the pie chart characteristics
        piePlot = (PiePlot3D) pieChart.getPlot();
        piePlot.setOutlineVisible(false);
        piePlot.setDarkerSides(true);
        piePlot.setBackgroundPaint(pieChartPanel.getBackground());

        // Step through the ID list
        for (PerformanceID pid : perfIDs.getPerfIDList())
        {
            // Check if this ID is selected to plot
            if (pid.isPlotPerfID())
            {
                // Set the pie section color to the ID color
                piePlot.setSectionPaint(pid.getPerfIDName(true),
                                        pid.getPerfIDColor());
            }
        }

        // Set up the constant tool tip format parameters
        numFormat = NumberFormat.getNumberInstance();
        pctFormat = NumberFormat.getPercentInstance();
        pctFormat.setMinimumFractionDigits(3);

        // Listen for and take action on key presses
        createPieChartKeyListener();

        // Listen for and take action on chart (re)draw progress
        createPieChartProgressListener();
    }

    /**************************************************************************
     * Create a listener for key presses in the pie chart
     *************************************************************************/
    private void createPieChartKeyListener()
    {
        // Monitor the plot window for key presses
        pieChartPanel.addKeyListener(new KeyAdapter()
        {
            /******************************************************************
             * Handle a key press event
             *****************************************************************/
            @Override
            public void keyPressed(KeyEvent ke)
            {
                // Check if the '+' key is pressed
                if (ke.getKeyChar() == '+')
                {
                    // Rotate the pie chart clockwise
                    piePlot.setStartAngle(piePlot.getStartAngle() - 3);
                }
                // Check if the '-' key is pressed
                else if (ke.getKeyChar() == '-')
                {
                    // Rotate the pie chart counterclockwise
                    piePlot.setStartAngle(piePlot.getStartAngle() + 3);
                }
            }
        });
    }

    /**************************************************************************
     * Create a pie chart progress listener
     *************************************************************************/
    private void createPieChartProgressListener()
    {
        // Add a listener for pie chart (re)draw progress
        pieChartPanel.getChart().addProgressListener(new ChartProgressListener()
        {
            /******************************************************************
             * Handle a pie chart (re)draw event
             *****************************************************************/
            @Override
            public void chartProgress(ChartProgressEvent cpe)
            {
                // Check if the plot has completed being drawn
                if (cpe.getType() == ChartProgressEvent.DRAWING_FINISHED)
                {
                    // Request focus for the plot window so that key presses
                    // can be detected. This call must be made after the plot
                    // window is displayed
                    pieChartPanel.requestFocusInWindow();
                }
            }
        });
    }

    /**************************************************************************
     * Extend CategoryPlot to override its y-axis panning method so that the
     * y-axis scroll bar follows the plot's y-axis panning
     *************************************************************************/
    private class CPMCategoryPlot extends CategoryPlot
    {
        private double accumulator;

        /**********************************************************************
         * CPMCategoryPlot constructor
         *********************************************************************/
        public CPMCategoryPlot(CategoryDataset dataset,
                               CategoryAxis domainAxis,
                               ValueAxis rangeAxis,
                               CategoryItemRenderer renderer)
        {
            super(dataset, domainAxis, rangeAxis, renderer);

            accumulator = 0.0;
        }

        /**********************************************************************
         * Pass the domain (y-axis) panning event through to its normal
         * handler, then adjust the y-axis scroll bar
         *********************************************************************/
        @Override
        public void panDomainAxes(double percent,
                                  PlotRenderingInfo info,
                                  Point2D source)
        {
            super.panDomainAxes(percent, info, source);

            // Get a reference to the bar chart x-axis to shorten subsequent
            // calls
            ValueAxis axis = barPlot.getRangeAxis();

            // Check if the lower boundary value is less than the minimum lower
            // boundary value
            if (axis.getLowerBound() < 0.0)
            {
                // Set the lower boundary value to the minimum
                axis.setLowerBound(0.0);
            }

            // Check if the upper boundary value is less than the maximum upper
            // boundary value
            if (axis.getUpperBound() > maxXAxisValue)
            {
                // Set the upper boundary value to the maximum
                axis.setUpperBound(maxXAxisValue);
            }

            // Multiply the percentage change by an adjustment factor and add
            // the result to the running change total
            accumulator += percent * 50.0;

            // Convert the running change to an integer, which truncates the
            // fractional portion
            int delta = (int) accumulator;

            // Check if the difference in position has reached 1 or -1
            if (delta != 0)
            {
                // Adjust the running total to keep it near zero
                accumulator -= delta;
            }

            // Determine the new y-axis scroll bar thumb position from the
            // y-axis pan percentage change
            barScrollBar.setValue(barScrollBar.getValue() + delta);

            // Update the y-axis based on the scroll bar thumb position
            adjustYAxisFromScrollBar();
        }
    }

    /**************************************************************************
     * Update the y-axis based on the scroll bar thumb position
     *************************************************************************/
    private void adjustYAxisFromScrollBar()
    {
        // Check that at least one ID is set to be displayed. If none are
        // selected then the call below results in an error if allowed to
        // execute
        if (barDataset.getColumnCount() != 0)
        {
            // Set the first ID displayed based on the scroll bar's thumb
            // position
            ((SlidingCategoryDataset) barPlot.getDataset()).setFirstCategoryIndex(barScrollBar.getValue());
        }
    }

    /**************************************************************************
     * Create the statistics bar chart
     *************************************************************************/
    private void createBarChart()
    {
        // Create the domain axis (performance ID names) and set its sizing
        // characteristics and font
        CategoryAxis domainAxis = new CategoryAxis(null);
        domainAxis.setMaximumCategoryLabelWidthRatio(0.8f);
        domainAxis.setLowerMargin(0.02);
        domainAxis.setUpperMargin(0.02);
        domainAxis.setTickLabelFont(STATS_Y_LABEL_FONT);

        // Create the range axis (statistics values) and set its tick label
        // format, minor grid line count, and font
        ValueAxis rangeAxis = new NumberAxis(PLOT_UNITS[0]);
        ((NumberAxis) rangeAxis).setNumberFormatOverride(new DecimalFormat(X_AXIS_LABEL_FORMAT));
        rangeAxis.setMinorTickCount(5);
        rangeAxis.setTickLabelFont(PLOT_LABEL_FONT);

        // Create a renderer to allow coloring the bars based on the user-
        // selected ID colors. Enable tool tips
        CPMBarRenderer renderer = new CPMBarRenderer();
        renderer.setBarPainter(new GradientBarPainter());
        renderer.setShadowVisible(false);
        renderer.setDrawBarOutline(true);
        renderer.setBaseOutlinePaint(Color.BLACK);
        renderer.setBaseToolTipGenerator(new StandardCategoryToolTipGenerator());

        // Create the bar chart plot. Default to the first data set and 10 IDs
        // displayed. Display the bars horizontally
        barPlot = new CPMCategoryPlot(new SlidingCategoryDataset(barDataset,
                                                                 0,
                                                                 10),
                                      domainAxis,
                                      rangeAxis,
                                      renderer);
        barPlot.setOrientation(PlotOrientation.HORIZONTAL);

        // Position the statistics value label at the bottom of the chart and
        // make the minor grid lines visible
        barPlot.setRangeAxisLocation(AxisLocation.BOTTOM_OR_LEFT);
        barPlot.setRangeMinorGridlinesVisible(true);

        // Set the range (x-axis; values) so it can be panned. The CategoryPlot
        // class is extended to allow panning of the domain (y-axis; names)
        barPlot.setRangePannable(true);

        // Create the bar chart
        JFreeChart barChart = new JFreeChart(null, null, barPlot, false);

        // Set bar chart characteristics and create the chart panel
        barChartPanel = setChartCharacteristics(barChart);

        // Create a scroll bar
        barScrollBar = new JScrollBar(SwingConstants.VERTICAL);

        // Add a listener for scroll bar thumb position changes
        barScrollBar.getModel().addChangeListener(new ChangeListener()
        {
            /******************************************************************
             * Handle scroll bar thumb position changes
             *****************************************************************/
            @Override
            public void stateChanged(ChangeEvent ce)
            {
                // Update the y-axis based on the scroll bar thumb position
                adjustYAxisFromScrollBar();
            }
        });

        // Adjust the chart y-axis based on the mouse wheel movement
        barScrollBar.addMouseWheelListener(new MouseWheelListener()
        {
            /******************************************************************
             * Handle a y-axis mouse wheel movement event
             *****************************************************************/
            @Override
            public void mouseWheelMoved(MouseWheelEvent mwe)
            {
                // Adjust the new y-axis scroll bar thumb position by one unit
                // scroll amount based on mouse wheel rotation direction. The
                // scroll "speed" is reduced by half to make finer adjustment
                // possible
                barScrollBar.setValue(barScrollBar.getValue()
                                      + barScrollBar.getUnitIncrement()
                                      * (mwe.getUnitsToScroll() > 0
                                                                   ? 1
                                                                   : -1));
            }
        });

        // Create a panel for the bar chart scroll bar
        JPanel scrollBarPanel = new JPanel(new BorderLayout());
        scrollBarPanel.add(barScrollBar);
        scrollBarPanel.setBackground(barChartPanel.getBackground());

        // Create a panel to hold the bar chart and its associated scroll bar
        barPanel = new JPanel(new BorderLayout());
        barPanel.add(barChartPanel);
        barPanel.add(scrollBarPanel, BorderLayout.EAST);

        // Set the bar chart background and grid line colors
        setPlotColors();

        // Display/hide the bar chart grid lines
        setVerticalGridLines();

        // Listen for and take action on key presses
        createBarChartKeyListener();

        // Listen for and take action on chart (re)draw progress
        createBarChartProgressListener();

        // Listen for and take action on mouse wheel events
        createBarChartMouseWheelListener();
    }

    /**************************************************************************
     * Create a listener for key presses in the bar chart
     *************************************************************************/
    private void createBarChartKeyListener()
    {
        // Monitor the plot window for key presses
        barChartPanel.addKeyListener(new KeyAdapter()
        {
            /******************************************************************
             * Handle a key press event
             *****************************************************************/
            @Override
            public void keyPressed(KeyEvent ke)
            {
                // Check if the '+' key is pressed
                if (ke.getKeyChar() == '+')
                {
                    // Zoom in, centered on the plot window
                    zoomBarChart(0.5);
                }
                // Check if the '-' key is pressed
                else if (ke.getKeyChar() == '-')
                {
                    // Zoom out, centered on the plot window
                    zoomBarChart(2.0);
                }
                // Check for key presses relevant to the y-axis scroll bar
                else
                {
                    // Check if the shift and up arrow keys are pressed
                    // together
                    if (ke.isShiftDown()
                        && ke.getKeyCode() == KeyEvent.VK_UP)
                    {
                        // Create and send an event to block scroll the plot
                        // vertically upwards
                        barScrollBar.dispatchEvent(new KeyEvent(barScrollBar,
                                                                ke.getID(),
                                                                ke.getWhen(),
                                                                0,
                                                                KeyEvent.VK_PAGE_UP,
                                                                KeyEvent.CHAR_UNDEFINED));
                    }
                    // Check if the shift and down arrow keys are pressed
                    // together
                    else if (ke.isShiftDown()
                             && ke.getKeyCode() == KeyEvent.VK_DOWN)
                    {
                        // Create and send an event to block scroll the plot
                        // vertically downwards
                        barScrollBar.dispatchEvent(new KeyEvent(barScrollBar,
                                                                ke.getID(),
                                                                ke.getWhen(),
                                                                0,
                                                                KeyEvent.VK_PAGE_DOWN,
                                                                KeyEvent.CHAR_UNDEFINED));
                    }
                    // Check if the up, down, page up, or page down arrow key
                    // is pressed
                    else if (ke.getKeyCode() == KeyEvent.VK_UP
                             || ke.getKeyCode() == KeyEvent.VK_DOWN
                             || ke.getKeyCode() == KeyEvent.VK_PAGE_UP
                             || ke.getKeyCode() == KeyEvent.VK_PAGE_DOWN)
                    {
                        // Scroll the plot vertically
                        barScrollBar.dispatchEvent(ke);
                    }
                }
            }
        });
    }

    /**************************************************************************
     * Create a bar chart progress listener
     *************************************************************************/
    private void createBarChartProgressListener()
    {
        // Add a listener for bar chart (re)draw progress
        barChartPanel.getChart().addProgressListener(new ChartProgressListener()
        {
            /******************************************************************
             * Handle a bar chart (re)draw event
             *****************************************************************/
            @Override
            public void chartProgress(ChartProgressEvent cpe)
            {
                // Check if the plot has completed being drawn
                if (cpe.getType() == ChartProgressEvent.DRAWING_FINISHED)
                {
                    // Check if the plot height has changed
                    if (barChartHeight != controlAndChartPanel.getHeight())
                    {
                        // Store the new plot height
                        barChartHeight = controlAndChartPanel.getHeight();

                        // Calculate the number of bars to display based on the
                        // height of the plot window, the label font height,
                        // plus some padding
                        int numBars = barChartHeight
                                      / (getFontMetrics(CELL_FONT).getHeight()
                                      + 10);

                        // Set the number of IDs visible based on main window
                        // height
                        ((SlidingCategoryDataset) barPlot.getDataset()).setMaximumCategoryCount(numBars);

                        // Scroll bar needs adjustment, too
                        barScrollBar.setVisibleAmount(numBars);

                        // Check if the scroll bar thumb extends past the
                        // bottom of the resized scroll bar
                        if (barScrollBar.getValue() + numBars > barDataset.getColumnCount())
                        {
                            // Adjust the scroll bar thumb position so that it
                            // remains within bounds
                            barScrollBar.setValue(barDataset.getColumnCount()
                                                  - numBars);
                        }

                        // Size and position the bar chart scroll bar based on
                        // the size of the chart. This has to be done after the
                        // chart is drawn since the values aren't known until
                        // this point
                        ((JPanel) barScrollBar.getParent()).setBorder(BorderFactory.createEmptyBorder((int) barChartPanel.getScreenDataArea().getY(),
                                                                                                      0,
                                                                                                      controlAndChartPanel.getHeight()
                                                                                                          - ((int) barChartPanel.getScreenDataArea().getHeight()
                                                                                                             + (int) barChartPanel.getScreenDataArea().getY()
                                                                                                             + 3),
                                                                                                      PLOT_SCROLL_BAR_PAD));
                    }

                    // Request focus for the plot window so that key presses
                    // can be detected. This call must be made after the plot
                    // window is displayed
                    barChartPanel.requestFocusInWindow();
                }
            }
        });
    }

    /**************************************************************************
     * Create a listener for bar chart mouse wheel events
     *************************************************************************/
    private void createBarChartMouseWheelListener()
    {
        // Monitor the bar chart window for mouse wheel changes
        barChartPanel.addMouseWheelListener(new MouseWheelListener()
        {
            /******************************************************************
             * Handle a mouse wheel movement event
             *****************************************************************/
            @Override
            public void mouseWheelMoved(MouseWheelEvent mwe)
            {
                // Check if the wheel was rotated "down"
                if (mwe.getWheelRotation() < 0)
                {
                    // Zoom in
                    zoomBarChart(0.75);
                }
                // The wheel was rotated "up"
                else
                {
                    // Zoom out
                    zoomBarChart(1.0 / 0.75);
                }
            }
        });
    }

    /**************************************************************************
     * Set common chart characteristics and create the chart panel
     *************************************************************************/
    private CPMChartPanel setChartCharacteristics(JFreeChart chart)
    {
        // Create the chart panel
        CPMChartPanel chartPanel = new CPMChartPanel(chart,
                                                     false,
                                                     true,
                                                     true,
                                                     false,
                                                     true);

        // Set the color of the area surrounding the chart to the default
        chart.setBackgroundPaint(chartPanel.getBackground());

        // Turn on anti-aliasing to smooth the plotted data and text
        chart.setAntiAlias(true);

        // Detect when the chart window has been completely drawn
        chart.addProgressListener(new ChartProgressListener()
        {
            /******************************************************************
             * Handle a chart progress event
             *****************************************************************/
            @Override
            public void chartProgress(ChartProgressEvent event)
            {
                // Check if the chart has completed being drawn
                if (event.getType() == ChartProgressEvent.DRAWING_FINISHED)
                {
                    // Restore the default mouse pointer
                    perfMain.setMainCursor(Cursor.DEFAULT_CURSOR);
                }
            }
        });

        // Set the min/max drawing height to keep the font from being resized
        // when the chart is resized
        chartPanel.setMaximumDrawHeight(10000);
        chartPanel.setMinimumDrawHeight(10);
        chartPanel.setMaximumDrawWidth(10000);
        chartPanel.setMinimumDrawWidth(10);

        // Allow pie chart rotation and bar chart zooming and unzooming using
        // the mouse wheel
        chartPanel.setMouseWheelEnabled(true);

        // Set the delay (in milliseconds) for showing the data tool tip
        chartPanel.setInitialDelay(100);

        // Allow the keyboard focus to select the chart
        chartPanel.setFocusable(true);

        return chartPanel;
    }

    /**************************************************************************
     * Create the components that make up and arrange the plot window
     *************************************************************************/
    private void createPlotWindowComponents()
    {
        // Set the initial layout manager characteristics
        GridBagConstraints gbc = new GridBagConstraints(0,
                                                        1,
                                                        GridBagConstraints.REMAINDER,
                                                        1,
                                                        1.0,
                                                        0.0,
                                                        GridBagConstraints.LINE_START,
                                                        GridBagConstraints.NONE,
                                                        new Insets(LABEL_VERTICAL_SPACING,
                                                                   LABEL_HORIZONTAL_SPACING,
                                                                   0,
                                                                   LABEL_HORIZONTAL_SPACING),
                                                        LABEL_HORIZONTAL_SPACING,
                                                        0);

        // Create a panel to hold the plot controls
        JPanel controlsPanel = new JPanel(new GridBagLayout());

        // Use an outer panel so that the components can be easily forced
        // to the top of the controls area
        JPanel controlsOuterPanel = new JPanel(new BorderLayout());

        // Put a border between the controls and the plot
        controlsOuterPanel.setBorder(BorderFactory.createMatteBorder(0,
                                                                     0,
                                                                     0,
                                                                     1,
                                                                     Color.GRAY));
        controlsOuterPanel.add(controlsPanel, BorderLayout.PAGE_START);

        // Set the sub-panel initial layout manager characteristics
        GridBagConstraints subgbc = new GridBagConstraints(0,
                                                           0,
                                                           1,
                                                           1,
                                                           1.0,
                                                           0.0,
                                                           GridBagConstraints.LINE_START,
                                                           GridBagConstraints.NONE,
                                                           new Insets(0, 0, 0, 0),
                                                           LABEL_HORIZONTAL_SPACING,
                                                           0);

        // Create a sub-panel to hold the selection controls. This allows the
        // sub-panel's components to be positioned as desired relative to the
        // other components in the controls panel
        JPanel selectPanel = new JPanel(new GridBagLayout());

        // Create a label for plot selection and place it on the sub-panel
        JLabel selectLabel = new JLabel("Select plot");
        selectLabel.setFont(LABEL_FONT_BOLD);
        selectPanel.add(selectLabel, subgbc);

        // Create a combo box for selecting which statistic to plot
        statsComboBox = new JComboBox(PLOT_NAMES);
        statsComboBox.setFont(LABEL_FONT_PLAIN);
        statsComboBox.setForeground(Color.BLACK);
        statsComboBox.setBackground(Color.WHITE);
        statsComboBox.setBorder(BorderFactory.createBevelBorder(BevelBorder.LOWERED,
                                                                Color.LIGHT_GRAY,
                                                                Color.GRAY));
        statsComboBox.setMaximumRowCount(NUM_PLOTS);
        subgbc.insets.left = LABEL_HORIZONTAL_SPACING;
        subgbc.gridy++;
        selectPanel.add(statsComboBox, subgbc);
        controlsPanel.add(selectPanel, gbc);

        // Listen for combo box selection changes
        statsComboBox.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Select the plot to display based on the combo box selection
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Check if this event is not to be ignored
                if (!ignoreEvent)
                {
                    // Since this could take a while, show the "wait" mouse
                    // pointer to alert the user. The pointer is restored to
                    // the default when the plot display completes
                    perfMain.setMainCursor(Cursor.WAIT_CURSOR);

                    // Save the index of the plot selected, then select that
                    // plot
                    selectedStat = statsComboBox.getSelectedIndex();
                    selectStatsPlot(selectedStat);
                }
            }
        });

        // Add a separator
        JSeparator separator1 = new JSeparator(JSeparator.HORIZONTAL);
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.insets.left = LABEL_HORIZONTAL_SPACING;
        gbc.gridx = 0;
        gbc.gridy++;
        controlsPanel.add(separator1, gbc);
        gbc.fill = GridBagConstraints.NONE;

        // Create radio buttons for displaying the statistics in pie or bar
        // chart format
        chartPieButton = new JRadioButton("Pie",
                                          (plotPrefs.getStatChartType(false) == PIE));
        chartPieButton.setName(Integer.toString(PIE));
        chartPieButton.setFont(LABEL_FONT_BOLD);
        chartBarButton = new JRadioButton("Bar",
                                          (plotPrefs.getStatChartType(false) == BAR));
        chartBarButton.setName(Integer.toString(BAR));
        chartBarButton.setFont(LABEL_FONT_BOLD);

        // Add the radio buttons to a group so that only one is active at a
        // time
        ButtonGroup chartGroup = new ButtonGroup();
        chartGroup.add(chartPieButton);
        chartGroup.add(chartBarButton);

        JLabel chartLabel = new JLabel("Chart type");
        chartLabel.setFont(LABEL_FONT_BOLD);
        gbc.gridwidth = 1;
        gbc.weightx = 0.0;
        gbc.gridy++;
        controlsPanel.add(chartLabel, gbc);
        gbc.insets.left = LABEL_HORIZONTAL_SPACING * 2;
        gbc.gridy++;
        controlsPanel.add(chartPieButton, gbc);
        gbc.insets.top = 0;
        gbc.gridy++;
        controlsPanel.add(chartBarButton, gbc);

        // Listener for chart type radio button selection changes
        ActionListener chartListener = new ActionListener()
        {
            /******************************************************************
             * Handle a sort order radio button selection change
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Get the "name" of the radio button selected (PIE or BAR)
                int chartType = Integer.valueOf(((JRadioButton) ae.getSource()).getName());

                // Check if the chart type selected differs from the one
                // currently in use
                if (plotPrefs.getStatChartType(false) != chartType)
                {
                    // Update the chart type
                    selectStatsChartType(chartType);
                }
            }
        };

        // Listen for chart type radio button selection changes
        chartPieButton.addActionListener(chartListener);
        chartBarButton.addActionListener(chartListener);

        // Add a separator
        JSeparator separator2 = new JSeparator(JSeparator.HORIZONTAL);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.insets.left = LABEL_HORIZONTAL_SPACING;
        gbc.insets.top = LABEL_VERTICAL_SPACING;
        gbc.gridy++;
        controlsPanel.add(separator2, gbc);
        gbc.fill = GridBagConstraints.NONE;

        // Create a checkbox for showing/hiding the CPU idle time. Initialize
        // the box to the Preferences dialog selection
        cpuIdleCheckbox = new JCheckBox("Show CPU idle time",
                                        plotPrefs.isShowCPUIdle(false));
        cpuIdleCheckbox.setHorizontalAlignment(JCheckBox.LEFT);
        cpuIdleCheckbox.setFont(LABEL_FONT_BOLD);
        gbc.gridy++;
        controlsPanel.add(cpuIdleCheckbox, gbc);

        // Listen for CPU Idle check box selection changes
        cpuIdleCheckbox.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Show/hide the CPU idle data based on the check box selection
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                setCPUIdleTime(cpuIdleCheckbox.isSelected());
            }
        });

        // Add a separator
        JSeparator separator3 = new JSeparator(JSeparator.HORIZONTAL);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridy++;
        controlsPanel.add(separator3, gbc);
        gbc.fill = GridBagConstraints.NONE;

        // Create radio buttons for sorting the statistics plot by ID/name or
        // value
        sortNameButton = new JRadioButton("ID/name",
                                          (plotPrefs.getSortOrder(false) == BY_NAME));
        sortNameButton.setName(Integer.toString(BY_NAME));
        sortNameButton.setFont(LABEL_FONT_BOLD);
        sortValueButton = new JRadioButton("Value",
                                           (plotPrefs.getSortOrder(false) == BY_VALUE));
        sortValueButton.setName(Integer.toString(BY_VALUE));
        sortValueButton.setFont(LABEL_FONT_BOLD);

        // Add the radio buttons to a group so that only one is active at a
        // time
        ButtonGroup sortGroup = new ButtonGroup();
        sortGroup.add(sortNameButton);
        sortGroup.add(sortValueButton);

        JLabel sortLabel = new JLabel("Arrange data by");
        sortLabel.setFont(LABEL_FONT_BOLD);

        // Adjust the border around the label so that the gap between the
        // separator and the label is the same as the gap in the Preferences
        // dialog between the same two components. A difference in the gap
        // between the plot and the dialog can exist if there's a wider border
        // surrounding the radio button, which in the Preferences dialog is on
        // the same row as the label and therefore affects the spacing
        Insets buttonInsets = new Insets(0, 0, 0, 0);
        Insets labelInsets = new Insets(0, 0, 0, 0);

        // Check if the radio button has a border in the current look & feel
        if (sortNameButton.getBorder() != null)
        {
            // Get the radio button's insets
            buttonInsets = sortNameButton.getBorder().getBorderInsets(sortNameButton);
        }

        // Check if the label has a border in the current look & feel
        if (sortLabel.getBorder() != null)
        {
            // Get the label's insets
            labelInsets = sortLabel.getBorder().getBorderInsets(sortLabel);
        }

        // Check if the button's top inset is greater than the label's
        if (buttonInsets.top > labelInsets.top)
        {
            // Update the label's top inset using the button's value
            sortLabel.setBorder(BorderFactory.createEmptyBorder(buttonInsets.top,
                                                                labelInsets.left,
                                                                labelInsets.bottom,
                                                                labelInsets.right));
        }

        gbc.weightx = 0.0;
        gbc.gridwidth = 1;
        gbc.gridy++;
        controlsPanel.add(sortLabel, gbc);
        gbc.insets.left = LABEL_HORIZONTAL_SPACING * 2;
        gbc.gridy++;
        controlsPanel.add(sortNameButton, gbc);
        gbc.insets.top = 0;
        gbc.gridy++;
        controlsPanel.add(sortValueButton, gbc);

        // Listener for sort order radio button selection changes
        ActionListener sortListener = new ActionListener()
        {
            /******************************************************************
             * Handle a sort order radio button selection change
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Get the "name" of the radio button selected (BY_NAME or
                // BY_VALUE)
                int sortOrder = Integer.valueOf(((JRadioButton) ae.getSource()).getName());

                // Check if the sort order selected differs from the one
                // currently in use
                if (plotPrefs.getSortOrder(false) != sortOrder)
                {
                    // Update the sort order
                    setSortOrder(sortOrder);
                }
            }
        };

        // Listen for sort order radio button selection changes
        sortNameButton.addActionListener(sortListener);
        sortValueButton.addActionListener(sortListener);

        // Controls and chart panel: chart selection combo box and other
        // controls go on the left side, selected chart goes on the right side
        controlAndChartPanel = new JPanel(new BorderLayout());
        controlAndChartPanel.add(controlsOuterPanel, BorderLayout.LINE_START);
    }

    /**************************************************************************
     * This class is used to override the following ChartPanel method:
     * 
     * <pre>
     * - restoreAutoRangeBounds: force the y-axis minimum and maximum to remain
     *                           unchanged when zooming
     * </pre>
     *************************************************************************/
    private class CPMChartPanel extends ChartPanel
    {
        /**********************************************************************
         * CPMChartPanel constructor with context menu options
         * 
         * @param chart
         *            chart for which to create the chart panel
         * 
         * @param properties
         *            true to enable the Properties command in the context menu
         * 
         * @param save
         *            true to enable the Save command in the context menu
         * 
         * @param print
         *            true to enable the Print command in the context menu
         * 
         * @param zoom
         *            true to enable the Zoom commands in the context menu
         * 
         * @param tooltips
         *            true to enable tool tips in the chart
         *********************************************************************/
        private CPMChartPanel(JFreeChart chart,
                              boolean properties,
                              boolean save,
                              boolean print,
                              boolean zoom,
                              boolean tooltips)
        {
            super(chart, properties, save, print, zoom, tooltips);
        }

        /**********************************************************************
         * Force the plot x-axis boundaries to their initial values when
         * unzooming
         *********************************************************************/
        @Override
        public void restoreAutoRangeBounds()
        {
            // Set the bar chart range axis maximum derived from the selected
            // statistic's maximum value
            barPlot.getRangeAxis().setRange(0.0, maxXAxisValue);
        }

        /**********************************************************************
         * Adjust the x-axis minimum and maximum when zooming in on a plot
         *********************************************************************/
        @Override
        public void zoom(Rectangle2D zoomRectangle)
        {
            super.zoom(zoomRectangle);

            // Get a reference to the bar chart x-axis to shorten subsequent
            // calls
            ValueAxis axis = barPlot.getRangeAxis();

            // Set the lower boundary to keep it at zero
            axis.setLowerBound(0.0);

            // Check if the difference between the upper boundary value is less
            // than the minimum allowed
            if (axis.getUpperBound() < MINIMUM_TIME_DELTA)
            {
                // Set the upper values equal to the minimum allowed
                axis.setUpperBound(MINIMUM_TIME_DELTA);
            }
        }
    }

    /**************************************************************************
     * Zoom the bar chart in/out, keeping the range (x-axis) minimum at 0
     *************************************************************************/
    private void zoomBarChart(double zoomFactor)
    {
        // Calculate the new maximum x-axis boundary value based on the current
        // value multiplied by the zoom factor
        double maxX = barPlot.getRangeAxis().getRange().getUpperBound()
                      * zoomFactor;

        // Calculate the smallest allowed maximum chart boundary value based on
        // the data's time precision
        double minMaxX = MINIMUM_TIME_DELTA;

        // Check if the new maximum boundary value is less than the smallest
        // value allowed
        if (maxX < minMaxX)
        {
            // Use the smallest allowable maximum value
            maxX = minMaxX;
        }
        // Check if the new maximum boundary value is greater than the maximum
        // boundary value based on the maximum statistical value displayed on
        // this chart
        else if (maxX >= maxXAxisValue)
        {
            // Use the maximum statistical value derived boundary
            maxX = maxXAxisValue;
        }

        // Set the new maximum boundary value
        barPlot.getRangeAxis().setRange(0.0, maxX);
    }

    /**************************************************************************
     * Select a statistics plot to display and create the appropriate tool tip
     * format
     * 
     * @param plotIndex
     *            index of the statistics plot to display
     *************************************************************************/
    protected void selectStatsPlot(int plotIndex)
    {
        // Save the index of the selected plot and update the combo box to
        // reflect this selection. Set the ignore flag so that the combo box
        // action listener doesn't respond and call this method again
        selectedStat = plotIndex;
        ignoreEvent = true;
        statsComboBox.setSelectedIndex(selectedStat);
        ignoreEvent = false;

        // Create the pie and bar chart data sets for the selected statistic
        convertStatisticsToPieDataset();
        convertStatisticsToBarDataset();

        // Set up a tool tip to show the ID/ID name, pie slice value (with
        // appropriate units), and pie slice percentage whenever the mouse
        // pointer hovers over a plotted point
        numFormat.setMinimumFractionDigits(PLOT_FRAC[selectedStat]);
        piePlot.setToolTipGenerator(new StandardPieToolTipGenerator("<html><b>{0}</b><br>{1} "
                                                                    + PLOT_UNITS[selectedStat]
                                                                    + "<br>{2}",
                                                                    numFormat,
                                                                    pctFormat));

        // Set the bar chart tool tip format
        ((BarRenderer) barPlot.getRenderer()).setBaseToolTipGenerator(new StandardCategoryToolTipGenerator("<html><b>{1}</b><br>{2} "
                                                                                                           + PLOT_UNITS[selectedStat],
                                                                                                           numFormat));

        // Set the bar chart range axis text to the statistic's units
        barPlot.getRangeAxis().setLabel(PLOT_UNITS[selectedStat]);
    }

    /**************************************************************************
     * Set the bar chart background and grid line colors based on the plot
     * Preference dialog selections
     *************************************************************************/
    protected void setPlotColors()
    {
        barPlot.setBackgroundPaint(plotPrefs.getBackgroundColor(false));
        barPlot.setRangeGridlinePaint(plotPrefs.getMajorGridColor(false));
        barPlot.setRangeMinorGridlinePaint(plotPrefs.getMinorGridColor(false));
    }

    /**************************************************************************
     * Show or hide the vertical grid lines based on the user's selection in
     * the plot Preferences dialog
     *************************************************************************/
    protected void setVerticalGridLines()
    {
        barPlot.setRangeGridlinesVisible(plotPrefs.isShowPlotVerticalGrid(false));
    }

    /**************************************************************************
     * Select the display method (pie or bar chart) for the statistics data
     * 
     * @param type
     *            plot chart type to display (PIE or BAR)
     *************************************************************************/
    protected void selectStatsChartType(int type)
    {
        // Check if the user has selected to display the statistics data in a
        // pie chart
        if (type == PIE)
        {
            // Set the radio button to 'selected'. This is necessary if the
            // user toggles the "chart type" radio button in the Preferences
            // dialog; otherwise the local radio button will be out of sync
            chartPieButton.setSelected(true);
            plotPrefs.setStatChartType(PIE);

            // Set the pie chart to display and remove the bar chart (if
            // present)
            controlAndChartPanel.add(pieChartPanel, BorderLayout.CENTER);
            pieChartPanel.setVisible(true);
            controlAndChartPanel.remove(barPanel);
            barPanel.setVisible(false);
        }
        // The user has selected to display the statistics data in a bar chart
        else
        {
            // Set the radio button to 'selected'. This is necessary if the
            // user toggles the "chart type" radio button in the Preferences
            // dialog; otherwise the local radio button will be out of sync
            chartBarButton.setSelected(true);
            plotPrefs.setStatChartType(BAR);

            // Set the bar chart to display and remove the pie chart (if
            // present)
            controlAndChartPanel.add(barPanel, BorderLayout.CENTER);
            barPanel.setVisible(true);
            controlAndChartPanel.remove(pieChartPanel);
            pieChartPanel.setVisible(false);
        }
    }

    /**************************************************************************
     * Show or hide the CPU idle time in the statistics plot
     * 
     * @param showIdle
     *            true to display the CPU idle time
     *************************************************************************/
    protected void setCPUIdleTime(boolean showIdle)
    {
        // Adjust the check box selection. This is necessary if the user
        // toggles the "Show idle" check box in the Preferences dialog;
        // otherwise the local check box will be out of sync
        cpuIdleCheckbox.setSelected(showIdle);
        plotPrefs.setShowCPUIdle(showIdle);

        // Check if the ID Time Active plot is selected. This is the only plot
        // that can display the CPU idle time
        if (selectedStat == TIME_ON)
        {
            // Update the pie chart based on if the CPU idle time is displayed.
            // Check if the user has selected to show the idle CPU time and
            // that the idle time isn't already showing
            if (showIdle && !pieDataset.getKeys().contains("CPU Idle"))
            {
                // Add the idle time to the data set
                pieDataset.setValue("CPU Idle",
                                    perfLog.getAdjustedDataTimeSpan()
                                        - perfLog.getTotalTimeActive());

                // Check if the pie plot has been created
                if (piePlot != null)
                {
                    // Set the CPU Idle pie section color to white
                    piePlot.setSectionPaint("CPU Idle", Color.WHITE);
                }
            }
            // Check if the user has selected to hide the idle CPU time and
            // that the idle time hasn't already been hidden
            else if (!showIdle && pieDataset.getKeys().contains("CPU Idle"))
            {
                // Remove the idle time data set
                pieDataset.remove("CPU Idle");
            }

            // Rebuild the bar chart data set based on whether or not the CPU
            // idle time is to be included
            convertStatisticsToBarDataset();

            // Set the bar chart scroll bar maximum value
            barScrollBar.setMaximum(barDataset.getColumnCount());
        }
    }

    /**************************************************************************
     * Sort the data in the statistics plot
     * 
     * @param sortOrder
     *            sort order: BY_NAME or BY_VALUE
     *************************************************************************/
    protected void setSortOrder(int sortOrder)
    {
        // Check if the user has selected to sort the data by ID order
        if (sortOrder == BY_NAME)
        {
            // Set the radio button to 'selected'. This is necessary if the
            // user toggles the "sort by name" radio button in the Preferences
            // dialog; otherwise the local radio button will be out of sync
            sortNameButton.setSelected(true);
            plotPrefs.setSortOrder(BY_NAME);

            // Sort the data by name
            pieDataset.sortByKeys(SortOrder.ASCENDING);
        }
        // The user has selected to sort the data by value
        else
        {
            // Set the radio button to 'selected'. This is necessary if the
            // user toggles the "sort by value" radio button in the Preferences
            // dialog; otherwise the local radio button will be out of sync
            sortValueButton.setSelected(true);
            plotPrefs.setSortOrder(BY_VALUE);

            // Sort the data by value
            pieDataset.sortByValues(SortOrder.ASCENDING);
        }

        // Convert the statistics data to a data set for the bar chart
        convertStatisticsToBarDataset();
    }

    /**************************************************************************
     * Copy the statistics data to pie chart data set for displaying the
     * statistics
     *************************************************************************/
    private void convertStatisticsToPieDataset()
    {
        // Check if the data set exists
        if (pieDataset.getItemCount() != 0)
        {
            // Clear the data from the data set
            pieDataset.clear();
        }

        // Step through the performance IDs
        for (PerformanceID pid : perfIDs.getPerfIDList())
        {
            // Only add this ID's data if the user has it selected
            if (pid.isPlotPerfID())
            {
                // Store the statistics value in the pie chart data set
                pieDataset.setValue(pid.getPerfIDName(true),
                                    getStatisticsValue(pid));
            }
        }

        // Changing the data set clears the currently selected sort order, so
        // it must be restored after rebuilding the data set. Check if the sort
        // order is by ID name
        if (plotPrefs.getSortOrder(false) == BY_NAME)
        {
            // Sort the data by name
            pieDataset.sortByKeys(SortOrder.ASCENDING);
        }
        // Sort order is by statistics value
        else
        {
            // Sort the data by value
            pieDataset.sortByValues(SortOrder.ASCENDING);
        }

        // Add the idle CPU time, if selected by the user and if appropriate
        // for the plot selected
        setCPUIdleTime(plotPrefs.isShowCPUIdle(false));
    }

    /**************************************************************************
     * Copy the statistics data to bar chart data set based on the specified
     * sort order
     *************************************************************************/
    private void convertStatisticsToBarDataset()
    {
        // Check if the data set exists
        if (barDataset.getColumnCount() != 0)
        {
            // Clear the data from the data set and clear the color list
            barDataset.clear();
            idColors.clear();
        }

        // Create a list to hold the parameters to sort
        List<Object> sortList = new ArrayList<Object>();

        // Step through the performance IDs
        for (PerformanceID pid : perfIDs.getPerfIDList())
        {
            // Only add this ID's data if the user has it selected
            if (pid.isPlotPerfID())
            {
                // Based on the sort order selected, get the ID name or
                // statistics value and add it to the list
                sortList.add(plotPrefs.getSortOrder(false) == BY_NAME
                                                                     ? pid.getPerfIDName(true)
                                                                     : getStatisticsValue(pid));
            }
        }

        // Convert the list to an array
        Object[] sortedList = sortList.toArray();

        // Check if the sort order is by ID name
        if (plotPrefs.getSortOrder(false) == BY_NAME)
        {
            // Sort the ID names in ascending order
            Arrays.sort(sortedList);
        }
        // Sort is by statistics value
        else
        {
            // Sort the statistics in descending order
            Arrays.sort(sortedList, Collections.reverseOrder());
        }

        // Check if the ID Time Active plot is selected and CPU idle time is
        // set to be displayed. This is the only plot that can display the CPU
        // idle time
        if (selectedStat == TIME_ON && cpuIdleCheckbox.isSelected())
        {
            // Add the CPU idle time to the data set and its color to the color
            // array. This is the top item displayed, regardless of sort order
            barDataset.addValue(perfLog.getAdjustedDataTimeSpan()
                                - perfLog.getTotalTimeActive(),
                                "0",
                                "CPU Idle");
            idColors.add(Color.WHITE);
        }

        // Clear the ID used flags. This flag will be set if an ID has been
        // matched to an ID in the sorted list in order to prevent reusing an
        // ID that has the same value as another ID
        perfIDs.resetIDUsed(false);

        // Step through the sorted list
        for (int index = 0; index < sortedList.length; index++)
        {
            // Step through the performance IDs
            for (PerformanceID pid : perfIDs.getPerfIDList())
            {
                // Only add this ID's data if the user has it selected and if
                // the ID hasn't already been added to the chart
                if (pid.isPlotPerfID() && !pid.isUsed())
                {
                    // Get the ID name (or the ID if no name is supplied)
                    String idName = pid.getPerfIDName(true);

                    // Get the ID statistics value
                    Double idValue = getStatisticsValue(pid);

                    // Set what to sort on (name or value) based on the current
                    // sort order
                    Object sort = plotPrefs.getSortOrder(false) == BY_NAME
                                                                          ? idName
                                                                          : idValue;

                    // Check if the sort object matches the sorted list object
                    if (sort.equals(sortedList[index]))
                    {
                        // Set the flag indicating this ID has been used in the
                        // chart
                        pid.setUsed(true);

                        // Store the statistics value in the bar chart data set
                        barDataset.addValue(idValue, "0", idName);

                        // Set the bar color to the ID color
                        idColors.add(pid.getPerfIDColor());

                        break;
                    }
                }
            }
        }

        // Determine the maximum statistical value and set the x-axis maximum
        // boundary value
        maxXAxisValue = findMaximumStatisticsValue();
    }

    /**************************************************************************
     * Determine the maximum statistical value and use it to set the x-axis
     * maximum boundary value
     * 
     * @return The value of the x-axis maximum boundary
     *************************************************************************/
    private Double findMaximumStatisticsValue()
    {
        // Initialize the maximum statistics data value
        Double maxValue = 0.0;

        // Step through each statistical value
        for (int index = 0; index < barDataset.getColumnCount(); index++)
        {
            // Get the value from the data set
            Double idValue = (Double) barDataset.getValue(0, index);

            // Check if this is a valid value and if so, if it's the largest
            // found so far
            if (idValue != null && idValue > maxValue)
            {
                // Store the maximum value
                maxValue = idValue;
            }
        }

        // Check if no maximum was found. This would be due to having no IDs
        // selected for display or if the selected IDs do not have a value for
        // the chosen statistic
        if (maxValue == 0.0)
        {
            // Set the maximum to a value greater than the minimum (0.0). This
            // prevents zooming out so far that an exception occurs due to the
            // large value
            maxValue = 0.1;
        }
        // A maximum value was found
        else
        {
            // Pad the maximum value to provide some space at the right end of
            // the plotted bar
            maxValue *= 1.05;
        }

        // Set the new x-axis maximum boundary value
        barPlot.getRangeAxis().setRange(0.0, maxValue);

        return maxValue;
    }

    /**************************************************************************
     * Get the selected statistics value for a performance ID
     * 
     * @param pid
     *            performance ID
     * 
     * @return The value of the selected statistic for the performance ID
     *         specified
     *************************************************************************/
    private Double getStatisticsValue(PerformanceID pid)
    {
        Double statValue = null;

        switch (selectedStat)
        {
            case TIME_ON:
                // Get the ID's time on data
                statValue = pid.getTimeActive();
                break;

            case EVENT_ON:
                // Get the ID's entry event count data
                statValue = (double) pid.getEntryCount();
                break;

            case AVG_FREQ:
                // Get the ID's average frequency data
                statValue = pid.getAvgFreq();
                break;

            case MIN_TIME_ON:
                // Get the ID's minimum time on data
                statValue = pid.getMinTimeActive();
                break;

            case MAX_TIME_ON:
                // Get the ID's maximum time off data
                statValue = pid.getMaxTimeActive();
                break;

            case MIN_INTERVAL:
                // Get the ID's minimum interval data
                statValue = pid.getMinInterval();
                break;

            case MAX_INTERVAL:
                // Get the ID's maximum interval data
                statValue = pid.getMaxInterval();
                break;

            case MIN_OVERRUN:
                // Get the ID's minimum frame overrun data
                statValue = pid.getMinOverrun();
                break;

            case MAX_OVERRUN:
                // Get the ID's maximum frame overrun data
                statValue = pid.getMaxOverrun();
                break;
        }

        // Check if no value exists for this statistic
        if (statValue == null)
        {
            // Set the default value to zero
            statValue = 0.0;
        }

        return statValue;
    }
}
