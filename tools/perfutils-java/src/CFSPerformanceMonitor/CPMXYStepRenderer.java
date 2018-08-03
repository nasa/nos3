/**
 * CFS Performance Monitor Modified JFreeChart XYStepRenderer class - allows
 * shading of the area below a step line, speeding up of the plot rendering,
 * and hiding the horizontal lines between exit and enter events
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.EXIT;

import java.awt.Graphics2D;
import java.awt.Paint;
import java.awt.Polygon;
import java.awt.Stroke;
import java.awt.geom.Line2D;
import java.awt.geom.Rectangle2D;

import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.entity.EntityCollection;
import org.jfree.chart.plot.CrosshairState;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.PlotRenderingInfo;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRendererState;
import org.jfree.chart.renderer.xy.XYStepRenderer;
import org.jfree.data.xy.XYDataset;
import org.jfree.ui.RectangleEdge;

/******************************************************************************
 * CFS Performance Monitor modified JFreeChart XYStepRenderer class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMXYStepRenderer extends XYStepRenderer
{
    private final CPMLogHandler perfLog;
    private final CPMPreferencesHandler plotPrefs;

    // Flag that indicates a data gap exists between two data points
    private boolean gapFlag;

    // Start and end of the data gap
    private double x0gap;
    private double x1gap;

    // Storage for the second x-coordinate from the previous set of coordinates
    private int prevX1;

    /**************************************************************************
     * Modified JFreeChart XYStepRenderer constructor
     *************************************************************************/
    public CPMXYStepRenderer(CPMLogHandler perfLog,
                             CPMPreferencesHandler plotPrefs)
    {
        super();

        this.perfLog = perfLog;
        this.plotPrefs = plotPrefs;
    }

    // ////////////////////////////////////////////////////////////////////////
    // Begin modification to JFreeChart stock code

    /**************************************************************************
     * Shade the area under a step (enter to exit event sequence)
     * 
     * @param g2
     *            the graphics target
     * 
     * @param plot
     *            the plot (can be used to obtain standard color information
     *            etc.)
     * 
     * @param series
     *            the series index (zero-based)
     * @param x0
     * 
     *            the x-coordinate for the starting point of the line
     * 
     * @param y0
     *            the y-coordinate for the starting point of the line
     * 
     * @param x1
     *            the x-coordinate for the ending point of the line
     * 
     * @param y1
     *            the y-coordinate for the ending point of the line
     *************************************************************************/
    private void shadeUnderStep(Graphics2D g2,
                                XYPlot plot,
                                int series,
                                double x0,
                                double y0,
                                double x1,
                                double y1)
    {
        Polygon pArea = new Polygon();

        // Set the color to the outline (fill) color
        g2.setPaint(plot.getRenderer().getSeriesOutlinePaint(series));

        // Check if the two data points encompass a gap in the log data (due
        // to multiple log files); don't fill the gap area
        if (gapFlag)
        {
            // Check if there is any space between the starting point and the
            // start of the gap
            if (x0 < x0gap)
            {
                // Set the four corners of the shaded rectangle starting with
                // the first point and stopping at the beginning of the gap
                pArea.addPoint((int) x0, (int) y0);
                pArea.addPoint((int) x0, (int) y1);
                pArea.addPoint((int) x0gap, (int) y1);
                pArea.addPoint((int) x0gap, (int) y0);

                // Fill the polygon
                g2.fill(pArea);
            }

            // Check is there is any space between the end of the gap and the
            // ending point
            if (x1 > x1gap)
            {
                // Set the four corners of the shaded rectangle starting with
                // the end of the gap and stopping at the second point
                pArea.addPoint((int) x1gap, (int) y0);
                pArea.addPoint((int) x1gap, (int) y1);
                pArea.addPoint((int) x1, (int) y1);
                pArea.addPoint((int) x1, (int) y0);

                // Fill the polygon
                g2.fill(pArea);
            }
        }
        // No data gap; fill the entire area
        else
        {
            // Set the four corners of the shaded rectangle starting at the
            // first point and stopping at the second
            pArea.addPoint((int) x0, (int) y0);
            pArea.addPoint((int) x0, (int) y1);
            pArea.addPoint((int) x1, (int) y1);
            pArea.addPoint((int) x1, (int) y0);

            // Fill the polygon
            g2.fill(pArea);
        }

        // Restore the color to the line color
        g2.setPaint(plot.getRenderer().getSeriesPaint(0));
    }

    // End modification to stock code
    // ////////////////////////////////////////////////////////////////////////

    /**
     * A utility method that draws a line but only if none of the coordinates
     * are NaN values.
     * 
     * @param g2
     *            the graphics target.
     * @param line
     *            the line object.
     * @param x0
     *            the x-coordinate for the starting point of the line.
     * @param y0
     *            the y-coordinate for the starting point of the line.
     * @param x1
     *            the x-coordinate for the ending point of the line.
     * @param y1
     *            the y-coordinate for the ending point of the line.
     */
    private void drawLine(Graphics2D g2, Line2D line, double x0, double y0,
                          double x1, double y1)
    {
        if (Double.isNaN(x0) || Double.isNaN(x1) || Double.isNaN(y0)
            || Double.isNaN(y1))
        {
            return;
        }
        line.setLine(x0, y0, x1, y1);
        g2.draw(line);
    }

    /**
     * Draws the visual representation of a single data item.
     * 
     * @param g2
     *            the graphics device.
     * @param state
     *            the renderer state.
     * @param dataArea
     *            the area within which the data is being drawn.
     * @param info
     *            collects information about the drawing.
     * @param plot
     *            the plot (can be used to obtain standard color information
     *            etc).
     * @param domainAxis
     *            the domain axis.
     * @param rangeAxis
     *            the vertical axis.
     * @param dataset
     *            the dataset.
     * @param series
     *            the series index (zero-based).
     * @param item
     *            the item index (zero-based).
     * @param crosshairState
     *            crosshair information for the plot (<code>null</code>
     *            permitted).
     * @param pass
     *            the pass index.
     */
    @Override
    public void drawItem(Graphics2D g2, XYItemRendererState state,
                         Rectangle2D dataArea, PlotRenderingInfo info, XYPlot plot,
                         ValueAxis domainAxis, ValueAxis rangeAxis, XYDataset dataset,
                         int series, int item, CrosshairState crosshairState, int pass)
    {

        // do nothing if item is not visible
        if (!getItemVisible(series, item))
        {
            return;
        }

        PlotOrientation orientation = plot.getOrientation();

        Paint seriesPaint = getItemPaint(series, item);
        Stroke seriesStroke = getItemStroke(series, item);
        g2.setPaint(seriesPaint);
        g2.setStroke(seriesStroke);

        // get the data point...
        double x1 = dataset.getXValue(series, item);
        double y1 = dataset.getYValue(series, item);

        RectangleEdge xAxisLocation = plot.getDomainAxisEdge();
        RectangleEdge yAxisLocation = plot.getRangeAxisEdge();
        double transX1 = domainAxis.valueToJava2D(x1, dataArea, xAxisLocation);
        double transY1 = (Double.isNaN(y1) ? Double.NaN
                                          : rangeAxis.valueToJava2D(y1, dataArea, yAxisLocation));

        // ////////////////////////////////////////////////////////////////////
        // Begin modification to JFreeChart stock code

        // Check if this is the first point to be plotted
        if (item == 0)
        {
            // Reset the x-coordinate comparison value
            prevX1 = -1;
        }
        // End modification to stock code
        // ////////////////////////////////////////////////////////////////////

        if (pass == 0 && item > 0)
        {
            // get the previous data point...
            double x0 = dataset.getXValue(series, item - 1);
            double y0 = dataset.getYValue(series, item - 1);
            double transX0 = domainAxis.valueToJava2D(x0, dataArea,
                                                      xAxisLocation);
            double transY0 = (Double.isNaN(y0) ? Double.NaN
                                              : rangeAxis.valueToJava2D(y0, dataArea, yAxisLocation));

            // ////////////////////////////////////////////////////////////////
            // Begin modification to JFreeChart stock code

            // Check that the points aren't redrawing the same vertical line
            // on the screen as the previous points. Skipping these saves time
            // when redrawing
            if ((int) transX1 != prevX1)
            {
                /**
                 * Note: The code below allows plotting of the step lines
                 * either horizontally or vertically. CPM only plots
                 * horizontally so this code is commented out
                 */

                // if (orientation == PlotOrientation.HORIZONTAL)
                // {
                // if (transY0 == transY1)
                // {
                // // this represents the situation
                // // for drawing a horizontal bar.
                // drawLine(g2, state.workingLine, transY0, transX0, transY1,
                // transX1);
                // }
                // else
                // { // this handles the need to perform a 'step'.
                //
                // // calculate the step point
                // double transXs = transX0 + (getStepPoint()
                // * (transX1 - transX0));
                // drawLine(g2, state.workingLine, transY0, transX0, transY0,
                // transXs);
                // drawLine(g2, state.workingLine, transY0, transXs, transY1,
                // transXs);
                // drawLine(g2, state.workingLine, transY1, transXs, transY1,
                // transX1);
                // }
                // }
                // else if (orientation == PlotOrientation.VERTICAL)
                // {

                // Initialize the flag to assume no gap between the two points
                gapFlag = false;

                // Step through the array of gaps
                for (int index = 0; index < perfLog.getDataGapIndex().size(); index++)
                {
                    // Check if the two points encompass this gap
                    if (x0 <= perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(index) - 1).getTimeStamp()
                        && x1 >= perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(index)).getTimeStamp())
                    {
                        // Store the x-coordinates of the beginning and ending
                        // of the gap; the values are adjusted by 1 to prevent
                        // shading over a plot line
                        x0gap = domainAxis.valueToJava2D(perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(index) - 1).getTimeStamp(),
                                                         dataArea,
                                                         xAxisLocation) - 1;
                        x1gap = domainAxis.valueToJava2D(perfLog.getPerfLogData().get(perfLog.getDataGapIndex().get(index)).getTimeStamp(),
                                                         dataArea,
                                                         xAxisLocation) + 1;

                        // Set the flag to indicate that a gap exists
                        gapFlag = true;
                    }
                }
                // End modification to stock code
                // ////////////////////////////////////////////////////////

                if (transY0 == transY1)
                {
                    // ////////////////////////////////////////////////////
                    // Begin modification to JFreeChart stock code

                    // Check if this horizontal line is not at the exit
                    // event level (i.e., between an exit and enter event).
                    // If it is, check if showing these lines is enabled
                    if (y0 != EXIT || plotPrefs.isStepPlotShowExit(false))
                    {
                        // End modification to stock code
                        // ////////////////////////////////////////////////

                        // this represents the situation
                        // for drawing a horizontal bar.
                        drawLine(g2, state.workingLine, transX0, transY0, transX1,
                                 transY1);

                        // ////////////////////////////////////////////////////
                        // Begin modification to JFreeChart stock code
                    }

                    // Check if the user has shading enabled and that both of
                    // the y-values are entry events (= 1). This accounts
                    // for two entry events occurring without an
                    // intervening exit event, and also if the last y-value
                    // is an exit event
                    if (y0 != EXIT && plotPrefs.isStepPlotShowShading(false))
                    {
                        double transYx = rangeAxis.valueToJava2D(EXIT,
                                                                 dataArea,
                                                                 yAxisLocation);

                        // Shade within the four corners of the rectangle
                        shadeUnderStep(g2, plot, series,
                                       transX0 + 1, transYx + 1,
                                       transX1 + 1, transY1 + 1);
                    }
                    // End modification to stock code
                    // ////////////////////////////////////////////////////////
                }
                else
                { // this handles the need to perform a 'step'.
                  // calculate the step point
                    double transXs = transX0 + (getStepPoint()
                                     * (transX1 - transX0));

                    // ////////////////////////////////////////////////////////
                    // Begin modification to JFreeChart stock code

                    // Check if user has shading enabled
                    if (plotPrefs.isStepPlotShowShading(false))
                    {
                        // Only shade when stepping "down"
                        if ((int) transY0 < (int) transY1)
                        {
                            // Shade within the four corners of the
                            // rectangle
                            shadeUnderStep(g2, plot, series,
                                           transX0 + 1, transY0,
                                           transXs, transY1 + 1);
                        }

                    }

                    // Check if this horizontal line is not at the exit
                    // event level (i.e., between an exit and enter event).
                    // If it is, check if showing these lines is enabled
                    if (y0 != EXIT || plotPrefs.isStepPlotShowExit(false))
                    {
                        // End modification to stock code
                        // ////////////////////////////////////////////////////

                        drawLine(g2, state.workingLine, transX0, transY0, transXs,
                                 transY0);

                        // ////////////////////////////////////////////////////
                        // Begin modification to JFreeChart stock code
                    }
                    // End modification to stock code
                    // ////////////////////////////////////////////////////////

                    drawLine(g2, state.workingLine, transXs, transY0, transXs,
                             transY1);

                    // ////////////////////////////////////////////////////////
                    // Begin modification to JFreeChart stock code

                    // Check if this horizontal line is not at the exit
                    // event level (i.e., between an exit and enter event).
                    // If it is, check if showing these lines is enabled
                    if (y1 != EXIT || plotPrefs.isStepPlotShowExit(false))
                    {
                        // End modification to stock code
                        // ////////////////////////////////////////////////////

                        drawLine(g2, state.workingLine, transXs, transY1, transX1,
                                 transY1);

                        // ////////////////////////////////////////////////////
                        // Begin modification to JFreeChart stock code
                    }
                    // End modification to stock code
                    // ////////////////////////////////////////////////////////
                }

                // ////////////////////////////////////////////////////////////
                // Begin modification to JFreeChart stock code

                // } // Note: Closing brace for code commented out above
            }

            // Store the second x-coordinate of this point pair for comparison
            // on the next pass
            prevX1 = (int) transX1;

            // End modification to stock code
            // ////////////////////////////////////////////////////////////////

            // submit this data item as a candidate for the crosshair point
            int domainAxisIndex = plot.getDomainAxisIndex(domainAxis);
            int rangeAxisIndex = plot.getRangeAxisIndex(rangeAxis);
            updateCrosshairValues(crosshairState, x1, y1, domainAxisIndex,
                                  rangeAxisIndex, transX1, transY1, orientation);

            // collect entity and tool tip information...
            EntityCollection entities = state.getEntityCollection();

            if (entities != null)
            {
                addEntity(entities, null, dataset, series, item, transX1,
                          transY1);
            }
        }

        if (pass == 1)
        {
            // draw the item label if there is one...
            if (isItemLabelVisible(series, item))
            {
                double xx = transX1;
                double yy = transY1;
                if (orientation == PlotOrientation.HORIZONTAL)
                {
                    xx = transY1;
                    yy = transX1;
                }
                drawItemLabel(g2, orientation, dataset, series, item, xx, yy,
                              (y1 < 0.0));
            }
        }
    }
}
