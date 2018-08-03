/**
 * CFS Performance Monitor XY Plot Handler
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.CURSOR_FILE;
import static CFSPerformanceMonitor.CPMConstants.HOTSPOT_SIZE;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LABEL_VERTICAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.MINIMUM_TIME_DELTA;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.OVERRUN_INDICATOR_SIZE;
import static CFSPerformanceMonitor.CPMConstants.PLOT_LABEL_FONT;
import static CFSPerformanceMonitor.CPMConstants.PLOT_SCROLL_BAR_PAD;
import static CFSPerformanceMonitor.CPMConstants.PLOT_XAXIS_LABEL_PAD;
import static CFSPerformanceMonitor.CPMConstants.STEP_PLOT;
import static CFSPerformanceMonitor.CPMConstants.THREAD_PLOT;
import static CFSPerformanceMonitor.CPMConstants.TIME_FORMAT;
import static CFSPerformanceMonitor.CPMConstants.TIME_PRECISION;
import static CFSPerformanceMonitor.CPMConstants.TOOL_TIP_WORD_WRAP;
import static CFSPerformanceMonitor.CPMConstants.X_AXIS_LABEL_FORMAT;
import static CFSPerformanceMonitor.CPMConstants.X_SCROLLBAR_MULT;
import static CFSPerformanceMonitor.CPMConstants.Y_SCROLLBAR_MULT;

import java.awt.BasicStroke;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Paint;
import java.awt.Point;
import java.awt.RenderingHints;
import java.awt.RenderingHints.Key;
import java.awt.Toolkit;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.geom.Rectangle2D;
import java.io.IOException;
import java.net.URL;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.ScrollPaneConstants;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;

import org.jfree.chart.ChartMouseEvent;
import org.jfree.chart.ChartMouseListener;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.annotations.AbstractXYAnnotation;
import org.jfree.chart.annotations.XYDrawableAnnotation;
import org.jfree.chart.axis.AxisSpace;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.SymbolAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.entity.ChartEntity;
import org.jfree.chart.entity.PlotEntity;
import org.jfree.chart.entity.XYItemEntity;
import org.jfree.chart.event.ChartProgressEvent;
import org.jfree.chart.event.ChartProgressListener;
import org.jfree.chart.labels.StandardXYToolTipGenerator;
import org.jfree.chart.plot.CombinedDomainXYPlot;
import org.jfree.chart.plot.IntervalMarker;
import org.jfree.chart.plot.PlotRenderingInfo;
import org.jfree.chart.plot.ValueMarker;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.Drawable;
import org.jfree.ui.Layer;
import org.jfree.ui.RectangleEdge;

import CFSPerformanceMonitor.CPMConstants.CursorMode;
import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;
import CFSPerformanceMonitor.CPMLogHandler.PerfLog;

/******************************************************************************
 * CFS Performance Monitor XY plot handler class
 *****************************************************************************/
public abstract class CPMXYPlotHandler
{
    protected CPMLogHandler perfLog;
    protected CPMIDHandler perfIDs;
    protected CPMMain perfMain;
    protected CPMPreferencesHandler plotPrefs;

    // Indicates the type of plot (STEP_PLOT or THREAD_PLOT)
    private final int plotType;

    // Components that need to be accessed by other classes or listeners
    private JTextArea startTextArea;
    private JTextArea endTextArea;
    private JTextArea deltaTextArea;
    private JPanel plotAndLabelPanel;
    private JPanel labelPanel;
    private JPanel xAxisScrollBarPanel;
    private JScrollBar xAxisScrollBar;
    private JScrollBar yAxisScrollBar;
    private final List<XYPlot> plots;
    private CPMChartPanel plotChartPanel;
    private ChartPanel labelChartPanel;
    private final List<XYSeries> plotSeries;
    private NumberAxis plotDomainAxis;
    private NumberAxis labelDomainAxis;
    private final List<SymbolAxis> symbolAxis;
    private final ValueMarker startMarker;
    private final ValueMarker endMarker;
    private final IntervalMarker shadeMarker;
    private final List<IntervalMarker> gapMarker;
    private final List<ValueMarker> minorMarker;
    private final List<ValueMarker> majorMarker;
    private JFreeChart plotChart;
    private JFreeChart labelChart;
    private JPanel cursorPanel;
    private JLabel startLabel;
    private JLabel endLabel;
    private JLabel deltaLabel;
    private Cursor cursor;
    private XYPlot labelPlot;

    // Tool tip generator for plotted points
    private StandardXYToolTipGenerator toolTipGenerator;

    // Plot x- and y-axis label pixel sizes
    private int prevPlotYLabelWidth;
    private int prevXBarWidth;

    // X-axis limits (current and initial)
    private double xLower;
    private double xUpper;
    private double xLowerInit;
    private double xUpperInit;

    // Amount to add to either side of the logged data minimum and maximum
    // x-axis values; based on a percentage of the total x-axis span
    private double xPadding;

    // Y-axis limits (current and initial)
    private double yLower;
    private double yUpper;
    private double yLowerInit;
    private double yUpperInit;

    // Current mode for the cursor information text fields
    private CursorMode cursorMode;

    // Cursor information mode start marker, end marker, and current mouse
    // pointer x-axis coordinates
    private Double startPosition;
    private Double endPosition;
    private Double currentPosition;

    // Screen x-coordinate of the start position marker
    private int startCoord;

    // Height of the cursor information field text areas in pixels
    private int textHeight;

    // Set to true if the plot has a y-axis scroll bar controlled by the
    // custom methods contained in this class; set to false otherwise
    private boolean yAxisCustomScrollBar;

    // Flag to indicate whether or not the chart progress listener should
    // process events
    private boolean progressListenerEnabled;

    // List containing the line colors (ID and CPU Activity lines)
    private final List<Color> lineColor;

    /**************************************************************************
     * Common XY plot class constructor
     *************************************************************************/
    public CPMXYPlotHandler(int plotType)
    {
        this.plotType = plotType;

        plotSeries = new ArrayList<XYSeries>();
        plots = new ArrayList<XYPlot>();
        symbolAxis = new ArrayList<SymbolAxis>();

        xLower = 0;
        xUpper = 0;
        xLowerInit = 0;
        xUpperInit = 0;
        yLower = 0;
        yUpper = 0;
        yLowerInit = 0;
        yUpperInit = 0;

        cursorMode = CursorMode.START;

        prevPlotYLabelWidth = -1;
        prevXBarWidth = -1;

        startMarker = new ValueMarker(0.0);
        endMarker = new ValueMarker(0.0);
        shadeMarker = new IntervalMarker(0.0, 0.0);
        gapMarker = new ArrayList<IntervalMarker>();
        minorMarker = new ArrayList<ValueMarker>();
        majorMarker = new ArrayList<ValueMarker>();

        lineColor = new ArrayList<Color>();

        progressListenerEnabled = false;
    }

    /**************************************************************************
     * Return the XYPlot list
     * 
     * @return XY plot list
     *************************************************************************/
    protected List<XYPlot> getPlots()
    {
        return plots;
    }

    /**************************************************************************
     * Return the plot chart panel component
     * 
     * @return Plot chart panel component
     *************************************************************************/
    protected CPMChartPanel getPlotChartPanel()
    {
        return plotChartPanel;
    }

    /**************************************************************************
     * Get the plot chart
     * 
     * @return Plot chart
     *************************************************************************/
    protected JFreeChart getPlotChart()
    {
        return plotChart;
    }

    /**************************************************************************
     * Get the tool tip generator
     * 
     * @return Tool tip generator
     *************************************************************************/
    protected StandardXYToolTipGenerator getToolTipGenerator()
    {
        return toolTipGenerator;
    }

    /**************************************************************************
     * Return the label chart panel component
     * 
     * @return Label chart panel component
     *************************************************************************/
    protected ChartPanel getLabelChartPanel()
    {
        return labelChartPanel;
    }

    /**************************************************************************
     * Return the plot series list
     * 
     * @return Plot series list
     *************************************************************************/
    protected List<XYSeries> getPlotSeries()
    {
        return plotSeries;
    }

    /**************************************************************************
     * Return the plot domain axis
     * 
     * @return Plot domain axis
     *************************************************************************/
    protected NumberAxis getPlotDomainAxis()
    {
        return plotDomainAxis;
    }

    /**************************************************************************
     * Return the plot symbol axis list
     * 
     * @return Plot symbol axis list
     *************************************************************************/
    protected List<SymbolAxis> getSymbolAxis()
    {
        return symbolAxis;
    }

    /**************************************************************************
     * Enable or disable the chart (re)draw progress listener
     * 
     * @param setEnabled
     *            true to enable the chart redraw progress listener
     *************************************************************************/
    protected void setChartProgressListenerEnable(boolean setEnabled)
    {
        progressListenerEnabled = setEnabled;
    }

    /**************************************************************************
     * Return the amount to add to either side of the logged data minimum and
     * maximum x-axis values; based on a percentage of the total x-axis span
     * 
     * @param maximumTimeStamp
     *            time stamp for the last logged event
     *************************************************************************/
    protected void setXPadding(double maximumTimeStamp)
    {
        xPadding = maximumTimeStamp / 50.0;
    }

    /**************************************************************************
     * Return the label plot
     * 
     * @return Label plot
     *************************************************************************/
    protected XYPlot getLabelPlot()
    {
        return labelPlot;
    }

    /**************************************************************************
     * Return the plot line color list
     * 
     * @return Plot line color list
     *************************************************************************/
    protected List<Color> getLineColor()
    {
        return lineColor;
    }

    /**************************************************************************
     * Placeholder for method to create the JFreeChart chart
     * 
     * @return Plot chart
     *************************************************************************/
    protected abstract JFreeChart createPlotChart();

    /**************************************************************************
     * Placeholder to convert the log data to a plot series
     *************************************************************************/
    protected abstract void convertLogDataToXYSeries();

    /**************************************************************************
     * Placeholder for adjust the plot height and spacing (applicable to step
     * plots)
     *************************************************************************/
    protected void adjustPlotHeightAndSpacing()
    {
    }

    /**************************************************************************
     * Create the plot components and listeners, convert the data into plot
     * series format, and display the plot
     * 
     * @param plotPanel
     *            plot panel
     * 
     * @param useCustomYAxisScrollBar
     *            true to enable the custom y-axis scroll bar
     *************************************************************************/
    protected void createPlot(JPanel plotPanel,
                              boolean useCustomYAxisScrollBar)
    {
        // Since this could take a while, show the "wait" mouse pointer to
        // alert the user. The pointer is restored to the default when the plot
        // display completes
        perfMain.setMainCursor(Cursor.WAIT_CURSOR);

        // Enable/disable the custom y-axis scroll bar methods
        yAxisCustomScrollBar = useCustomYAxisScrollBar;

        // Convert log data into a plot-able format
        convertLogDataToXYSeries();

        // Create the plots' x-axes
        createPlotDomainAxes();

        // Create the tool tip generator
        createToolTipGenerator();

        // Create the plot
        plotChart = createPlotChart();

        // Create the plot and label chart panels and set their characteristics
        createChartPanels();

        // Show/hide frame overrun indicators
        showOverruns();

        // Create the crosshair cursor
        createCrosshairCursor();

        // Create the cursor information fields components
        createCursorInfoFields();

        // Add a key listener for the plot chart panel
        createPlotChartKeyListener();

        // Add a mouse button, motion, and wheel listener for the plot chart
        // panel
        createPlotMouseListener();

        // Enable the chart (re)draw progress listener
        setChartProgressListenerEnable(true);

        // Create a listener for monitoring the plot redraw progress
        createPlotChartProgressListener();

        // Set the subplot height, spacing, and y-axis tick label state
        adjustPlotHeightAndSpacing();

        // Set up the plot markers
        setupPlotMarkers();

        // Display markers to show the gaps between data files
        showLogDataGaps();

        // Create and arrange the plot window components and place them into
        // the plot panel
        plotPanel.add(createPlotWindowComponents());

        // Validate causes the frame to determine the sizes & positions of its
        // components before they're displayed
        plotPanel.validate();

        // Set the x-axis label plot pane height now that the sizes and
        // positions of the other components are known
        setXAxisLabelPaneHeight();

        // Draw the plots, etc. in the main application window
        plotPanel.setVisible(true);
    }

