/**
 * CFS Performance Monitor User-adjustable plot preferences
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.BY_NAME;
import static CFSPerformanceMonitor.CPMConstants.EXIT_MASK;
import static CFSPerformanceMonitor.CPMConstants.ID_MASK;
import static CFSPerformanceMonitor.CPMConstants.PIE;

import java.awt.Color;

/******************************************************************************
 * CFS Performance Monitor plot preferences handler class
 *****************************************************************************/
public class CPMPreferencesHandler
{
    // Pixel height of an individual step sub-plot
    private int stepPlotHeight;

    // Pixel spacing between step sub-plots
    private int stepPlotSpacing;

    // Show or hide the step sub-plot y-axis tick labels
    private boolean stepPlotShowLabels;

    // Shade or don't shade the area between the entry and exit events on the
    // step sub-plots
    private boolean stepPlotShowShading;

    // Draw or don't draw the horizontal lines between exit and enter events
    private boolean stepPlotShowExit;

    // Show/hide the thread of execution plot horizontal grid lines
    private boolean showPlotHorizontalGrid;

    // Show/hide the step and thread of execution plot vertical grid lines
    private boolean showPlotVerticalGrid;

    // Show/hide the vertical lines marking the minor frames
    private boolean showPlotMinorFrames;

    // Show/hide the vertical lines marking the major frames
    private boolean showPlotMajorFrames;

    // ID value for the minor frame performance ID
    private long minorID;

    // ID value for the major frame performance ID
    private long majorID;

    // Statistics data display format (pie or bar chart)
    private int statChartType;

    // Show/hide the CPU idle time in the statistics plot
    private boolean showCPUIdle;

    // Sort order of the data in the statistics plot (low to high); by ID/name
    // or value
    private int sortOrder;

    // Show/hide the gaps in the log data caused when multiple log files have
    // been read
    private boolean showDataGaps;

    // Show/hide frame overrun indicators on the step and thread of execution
    // plots
    private boolean showOverruns;

    // Plot background color
    private Color backgroundColor;

    // Plot major grid line color
    private Color majorGridColor;

    // Plot minor grid line color
    private Color minorGridColor;

    // Plot minor frame line color
    private Color minorFrameColor;

    // Plot major frame line color
    private Color majorFrameColor;

    /**************************************************************************
     * Plot preferences class constructor
     *************************************************************************/
    protected CPMPreferencesHandler()
    {
        // Set the preferences to the default values
        stepPlotHeight = getStepPlotHeight(true);
        stepPlotSpacing = getStepPlotSpacing(true);
        stepPlotShowLabels = isStepPlotShowLabels(true);
        stepPlotShowShading = isStepPlotShowShading(true);
        stepPlotShowExit = isStepPlotShowExit(true);
        showPlotHorizontalGrid = isShowPlotHorizontalGrid(true);
        showPlotVerticalGrid = isShowPlotVerticalGrid(true);
        showPlotMinorFrames = isShowPlotMinorFrames(true);
        showPlotMajorFrames = isShowPlotMajorFrames(true);
        minorID = getMinorID(true);
        majorID = getMajorID(true);
        statChartType = getStatChartType(true);
        showCPUIdle = isShowCPUIdle(true);
        sortOrder = getSortOrder(true);
        showDataGaps = isShowDataGaps(true);
        showOverruns = isShowOverruns(true);
        backgroundColor = getBackgroundColor(true);
        majorGridColor = getMajorGridColor(true);
        minorGridColor = getMinorGridColor(true);
        minorFrameColor = getMinorFrameColor(true);
        majorFrameColor = getMajorFrameColor(true);
    }

    /**************************************************************************
     * Get the step plot height
     * 
     * @param useDefault
     *            true to return the default step plot height; false to return
     *            the current step plot height
     * 
     * @return Step plot height, pixels
     *************************************************************************/
    protected int getStepPlotHeight(boolean useDefault)
    {
        int height;

        if (useDefault)
        {
            height = 40;
        }
        else
        {
            height = stepPlotHeight;
        }

        return height;
    }

    /**************************************************************************
     * Set the step plot height
     * 
     * @param step
     *            plot height, pixels
     *************************************************************************/
    protected void setStepPlotHeight(int height)
    {
        stepPlotHeight = height;
    }

