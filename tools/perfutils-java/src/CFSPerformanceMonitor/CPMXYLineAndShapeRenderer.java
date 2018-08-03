/**
 * CFS Performance Monitor Modified JFreeChart XYLineAndShapeRenderer class -
 * allows speeding up of the plot rendering
 */
package CFSPerformanceMonitor;

import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.Rectangle2D;

import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.entity.EntityCollection;
import org.jfree.chart.plot.CrosshairState;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRendererState;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.chart.util.LineUtilities;
import org.jfree.data.xy.XYDataset;
import org.jfree.ui.RectangleEdge;
import org.jfree.util.ShapeUtilities;

@SuppressWarnings("serial")
public class CPMXYLineAndShapeRenderer extends XYLineAndShapeRenderer
{
    // Storage for the previous plotted coordinates
    private int prevX1;
    private int prevY1;

    public CPMXYLineAndShapeRenderer(boolean lines, boolean shapes)
    {
        super(lines, shapes);
    }

    @Override
    protected void drawPrimaryLine(XYItemRendererState state,
                                   Graphics2D g2,
                                   XYPlot plot,
                                   XYDataset dataset,
                                   int pass,
                                   int series,
                                   int item,
                                   ValueAxis domainAxis,
                                   ValueAxis rangeAxis,
                                   Rectangle2D dataArea)
    {
        if (item == 0)
        {
            return;
        }

        // get the data point...
        double x1 = dataset.getXValue(series, item);
        double y1 = dataset.getYValue(series, item);
        if (Double.isNaN(y1) || Double.isNaN(x1))
        {
            return;
        }

        double x0 = dataset.getXValue(series, item - 1);
        double y0 = dataset.getYValue(series, item - 1);
        if (Double.isNaN(y0) || Double.isNaN(x0))
        {
            return;
        }

        RectangleEdge xAxisLocation = plot.getDomainAxisEdge();
        RectangleEdge yAxisLocation = plot.getRangeAxisEdge();

        double transX0 = domainAxis.valueToJava2D(x0, dataArea, xAxisLocation);
        double transY0 = rangeAxis.valueToJava2D(y0, dataArea, yAxisLocation);

        double transX1 = domainAxis.valueToJava2D(x1, dataArea, xAxisLocation);
        double transY1 = rangeAxis.valueToJava2D(y1, dataArea, yAxisLocation);

        // only draw if we have good values
        if (Double.isNaN(transX0) || Double.isNaN(transY0)
            || Double.isNaN(transX1) || Double.isNaN(transY1))
        {
            return;
        }

        // ////////////////////////////////////////////////////////////////////
        // Begin modification to JFreeChart stock code

        // Check if the points don't coincide; if so, skip this point in order
        // to speed drawing the plot
        if ((int) transX0 != (int) transX1
            || (int) transY0 != (int) transY1)
        {
            // End modification to stock code
            // ////////////////////////////////////////////////////////////////

            PlotOrientation orientation = plot.getOrientation();
            boolean visible;
            if (orientation == PlotOrientation.HORIZONTAL)
            {
                state.workingLine.setLine(transY0, transX0, transY1, transX1);
            }
            else if (orientation == PlotOrientation.VERTICAL)
            {
                state.workingLine.setLine(transX0, transY0, transX1, transY1);
            }
            visible = LineUtilities.clipLine(state.workingLine, dataArea);
            if (visible)
            {
                drawFirstPassShape(g2, pass, series, item, state.workingLine);
            }

            // ////////////////////////////////////////////////////////////////
            // Begin modification to JFreeChart stock code
        }
        // End modification to stock code
        // ////////////////////////////////////////////////////////////////////
    }