    /**************************************************************************
     * Create the crosshair cursor for display in the plot area
     *************************************************************************/
    private void createCrosshairCursor()
    {
        // Get the path to the custom crosshair mouse pointer image file
        URL cursorURL = getClass().getResource(CURSOR_FILE);

        // Check if the custom crosshair mouse pointer file exists
        if (cursorURL != null)
        {
            // Create the custom crosshair mouse pointer for use when over the
            // plot window
            Toolkit toolkit = Toolkit.getDefaultToolkit();
            cursor = toolkit.createCustomCursor(toolkit.getImage(cursorURL),
                                                new Point(16, 16),
                                                CURSOR_FILE);
        }
        // The custom pointer file couldn't be opened
        else
        {
            // Use the default crosshair pointer
            cursor = Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR);
        }
    }

    /**************************************************************************
     * Create the components for the cursor information display fields
     *************************************************************************/
    private void createCursorInfoFields()
    {

        // Create a border for the cursor information text boxes
        Border border = BorderFactory.createBevelBorder(BevelBorder.LOWERED,
                                                        Color.LIGHT_GRAY,
                                                        Color.GRAY);

        // Create the cursor information display fields: start time, end time,
        // and delta time. Base the text field sizes on the font
        int textWidth = (int) perfMain.getMainFrame().getFontMetrics(LABEL_FONT_PLAIN).getStringBounds("00000000000", null).getWidth();
        textHeight = perfMain.getMainFrame().getFontMetrics(LABEL_FONT_PLAIN).getHeight() + 4;

        startLabel = new JLabel("Start");
        startLabel.setFont(LABEL_FONT_BOLD);
        startLabel.setHorizontalAlignment(JLabel.RIGHT);
        startLabel.setVerticalAlignment(JLabel.CENTER);
        startLabel.setOpaque(true);
        startLabel.setForeground(Color.BLACK);

        startTextArea = new JTextArea();
        startTextArea.setFont(LABEL_FONT_PLAIN);
        startTextArea.setEditable(false);
        startTextArea.setFocusable(false);
        startTextArea.setForeground(Color.BLACK);
        startTextArea.setBackground(Color.WHITE);
        startTextArea.setPreferredSize(new Dimension(textWidth, textHeight));
        startTextArea.setMinimumSize(new Dimension(textWidth, textHeight));
        startTextArea.setMaximumSize(new Dimension(textWidth, textHeight));
        startTextArea.setBorder(border);

        endLabel = new JLabel("End");
        endLabel.setFont(LABEL_FONT_BOLD);
        endLabel.setHorizontalAlignment(JLabel.RIGHT);
        endLabel.setVerticalAlignment(JLabel.CENTER);
        endLabel.setOpaque(true);
        endLabel.setForeground(Color.BLACK);

        endTextArea = new JTextArea();
        endTextArea.setFont(LABEL_FONT_PLAIN);
        endTextArea.setEditable(false);
        endTextArea.setFocusable(false);
        endTextArea.setForeground(Color.BLACK);
        endTextArea.setBackground(Color.WHITE);
        endTextArea.setPreferredSize(new Dimension(textWidth, textHeight));
        endTextArea.setMinimumSize(new Dimension(textWidth, textHeight));
        endTextArea.setMaximumSize(new Dimension(textWidth, textHeight));
        endTextArea.setBorder(border);

        deltaLabel = new JLabel("Delta");
        deltaLabel.setFont(LABEL_FONT_BOLD);
        deltaLabel.setHorizontalAlignment(JLabel.RIGHT);
        deltaLabel.setVerticalAlignment(JLabel.CENTER);
        deltaLabel.setOpaque(true);
        deltaLabel.setForeground(Color.BLACK);

        deltaTextArea = new JTextArea();
        deltaTextArea.setFont(LABEL_FONT_PLAIN);
        deltaTextArea.setEditable(false);
        deltaTextArea.setFocusable(false);
        deltaTextArea.setForeground(Color.BLACK);
        deltaTextArea.setBackground(Color.WHITE);
        deltaTextArea.setPreferredSize(new Dimension(textWidth, textHeight));
        deltaTextArea.setMinimumSize(new Dimension(textWidth, textHeight));
        deltaTextArea.setMaximumSize(new Dimension(textWidth, textHeight));
        deltaTextArea.setBorder(border);

        // Create a panel to hold the cursor information fields
        cursorPanel = new JPanel();
        cursorPanel.setLayout(new BoxLayout(cursorPanel, BoxLayout.X_AXIS));

        // Add a dummy text area so that space is created for the cursor
        // information fields. The actual fields are created once the width of
        // the chart y-axis labels is known (i.e., chart drawing complete)
        cursorPanel.add(new JTextArea());
    }

    /**************************************************************************
     * Create the components that make up and arrange the plot window
     * 
     * @return Panel containing the plot components
     *************************************************************************/
    private JPanel createPlotWindowComponents()
    {
        // Create an empty border to surround the panes. This removes graphic
        // artifacts that otherwise can appear at the corners of the panes
        Border emptyBorder = BorderFactory.createEmptyBorder();

        // Create a border that adds padding between the plot's y-axis scroll
        // bar and the edge of the main window, and between the bottom of the
        // plot and the x-axis label plot
        Border plotAndLabelBorder = BorderFactory.createEmptyBorder(0,
                                                                    0,
                                                                    PLOT_XAXIS_LABEL_PAD,
                                                                    PLOT_SCROLL_BAR_PAD);

        // Plot and label plot panel: The plot goes at the top and the x-axis
        // label plot goes at the bottom
        plotAndLabelPanel = new JPanel(new BorderLayout());
        plotAndLabelPanel.setBorder(emptyBorder);

        // Create the x-axis scroll bar
        createXAxisScrollBar();

        // Check if the custom y-axis scroll bar is in use
        if (yAxisCustomScrollBar)
        {
            // Create the y-axis scroll bar
            createYAxisScrollBar();

            // Plot & y-axis scroll bar panel: Plots go on left side, plot
            // y-axis scroll bar goes on right side
            JPanel plotAndYBarPanel = new JPanel(new BorderLayout());
            plotAndYBarPanel.setBorder(plotAndLabelBorder);

            // Place the plot panel on the left side and the y-axis scroll bar
            // on the right side of the plot & y-axis scroll bar panel
            plotAndYBarPanel.add(plotChartPanel, BorderLayout.CENTER);
            plotAndYBarPanel.add(yAxisScrollBar, BorderLayout.LINE_END);

            // Place the plot & y-axis scroll bar panel at the top of the plot
            // & label panel
            plotAndLabelPanel.add(plotAndYBarPanel, BorderLayout.CENTER);
        }
        // Use the scroll pane's y-axis scroll bar
        else
        {
            // Create a scroll pane for the plots to reside in
            JScrollPane plotScrollPane = new JScrollPane(plotChartPanel,
                                                         ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
                                                         ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
            plotScrollPane.setViewportBorder(emptyBorder);
            plotScrollPane.setBorder(plotAndLabelBorder);

            // Set y-axis scroll bar to the scroll pane's scroll bar. Set the
            // scroll bar's unit increment (otherwise the mouse and keyboard
            // scroll different amounts)
            yAxisScrollBar = plotScrollPane.getVerticalScrollBar();
            yAxisScrollBar.setUnitIncrement(10);

            // Place the scroll pane at the center of the plot & label panel
            plotAndLabelPanel.add(plotScrollPane, BorderLayout.CENTER);
        }

        // Remove the border from the y-axis scroll bar
        yAxisScrollBar.setBorder(emptyBorder);

        // Cursor & plot panel: Cursor information goes at the top, plot &
        // x-axis scroll bar panel goes at the bottom
        JPanel cursorAndPlotPanel = new JPanel(new BorderLayout());
        cursorAndPlotPanel.setBorder(BorderFactory.createEmptyBorder(0,
                                                                     0,
                                                                     PLOT_SCROLL_BAR_PAD,
                                                                     0));

        // Plot & x-axis scroll bar panel: Plots go at the top, plot x-axis
        // scroll bar goes at the bottom
        JPanel plotAndXBarPanel = new JPanel(new BorderLayout());
        plotAndXBarPanel.setBorder(emptyBorder);

        // Place the x-axis label chart at the bottom of the plot & label
        // panel. The chart is first placed into a panel so that the vertical
        // size of the x-axis chart can be adjusted
        labelPanel = new JPanel(new BorderLayout());
        labelPanel.setBorder(emptyBorder);
        labelPanel.add(labelChartPanel, BorderLayout.PAGE_END);
        plotAndLabelPanel.add(labelPanel, BorderLayout.PAGE_END);

        // Place the cursor information label at the top and the plot & x-axis
        // panel in the lower portion of the cursor & plot panel
        cursorAndPlotPanel.add(cursorPanel, BorderLayout.PAGE_START);
        cursorAndPlotPanel.add(plotAndXBarPanel, BorderLayout.CENTER);

        // X-axis scroll bar panel: This corrects a redraw issue when using the
        // GTK+ look & feel where the left side of the bar doesn't always meet
        // the left arrow of the scroll bar
        xAxisScrollBarPanel = new JPanel(new BorderLayout());
        xAxisScrollBar.setBorder(emptyBorder);
        xAxisScrollBarPanel.add(xAxisScrollBar);

        // Place the plot & label panel at the top and the x-axis scroll bar
        // (in its panel) at the bottom of the plot & x-axis scroll bar panel
        plotAndXBarPanel.add(plotAndLabelPanel, BorderLayout.CENTER);
        plotAndXBarPanel.add(xAxisScrollBarPanel, BorderLayout.PAGE_END);

        return cursorAndPlotPanel;
    }

    /**************************************************************************
     * Create the tool tip generator for plotted data points
     *************************************************************************/
    private void createToolTipGenerator()
    {
        // Set up a tool tip to show the x-axis coordinate whenever the mouse
        // cursor hovers over a plotted point
        NumberFormat format = NumberFormat.getNumberInstance();
        format.setMaximumFractionDigits(TIME_PRECISION);
        toolTipGenerator = new StandardXYToolTipGenerator("<html><b>{0}</b><br>{1} sec",
                                                          format,
                                                          format);
    }

    /**************************************************************************
     * Create the plot and label chart panels and set their characteristics
     *************************************************************************/
    private void createChartPanels()
    {
        // Create the plot's chart panel
        plotChartPanel = new CPMChartPanel(plotChart,
                                           false, // Properties
                                           true, // Save
                                           true, // Print
                                           true, // Zoom
                                           true); // Tooltips

        // Set an empty border around the plot chart panel
        plotChartPanel.setBorder(BorderFactory.createEmptyBorder());

        // Draw or hide the minor and major frame markers
        adjustFrameLines();

        // Prevent zooming the y-axis unless the custom y-axis scroll bar is in
        // use (the x-axis can still be zoomed)
        plotChartPanel.setRangeZoomable(yAxisCustomScrollBar);

        // Allow zooming and unzooming using the mouse wheel
        plotChartPanel.setMouseWheelEnabled(true);

        // Set the zoom factors for the axes. By using inverse values for the
        // zoom factors the original axis range can be reestablished by zooming
        // in the opposite direction (if +x% and -x% were used then zooming one
        // way and then the opposite way doesn't return to the original values)
        plotChartPanel.setZoomInFactor(0.95);
        plotChartPanel.setZoomOutFactor(1.0 / 0.95);

        // Set the delay (in milliseconds) for showing the x-axis
        // coordinate tool tip
        plotChartPanel.setInitialDelay(100);

        // Allow the keyboard focus to select the plot
        plotChartPanel.setFocusable(true);

        // Turn off anti-aliasing for the plotted data, and on for the text.
        // This makes the plots draw faster and the lines sharper (e.g., the
        // grid lines are easier to see), and makes the text smoother
        Map<Key, Object> hints = new HashMap<Key, Object>();
        hints.put(RenderingHints.KEY_ANTIALIASING,
                  RenderingHints.VALUE_ANTIALIAS_OFF);
        hints.put(RenderingHints.KEY_TEXT_ANTIALIASING,
                  RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
        plotChart.setRenderingHints(new RenderingHints(hints));

        // Set the min/max drawing height to keep the font from being resized
        // when the chart is resized
        plotChartPanel.setMaximumDrawHeight(10000);
        plotChartPanel.setMinimumDrawHeight(10);
        plotChartPanel.setMaximumDrawWidth(10000);
        plotChartPanel.setMinimumDrawWidth(10);

        // Create the x-axis label plot
        labelChart = createXAxisLabelChart();
        labelChartPanel = new CPMChartPanel(labelChart);

        // Don't allow the keyboard focus to select the label plot
        labelChartPanel.setFocusable(false);

        // Set the x-axis label plot min/max drawing height to keep the font
        // from being resized when the chart is resized
        labelChartPanel.setMaximumDrawHeight(10000);
        labelChartPanel.setMinimumDrawHeight(10);
        labelChartPanel.setMaximumDrawWidth(10000);
        labelChartPanel.setMinimumDrawWidth(10);

        // Hide the label plot domain axis until the size is determined (when
        // the plot is displayed)
        showLabelDomainAxis(false);
    }

    /**************************************************************************
     * Create a listener for key presses in the plot
     *************************************************************************/
    private void createPlotChartKeyListener()
    {
        // Monitor the plot window for key presses
        plotChartPanel.addKeyListener(new KeyAdapter()
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
                    // Decrease the axes' ranges, centered on the plot window
                    plotChartPanel.zoomInBoth((xUpper - xLower) / 2.0,
                                              (yUpper - yLower) / 2.0);
                }
                // Check if the '-' key is pressed
                else if (ke.getKeyChar() == '-')
                {
                    // Increase the axes' ranges, centered on the plot window
                    plotChartPanel.zoomOutBoth((xUpper - xLower) / 2.0,
                                               (yUpper - yLower) / 2.0);
                }
                // Check if the ESC key is pressed
                else if (ke.getKeyCode() == KeyEvent.VK_ESCAPE)
                {
                    cursorMode = CursorMode.START;

                    // Check that the start position exists. This covers the
                    // case where the ESC key is pressed when the plot is first
                    // displayed and the mouse hasn't yet moved
                    if (currentPosition != null)
                    {
                        // Clear the start, end, and delta text fields, remove
                        // the vertical lines, and update the start position to
                        // match the current cursor position
                        startPosition = currentPosition;
                        resetStartEndLines(" ");
                    }
                }
                // Check if the shift and left arrow keys are pressed together
                else if (ke.isShiftDown()
                         && ke.getKeyCode() == KeyEvent.VK_LEFT)
                {
                    // Create and send an event to the x-axis scroll bar to
                    // block scroll the plot horizontally to the left
                    xAxisScrollBar.dispatchEvent(new KeyEvent(xAxisScrollBar,
                                                              ke.getID(),
                                                              ke.getWhen(),
                                                              0,
                                                              KeyEvent.VK_PAGE_UP,
                                                              KeyEvent.CHAR_UNDEFINED));
                }
                // Check if the shift and right arrow keys are pressed together
                else if (ke.isShiftDown()
                         && ke.getKeyCode() == KeyEvent.VK_RIGHT)
                {
                    // Create and send an event to the x-axis scroll bar to
                    // block scroll the plot horizontally to the right
                    xAxisScrollBar.dispatchEvent(new KeyEvent(xAxisScrollBar,
                                                              ke.getID(),
                                                              ke.getWhen(),
                                                              0,
                                                              KeyEvent.VK_PAGE_DOWN,
                                                              KeyEvent.CHAR_UNDEFINED));
                }
                // Check if the left or right arrow keys are pressed
                else if (ke.getKeyCode() == KeyEvent.VK_LEFT
                         || ke.getKeyCode() == KeyEvent.VK_RIGHT)
                {
                    // Scroll the plot horizontally in the selected direction
                    xAxisScrollBar.dispatchEvent(ke);
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
                        yAxisScrollBar.dispatchEvent(new KeyEvent(yAxisScrollBar,
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
                        yAxisScrollBar.dispatchEvent(new KeyEvent(yAxisScrollBar,
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
                        yAxisScrollBar.dispatchEvent(ke);
                    }
                }
            }
        });
    }

    /**************************************************************************
     * Create a listener for mouse button, motion, and wheel changes in the
     * plot
     *************************************************************************/
    private void createPlotMouseListener()
    {
        // Monitor the plot window for mouse clicks and movement
        plotChartPanel.addChartMouseListener(new ChartMouseListener()
        {
            /******************************************************************
             * Handle a mouse button click event
             *****************************************************************/
            @Override
            public void chartMouseClicked(ChartMouseEvent cme)
            {
                ChartEntity chartEntity = cme.getEntity();

                // Only trigger if the mouse cursor is within a plot window
                if (chartEntity instanceof PlotEntity)
                {
                    // Get the x-axis plot coordinate of the mouse cursor
                    Point point = cme.getTrigger().getPoint();
                    currentPosition = getMouseXCoordinate(point);

                    // If the Ctrl or Shift key is also pressed then the start
                    // and/or end positions are based on the nearest points to
                    // the mouse cursor
                    if (cme.getTrigger().isControlDown()
                        || cme.getTrigger().isShiftDown())
                    {
                        XYSeries ps;

                        // Check if this is a thread of execution plot
                        if (plotType == THREAD_PLOT)
                        {
                            // Get the thread of execution line index
                            ps = plotSeries.get(0);
                        }
                        // This is a step plot
                        else
                        {
                            // Determine which subplot the cursor was on when
                            // the mouse was clicked and get the number of
                            // points in the plot
                            ps = plotSeries.get(plotChartPanel.getChartRenderingInfo().getPlotInfo().getSubplotIndex(point));
                        }

                        // Only get the start and end positions if the plot
                        // has data
                        if (ps.getItemCount() > 0)
                        {
                            // Search for the x-axis value in the plot under
                            // the mouse pointer closest to where the pointer
                            // is positioned
                            int dataIndex = findClosestXAxisIndex(ps, currentPosition);

                            // If the Ctrl key is also pressed then set the
                            // start and end positions based on the nearest
                            // points to the mouse cursor
                            if (cme.getTrigger().isControlDown())
                            {
                                // Set the new start and end positions, then
                                // draw a vertical line across all plots at
                                // these positions and shade the area in
                                // between
                                clearStartEndLinesAndAreas(shadeMarker,
                                                           startMarker,
                                                           endMarker);
                                startPosition = ps.getX(dataIndex - 1).doubleValue();
                                endPosition = ps.getX(dataIndex).doubleValue();
                                adjustStartEndLine(startMarker,
                                                   startPosition,
                                                   true);
                                adjustStartEndLine(endMarker,
                                                   endPosition,
                                                   true);
                                fillShadedArea(shadeMarker,
                                               startPosition,
                                               endPosition);

                                // Update the start, end, and delta fields, and
                                // set the mode so that mouse movement doesn't
                                // move the start or end vertical line
                                cursorMode = CursorMode.DELTA;
                                startTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                    startPosition));
                                endTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                  endPosition));
                                deltaTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                    endPosition
                                                                        - startPosition));
                            }
                            // The SHIFT key is also pressed; set the start (or
                            // end, depending on the cursor mode) position
                            // based on the nearest points to the mouse cursor
                            else
                            {
                                // Use the point that's closest to the mouse
                                // position (use the point to the left if the
                                // distances are equal)
                                if (currentPosition - ps.getX(dataIndex - 1).doubleValue()
                                < ps.getX(dataIndex).doubleValue() - currentPosition)
                                {
                                    currentPosition = ps.getX(dataIndex - 1).doubleValue();
                                }
                                else
                                {
                                    currentPosition = ps.getX(dataIndex).doubleValue();
                                }

                                // Set the new start (end) position, then
                                // draw a vertical line across all plots at
                                // the new positions
                                switch (cursorMode)
                                {
                                    case START:
                                    case DELTA:
                                        clearStartEndLinesAndAreas(shadeMarker,
                                                                   startMarker,
                                                                   endMarker);
                                        startPosition = currentPosition;
                                        adjustStartEndLine(startMarker,
                                                           startPosition,
                                                           true);
                                        cursorMode = CursorMode.END;
                                        startTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                            startPosition));
                                        break;

                                    case END:
                                        endPosition = currentPosition;
                                        adjustStartEndLine(endMarker,
                                                           endPosition,
                                                           true);
                                        fillShadedArea(shadeMarker,
                                                       startPosition,
                                                       endPosition);
                                        cursorMode = CursorMode.DELTA;
                                        endTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                          endPosition));
                                        deltaTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                            endPosition
                                                                                - startPosition));
                                        break;
                                }
                            }
                        }
                    }
                    // No relevant modifier keys are also pressed; position the
                    // vertical line at the current mouse position based on the
                    // cursor mode and update the appropriate text field
                    else
                    {
                        switch (cursorMode)
                        {
                            case START:
                                // Check if no start position has been set
                                if (startPosition == null)
                                {
                                    // Store the screen x-coordinate of the
                                    // start position marker. This is used to
                                    // prevent the end position marker from
                                    // erasing the start marker
                                    startCoord = cme.getTrigger().getPoint().x;

                                    // Adjust the position of the start line
                                    // and text
                                    startPosition = currentPosition;
                                    startTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                        startPosition));
                                }

                                // Move the vertical line across all plots
                                // to the new start position
                                adjustStartEndLine(startMarker,
                                                   startPosition,
                                                   true);

                                cursorMode = CursorMode.END;
                                endPosition = startPosition;
                                endTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                  endPosition));
                                break;

                            case END:
                                fillShadedArea(shadeMarker,
                                               startPosition,
                                               endPosition);
                                cursorMode = CursorMode.DELTA;
                                deltaTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                    endPosition
                                                                        - startPosition));
                                break;

                            case DELTA:
                                cursorMode = CursorMode.START;
                                startPosition = currentPosition;
                                resetStartEndLines(String.format(" " + TIME_FORMAT,
                                                                 startPosition));
                                break;
                        }
                    }
                }
            }

            /******************************************************************
             * Handle a mouse movement event
             *****************************************************************/
            @Override
            public void chartMouseMoved(ChartMouseEvent cme)
            {
                ChartEntity chartEntity = cme.getEntity();

                // Only trigger while the mouse cursor is moving through the
                // plot window or if it's over a plotted point
                if (chartEntity instanceof PlotEntity
                    || chartEntity instanceof XYItemEntity)
                {
                    // Check if the crosshair mouse pointer is already selected
                    if (plotChartPanel.getCursor() != cursor)
                    {
                        // Select the crosshair pointer
                        plotChartPanel.setCursor(cursor);
                    }

                    // Get the x-axis plot coordinate of the mouse cursor
                    currentPosition = getMouseXCoordinate(cme.getTrigger().getPoint());

                    // Base the action to take on the current cursor mode
                    switch (cursorMode)
                    {
                        case START:
                            // Store the screen x-coordinate of the start
                            // position marker. This is used to prevent the end
                            // position marker from erasing the start marker
                            startCoord = cme.getTrigger().getPoint().x;

                            // Adjust the position of the start line and text
                            startPosition = currentPosition;
                            startTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                startPosition));

                            // Move the vertical line across all plots to the
                            // new start position
                            adjustStartEndLine(startMarker,
                                               startPosition,
                                               true);
                            break;

                        case END:

                            // Adjust the position of the end line and text
                            endPosition = currentPosition;
                            endTextArea.setText(String.format(" " + TIME_FORMAT,
                                                              endPosition));
                            deltaTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                endPosition
                                                                    - startPosition));

                            // Move the vertical line across all plots to the
                            // new end position. If the start and end markers
                            // have the same screen x-coordinate the new end
                            // marker isn't drawn; otherwise the start marker
                            // is erased
                            adjustStartEndLine(endMarker,
                                               endPosition,
                                               startCoord != cme.getTrigger().getPoint().x);
                            break;

                        case DELTA:
                            // No lines to move while the delta time is
                            // displayed; do nothing
                            break;
                    }
                }
                // Not moving through a plot window (i.e., moving through the
                // label area)
                else
                {
                    // Check if the main application mouse pointer is already
                    // selected
                    if (plotChartPanel.getCursor() != perfMain.getMainFrame().getCursor())
                    {
                        // Select the main application pointer
                        plotChartPanel.setCursor(perfMain.getMainFrame().getCursor());
                    }

                    // Remove the start or end domain marker
                    removeStartEndLine();
                }
            }
        });

        // Monitor the plot window for mouse wheel changes
        plotChartPanel.addMouseWheelListener(new MouseWheelListener()
        {
            /******************************************************************
             * Handle a mouse wheel movement event
             *****************************************************************/
            @Override
            public void mouseWheelMoved(MouseWheelEvent mwe)
            {
                // Adjust the x- and y-axis scroll bar unit and block increment
                // values based on the zoom level
                adjustScrollBarIncrements();
            }
        });
    }

    /**************************************************************************
     * Perform a binary search on the x-axis values for the plot under the
     * mouse pointer to find the index into the data set closest to where the
     * pointer is positioned
     * 
     * @param ps
     *            reference to the plot to be searched
     * 
     * @param targetValue
     *            x-axis coordinate to search for
     * 
     * @return The index of the point in the plot's data set nearest to the
     *         target value
     *************************************************************************/
    private int findClosestXAxisIndex(XYSeries ps, double targetValue)
    {
        // Set the initial start and stop indices, ignoring the first and last
        // points
        int lowerIndex = 1;
        int upperIndex = ps.getItemCount() - 2;

        // Check if the upper index is less than the lower index
        if (upperIndex < lowerIndex)
        {
            // Set the upper index to the lower index. This prevents the
            // possibility of calculating a negative data index
            upperIndex = lowerIndex;
        }

        // Begin the search at the plot's midpoint data index
        int dataIndex = (lowerIndex + upperIndex) / 2;

        // Continue the search until the plot value matches the mouse pointer
        // position or until the lower and upper indices meet
        while ((ps.getX(dataIndex).doubleValue() != targetValue)
               && (lowerIndex <= upperIndex))
        {
            // Check if the plot value is greater than the mouse position value
            if (ps.getX(dataIndex).doubleValue() > targetValue)
            {
                // Decrement the upper index
                upperIndex = dataIndex - 1;
            }
            // The plot value is less than the mouse position value
            else
            {
                // Increment the lower index
                lowerIndex = dataIndex + 1;
            }

            // Recalculate the midpoint index between the updated upper and
            // lower indices
            dataIndex = (lowerIndex + upperIndex) / 2;
        }

        // Return the next index after the one located by the search
        return ++dataIndex;
    }

    /**************************************************************************
     * Create a listener for monitoring the progress of redrawing a plot
     *************************************************************************/
    private void createPlotChartProgressListener()
    {
        // Detect when the plot window has been completely drawn
        plotChart.addProgressListener(new ChartProgressListener()
        {
            /******************************************************************
             * Handle a plot chart (re)draw event
             *****************************************************************/
            @Override
            public void chartProgress(ChartProgressEvent cpe)
            {
                // Check if the plot has completed being drawn
                if (cpe.getType() == ChartProgressEvent.DRAWING_FINISHED)
                {
                    // Get the width of the y-axis labels
                    int plotYLabelWidth = (int) plotChartPanel.getScreenDataArea().getX();

                    // Check if the x-axis scroll bar has changed in width.
                    // Also check if the y-axis label width has changed. This
                    // can occur if the plot is zoomed and the length of the
                    // longest visible label changes. If these widths are
                    // unchanged then skip resizing/repositioning the plot
                    // window, x-axis scroll bar, and cursor info components
                    if (xAxisScrollBar.getWidth() != prevXBarWidth
                        || (yAxisCustomScrollBar
                        && plotYLabelWidth != prevPlotYLabelWidth))
                    {
                        // Save the x-axis scroll bar width
                        prevXBarWidth = xAxisScrollBar.getWidth();

                        // Save the y-axis label width
                        prevPlotYLabelWidth = plotYLabelWidth;

                        // Enable the listener
                        progressListenerEnabled = true;
                    }

                    // Check that the listener is enabled
                    if (progressListenerEnabled)
                    {
                        // Get the plot window position
                        AxisSpace space = new AxisSpace();

                        // Set the left side x-axis spacing to account for the
                        // plot's y-axis labels; the '- 8' appears to be a
                        // fixed amount unaffected by font, L&F, or tick/label
                        // changes
                        space.setLeft(plotYLabelWidth - 8);

                        // Set the right side spacing so that the y-axis scroll
                        // bar doesn't overlap the plots
                        space.setRight(yAxisScrollBar.getWidth()
                                       + PLOT_SCROLL_BAR_PAD);

                        // Set the new x-axis bounds
                        labelPlot.setFixedRangeAxisSpace(space);

                        // Resize and reposition the x-axis scroll bar by
                        // adjusting the border of the panel containing the
                        // scroll bar. The left border width determines the
                        // scroll bar position and is based on the width of the
                        // plot's y-axis labels. The right border width
                        // determines the overall scroll bar width and accounts
                        // for the width of the y-axis scroll bar plus its
                        // padding
                        xAxisScrollBarPanel.setBorder(BorderFactory.createEmptyBorder(0,
                                                                                      plotYLabelWidth,
                                                                                      0,
                                                                                      yAxisScrollBar.getWidth()
                                                                                          + PLOT_SCROLL_BAR_PAD
                                                                                          + 7));

                        // Make the scroll bar and label plot visible now that
                        // the correct width has been determined and set
                        xAxisScrollBar.setVisible(true);
                        showLabelDomainAxis(true);

                        // Set up the cursor information fields; this is done
                        // here so that the first field can be aligned with the
                        // left edge of the plot windows
                        cursorPanel.removeAll();
                        cursorPanel.setBorder(BorderFactory.createEmptyBorder(LABEL_VERTICAL_SPACING,
                                                                              plotYLabelWidth,
                                                                              LABEL_VERTICAL_SPACING,
                                                                              0));
                        cursorPanel.add(startLabel);
                        cursorPanel.add(Box.createRigidArea(new Dimension(10, 0)));
                        cursorPanel.add(startTextArea);
                        cursorPanel.add(Box.createRigidArea(new Dimension(30, 0)));
                        cursorPanel.add(endLabel);
                        cursorPanel.add(Box.createRigidArea(new Dimension(10, 0)));
                        cursorPanel.add(endTextArea);
                        cursorPanel.add(Box.createRigidArea(new Dimension(30, 0)));
                        cursorPanel.add(deltaLabel);
                        cursorPanel.add(Box.createRigidArea(new Dimension(10, 0)));
                        cursorPanel.add(deltaTextArea);

                        // Must output something in order for the fields to
                        // display; use the latest start position (blank if no
                        // position has yet been determined)
                        if (startPosition == null)
                        {
                            startTextArea.setText(" ");
                        }
                        else
                        {
                            startTextArea.setText(String.format(" " + TIME_FORMAT,
                                                                startPosition));
                        }

                        // Request focus for the plot window so that key
                        // presses can be detected. This call must be made
                        // after the plot window is displayed
                        plotChartPanel.requestFocusInWindow();

                        // Disable the chart progress listener so that this
                        // section of code won't repeat every time the vertical
                        // lines (domain markers) are moved. This flag must be
                        // reenabled whenever the main or plotting window size
                        // is changed
                        progressListenerEnabled = false;

                        // Restore the default mouse pointer
                        perfMain.setMainCursor(Cursor.DEFAULT_CURSOR);
                    }
                }
            }
        });
    }

    /**************************************************************************
     * Set the height, in pixels, of the pane containing the x-axis labels
     *************************************************************************/
    private void setXAxisLabelPaneHeight()
    {
        // Once sizes & position are known calculate the height of the label
        // plot window position based on the label plot font sizes and padding
        // attributes
        int plotXLabelHeight = perfMain.getMainFrame().getFontMetrics(labelDomainAxis.getLabelFont()).getHeight()
                               + perfMain.getMainFrame().getFontMetrics(labelDomainAxis.getTickLabelFont()).getHeight()
                               + (int) (labelChart.getPadding().getTop()
                                        + labelChart.getPadding().getBottom()
                                        + labelDomainAxis.getTickMarkOutsideLength()
                                        + labelDomainAxis.getTickMarkInsideLength()
                                        + labelDomainAxis.getLabelInsets().getTop()
                                        + labelDomainAxis.getLabelInsets().getBottom()
                                        + labelDomainAxis.getTickLabelInsets().getTop()
                                        + labelDomainAxis.getTickLabelInsets().getBottom())
                               + 1;

        // Set the size of the label plot panel. The width value is ignored
        Dimension labelSize = new Dimension(0, plotXLabelHeight);
        labelPanel.setPreferredSize(labelSize);
        labelPanel.setMinimumSize(labelSize);
        labelPanel.setMaximumSize(labelSize);

        // Force the layout manager to redraw the window contents. Without this
        // the plot won't be displayed initially unless the main window is
        // resized
        plotAndLabelPanel.setSize(0, 0);
    }

    /**************************************************************************
     * Set up the plot markers
     *************************************************************************/
    private void setupPlotMarkers()
    {
        // Set up the vertical lines to be displayed for the start/end
        // positions (as determined by the mouse cursor) and set the color
        // and transparency for the shaded area in between
        startMarker.setPaint(Color.LIGHT_GRAY);
        endMarker.setPaint(Color.LIGHT_GRAY);
        startMarker.setStroke(new BasicStroke(1));
        endMarker.setStroke(new BasicStroke(1));
        shadeMarker.setPaint(new Color(80, 80, 100));

        // For each gap between logs (if multiple logs were read)
        for (int index = 0; index < perfLog.getDataGapIndex().size(); index++)
        {
            // Create a shaded area with a solid border to indicate the
            // data gap
            gapMarker.add(new IntervalMarker(0.0,
                                             0.0,
                                             new Color(120, 0, 120)));
            gapMarker.get(index).setOutlinePaint(Color.RED);
            gapMarker.get(index).setOutlineStroke(new BasicStroke(1));
        }
    }

    /**************************************************************************
     * Create the x-axis scroll bar
     *************************************************************************/
    private void createXAxisScrollBar()
    {
        // Create the horizontal scroll bar used for scrolling the
        // plots' x-axes
        xAxisScrollBar = new JScrollBar(JScrollBar.HORIZONTAL, 0, 1, 0, 1);

        // Hide the x-axis scroll bar until the plot finishes being drawn. This
        // keeps the scroll bar from showing up initially with the wrong length
        xAxisScrollBar.setVisible(false);

        // Set the scroll bar background color so that its the same as the rest
        // of the plot area
        xAxisScrollBar.setBackground((Color) plotChart.getBackgroundPaint());

        // Adjust the plot x-axis based on x-axis scroll bar position
        xAxisScrollBar.addAdjustmentListener(new AdjustmentListener()
        {
            /******************************************************************
             * Handle an x-axis scroll bar thumb movement event
             *****************************************************************/
            @Override
            public void adjustmentValueChanged(AdjustmentEvent ae)
            {
                // Calculate the new x-axis min/max values and use these to set
                // the domain
                double xLowerNew = xLower + ae.getValue()
                                   * (xUpperInit - xLowerInit)
                                   / X_SCROLLBAR_MULT;
                double xUpperNew = xUpper + xLowerNew - xLower;
                plotDomainAxis.setRange(xLowerNew, xUpperNew);
                labelDomainAxis.setRange(xLowerNew, xUpperNew);
            }
        });

        // Adjust the plot x-axis based on the mouse wheel movement
        xAxisScrollBar.addMouseWheelListener(new MouseWheelListener()
        {
            /******************************************************************
             * Handle a x-axis mouse wheel movement event
             *****************************************************************/
            @Override
            public void mouseWheelMoved(MouseWheelEvent mwe)
            {
                // Adjust the new x-axis scroll bar thumb position by one unit
                // scroll amount based on mouse wheel rotation direction. The
                // scroll "speed" is reduced by half to make finer adjustment
                // possible
                xAxisScrollBar.setValue(xAxisScrollBar.getValue()
                                        - xAxisScrollBar.getUnitIncrement()
                                        * (mwe.getUnitsToScroll() > 0
                                                                     ? 1
                                                                     : -1));
            }
        });
    }

    /**************************************************************************
     * Create the y-axis scroll bar
     *************************************************************************/
    private void createYAxisScrollBar()
    {
        // Create the vertical scroll bar used for scrolling the plot's y-axis
        yAxisScrollBar = new JScrollBar(JScrollBar.VERTICAL, 0, 1, 0, 1);

        // Adjust the plot y-axis based on y-axis scroll bar position
        yAxisScrollBar.addAdjustmentListener(new AdjustmentListener()
        {
            /******************************************************************
             * Handle a y-axis scroll bar thumb movement event
             *****************************************************************/
            @Override
            public void adjustmentValueChanged(AdjustmentEvent ae)
            {
                // Calculate the new y-axis min/max values and use these to set
                // the range
                double yLowerNew = yLower - ae.getValue()
                                   * (yUpperInit - yLowerInit)
                                   / Y_SCROLLBAR_MULT;
                double yUpperNew = yUpper + yLowerNew - yLower;
                symbolAxis.get(0).setRange(yLowerNew, yUpperNew);
            }
        });

        // Adjust the plot y-axis based on the mouse wheel movement
        yAxisScrollBar.addMouseWheelListener(new MouseWheelListener()
        {
            /******************************************************************
             * Handle a y-axis mouse wheel movement event
             *****************************************************************/
            @Override
            public void mouseWheelMoved(MouseWheelEvent mwe)
            {
                // Adjust the new y-axis scroll bar thumb position by one unit
                // scroll amount based on mouse wheel rotation direction
                yAxisScrollBar.setValue(yAxisScrollBar.getValue()
                                        + yAxisScrollBar.getUnitIncrement()
                                        * (mwe.getUnitsToScroll() > 0
                                                                     ? 1
                                                                     : -1));
            }
        });
    }

    /**************************************************************************
     * Display markers to show the gaps between individual log files (if
     * multiple files have been opened and concatenated)
     *************************************************************************/
    private void showLogDataGaps()
    {
        // Check if any log gaps exist (i.e., multiple log files were read)
        if (gapMarker != null)
        {
            // For each log data gap
            for (int index = 0; index < gapMarker.size(); index++)
            {
                // Fill the gap area
                fillShadedArea(gapMarker.get(index),
                               perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(index) - 1).getTimeStamp(),
                               perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(index)).getTimeStamp());
            }
        }
    }

    /**************************************************************************
     * Remove the start and end lines (domain markers) across all plots
     *************************************************************************/
    private void removeStartEndLine()
    {
        if (startPosition != null && cursorMode == CursorMode.START)
        {
            startPosition = null;
            startTextArea.setText(" ");

            // Remove the start domain marker
            adjustStartEndLine(startMarker, startPosition, false);
        }
        // Check if the end maker is active
        else if (endPosition != null && cursorMode == CursorMode.END)
        {
            endPosition = null;
            endTextArea.setText(" ");

            // Remove the end domain marker
            adjustStartEndLine(endMarker, endPosition, false);
        }
    }

    /**************************************************************************
     * Adjust the position of a start or end line (domain marker) across all
     * plots
     * 
     * @param marker
     *            start or end marker
     * 
     * @param xValue
     *            marker's updated x-axis coordinate
     * 
     * @param drawNew
     *            true to draw the marker
     *************************************************************************/
    private void adjustStartEndLine(ValueMarker marker,
                                    Double xValue,
                                    boolean drawNew)
    {
        // Erase the current indicated vertical line from all plots
        for (XYPlot plot : plots)
        {
            plot.removeDomainMarker(0, marker, Layer.FOREGROUND);
        }

        // Check if the new marker is to be drawn. If this marker overlays an
        // existing one then this flag should be false to prevent erasing the
        // other marker
        if (drawNew)
        {
            // Draw a vertical line across all plots at the new indicated
            // position
            marker.setValue(xValue);

            for (XYPlot plot : plots)
            {
                plot.addDomainMarker(0, marker, Layer.FOREGROUND);
            }
        }
    }

    /**************************************************************************
     * Fill the areas between the two domain markers across all plots
     * 
     * @param shading
     *            interval marker
     * 
     * @param startPosition
     *            marker's starting x-axis coordinate
     * 
     * @param endPosition
     *            marker's ending x-axis coordinate
     *************************************************************************/
    private void fillShadedArea(IntervalMarker shadeMarker,
                                double startPosition,
                                double endPosition)
    {
        // Check if the start value is less than or equal to the end value
        if (startPosition <= endPosition)
        {
            // Set the shade start and end positions
            shadeMarker.setStartValue(startPosition);
            shadeMarker.setEndValue(endPosition);
        }
        // End value is less than the start value
        else
        {
            // Set the shade start and end positions
            shadeMarker.setStartValue(endPosition);
            shadeMarker.setEndValue(startPosition);
        }

        // Apply the shaded area to each plot
        for (XYPlot plot : plots)
        {
            plot.addDomainMarker(0, shadeMarker, Layer.BACKGROUND, false);
        }
    }

    /**************************************************************************
     * Remove existing start and end lines (domain markers) and the shaded area
     * between the vertical lines across all plots
     * 
     * @param shading
     *            interval marker
     * 
     * @param startMarker
     *            start line marker
     * 
     * @param endMarker
     *            end line marker
     *************************************************************************/
    private void clearStartEndLinesAndAreas(IntervalMarker shadeMarker,
                                            ValueMarker startMarker,
                                            ValueMarker endMarker)
    {
        // Erase all of the start and end lines, and the shaded areas on
        // each plot. Don't notify the plot listeners of the changes so as not
        // to send a large number of change events
        for (XYPlot plot : plots)
        {
            plot.removeDomainMarker(0, startMarker, Layer.FOREGROUND, false);
            plot.removeDomainMarker(0, endMarker, Layer.FOREGROUND, false);
            plot.removeDomainMarker(0, shadeMarker, Layer.BACKGROUND, false);
        }
    }

    /**************************************************************************
     * Reset the start/end/delta text fields, and the start and end lines
     * (domain markers) and shaded areas across all plots
     * 
     * @param position
     *            text to place in start field
     *************************************************************************/
    private void resetStartEndLines(String position)
    {
        // Initialize the text fields
        startTextArea.setText(position);
        endTextArea.setText(" ");
        deltaTextArea.setText(" ");

        // Erase the start and end lines (domain markers) and the shaded areas
        // from all of the plots
        clearStartEndLinesAndAreas(shadeMarker, startMarker, endMarker);

        // Move the vertical line across all plots to the new start position.
        // This also sends a plot change event which makes the plot redraw and
        // remove the markers
        adjustStartEndLine(startMarker, startPosition, true);
    }

    /**************************************************************************
     * Show the minor or major frame lines across all plots
     * 
     * @param marker
     *            minor (major) frame marker list
     * 
     * @param timeStamp
     *            frame line time stamp
     * 
     * @param lineColor
     *            frame line color
     *************************************************************************/
    private void showFrameLines(List<ValueMarker> marker,
                                double timeStamp,
                                Paint lineColor)
    {
        // Create a line at the time stamp for the minor or major frame ID
        ValueMarker mkr = new ValueMarker(timeStamp);

        // Set the frame line characteristics
        mkr.setPaint(lineColor);
        mkr.setStroke(new BasicStroke(1));

        // Add the line to the list (needed for when the line is removed)
        marker.add(mkr);

        // For each plot
        for (XYPlot plot : plots)
        {
            // Draw a vertical line across all plots at the minor or major
            // frame start position
            plot.addDomainMarker(0, mkr, Layer.BACKGROUND);
        }
    }

    /**************************************************************************
     * Hide the minor or major frame lines across all plots
     * 
     * @param marker
     *            minor (major) frame marker list
     *************************************************************************/
    private void hideFrameLines(List<ValueMarker> marker)
    {
        // Ensure that the markers exist before attempting to hide them
        if (marker != null)
        {
            // For each line
            for (ValueMarker mkr : marker)
            {
                // For each plot
                for (XYPlot plot : plots)
                {
                    // Remove the minor or major frame line (domain marker),
                    // if visible
                    plot.removeDomainMarker(0, mkr, Layer.BACKGROUND);
                }
            }

            // Clear the list of lines since they're no longer needed
            marker.clear();
        }
    }

    /**************************************************************************
     * Show or hide the minor and major frame lines across all plots
     *************************************************************************/
    protected void adjustFrameLines()
    {
        // Set the flags that determine if the minor or major frame lines
        // should be displayed or removed
        boolean doMinor = plotPrefs.isShowPlotMinorFrames(false)
                          && perfIDs.getPerfID(plotPrefs.getMinorID(false)) != null;
        boolean doMajor = plotPrefs.isShowPlotMajorFrames(false)
                          && perfIDs.getPerfID(plotPrefs.getMajorID(false)) != null;

        // Hide the minor and major frame lines, if present. This must be done
        // even if the lines are only being changed
        hideFrameLines(minorMarker);
        hideFrameLines(majorMarker);

        // Check if either the minor or major frame lines should be displayed
        if (doMinor || doMajor)
        {
            // Set through the log data
            for (PerfLog log : perfLog.getPerfLogData())
            {
                // Check if this is an entry event. Frame lines are only drawn
                // for entry events
                if (log.isEntryEvent())
                {
                    // Check if the minor frame lines should be displayed
                    if (doMinor)
                    {
                        // Check if the the log ID matches the minor frame ID
                        if (log.getPerfID() == plotPrefs.getMinorID(false))
                        {
                            // Add a minor frame marker at this time stamp
                            showFrameLines(minorMarker,
                                           log.getTimeStamp(),
                                           plotPrefs.getMinorFrameColor(false));
                        }
                    }

                    // Check if the major frame lines should be displayed
                    if (doMajor)
                    {
                        // Check if the the log ID matches the major frame ID
                        if (log.getPerfID() == plotPrefs.getMajorID(false))
                        {
                            // Add a major frame line at this time stamp
                            showFrameLines(majorMarker,
                                           log.getTimeStamp(),
                                           plotPrefs.getMajorFrameColor(false));
                        }
                    }
                }
            }
        }
    }

    /**************************************************************************
     * Convert the mouse pointer x-coordinate to the equivalent plot coordinate
     * 
     * @param point
     *            mouse pointer location in component coordinates
     * 
     * @return Mouse x-axis location in plot coordinates
     *************************************************************************/
    private double getMouseXCoordinate(Point point)
    {
        // Get a reference to the plot under the mouse pointer
        XYPlot plot = (XYPlot) plotChartPanel.getChart().getPlot();

        // Convert the mouse x-coordinate to the plot x-coordinate and return
        // the value
        return plot.getDomainAxis().java2DToValue(plotChartPanel.translateScreenToJava2D(point).getX(),
                                                  plotChartPanel.getScreenDataArea(),
                                                  plot.getDomainAxisEdge());
    }

    /**************************************************************************
     * Reset the x-axis (domain) minimum and maximum boundaries
     *************************************************************************/
    protected void setXAxisMinMax()
    {
        // The x-axis boundaries are 0.0 (min) and maximumTimeStamp (max); pad
        // these to make the plots look better
        plotDomainAxis.setRange(-xPadding,
                                perfLog.getMaximumTimeStamp() + xPadding);
        labelDomainAxis.setRange(-xPadding,
                                 perfLog.getMaximumTimeStamp() + xPadding);
    }

    /**************************************************************************
     * Reset the y-axis (range) minimum and maximum boundaries for each plot
     *************************************************************************/
    protected void setYAxisMinMax()
    {
        // For each plot
        for (SymbolAxis sa : symbolAxis)
        {
            // Check if the custom y-axis scroll bar is in use
            if (yAxisCustomScrollBar)
            {
                // Pad the y-axis boundaries by 1 to make the plots look
                // better. The range difference (upper - lower limit) must be
                // positive, so for the case where no IDs are selected
                // (plotSeries.size() = 1) the upper limit is 1 - 1.499999 =
                // -0.499999, which is greater than the lower limit of -0.5
                sa.setRange(-0.5, plotSeries.size() - 1.499999);
            }
            // The default y-axis scroll bar is in use
            else
            {
                // The y-axis boundaries are 0 (exit) and 1 (entry); pad these
                // to make the plots look better
                sa.setRange(-0.1, 1.1);
            }
        }
    }

    /**************************************************************************
     * Initialize the x-axis boundary values used by the horizontal scroll bar
     *************************************************************************/
    protected void initXAxisMinMax()
    {
        getXAxisMinMax();
        xLowerInit = xLower;
        xUpperInit = xUpper;
    }

    /**************************************************************************
     * Initialize the y-axis boundary values used by the vertical scroll bar
     *************************************************************************/
    protected void initYAxisMinMax()
    {
        getYAxisMinMax();
        yLowerInit = yLower;
        yUpperInit = yUpper;
    }

    /**************************************************************************
     * Get the current x-axis (domain) minimum and maximum boundary values
     *************************************************************************/
    private void getXAxisMinMax()
    {
        xLower = plotDomainAxis.getLowerBound();
        xUpper = plotDomainAxis.getUpperBound();
    }

    /**************************************************************************
     * Get the current y-axis (range) minimum and maximum boundary values
     *************************************************************************/
    private void getYAxisMinMax()
    {
        yLower = symbolAxis.get(0).getLowerBound();
        yUpper = symbolAxis.get(0).getUpperBound();
    }

    /**************************************************************************
     * Set the horizontal (x-axis) scroll bar thumb size and position based on
     * the current x-axis minimum and maximum values
     * 
     * @return Fraction to be displayed of the total plot's x-axis, times the
     *         x-axis multiplier
     *************************************************************************/
    private double setXAxisScrollBar()
    {
        // Get the x-axis boundaries
        getXAxisMinMax();

        // Calculate the fraction of the total chart (x-axis) that's showing
        double xFraction = X_SCROLLBAR_MULT
                           * (xUpper - xLower)
                           / (xUpperInit - xLowerInit);

        // Calculate where to start the left side of the thumb
        double xStart = X_SCROLLBAR_MULT
                        * (xLower - xLowerInit)
                        / (xUpperInit - xLowerInit);

        // Set new scroll bar thumb position and size
        xAxisScrollBar.setValues(0, // Thumb position
                                 (int) xFraction, // Thumb size
                                 (int) -xStart, // Minimum
                                 X_SCROLLBAR_MULT
                                     - (int) xStart); // Maximum

        return xFraction;
    }

    /**************************************************************************
     * Set the vertical (y-axis) scroll bar thumb size and position based on
     * the current y-axis minimum and maximum values
     * 
     * @return Fraction to be displayed of the total plot's y-axis, times the
     *         y-axis multiplier
     *************************************************************************/
    private double setYAxisScrollBar()
    {
        // Get the y-axis boundaries
        getYAxisMinMax();

        // Calculate the fraction of the total chart (y-axis) that's showing
        double yFraction = Y_SCROLLBAR_MULT
                           * (yUpper - yLower) /
                           (yUpperInit - yLowerInit);

        // Calculate where to start the top side of the thumb
        double yStart = Y_SCROLLBAR_MULT
                        - yFraction
                        - Y_SCROLLBAR_MULT
                        * (yLower - yLowerInit)
                        / (yUpperInit - yLowerInit);

        // Set new scroll bar thumb position and size
        yAxisScrollBar.setValues(0, // Thumb position
                                 (int) yFraction, // Thumb size
                                 (int) -yStart, // Minimum
                                 Y_SCROLLBAR_MULT
                                     - (int) yStart); // Maximum

        return yFraction;
    }

    /**************************************************************************
     * Adjust the x- and y-axis scroll bar unit and block increment values
     *************************************************************************/
    protected void adjustScrollBarIncrements()
    {
        // Adjust the x-axis scroll bar unit and block increment values
        // based on the zoom level
        adjustScrollBarIncrements(xAxisScrollBar);

        // Check if the custom y-axis scroll bar is active
        if (yAxisCustomScrollBar)
        {
            // Adjust the y-axis scroll bar unit and block increment values
            // based on the zoom level
            adjustScrollBarIncrements(yAxisScrollBar);
        }
    }

    /**************************************************************************
     * Adjust the scroll bar unit and block increment values based on the zoom
     * level
     * 
     * @param scrollBar
     *            scroll bar (x- or y-axis) to adjust
     *************************************************************************/
    private void adjustScrollBarIncrements(JScrollBar scrollBar)
    {
        double fraction;

        // Check if the x-axis scroll bar is to be adjusted
        if (scrollBar.equals(xAxisScrollBar))
        {
            // Adjust the x-axis boundaries if the new values are outside the
            // initial minimum and maximum values or the difference between the
            // maximum and minimum is below the minimum allowed
            limitAxisBoundaries(plots.get(0).getDomainAxis(),
                                xLowerInit,
                                xUpperInit,
                                MINIMUM_TIME_DELTA);

            // Adjust the x-axis scroll bar thumb size and position and get the
            // fraction of the total plot that's visible
            fraction = setXAxisScrollBar();
        }
        // Adjust the y-axis scroll bar
        else
        {
            // Adjust the y-axis boundaries if the new values are outside the
            // initial minimum and maximum values or the difference between the
            // maximum and minimum is below the minimum allowed
            limitAxisBoundaries(plots.get(0).getRangeAxis(),
                                yLowerInit,
                                yUpperInit,
                                1.2);

            // Adjust the y-axis scroll bar thumb size and position and get the
            // fraction of the total plot that's visible
            fraction = setYAxisScrollBar();
        }

        // Calculate the block and unit increments based on the new range. Each
        // block scroll is approximately the current span of the the x-axis
        // (slightly less so that some of the previous plot remains visible),
        // and each unit scroll is 1/30 of the block scroll. The scroll values
        // are set to 1 if the calculated values equal zero so that scrolling
        // is still possible no matter how far the plot is zoomed in
        int blockIncr = (int) (fraction * 0.95);

        if (blockIncr == 0)
        {
            blockIncr = 1;
        }

        int unitIncr = blockIncr / 30;

        if (unitIncr == 0)
        {
            unitIncr = 1;
        }

        // Set the scroll bar block and unit increments
        scrollBar.setBlockIncrement(blockIncr);
        scrollBar.setUnitIncrement(unitIncr);
    }

    /**************************************************************************
     * This class is used to override the following ChartPanel methods:
     * 
     * <pre>
     * - restoreAutoRangeBounds: force the y-axis minimum and maximum to remain
     *                           unchanged when zooming
     * - mouseExited: remove the domain markers when the mouse exits the plot
     * - zoom: to Adjust the x-axis scroll bar when zooming in on a plot
     * - zoomInRange: force the plot y-axis boundaries to their initial
     *                values when zooming
     * - zoomOutRange: force the plot y-axis boundaries to their initial
     *                 values when unzooming
     * - zoomInDomain: adjust the horizontal scroll bar thumb size and position
     *                 when zooming
     * - zoomOutDomain: adjust the horizontal scroll bar thumb size and
     *                  position when unzooming
     * </pre>
     *************************************************************************/
    @SuppressWarnings("serial")
    public class CPMChartPanel extends ChartPanel
    {
        // Data point tool tip text
        private String toolTipText;

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
         * CPMChartPanel constructor with no options
         * 
         * @param chart
         *            chart for which to create the chart panel
         *********************************************************************/
        private CPMChartPanel(JFreeChart chart)
        {
            super(chart);
        }

        /**********************************************************************
         * Override the context menu Save As command. The chart x-axis label
         * and tick labels are normally disabled; these are displayed in a
         * separate (label) chart, which is necessary for the combined step
         * plots embedded in a scroll pane so that the labels can't scroll out
         * of view. The chart labels are enabled briefly so that the saved
         * version of the plot will display these labels
         *********************************************************************/
        @Override
        public void doSaveAs()
        {
            try
            {
                // Enable the "actual" x-axis label display. For screen display
                // a separate chart is used for displaying the x-axis labels
                plotDomainAxis.setVisible(true);

                // Call the context menu SaveAs method
                super.doSaveAs();

                // Disable the x-axis label display
                plotDomainAxis.setVisible(false);
            }
            catch (IOException ioe)
            {
                // Inform the user that the plot can't be saved
                new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                         "<html><b>Plot save failed",
                                                         "Save Fail",
                                                         JOptionPane.WARNING_MESSAGE,
                                                         OK_OPTION);
            }
        }

        /**********************************************************************
         * Override the context menu Copy command. The chart x-axis label and
         * tick labels are normally disabled; these are displayed in a separate
         * (label) chart, which is necessary for the combined step plots
         * embedded in a scroll pane so that the labels can't scroll out of
         * view. The chart labels are enabled briefly so that the copy of the
         * plot will display these labels
         *********************************************************************/
        @Override
        public void doCopy()
        {
            // Enable the "actual" x-axis label display. For screen display
            // a separate chart is used for displaying the x-axis labels
            plotDomainAxis.setVisible(true);

            // Call the context menu Copy method
            super.doCopy();

            // Disable the x-axis label display
            plotDomainAxis.setVisible(false);
        }

        /**********************************************************************
         * Override the context menu Print command. The chart x-axis label and
         * tick labels are normally disabled; these are displayed in a separate
         * (label) chart, which is necessary for the combined step plots
         * embedded in a scroll pane so that the labels can't scroll out of
         * view. The chart labels are enabled briefly so that the printed of
         * the plot will display these labels
         *********************************************************************/
        @Override
        public void createChartPrintJob()
        {
            // Enable the "actual" x-axis label display. For screen display a
            // separate chart is used for displaying the x-axis labels
            plotDomainAxis.setVisible(true);

            // Call the context menu Print method
            super.createChartPrintJob();

            // Disable the x-axis label display
            plotDomainAxis.setVisible(false);
        }

        /**********************************************************************
         * Force the plot x-axis boundaries to their initial values when
         * unzooming
         *********************************************************************/
        @Override
        public void restoreAutoRangeBounds()
        {
            // Reset the plot x-axis to its initial (unzoomed) min/max values
            setXAxisMinMax();

            // Get the x-axis boundaries
            getXAxisMinMax();

            // Reset the scroll bar to the "all showing" values
            xAxisScrollBar.setValues(0, 1, 0, 1);
        }

        /**********************************************************************
         * Force the plot y-axis boundaries to their initial values when
         * unzooming
         *********************************************************************/
        @Override
        public void restoreAutoDomainBounds()
        {
            // Reset the plot y-axis to its initial (unzoomed) min/max values
            setYAxisMinMax();

            // Check if the custom y-axis scroll bar is in use
            if (yAxisCustomScrollBar)
            {
                // Get the y-axis boundaries
                getYAxisMinMax();

                // Reset the scroll bar to the "all showing" values
                yAxisScrollBar.setValues(0, 1, 0, 1);
            }
        }

        /**********************************************************************
         * Adjust the x-axis scroll bar when zooming in on a plot
         *********************************************************************/
        @Override
        public void zoom(Rectangle2D zoomRectangle)
        {
            super.zoom(zoomRectangle);

            // Adjust the x- and y-axis scroll bar unit and block increment
            // values based on the zoom level
            adjustScrollBarIncrements();
        }

        /**********************************************************************
         * This is called when using the context menu Zoom In Domain Axis
         * command. Adjust the x-axis scroll bar thumb size and position when
         * zooming in
         *********************************************************************/
        @Override
        public void zoomInDomain(double x, double y)
        {
            super.zoomInDomain(x, y);

            // Adjust the x-axis scroll bar unit and block increment values
            // based on the zoom level
            adjustScrollBarIncrements(xAxisScrollBar);
        }

        /**********************************************************************
         * This is called when using the context menu Zoom Out Domain Axis
         * command. Adjust the x-axis scroll bar thumb size and position when
         * zooming out
         *********************************************************************/
        @Override
        public void zoomOutDomain(double x, double y)
        {
            super.zoomOutDomain(x, y);

            // Adjust the x-axis scroll bar unit and block increment values
            // based on the zoom level
            adjustScrollBarIncrements(xAxisScrollBar);
        }

        /**********************************************************************
         * This is called when using the context menu Zoom In Range Axis
         * command. If the custom y-axis scroll bar is in use (i.e., for the
         * thread plot) adjust the y-axis scroll bar thumb size and position
         * when zooming in. If the custom y-axis scroll bar is not in use
         * (i.e., for the step plot) force the plot y-axis boundaries to their
         * initial values
         *********************************************************************/
        @Override
        public void zoomInRange(double x, double y)
        {
            super.zoomInRange(x, y);

            // Check if the custom y-axis scroll bar is not in use
            if (!yAxisCustomScrollBar)
            {
                // Reset the plot y-axis boundaries
                setYAxisMinMax();
            }
            // The custom y-axis scroll bar is in use
            else
            {
                // Adjust the y-axis scroll bar unit and block increment values
                // based on the zoom level
                adjustScrollBarIncrements(yAxisScrollBar);
            }
        }

        /**********************************************************************
         * This is called when using the context menu Zoom Out Range Axis
         * command. If the custom y-axis scroll bar is in use (i.e., for the
         * thread plot) adjust the y-axis scroll bar thumb size and position
         * when zooming out. If the custom y-axis scroll bar is not in use
         * (i.e., for the step plot) force the plot y-axis boundaries to their
         * initial values
         *********************************************************************/
        @Override
        public void zoomOutRange(double x, double y)
        {
            super.zoomOutRange(x, y);

            // Check if the custom y-axis scroll bar is not in use
            if (!yAxisCustomScrollBar)
            {
                // Reset the plot y-axis boundaries
                setYAxisMinMax();
            }
            // The custom y-axis scroll bar is in use
            else
            {
                // Adjust the y-axis scroll bar unit and block increment values
                // based on the zoom level
                adjustScrollBarIncrements(yAxisScrollBar);
            }
        }

        /**********************************************************************
         * This is called when using the context menu Zoom In Both Axes
         * command. Adjust the x- and y-axis scroll bar thumb sizes and
         * position when zooming in
         *********************************************************************/
        @Override
        public void zoomInBoth(double x, double y)
        {
            zoomInDomain(x, y);
            zoomInRange(x, y);
        }

        /**********************************************************************
         * This is called when using the context menu Zoom out Both Axes
         * command. Adjust the x- and y-axis scroll bar thumb sizes and
         * position when zooming out
         *********************************************************************/
        @Override
        public void zoomOutBoth(double x, double y)
        {
            zoomOutDomain(x, y);
            zoomOutRange(x, y);
        }

        /**********************************************************************
         * Remove the active domain marker (start or end) and clear its text
         * field when the mouse cursor exits the plot
         *********************************************************************/
        @Override
        public void mouseExited(MouseEvent me)
        {
            // Remove the start or end domain marker
            removeStartEndLine();
        }

        /**********************************************************************
         * Create a tool tip by converting the screen X-Y coordinates into
         * chart area X-Y coordinates and then looks for a data point in a data
         * set that lies within a "hot spot" bounding box around that point.
         * Overriding the method is necessary to improve the execution speed,
         * otherwise the plot updates very slowly with tool tips enabled
         *********************************************************************/
        @Override
        public String getToolTipText(MouseEvent me)
        {
            // Get the baseline tool tip text for the selected point
            toolTipText = super.getToolTipText(me);

            // Check if the baseline tool tip contains any text
            if (toolTipText != null)
            {
                // Get the mouse pointer location
                Point point = me.getPoint();

                // Get the plot and rendering information
                XYPlot plot = (XYPlot) getChart().getPlot();
                PlotRenderingInfo info = getChartRenderingInfo().getPlotInfo();

                // Check if this the step plot
                if (plot instanceof CombinedDomainXYPlot)
                {
                    // Get the index of the subplot under the pointer
                    int index = info.getSubplotIndex(point);

                    // Check if the pointer is over a subplot and not the gap
                    // between subplots. If the pointer is over a gap then the
                    // range axis will return null below and the tool tip check
                    // is skipped
                    if (index >= 0)
                    {
                        // Get the subplot and rendering information
                        plot = (XYPlot) ((CombinedDomainXYPlot) plot).getSubplots().get(index);
                        info = getChartRenderingInfo().getPlotInfo().getSubplotInfo(index);
                    }
                }

                ValueAxis rangeAxis = plot.getRangeAxis();

                // Check if the plot has a valid range axis. The label plot,
                // used to display the x-axis labels, does not have a range
                // axis and can be skipped
                if (rangeAxis != null)
                {
                    ValueAxis domainAxis = plot.getDomainAxis();
                    Rectangle2D screenArea = scale(info.getDataArea());

                    double hotspotSizeX = HOTSPOT_SIZE * getScaleX();
                    double hotspotSizeY = HOTSPOT_SIZE * getScaleY();
                    double x0 = point.getX();
                    double y0 = point.getY();

                    // Calculate the boundaries of the current pointer position
                    // based on the hot spot size
                    double xLowerBound = domainAxis.java2DToValue(x0 - hotspotSizeX,
                                                                  screenArea,
                                                                  RectangleEdge.BOTTOM);
                    double yLowerBound = rangeAxis.java2DToValue(y0 + hotspotSizeY,
                                                                 screenArea,
                                                                 RectangleEdge.LEFT);
                    double xUpperBound = domainAxis.java2DToValue(x0 + hotspotSizeX,
                                                                  screenArea,
                                                                  RectangleEdge.BOTTOM);
                    double yUpperBound = rangeAxis.java2DToValue(y0 - hotspotSizeY,
                                                                 screenArea,
                                                                 RectangleEdge.LEFT);

                    // Get the data set to check based on the plot type
                    int datasetIndex = plotType == STEP_PLOT ? 0 : 1;
                    XYDataset dataset = plot.getDataset(datasetIndex);

                    // Initialize the flag that indicates if a note has been
                    // found
                    boolean foundPoint = false;

                    int seriesCount = dataset.getSeriesCount();

                    // Step through each series in this plot
                    for (int series = 0; series < seriesCount && !foundPoint; series++)
                    {
                        int itemCount = dataset.getItemCount(series);

                        // Step through each item in this series
                        for (int item = 0; item < itemCount && !foundPoint; item++)
                        {
                            double xValue = dataset.getXValue(series, item);

                            // Check if the data set 'X' value lies between the
                            // hot spot boundaries (xLowerBound < xValue <
                            // xUpperBound)
                            if (xLowerBound < xValue)
                            {
                                if (xValue < xUpperBound)
                                {
                                    double yValue = dataset.getYValue(series, item);

                                    // Check if the data set 'Y' value lies
                                    // between the hot spot boundaries
                                    // (yLowerBound < yValue < yUpperBound)
                                    if (yLowerBound < yValue && yValue < yUpperBound)
                                    {
                                        // Point is within the hot spot
                                        // boundaries; get the overrun and/or
                                        // annotation associated with the point
                                        appendToolTipText(plot,
                                                          datasetIndex,
                                                          dataset,
                                                          series,
                                                          item);

                                        // Set the flag indicating a point was
                                        // found in order to exit the loops
                                        foundPoint = true;
                                    }
                                }
                                // The x-values only get larger, so no need to
                                // keep checking once the maximum possible for
                                // this tool tip is exceeded
                                else
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            return toolTipText;
        }

        /**********************************************************************
         * Append an overrun and/or an annotation, if these exists, to the tool
         * tip text string associated with a specified point. Apply word
         * wrapping to the note to prevent making the tool tip box too wide
         * 
         * @param plot
         *            containing the point
         * 
         * @param datasetIndex
         *            index of the data set containing the point
         * 
         * @param dataset
         *            data set containing the point
         * 
         * @param series
         *            series containing the point
         * 
         * @param item
         *            index of the point in the series
         *********************************************************************/
        private void appendToolTipText(XYPlot plot,
                                       int datasetIndex,
                                       XYDataset dataset,
                                       int series,
                                       int item)
        {
            // Step through the log data
            for (PerfLog log : perfLog.getPerfLogData())
            {
                // Check if this log entry is the one under the mouse pointer
                // (i.e., the time stamp matches the selected point and the
                // series key matches the ID name)
                if (log.getTimeStamp() == dataset.getXValue(series, item)
                    && dataset.getSeriesKey(series).equals(perfIDs.getPerfID(log.getPerfID()).getPerfIDName(true)))
                {
                    // Check if a potential overrun occurred for this log entry
                    if (log.getOverrun() != 0.0)
                    {
                        // Add the overrun amount to the tool tip text
                        toolTipText += "<br><i>("
                                       + String.format(TIME_FORMAT,
                                                       log.getOverrun())
                                       + " sec overrun)</i>";
                    }

                    // Check if a note exists for this log entry
                    if (!log.getNotes().isEmpty())
                    {
                        // Get the note associated with this log entry
                        String text = log.getNotes();

                        // Process the note to break it into pieces for word
                        // wrapping purposes
                        while (text.length() > TOOL_TIP_WORD_WRAP)
                        {
                            // Search for a break point going backwards
                            int index = searchForBreak(text, -1);

                            // Check if no break was found
                            if (index == 1)
                            {
                                // Search for a break going forwards
                                index = searchForBreak(text, 1);
                            }

                            // Get the remaining text in the note string, if
                            // any
                            text = text.substring(index);
                        }

                        // Check if any text is remains after breaking the note
                        // into pieces
                        if (text.length() != 0)
                        {
                            // Add the remainder to the tool tip text
                            toolTipText += "<br>" + text;
                        }

                        break;
                    }
                }
            }
        }

        /**********************************************************************
         * Search for a space within a string starting at a predefined break
         * point and in the specified direction. Append the portion of the
         * string up to that point to the tool tip text string
         * 
         * @param text
         *            note text in which to search for a space
         * 
         * @param direction
         *            direction in which to search (-1 for backwards, 1 for
         *            forwards)
         * 
         * @return Index into the note string where the search stopped
         *********************************************************************/
        private int searchForBreak(String text,
                                   int direction)
        {
            int index;
            int stopIndex;

            // Check if the search direction is backwards
            if (direction == -1)
            {
                // Set stopping point to the beginning of the text
                stopIndex = 1;
            }
            // Search is forwards
            else
            {
                // Set the stopping point t the end of the text
                stopIndex = text.length() - 2;
            }

            // Step backwards through the string starting at the target wrap
            // point
            for (index = TOOL_TIP_WORD_WRAP - 1; index != stopIndex; index += direction)
            {
                // Check for a space character
                if (text.charAt(index) == ' ')
                {
                    // Insert the string portion into the tool tip text
                    toolTipText += "<br>" + text.substring(0, index);

                    // Stop looking
                    break;
                }
            }

            return index;
        }
    }

    /**************************************************************************
     * Check the axis lower and upper boundary values and, if the value is
     * outside the specified limits then restore the boundary to the limit
     * value. Check the difference between the upper and lower boundary values
     * and, if the difference is below a specified minimum, adjust the
     * difference to equal the minimum
     * 
     * @param axis
     *            reference to the axis to limit
     * 
     * @param minimum
     *            minimum boundary value allowed for this axis
     * 
     * @param maximum
     *            maximum boundary value allowed for this axis
     * 
     * @param minimumDelta
     *            minimum allowed difference between the upper and lower
     *            boundary values
     *************************************************************************/
    private void limitAxisBoundaries(ValueAxis axis,
                                     double minimum,
                                     double maximum,
                                     double minimumDelta)
    {
        // Check if the lower boundary value is less than the minimum lower
        // boundary value
        if (axis.getLowerBound() < minimum)
        {
            // Set the lower boundary value to the minimum
            axis.setLowerBound(minimum);
        }

        // Check if the upper boundary value is less than the maximum upper
        // boundary value
        if (axis.getUpperBound() > maximum)
        {
            // Set the upper boundary value to the maximum
            axis.setUpperBound(maximum);
        }

        // Check if the difference between the upper and lower boundary values
        // is less than the minimum allowed
        if (axis.getUpperBound() - axis.getLowerBound() < minimumDelta)
        {
            // Check if the lower value is at the minimum limit
            if (Math.abs(axis.getLowerBound() - minimum) < 0.000001)
            {
                axis.setLowerBound(minimum);
                axis.setUpperBound(minimum + minimumDelta);
            }
            // Check if the upper value is at the maximum limit
            else if (Math.abs(axis.getUpperBound() - maximum) < 0.000001)
            {
                axis.setLowerBound(maximum - minimumDelta);
                axis.setUpperBound(maximum);
            }
            // Lower and upper values are not at the limits
            else
            {
                // Calculate the midpoint between the boundary values
                double mid = (axis.getUpperBound() + axis.getLowerBound()) / 2.0;

                // Set the lower and upper values equidistant from the midpoint
                // with a difference equal to the minimum allowed
                axis.setLowerBound(mid - minimumDelta / 2.0);
                axis.setUpperBound(mid + minimumDelta / 2.0);
            }
        }
    }

    /**************************************************************************
     * Create the plot and label x-axes
     *************************************************************************/
    private void createPlotDomainAxes()
    {
        // Create the x-axis for the plot
        plotDomainAxis = new NumberAxis();
        plotDomainAxis.setMinorTickCount(5);

        // Hide the plot x-axis labels since these are shown on the label plot
        plotDomainAxis.setVisible(false);

        // Create the x-axis for the x-axis label plot
        labelDomainAxis = new NumberAxis("Time (seconds)");

        // Set the x-axis tick label format and the number of minor tick lines
        // between each labeled tick mark
        labelDomainAxis.setNumberFormatOverride(new DecimalFormat(X_AXIS_LABEL_FORMAT));
        labelDomainAxis.setMinorTickCount(5);

        // Show tick marks above the x-axis labels
        labelDomainAxis.setTickMarkOutsideLength(2);
        labelDomainAxis.setTickMarkInsideLength(2);
        labelDomainAxis.setMinorTickMarkOutsideLength(0);
        labelDomainAxis.setMinorTickMarkInsideLength(2);
        labelDomainAxis.setMinorTickMarksVisible(true);

        // Set x-axis label fonts
        labelDomainAxis.setLabelFont(PLOT_LABEL_FONT);
        labelDomainAxis.setTickLabelFont(PLOT_LABEL_FONT);
    }

    /**************************************************************************
     * Show or hide the vertical grid lines based on the user's selection in
     * the plot Preferences dialog
     *************************************************************************/
    protected void setVerticalGridLines()
    {
        // Step through each subplot
        for (int index = 0; index < plots.size(); index++)
        {
            // Draw the vertical grid lines
            plots.get(index).setDomainMinorGridlinesVisible(plotPrefs.isShowPlotVerticalGrid(false));
            plots.get(index).setDomainGridlinesVisible(plotPrefs.isShowPlotVerticalGrid(false));
        }
    }

    /**************************************************************************
     * Create the plot used for displaying the x-axis in a separate window
     * 
     * @return X-axis label chart
     *************************************************************************/
    private JFreeChart createXAxisLabelChart()
    {
        // Create a series for the x-axis label plot and populate it with the
        // min/max x values. The y values aren't used for the label plot
        XYSeries xAxisLabelSeries = new XYSeries(0);
        xAxisLabelSeries.add(0.0, 0.0);
        xAxisLabelSeries.add(perfLog.getMaximumTimeStamp(), 0.0);

        XYSeriesCollection dataset = new XYSeriesCollection();
        dataset.addSeries(xAxisLabelSeries);

        // Create a single plot labelDomainAxis
        labelPlot = new XYPlot(dataset, labelDomainAxis, null, null);

        // Allow panning of the label plot's x-axis
        labelPlot.setDomainPannable(true);

        // Create the chart
        JFreeChart chart = new JFreeChart(labelPlot);
        chart.removeLegend();

        // Set the background color to match the application's color scheme
        chart.setBackgroundPaint(plotChart.getBackgroundPaint());

        return chart;
    }

    /**************************************************************************
     * Show or hide the label domain axis (x-axis) components by setting the
     * colors. This allows the domain axis to be displayed, but hidden, until
     * the size can be determined, at which point the colors are set to show
     * the domain axis components
     * 
     * @param show
     *            true to enable display of the x-axis components
     *************************************************************************/
    private void showLabelDomainAxis(boolean show)
    {
        Paint foreground;
        Paint background;

        // Check if the axis components are to be visible
        if (show)
        {
            // Set the visible colors
            foreground = Color.BLACK;
            background = Color.WHITE;
        }
        // Hide the axis
        else
        {
            // Set the components to the background color
            foreground = plotChart.getBackgroundPaint();
            background = plotChart.getBackgroundPaint();
        }

        // Set the label plot window background and outline colors
        labelPlot.setBackgroundPaint(background);
        labelPlot.setOutlinePaint(foreground);

        // Set the label plot x-axis label and tick colors
        labelDomainAxis.setLabelPaint(foreground);
        labelDomainAxis.setAxisLinePaint(foreground);
        labelDomainAxis.setTickLabelPaint(foreground);
        labelDomainAxis.setTickMarkPaint(foreground);
    }

    /**************************************************************************
     * Set the plot background and grid line colors based on the plot
     * Preference dialog selections
     * 
     * @param adjustFirstColor
     *            true if the first line is not a performance ID (e.g. the CPU
     *            activity or thread of execution lines)
     *************************************************************************/
    protected void setPlotColors(boolean adjustFirstColor)
    {
        // Get the selected plot background color
        Color backColor = plotPrefs.getBackgroundColor(false);

        // Check if the first line's color needs to be determined. This is true
        // if the first line represents the CPU activity
        if (adjustFirstColor)
        {
            // Set the RGB value to fully off (0) or fully on (255) based on
            // the combination of the background color's red, green, and blue
            // components. A dark background results in a white line color; a
            // light background results in a black line color
            int rgb = (backColor.getRed()
                       + backColor.getGreen()
                       + backColor.getBlue()) / 3 > 128
                                                       ? 0
                                                       : 255;

            // Set the line color (white or black)
            plots.get(0).getRenderer().setSeriesPaint(0,
                                                      new Color(rgb,
                                                                rgb,
                                                                rgb));
        }

        // Step through each subplot
        for (XYPlot plot : plots)
        {
            // Set the plot window background color
            plot.setBackgroundPaint(backColor);

            // Set the plot grid line colors and visibility
            plot.setDomainGridlinePaint(plotPrefs.getMajorGridColor(false));
            plot.setDomainMinorGridlinePaint(plotPrefs.getMinorGridColor(false));
        }
    }

    /**************************************************************************
     * Add or remove frame overrun indicators based on the plot Preference
     * dialog selection
     *************************************************************************/
    protected void showOverruns()
    {
        // Create the frame overrun circle indicator shape
        Drawable indicator = new Drawable()
        {
            /******************************************************************
             * Draw the overrun indicator
             *****************************************************************/
            @Override
            public void draw(Graphics2D g2, Rectangle2D area)
            {
                // Draw the indicator circle
                g2.setPaint(Color.RED);
                g2.setStroke(new BasicStroke(2.0f));
                g2.drawOval((int) (area.getCenterX() - area.getWidth() / 2),
                            (int) (area.getCenterY()
                            - (int) area.getHeight() / 2),
                            (int) area.getWidth(),
                            (int) area.getHeight());
            }
        };

        int index = 0;
        int idIndex = 0;
        double yCoord;

        // Check if this is the step plot and if more than one subplot is
        // displayed
        if (plotType == STEP_PLOT && plotSeries.size() != 1)
        {
            // Set the index to the first ID plot (skip the CPU idle plot, if
            // present)
            index = 1;
        }

        // Create a series for each performance ID
        for (PerformanceID pid : perfIDs.getPerfIDList())
        {
            // Check if plotting flag is enabled for this ID
            if (pid.isPlotPerfID())
            {
                // Check if overruns are enabled to be shown
                if (plotPrefs.isShowOverruns(false))
                {
                    // Collect data for this ID from the log
                    for (PerfLog log : perfLog.getPerfLogData())
                    {
                        // Check if logged ID matches the series ID and if an
                        // overrun condition exists
                        if (log.getPerfID() == pid.getPerfID()
                            && log.getOverrun() != 0.0)
                        {
                            // Determine the y-coordinate of the indicator
                            yCoord = plotType == STEP_PLOT
                                                          ? (log.isEntryEvent()
                                                                               ? 1.0
                                                                               : 0.0)
                                                          : (log.isEntryEvent()
                                                                               ? idIndex + 0.25
                                                                               : idIndex - 0.25);

                            // Create the annotation for the overrun
                            AbstractXYAnnotation overrun = new XYDrawableAnnotation(log.getTimeStamp(),
                                                                                    yCoord,
                                                                                    OVERRUN_INDICATOR_SIZE,
                                                                                    OVERRUN_INDICATOR_SIZE,
                                                                                    indicator);

                            // Highlight the overrun
                            getPlots().get(index).getRenderer().addAnnotation(overrun,
                                                                              Layer.FOREGROUND);
                        }
                    }
                }
                // Overruns are to be hidden
                else
                {
                    // Remove the overruns
                    getPlots().get(index).getRenderer().removeAnnotations();
                }
                // Check if this is the step plot
                if (plotType == STEP_PLOT)
                {
                    index++;
                }

                idIndex++;
            }
        }
    }
}
