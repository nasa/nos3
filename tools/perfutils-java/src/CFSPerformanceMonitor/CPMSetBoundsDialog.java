/**
 * CFS Performance Monitor Plot Set Bounds Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LABEL_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LABEL_VERTICAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.OK_CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.TIME_FORMAT;

import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JTextField;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;

import org.jfree.chart.axis.ValueAxis;

/******************************************************************************
 * CFS Performance Monitor set plot boundaries dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMSetBoundsDialog extends CPMDialogHandler
{
    private final CPMMain perfMain;

    // Components that need to be accessed by other classes or listeners
    private JTextField minXTextField;
    private JTextField maxXTextField;
    private JComboBox minYComboBox;
    private JComboBox maxYComboBox;

    // Flag used to inhibit combo box event handling
    private boolean inhibitEvent;

    /**************************************************************************
     * Set Bounds dialog class constructor - step plot
     * 
     * @param stepPlot
     *            step plot
     * 
     * @param perfMain
     *            main class
     *************************************************************************/
    protected CPMSetBoundsDialog(CPMStepPlot stepPlot,
                                 CPMMain perfMain)
    {
        this.perfMain = perfMain;
        this.inhibitEvent = false;

        // Set up temporary variables to shorten the subsequent method calls
        ValueAxis sDomain = stepPlot.getPlotChartPanel().getChart().getXYPlot().getDomainAxis();
        ValueAxis lDomain = stepPlot.getLabelChartPanel().getChart().getXYPlot().getDomainAxis();

        // Allow the user to select new x-axis boundaries. Perform a validity
        // check before implementing the updates
        if (showSetBoundsDialog(sDomain.getLowerBound(),
                                sDomain.getUpperBound(),
                                null,
                                null,
                                null) == OK_OPTION)
        {
            // Update the step plot with the new x-axis boundaries
            sDomain.setLowerBound(Double.valueOf(minXTextField.getText()));
            sDomain.setUpperBound(Double.valueOf(maxXTextField.getText()));
            lDomain.setLowerBound(Double.valueOf(minXTextField.getText()));
            lDomain.setUpperBound(Double.valueOf(maxXTextField.getText()));

            // Update the scroll bar to reflect the change in bounds
            stepPlot.adjustScrollBarIncrements();
        }
    }

    /**************************************************************************
     * Set Bounds dialog class constructor - thread of execution plot
     * 
     * @param threadPlot
     *            thread of execution plot
     * 
     * @param perfMain
     *            main class
     *************************************************************************/
    protected CPMSetBoundsDialog(CPMThreadPlot threadPlot,
                                 CPMMain perfMain)
    {
        this.perfMain = perfMain;

        // Set up temporary variables to shorten the subsequent method calls
        ValueAxis tDomain = threadPlot.getPlotChartPanel().getChart().getXYPlot().getDomainAxis();
        ValueAxis tRange = threadPlot.getPlotChartPanel().getChart().getXYPlot().getRangeAxis();
        ValueAxis lDomain = threadPlot.getLabelChartPanel().getChart().getXYPlot().getDomainAxis();
        List<String> tLabel = threadPlot.getYAxisLabel();

        // Allow the user to select new x- and y-axis boundaries. Perform a
        // validity check before implementing the updates
        if (showSetBoundsDialog(tDomain.getLowerBound(),
                                tDomain.getUpperBound(),
                                tLabel.get((int) Math.ceil(tRange.getLowerBound())),
                                tLabel.get((int) Math.floor(tRange.getUpperBound())),
                                tLabel.toArray(new String[tLabel.size()])) == OK_OPTION)
        {
            // Update the thread plot with the new x-axis boundaries
            tDomain.setLowerBound(Double.valueOf(minXTextField.getText()));
            tDomain.setUpperBound(Double.valueOf(maxXTextField.getText()));
            lDomain.setLowerBound(Double.valueOf(minXTextField.getText()));
            lDomain.setUpperBound(Double.valueOf(maxXTextField.getText()));

            // Update the thread plot's y-axis boundaries. The label indices
            // are biased slightly so that the y-axis will include the minimum
            // ID's exit point and the maximum ID's entry point
            tRange.setLowerBound(Double.valueOf(minYComboBox.getSelectedIndex())
                - 0.5);
            tRange.setUpperBound(Double.valueOf(maxYComboBox.getSelectedIndex())
                                 + minYComboBox.getSelectedIndex()
                                 + 0.5);

            // Update the scroll bars to reflect the change in bounds
            threadPlot.adjustScrollBarIncrements();
        }
    }

    /**************************************************************************
     * Create the Set Bounds dialog
     * 
     * @param minX
     *            minimum x-axis value
     * 
     * @param maxX
     *            maximum x-axis value
     * 
     * @param minY
     *            minimum y-axis ID name (or ID if no name is supplied)
     * 
     * @param maxY
     *            maximum y-axis ID name (or ID if no name is supplied)
     * 
     * @param yAxisLabels
     *            string array containing the y-axis labels (null if the y-axis
     *            bounds are not displayed)
     * 
     * @return Selected button
     *************************************************************************/
    private int showSetBoundsDialog(final double minX,
                                    final double maxX,
                                    final String minY,
                                    final String maxY,
                                    final String[] yAxisLabels)
    {
        // Button selected by the user
        int buttonSelected;

        // Create a border
        Border border = BorderFactory.createBevelBorder(BevelBorder.LOWERED,
                                                        Color.LIGHT_GRAY,
                                                        Color.GRAY);

        // Set the initial layout manager characteristics
        GridBagConstraints gbc = new GridBagConstraints(1,
                                                        0,
                                                        1,
                                                        1,
                                                        0.0,
                                                        0.0,
                                                        GridBagConstraints.LINE_START,
                                                        GridBagConstraints.NONE,
                                                        new Insets(0,
                                                                   LABEL_HORIZONTAL_SPACING,
                                                                   0,
                                                                   LABEL_HORIZONTAL_SPACING),
                                                        0,
                                                        0);

        // Create a panel to hold the components of the dialog
        JPanel setBoundsPanel = new JPanel();
        setBoundsPanel.setLayout(new GridBagLayout());

        // Create the dialog labels and input fields
        JLabel minLabel = new JLabel("Minimum", JTextField.CENTER);
        minLabel.setFont(LABEL_FONT_BOLD);
        minLabel.setFont(LABEL_FONT_BOLD);
        setBoundsPanel.add(minLabel, gbc);

        JLabel maxLabel = new JLabel("Maximum", JTextField.CENTER);
        maxLabel.setFont(LABEL_FONT_BOLD);
        gbc.gridx++;
        setBoundsPanel.add(maxLabel, gbc);

        JLabel xLabel = new JLabel("X");
        xLabel.setFont(LABEL_FONT_BOLD);
        gbc.insets.top = LABEL_VERTICAL_SPACING;

        // Check if y-axis labels are supplied
        if (yAxisLabels == null)
        {
            gbc.insets.bottom = LABEL_VERTICAL_SPACING * 2;
        }

        gbc.gridx = 0;
        gbc.gridy = 1;
        setBoundsPanel.add(xLabel, gbc);

        minXTextField = new JTextField(String.format(" " + TIME_FORMAT, minX),
                                       10);
        minXTextField.setFont(LABEL_FONT_PLAIN);
        minXTextField.setEditable(true);
        minXTextField.setForeground(Color.BLACK);
        minXTextField.setBackground(Color.WHITE);
        minXTextField.setBorder(border);
        gbc.gridx++;
        setBoundsPanel.add(minXTextField, gbc);

        maxXTextField = new JTextField(String.format(" " + TIME_FORMAT, maxX),
                                       10);
        maxXTextField.setFont(LABEL_FONT_PLAIN);
        maxXTextField.setEditable(true);
        maxXTextField.setForeground(Color.BLACK);
        maxXTextField.setBackground(Color.WHITE);
        maxXTextField.setBorder(border);
        gbc.gridx++;

        setBoundsPanel.add(maxXTextField, gbc);

        // Check if y-axis labels are supplied
        if (yAxisLabels != null)
        {
            // Create the y-axis min/max labels and combo boxes
            JLabel yLabel = new JLabel("Y");
            yLabel.setFont(LABEL_FONT_BOLD);
            gbc.insets.bottom = LABEL_VERTICAL_SPACING * 2;
            gbc.gridx = 0;
            gbc.gridy++;
            setBoundsPanel.add(yLabel, gbc);

            minYComboBox = new JComboBox(yAxisLabels);
            minYComboBox.setSelectedItem(minY);
            minYComboBox.setFont(LABEL_FONT_PLAIN);
            minYComboBox.setForeground(Color.BLACK);
            minYComboBox.setBackground(Color.WHITE);
            minYComboBox.setBorder(border);
            gbc.gridx++;
            setBoundsPanel.add(minYComboBox, gbc);

            maxYComboBox = new JComboBox(yAxisLabels);
            maxYComboBox.setSelectedItem(maxY);
            maxYComboBox.setFont(LABEL_FONT_PLAIN);
            maxYComboBox.setForeground(Color.BLACK);
            maxYComboBox.setBackground(Color.WHITE);
            maxYComboBox.setBorder(border);
            gbc.gridx++;
            setBoundsPanel.add(maxYComboBox, gbc);

            // Initialize the combo box' contents
            setMinYLabels(yAxisLabels);
            setMaxYLabels(yAxisLabels);

            // Monitor the minimum y combo box for changes
            minYComboBox.addActionListener(new ActionListener()
            {
                /**************************************************************
                 * Handle a minimum y combo box event. The list of maximum y
                 * IDs is adjusted so that IDs below the minimum do not appear
                 * in the maximum y combo box list
                 *************************************************************/
                @Override
                public void actionPerformed(ActionEvent ae)
                {
                    // Check if events have been inhibited
                    if (!inhibitEvent)
                    {
                        // The setMaxYLabels() call causes multiple events to
                        // be sent to maxYComboBox. These events need not be
                        // acted on, so the inhibit flag is set to force these
                        // events to be ignored
                        inhibitEvent = true;

                        // Adjust the maximum y labels
                        setMaxYLabels(yAxisLabels);

                        // Reenable event handling
                        inhibitEvent = false;
                    }
                }
            });

            // Monitor the maximum y combo box for changes
            maxYComboBox.addActionListener(new ActionListener()
            {
                /**************************************************************
                 * Handle a maximum y combo box event. The list of minimum y
                 * IDs is adjusted so that IDs above the maximum do not appear
                 * in the minimum y combo box list
                 *************************************************************/
                @Override
                public void actionPerformed(ActionEvent ae)
                {
                    // Check if events have been inhibited
                    if (!inhibitEvent)
                    {
                        // The setMinYLabels() call causes multiple events to
                        // be sent to minYComboBox. These events need not be
                        // acted on, so the inhibit flag is set to force these
                        // events to be ignored
                        inhibitEvent = true;

                        // Adjust the minimum y labels
                        setMinYLabels(yAxisLabels);

                        // Reenable event handling
                        inhibitEvent = false;
                    }
                }
            });
        }

        // Add a separator
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridx = 0;
        gbc.gridy++;
        gbc.insets.set(LABEL_VERTICAL_SPACING,
                       LABEL_HORIZONTAL_SPACING,
                       0,
                       LABEL_HORIZONTAL_SPACING);
        setBoundsPanel.add(new JSeparator(JSeparator.HORIZONTAL), gbc);

        // Display the set bounds dialog
        if (showOptionsDialog(perfMain.getMainFrame(),
                              setBoundsPanel,
                              "Set Bounds",
                              OK_CANCEL_OPTION,
                              false) == OK_OPTION)
        {
            // Check if the user changed the x or y minimum or maximum values
            if (minX != Double.valueOf(minXTextField.getText().trim())
                || maxX != Double.valueOf(maxXTextField.getText().trim())
                || (yAxisLabels != null
                && (!minY.equals(minYComboBox.getSelectedItem())
                || !maxY.equals(maxYComboBox.getSelectedItem()))))
            {
                buttonSelected = OK_OPTION;
            }
            // None of the values changed
            else
            {
                // Treat this the same as if Cancel was selected
                buttonSelected = CANCEL_OPTION;
            }
        }
        // Cancel was selected
        else
        {
            buttonSelected = CANCEL_OPTION;
        }

        // Since this dialog is modal it will wait here until the user selects
        // Okay or Cancel. Return which button was selected
        return buttonSelected;
    }

    /**************************************************************************
     * Create the subset of labels for the minimum y combo box
     * 
     * @param labels
     *            array of y-axis ID names (or ID if no name is supplied) up to
     *            an including the current maximum y-axis ID name
     *************************************************************************/
    private void setMinYLabels(String[] labels)
    {
        // Save the current minimum y selection so it can be restored after
        // recreating the list
        int selectedIndex = minYComboBox.getSelectedIndex();

        // Delete the current list of minimum y selections from the combo box
        minYComboBox.removeAllItems();

        // Rebuild the list based on the currently selected maximum y label.
        // The valid minimum y labels are those starting with the minimum
        // possible y label and ending with the maximum y label
        for (int index = 0; index <= labels.length
                                     - (maxYComboBox.getItemCount()
                                     - maxYComboBox.getSelectedIndex()); index++)
        {
            minYComboBox.addItem(labels[index]);
        }

        // Restore the minimum y selection to its original value; otherwise it
        // would be reset to 0
        minYComboBox.setSelectedIndex(selectedIndex);
    }

    /**************************************************************************
     * Create the subset of labels for the maximum y combo box
     * 
     * @param labels
     *            array of y-axis ID names (or ID if no name is supplied)
     *            beginning with the current minimum y-axis ID name
     *************************************************************************/
    private void setMaxYLabels(String[] labels)
    {
        // Save the current maximum y selection so it can be restored after
        // recreating the list
        int selectedIndex = maxYComboBox.getSelectedIndex();
        int itemCount = maxYComboBox.getItemCount();

        // Delete the current list of maximum y selections from the combo box
        maxYComboBox.removeAllItems();

        // Rebuild the list based on the currently selected minimum y label.
        // The valid maximum y labels are those starting with the minimum y and
        // ending with the maximum possible y label
        for (int index = minYComboBox.getSelectedIndex(); index < labels.length; index++)
        {
            maxYComboBox.addItem(labels[index]);
        }

        // Restore the maximum y selection to its originally selected label.
        // Since the number of items in the list changed, the original index
        // isn't the same as the new index, hence the calculation to
        // determine the equivalent index in the new list
        maxYComboBox.setSelectedIndex(maxYComboBox.getItemCount()
                                      - (itemCount - selectedIndex));
    }

    /**************************************************************************
     * Verify that the bounds are valid
     * 
     * @return true if the x-axis values are valid
     *************************************************************************/
    @Override
    protected boolean verifySelection()
    {
        // Assume the dialog input is valid
        boolean validFlag = true;

        // Get the input values as strings
        String newMinX = minXTextField.getText().trim();
        String newMaxX = maxXTextField.getText().trim();

        // Regular expression for a decimal number in the form #.#, #., .#, -#,
        // -#., -.#, or -#.#
        String pattern = "(-?\\d*\\.?\\d+)|(-?\\d+\\.?\\d*)";

        // Ensure that the x-axis minimum and maximum fields contain numbers
        // (not blank and no alphabetical characters)
        if (!newMinX.matches(pattern) || !newMaxX.matches(pattern))
        {
            // Inform the user that the x-value is invalid
            new CPMDialogHandler().showMessageDialog(this,
                                                     "<html><b>X-axis values cannot be blank<br>or contain non-numeric characters",
                                                     "Invalid Value",
                                                     JOptionPane.ERROR_MESSAGE,
                                                     OK_OPTION);

            // Set the flag to indicate an invalid input
            validFlag = false;
        }
        // Ensure that the x-axis minimum is less than the maximum. Y-axis
        // labels don't require checking since the combo box' contents limits
        // the selection to legal values
        else if (Double.valueOf(newMinX) >= Double.valueOf(newMaxX))
        {
            // Inform the user that the minimum x-value must be less
            // than the maximum x-value
            new CPMDialogHandler().showMessageDialog(this,
                                                     "<html><b>Minimum x-value must be<br>less than the maximum x-value",
                                                     "Invalid Value",
                                                     JOptionPane.ERROR_MESSAGE,
                                                     OK_OPTION);

            // Set the flag to indicate an invalid input
            validFlag = false;
        }

        return validFlag;
    }
}