    @Override
    protected void drawSecondaryPass(Graphics2D g2, XYPlot plot,
                                     XYDataset dataset, int pass, int series, int item,
                                     ValueAxis domainAxis, Rectangle2D dataArea, ValueAxis rangeAxis,
                                     CrosshairState crosshairState, EntityCollection entities)
    {

        Shape entityArea = null;

        // get the data point...
        double x1 = dataset.getXValue(series, item);
        double y1 = dataset.getYValue(series, item);
        if (Double.isNaN(y1) || Double.isNaN(x1))
        {
            return;
        }

        PlotOrientation orientation = plot.getOrientation();
        RectangleEdge xAxisLocation = plot.getDomainAxisEdge();
        RectangleEdge yAxisLocation = plot.getRangeAxisEdge();
        double transX1 = domainAxis.valueToJava2D(x1, dataArea, xAxisLocation);
        double transY1 = rangeAxis.valueToJava2D(y1, dataArea, yAxisLocation);

        // ////////////////////////////////////////////////////////////////////
        // Begin modification to JFreeChart stock code

        // Check if this is the first point to be plotted
        if (item == 0)
        {
            // Reset the coordinate comparison values
            prevX1 = -1;
            prevY1 = -1;
        }

        // Check if the points don't coincide; if so, skip this point in order
        // to speed drawing the plot
        if ((int) transX1 != prevX1
            || (int) transY1 != prevY1)
        {
            // End modification to stock code
            // ////////////////////////////////////////////////////////////////

            if (getItemShapeVisible(series, item))
            {
                Shape shape = getItemShape(series, item);
                if (orientation == PlotOrientation.HORIZONTAL)
                {
                    shape = ShapeUtilities.createTranslatedShape(shape, transY1,
                                                                 transX1);
                }
                else if (orientation == PlotOrientation.VERTICAL)
                {
                    shape = ShapeUtilities.createTranslatedShape(shape, transX1,
                                                                 transY1);
                }
                entityArea = shape;
                if (shape.intersects(dataArea))
                {
                    if (getItemShapeFilled(series, item))
                    {
                        // ////////////////////////////////////////////////////
                        // Begin modification to JFreeChart stock code
                        if (getUseFillPaint())
                        // End modification to stock code
                        // ////////////////////////////////////////////////////
                        {
                            g2.setPaint(getItemFillPaint(series, item));
                        }
                        else
                        {
                            g2.setPaint(getItemPaint(series, item));
                        }
                        g2.fill(shape);
                    }
                    // ////////////////////////////////////////////////////////
                    // Begin modification to JFreeChart stock code
                    if (getDrawOutlines())
                    // End modification to stock code
                    // ////////////////////////////////////////////////////////
                    {
                        if (getUseOutlinePaint())
                        {
                            g2.setPaint(getItemOutlinePaint(series, item));
                        }
                        else
                        {
                            g2.setPaint(getItemPaint(series, item));
                        }
                        g2.setStroke(getItemOutlineStroke(series, item));
                        g2.draw(shape);
                    }
                }
            }

            // ////////////////////////////////////////////////////////////////
            // Begin modification to JFreeChart stock code
        }

        // Store the last coordinates for comparison on the next pass
        prevX1 = (int) transX1;
        prevY1 = (int) transY1;

        // End modification to stock code
        // ////////////////////////////////////////////////////////////////////

        double xx = transX1;
        double yy = transY1;
        if (orientation == PlotOrientation.HORIZONTAL)
        {
            xx = transY1;
            yy = transX1;
        }

        // draw the item label if there is one...
        if (isItemLabelVisible(series, item))
        {
            drawItemLabel(g2, orientation, dataset, series, item, xx, yy,
                          (y1 < 0.0));
        }

        int domainAxisIndex = plot.getDomainAxisIndex(domainAxis);
        int rangeAxisIndex = plot.getRangeAxisIndex(rangeAxis);
        updateCrosshairValues(crosshairState, x1, y1, domainAxisIndex,
                              rangeAxisIndex, transX1, transY1, orientation);

        // add an entity for the item, but only if it falls within the data
        // area...
        if (entities != null && isPointInRect(dataArea, xx, yy))
        {
            addEntity(entities, entityArea, dataset, series, item, xx, yy);
        }
    }
}