    /**************************************************************************
     * Get the step plot spacing
     * 
     * @param useDefault
     *            true to return the default step plot spacing; false to return
     *            the current step plot spacing
     * 
     * @return Step plot spacing, pixels
     *************************************************************************/
    protected int getStepPlotSpacing(boolean useDefault)
    {
        int spacing;

        if (useDefault)
        {
            spacing = 5;
        }
        else
        {
            spacing = stepPlotSpacing;
        }

        return spacing;
    }

    /**************************************************************************
     * Set the step plot spacing
     * 
     * @param spacing
     *            plot spacing, pixels
     *************************************************************************/
    protected void setStepPlotSpacing(int spacing)
    {

        stepPlotSpacing = spacing;
    }

    /**************************************************************************
     * Return the status of the show step labels flag
     * 
     * @param useDefault
     *            true to return the default show step labels flag; false to
     *            return the current show step labels flag
     * 
     * @return true if the step labels are displayed
     *************************************************************************/
    protected boolean isStepPlotShowLabels(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = true;
        }
        else
        {
            show = stepPlotShowLabels;
        }

        return show;
    }

    /**************************************************************************
     * Set the step show labels flag
     * 
     * @param show
     *            true to display the step labels
     *************************************************************************/
    protected void setStepPlotShowLabels(boolean show)
    {
        stepPlotShowLabels = show;
    }

    /**************************************************************************
     * Return the status of the show step shading flag
     * 
     * @param useDefault
     *            true to return the default show step shading flag; false to
     *            return the current show step shading flag
     * 
     * @return true if the step labels are displayed
     *************************************************************************/
    protected boolean isStepPlotShowShading(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = true;
        }
        else
        {
            show = stepPlotShowShading;
        }

        return show;
    }

    /**************************************************************************
     * Set the step show shading flag
     * 
     * @param show
     *            true to shade beneath the steps
     *************************************************************************/
    protected void setStepPlotShowShading(boolean show)
    {
        stepPlotShowShading = show;
    }

    /**************************************************************************
     * Return the status of the show step exit line flag
     * 
     * @param useDefault
     *            true to return the default show step exit line flag; false to
     *            return the current show step exit line flag
     * 
     * @return true if the step lines between exit and enter events are
     *         displayed
     *************************************************************************/
    protected boolean isStepPlotShowExit(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = true;
        }
        else
        {
            show = stepPlotShowExit;
        }

        return show;
    }

    /**************************************************************************
     * Set the step show exit line flag
     * 
     * @param show
     *            true to draw the lines between exit and enter events
     *************************************************************************/
    protected void setStepPlotShowExit(boolean show)
    {
        stepPlotShowExit = show;
    }

    /**************************************************************************
     * Return the status of the show plot horizontal grid flag
     * 
     * @param useDefault
     *            true to return the default show plot horizontal grid flag;
     *            false to return the current show plot horizontal grid flag
     * 
     * @return true if the plot horizontal grid lines are displayed
     *************************************************************************/
    protected boolean isShowPlotHorizontalGrid(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = true;
        }
        else
        {
            show = showPlotHorizontalGrid;
        }

        return show;
    }

    /**************************************************************************
     * Set the show plot horizontal grid flag
     * 
     * @param show
     *            true to display the plot horizontal grid lines
     *************************************************************************/
    protected void setShowPlotHorizontalGrid(boolean show)
    {
        showPlotHorizontalGrid = show;
    }

    /**************************************************************************
     * Return the status of the show plot vertical grid flag
     * 
     * @param useDefault
     *            true to return the default show plot vertical grid flag;
     *            false to return the current show plot vertical grid flag
     * 
     * @return true if the plot vertical grid lines are displayed
     *************************************************************************/
    protected boolean isShowPlotVerticalGrid(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = true;
        }
        else
        {
            show = showPlotVerticalGrid;
        }

        return show;
    }

    /**************************************************************************
     * Set the show plot vertical grid flag
     * 
     * @param show
     *            true to display the plot vertical grid lines
     *************************************************************************/
    protected void setShowPlotVerticalGrid(boolean show)
    {
        showPlotVerticalGrid = show;
    }

    /**************************************************************************
     * Return the status of the show minor frame lines flag
     * 
     * @param useDefault
     *            true to return the default show minor frame lines flag; false
     *            to return the current show minor frame lines flag
     * 
     * @return true if the minor frame lines are displayed
     *************************************************************************/
    protected boolean isShowPlotMinorFrames(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = false;
        }
        else
        {
            show = showPlotMinorFrames;
        }

        return show;
    }

    /**************************************************************************
     * Set the show minor frame lines flag
     * 
     * @param show
     *            true to display the minor frame lines
     *************************************************************************/
    protected void setShowPlotMinorFrames(boolean show)
    {
        showPlotMinorFrames = show;
    }

    /**************************************************************************
     * Return the status of the show major frame lines flag
     * 
     * @param useDefault
     *            true to return the default show major frame lines flag; false
     *            to return the current show major frame lines flag
     * 
     * @return true if the major frame lines are displayed
     *************************************************************************/
    protected boolean isShowPlotMajorFrames(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = false;
        }
        else
        {
            show = showPlotMajorFrames;
        }

        return show;
    }

    /**************************************************************************
     * Set the show major frame lines flag
     * 
     * @param show
     *            true to display the major frame lines
     *************************************************************************/
    protected void setShowPlotMajorFrames(boolean show)
    {
        showPlotMajorFrames = show;
    }

    /**************************************************************************
     * Get the minor ID
     * 
     * @param useDefault
     *            true to return the default minor frame ID; false to return
     *            the current minor frame ID
     * 
     * @return Minor frame indicator performance ID
     *************************************************************************/
    protected long getMinorID(boolean useDefault)
    {
        long id;

        if (useDefault)
        {
            id = ID_MASK | EXIT_MASK;
        }
        else
        {
            id = minorID;
        }

        return id;
    }

    /**************************************************************************
     * Set the minor ID
     * 
     * @param minor
     *            frame indicator performance ID
     *************************************************************************/
    protected void setMinorID(long minor)
    {
        minorID = minor;
    }

    /**************************************************************************
     * Get the major ID
     * 
     * @param useDefault
     *            true to return the default major frame ID; false to return
     *            the current major frame ID
     * 
     * @return Major frame indicator performance ID
     *************************************************************************/
    protected long getMajorID(boolean useDefault)
    {
        long id;

        if (useDefault)
        {
            id = ID_MASK | EXIT_MASK;
        }
        else
        {
            id = majorID;
        }

        return id;
    }

    /**************************************************************************
     * Set the major ID
     * 
     * @param major
     *            frame indicator performance ID
     *************************************************************************/
    protected void setMajorID(long major)
    {
        majorID = major;
    }

    /**************************************************************************
     * Get the statistics chart type
     * 
     * @param useDefault
     *            true to return the default statistics chart type; false to
     *            return the current statistics chart type
     * 
     * @return Statistics chart type: PIE or BAR
     *************************************************************************/
    protected int getStatChartType(boolean useDefault)
    {
        int type;

        if (useDefault)
        {
            type = PIE;
        }
        else
        {
            type = statChartType;
        }

        return type;
    }

    /**************************************************************************
     * Set the statistics chart type
     * 
     * @param order
     *            statistics chart type: PIE or BAR
     *************************************************************************/
    protected void setStatChartType(int type)
    {
        statChartType = type;
    }

    /**************************************************************************
     * Return the status of the show CPU idle flag
     * 
     * @param useDefault
     *            true to return the default show CPU idle flag; false to
     *            return the current show CPU idle flag
     * 
     * @return true if the CPU idle time is displayed
     *************************************************************************/
    protected boolean isShowCPUIdle(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = true;
        }
        else
        {
            show = showCPUIdle;
        }

        return show;
    }

    /**************************************************************************
     * Set the show CPU idle flag
     * 
     * @param show
     *            true to display the CPU idle time
     *************************************************************************/
    protected void setShowCPUIdle(boolean show)
    {
        showCPUIdle = show;
    }

    /**************************************************************************
     * Get the statistics plot data sort order
     * 
     * @param useDefault
     *            true to return the default statistics plot data sort order;
     *            false to return the current statistics plot data sort order
     * 
     * @return Statistics plot data sort order: BY_NAME or BY_VALUE
     *************************************************************************/
    protected int getSortOrder(boolean useDefault)
    {
        int order;

        if (useDefault)
        {
            order = BY_NAME;
        }
        else
        {
            order = sortOrder;
        }

        return order;
    }

    /**************************************************************************
     * Set the data sort order
     * 
     * @param order
     *            data sort order: BY_NAME or BY_VALUE
     *************************************************************************/
    protected void setSortOrder(int order)
    {
        sortOrder = order;
    }

    /**************************************************************************
     * Return the status of the show data gaps flag
     * 
     * @param useDefault
     *            true to return the default show data gaps flag; false to
     *            return the current show data gaps flag
     * 
     * @return true to display the log data gaps
     *************************************************************************/
    protected boolean isShowDataGaps(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = true;
        }
        else
        {
            show = showDataGaps;
        }

        return show;
    }

    /**************************************************************************
     * Set the show data gaps flag
     * 
     * @param show
     *            true if the log data gaps are displayed
     *************************************************************************/
    protected void setShowDataGaps(boolean show)
    {
        showDataGaps = show;
    }

    /**************************************************************************
     * Return the status of the show overruns flag
     * 
     * @param useDefault
     *            true to return the default show overruns flag; false to
     *            return the current show overruns flag
     * 
     * @return true to display frame overruns
     *************************************************************************/
    protected boolean isShowOverruns(boolean useDefault)
    {
        boolean show;

        if (useDefault)
        {
            show = true;
        }
        else
        {
            show = showOverruns;
        }

        return show;
    }

    /**************************************************************************
     * Set the show overruns flag
     * 
     * @param show
     *            true to display frame overruns
     *************************************************************************/
    protected void setShowOverruns(boolean show)
    {
        showOverruns = show;
    }

    /**************************************************************************
     * Return the plot background color
     * 
     * @param useDefault
     *            true to return the default plot background color; false to
     *            return the current plot background color
     * 
     * @return Plot background color
     *************************************************************************/
    protected Color getBackgroundColor(boolean useDefault)
    {
        Color color;

        if (useDefault)
        {
            color = Color.BLACK;
        }
        else
        {
            color = backgroundColor;
        }

        return color;
    }

    /**************************************************************************
     * Set the plot background color
     * 
     * @param color
     *            plot background color
     *************************************************************************/
    protected void setBackgroundColor(Color color)
    {
        backgroundColor = color;
    }

    /**************************************************************************
     * Return the plot major grid line color
     * 
     * @param useDefault
     *            true to return the default plot major grid line color; false
     *            to return the current plot major grid line color
     * 
     * @return Plot major grid line color
     *************************************************************************/
    protected Color getMajorGridColor(boolean useDefault)
    {
        Color color;

        if (useDefault)
        {
            color = Color.LIGHT_GRAY;
        }
        else
        {
            color = majorGridColor;
        }

        return color;
    }

    /**************************************************************************
     * Set the plot major grid line color
     * 
     * @param color
     *            plot major grid line color
     *************************************************************************/
    protected void setMajorGridColor(Color color)
    {
        majorGridColor = color;
    }

    /**************************************************************************
     * Return the plot minor grid line color
     * 
     * @param useDefault
     *            true to return the default plot minor grid line color; false
     *            to return the current plot minor grid line color
     * 
     * @return Plot minor grid line color
     *************************************************************************/
    protected Color getMinorGridColor(boolean useDefault)
    {
        Color color;

        if (useDefault)
        {
            color = Color.GRAY;
        }
        else
        {
            color = minorGridColor;
        }

        return color;
    }

    /**************************************************************************
     * Set the plot minor grid line color
     * 
     * @param color
     *            plot minor grid line color
     *************************************************************************/
    protected void setMinorGridColor(Color color)
    {
        minorGridColor = color;
    }

    /**************************************************************************
     * Return the minor frame line color
     * 
     * @param useDefault
     *            true to return the default minor frame line color; false to
     *            return the current minor frame line color
     * 
     * @return Minor frame line color
     *************************************************************************/
    protected Color getMinorFrameColor(boolean useDefault)
    {
        Color color;

        if (useDefault)
        {
            color = Color.GRAY;
        }
        else
        {
            color = minorFrameColor;
        }

        return color;
    }

    /**************************************************************************
     * Set the minor frame line color
     * 
     * @param color
     *            minor frame line color
     *************************************************************************/
    protected void setMinorFrameColor(Color color)
    {
        minorFrameColor = color;
    }

    /**************************************************************************
     * Return the major frame line color
     * 
     * @param useDefault
     *            true to return the default major frame line color; false to
     *            return the current major frame line color
     * 
     * @return Major frame line color
     *************************************************************************/
    protected Color getMajorFrameColor(boolean useDefault)
    {
        Color color;

        if (useDefault)
        {
            color = Color.WHITE;
        }
        else
        {
            color = majorFrameColor;
        }

        return color;
    }

    /**************************************************************************
     * Set the major frame line color
     * 
     * @param color
     *            major frame line color
     *************************************************************************/
    protected void setMajorFrameColor(Color color)
    {
        majorFrameColor = color;
    }

    /**************************************************************************
     * Create a string representing the current user-selected application
     * preferences (used when saving a configuration to the program preferences
     * backing store)
     * 
     * @return User-selected preferences in string format
     *************************************************************************/
    protected String getPreferences()
    {
        // Create a string representing the user-selectable plot preferences
        return String.valueOf(stepPlotHeight)
               + ","
               + String.valueOf(stepPlotSpacing)
               + ","
               + String.valueOf(stepPlotShowLabels)
               + ","
               + String.valueOf(stepPlotShowShading)
               + ","
               + String.valueOf(showPlotHorizontalGrid)
               + ","
               + String.valueOf(showPlotVerticalGrid)
               + ","
               + String.valueOf(showPlotMinorFrames)
               + ","
               + String.valueOf(showPlotMajorFrames)
               + ","
               + String.format("0x%08x", minorID)
               + ","
               + String.format("0x%08x", majorID)
               + ","
               + String.valueOf(showCPUIdle)
               + ","
               + String.valueOf(sortOrder)
               + ","
               + String.valueOf(showDataGaps)
               + ","
               + String.format("0x%06x", backgroundColor.getRGB() & 0xffffff)
               + ","
               + String.format("0x%06x", majorGridColor.getRGB() & 0xffffff)
               + ","
               + String.format("0x%06x", minorGridColor.getRGB() & 0xffffff)
               + ","
               + String.format("0x%06x", minorFrameColor.getRGB() & 0xffffff)
               + ","
               + String.format("0x%06x", majorFrameColor.getRGB() & 0xffffff)
               + ","
               + String.valueOf(showOverruns)
               + ","
               + String.valueOf(statChartType)
               + ","
               + String.valueOf(stepPlotShowExit);
    }

    /**************************************************************************
     * Parse the configuration entry from the program preferences backing store
     * to obtain the user- selected plot preferences
     * 
     * @param parts
     *            User-selected preferences in string array format
     *************************************************************************/
    protected void setPreferences(String[] parts)
    {
        // Set the user-selectable plot preferences. The use of the switch
        // statement allows additions to the configurable parameters while
        // maintaining backwards compatibility to previously saved
        // configurations that would lack the new parameters
        switch (parts.length)
        {
            case 21:
                stepPlotShowExit = Boolean.valueOf(parts[20]);

            case 20:
                statChartType = Integer.valueOf(parts[19]);

            case 19:
                showOverruns = Boolean.valueOf(parts[18]);

            case 18:
                majorFrameColor = Color.decode(parts[17]);

            case 17:
                minorFrameColor = Color.decode(parts[16]);

            case 16:
                minorGridColor = Color.decode(parts[15]);

            case 15:
                majorGridColor = Color.decode(parts[14]);

            case 14:
                backgroundColor = Color.decode(parts[13]);

            case 13:
                showDataGaps = Boolean.valueOf(parts[12]);

            case 12:
                sortOrder = Integer.valueOf(parts[11]);

            case 11:
                showCPUIdle = Boolean.valueOf(parts[10]);

            case 10:
                majorID = Long.parseLong(parts[9].substring(2), 16);

            case 9:
                minorID = Long.parseLong(parts[8].substring(2), 16);

            case 8:
                showPlotMajorFrames = Boolean.valueOf(parts[7]);

            case 7:
                showPlotMinorFrames = Boolean.valueOf(parts[6]);

            case 6:
                showPlotVerticalGrid = Boolean.valueOf(parts[5]);

            case 5:
                showPlotHorizontalGrid = Boolean.valueOf(parts[4]);

            case 4:
                stepPlotShowShading = Boolean.valueOf(parts[3]);

            case 3:
                stepPlotShowLabels = Boolean.valueOf(parts[2]);

            case 2:
                stepPlotSpacing = Integer.valueOf(parts[1]);

            case 1:
                stepPlotHeight = Integer.valueOf(parts[0]);
        }
    }
}
