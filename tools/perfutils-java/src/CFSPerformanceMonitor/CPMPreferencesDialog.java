/**
 * CFS Performance Monitor Plot Preferences Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.BAR;
import static CFSPerformanceMonitor.CPMConstants.BUTTON_FONT;
import static CFSPerformanceMonitor.CPMConstants.BY_NAME;
import static CFSPerformanceMonitor.CPMConstants.BY_VALUE;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_ICON;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.CHECK_BOX_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LABEL_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LABEL_VERTICAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LAF_CHECK_BOX_SIZE;
import static CFSPerformanceMonitor.CPMConstants.MAX_STEP_HEIGHT;
import static CFSPerformanceMonitor.CPMConstants.MAX_STEP_SPACING;
import static CFSPerformanceMonitor.CPMConstants.MIN_STEP_HEIGHT;
import static CFSPerformanceMonitor.CPMConstants.MIN_STEP_SPACING;
import static CFSPerformanceMonitor.CPMConstants.OK_CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.OK_ICON;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.PIE;
import static CFSPerformanceMonitor.CPMConstants.RESET_ICON;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics2D;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.KeyboardFocusManager;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.image.BufferedImage;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.Arrays;
import java.util.Dictionary;
import java.util.Hashtable;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSeparator;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;

import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;

/******************************************************************************
 * CFS Performance Monitor plot preferences dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMPreferencesDialog extends CPMDialogHandler
{
    private final CPMLogHandler perfLog;
    private final CPMIDHandler perfIDs;
    private final CPMStepPlot stepPlot;
    private final CPMThreadPlot threadPlot;
    private final CPMStatisticsPlot statisticsPlot;
    private final CPMMain perfMain;
    private final CPMPreferencesHandler plotPrefs;

    // Components that need to be accessed by other classes or listeners
    private JPanel mainPanel;
    private JPanel buttonPanel;
    private JSlider heightSlider;
    private JSlider spacingSlider;
    private JCheckBox labelCheckbox;
    private JCheckBox shadingCheckbox;
    private JCheckBox exitCheckbox;
    private JCheckBox showGapsCheckbox;
    private JCheckBox showOverrunsCheckbox;
    private JCheckBox verticalGridCheckbox;
    private JCheckBox horizontalGridCheckbox;
    private JRadioButton chartPieButton;
    private JRadioButton chartBarButton;
    private JCheckBox cpuIdleCheckbox;
    private JRadioButton sortNameButton;
    private JRadioButton sortValueButton;
    private JCheckBox minorFrameCheckbox;
    private JCheckBox majorFrameCheckbox;
    private JComboBox minorIDComboBox;
    private JComboBox majorIDComboBox;
    private ColorCheckBox backColorBox;
    private ColorCheckBox minorGridColorBox;
    private ColorCheckBox majorGridColorBox;
    private ColorCheckBox minorFrameColorBox;
    private ColorCheckBox majorFrameColorBox;

    // Performance IDs and names for the minor and major frame IDs
    private String minorIDText;
    private String majorIDText;

    /**************************************************************************
     * Plot preferences dialog class constructor
     * 
     * @param perfLog
     *            performance log
     * 
     * @param perfIDs
     *            performance IDs
     * 
     * @param stepPlot
     *            step plot
     * 
     * @param threadPlot
     *            thread of execution plot
     * 
     * @param statisticsPlot
     *            statistics plot
     * 
     * @param perfMain
     *            main class
     * 
     * @param plotPrefs
     *            plot preferences
     *************************************************************************/
    protected CPMPreferencesDialog(CPMLogHandler perfLog,
                                   CPMIDHandler perfIDs,
                                   CPMStepPlot stepPlot,
                                   CPMThreadPlot threadPlot,
                                   CPMStatisticsPlot statisticsPlot,
                                   CPMMain perfMain,
                                   CPMPreferencesHandler plotPrefs)
    {
        this.perfLog = perfLog;
        this.perfIDs = perfIDs;
        this.stepPlot = stepPlot;
        this.threadPlot = threadPlot;
        this.statisticsPlot = statisticsPlot;
        this.perfMain = perfMain;
        this.plotPrefs = plotPrefs;

        minorIDText = null;
        majorIDText = null;

        // Create the Preferences dialog
        createPreferencesDialog();
    }

    /**************************************************************************
     * Initialize the preference controls to the plot preferences values
     * 
     * @param useDefaults
     *            true to set the dialog controls to the default preference
     *            values; false to set the controls to the current preference
     *            values
     *************************************************************************/
    private void initializeControls(boolean useDefaults)
    {
        heightSlider.setValue(plotPrefs.getStepPlotHeight(useDefaults));
        spacingSlider.setValue(plotPrefs.getStepPlotSpacing(useDefaults));
        labelCheckbox.setSelected(plotPrefs.isStepPlotShowLabels(useDefaults));
        shadingCheckbox.setSelected(plotPrefs.isStepPlotShowShading(useDefaults));
        exitCheckbox.setSelected(plotPrefs.isStepPlotShowExit(useDefaults));
        showGapsCheckbox.setSelected(plotPrefs.isShowDataGaps(useDefaults));
        showOverrunsCheckbox.setSelected(plotPrefs.isShowOverruns(useDefaults));
        verticalGridCheckbox.setSelected(plotPrefs.isShowPlotVerticalGrid(useDefaults));
        horizontalGridCheckbox.setSelected(plotPrefs.isShowPlotHorizontalGrid(useDefaults));
        chartPieButton.setSelected(plotPrefs.getStatChartType(useDefaults) == PIE);
        chartBarButton.setSelected(plotPrefs.getStatChartType(useDefaults) == BAR);
        cpuIdleCheckbox.setSelected(plotPrefs.isShowCPUIdle(useDefaults));
        sortNameButton.setSelected(plotPrefs.getSortOrder(useDefaults) == BY_NAME);
        sortValueButton.setSelected(plotPrefs.getSortOrder(useDefaults) == BY_VALUE);
        minorFrameCheckbox.setSelected(plotPrefs.isShowPlotMinorFrames(useDefaults));
        majorFrameCheckbox.setSelected(plotPrefs.isShowPlotMajorFrames(useDefaults));
        minorIDComboBox.setSelectedItem(minorIDText);
        majorIDComboBox.setSelectedItem(majorIDText);
        backColorBox.setIconColor(plotPrefs.getBackgroundColor(useDefaults));
        minorGridColorBox.setIconColor(plotPrefs.getMinorGridColor(useDefaults));
        majorGridColorBox.setIconColor(plotPrefs.getMajorGridColor(useDefaults));
        minorFrameColorBox.setIconColor(plotPrefs.getMinorFrameColor(useDefaults));
        majorFrameColorBox.setIconColor(plotPrefs.getMajorFrameColor(useDefaults));
    }

    /**************************************************************************
     * Create the plot preferences dialog
     *************************************************************************/
    private void createPreferencesDialog()
    {
        // Create the Preferences dialog components
        createDialogComponents(createIDNameList());

        // Display the Preferences dialog and update the preference values
        // based on the user's actions
        updatePreferences();
    }

    /**************************************************************************
     * Create the list of names of the selected performance IDs
     * 
     * @return Array of ID names
     *************************************************************************/
    private String[] createIDNameList()
    {
        // Create an array to hold the performance ID names
        String[] idNames = new String[perfIDs.getPerfIDList().size() + 1];

        // Add a blank line
        idNames[0] = "";
        int index = 1;

        // Step through the performance ID list
        for (PerformanceID pid : perfIDs.getPerfIDList())
        {
            // Get the ID's name (or the ID if the name is undefined)
            String idText = pid.getPerfIDName(true);

            // Add the ID name (or ID if the name is undefined) to the array
            idNames[index] = idText;

            // Check if this is the user-selected minor frame ID
            if (pid.getPerfID() == plotPrefs.getMinorID(false))
            {
                // Save the minor frame ID so that it can be automatically
                // selected in the minor frame combo box
                minorIDText = idText;
            }

            // Check if this is the user-selected major frame ID
            if (pid.getPerfID() == plotPrefs.getMajorID(false))
            {
                // Save the major frame ID so that it can be automatically
                // selected in the major frame combo box
                majorIDText = idText;
            }

            index++;
        }

        // Sort the ID names in alpha-numeric order
        Arrays.sort(idNames);

        return idNames;
    }

    /**************************************************************************
     * Create the plot preferences dialog components
     * 
     * @param idNames
     *            array of selected performance ID names
     *************************************************************************/
    private void createDialogComponents(String[] idNames)
    {
        // Create a flag that is true if the minor/major frame inputs
        // are enabled
        boolean enableFrames = idNames.length != 1;

        // Create a panel to hold the components of the dialog
        mainPanel = new JPanel();

        // Create a tabbed pane
        JTabbedPane tabbedPane = new JTabbedPane(JTabbedPane.TOP);
        tabbedPane.setFont(LABEL_FONT_BOLD);
        mainPanel.add(tabbedPane);

        // Create border for the tabbed panes and combo boxes
        Border tabBorder = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        Border comboBorder = BorderFactory.createBevelBorder(BevelBorder.LOWERED,
                                                             Color.LIGHT_GRAY,
                                                             Color.GRAY);

        // Set the initial layout manager characteristics
        GridBagConstraints gbc = new GridBagConstraints(0,
                                                        1,
                                                        2,
                                                        1,
                                                        1.0,
                                                        0.0,
                                                        GridBagConstraints.LINE_START,
                                                        GridBagConstraints.NONE,
                                                        new Insets(LABEL_VERTICAL_SPACING,
                                                                   CHECK_BOX_HORIZONTAL_SPACING,
                                                                   0,
                                                                   LABEL_HORIZONTAL_SPACING),
                                                        LABEL_HORIZONTAL_SPACING,
                                                        0);

        // Create the All tab components --------------------------------------
        JPanel allPanel = new JPanel(new GridBagLayout());

        // Use an outer panel so that the components can be easily forced
        // to the top of the tab area
        JPanel allOuterPanel = new JPanel(new BorderLayout());
        allOuterPanel.setBorder(tabBorder);
        allOuterPanel.add(allPanel, BorderLayout.PAGE_START);
        tabbedPane.addTab(" All ",
                          null,
                          allOuterPanel,
                          "Preferences for all plots");
        tabbedPane.setMnemonicAt(0, 'L');

        // Create the button for changing the plot background color
        backColorBox = new ColorCheckBox("Background Color");
        allPanel.add(backColorBox, gbc);

        // Create the button for changing the plot major grid line color
        majorGridColorBox = new ColorCheckBox("Major Grid Line Color");
        gbc.insets.top = LABEL_VERTICAL_SPACING - 4;
        gbc.gridy++;
        allPanel.add(majorGridColorBox, gbc);

        // Create the button for changing the plot minor grid line color
        minorGridColorBox = new ColorCheckBox("Minor Grid Line Color");
        gbc.gridy++;
        allPanel.add(minorGridColorBox, gbc);

        // Create a checkbox for showing/hiding the vertical grid lines
        verticalGridCheckbox = createCheckBox("Show vertical grid lines",
                                              true);
        gbc.gridy++;
        allPanel.add(verticalGridCheckbox, gbc);

        // Create a checkbox for showing/hiding the data gaps if multiple log
        // files are loaded
        showGapsCheckbox = createCheckBox("Show data gaps",
                                          !perfLog.getDataGapIndex().isEmpty());
        gbc.gridy++;
        allPanel.add(showGapsCheckbox, gbc);

        // Create a checkbox for showing/hiding the frame overrun indicators
        showOverrunsCheckbox = createCheckBox("Show frame overruns",
                                              perfLog.isOverrun());
        gbc.gridy++;
        allPanel.add(showOverrunsCheckbox, gbc);

        // Add a separator
        JSeparator allSeparator1 = new JSeparator(JSeparator.HORIZONTAL);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridy++;
        gbc.insets.top = LABEL_VERTICAL_SPACING * 2;
        allPanel.add(allSeparator1, gbc);

        // Create the minor frame check box, label, and combo box
        minorFrameCheckbox = createCheckBox("Show minor frames",
                                            enableFrames);
        gbc.insets.top = LABEL_VERTICAL_SPACING;
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridy++;
        allPanel.add(minorFrameCheckbox, gbc);

        JLabel minorLabel = new JLabel("Minor frame ID");
        minorLabel.setFont(LABEL_FONT_BOLD);
        minorLabel.setEnabled(enableFrames);
        gbc.insets.left = LABEL_HORIZONTAL_SPACING;
        gbc.gridwidth = 1;
        gbc.weightx = 0.0;
        gbc.gridy++;
        allPanel.add(minorLabel, gbc);

        minorIDComboBox = new JComboBox(idNames);
        minorIDComboBox.setFont(LABEL_FONT_PLAIN);
        minorIDComboBox.setForeground(Color.BLACK);
        minorIDComboBox.setBackground(Color.WHITE);
        minorIDComboBox.setBorder(comboBorder);
        minorIDComboBox.setEnabled(enableFrames);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx = 1;
        allPanel.add(minorIDComboBox, gbc);

        // Create the button for changing the minor frame line color
        minorFrameColorBox = new ColorCheckBox("Minor Frame Line Color");
        gbc.insets.left = CHECK_BOX_HORIZONTAL_SPACING;
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridx = 0;
        gbc.gridy++;
        allPanel.add(minorFrameColorBox, gbc);

        // Add a separator
        JSeparator allSeparator2 = new JSeparator(JSeparator.HORIZONTAL);
        gbc.insets.left = LABEL_HORIZONTAL_SPACING;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridy++;
        gbc.insets.top = LABEL_VERTICAL_SPACING * 2;
        allPanel.add(allSeparator2, gbc);

        // Create the major frame check box, label, and combo box
        majorFrameCheckbox = createCheckBox("Show major frames",
                                            enableFrames);
        gbc.insets.left = CHECK_BOX_HORIZONTAL_SPACING;
        gbc.insets.top = LABEL_VERTICAL_SPACING;
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridy++;
        allPanel.add(majorFrameCheckbox, gbc);

        JLabel majorLabel = new JLabel("Major frame ID");
        majorLabel.setFont(LABEL_FONT_BOLD);
        majorLabel.setEnabled(enableFrames);
        gbc.insets.left = LABEL_HORIZONTAL_SPACING;
        gbc.gridwidth = 1;
        gbc.gridy++;
        allPanel.add(majorLabel, gbc);

        majorIDComboBox = new JComboBox(idNames);
        majorIDComboBox.setFont(LABEL_FONT_PLAIN);
        majorIDComboBox.setForeground(Color.BLACK);
        majorIDComboBox.setBackground(Color.WHITE);
        majorIDComboBox.setBorder(comboBorder);
        majorIDComboBox.setEnabled(enableFrames);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx++;
        allPanel.add(majorIDComboBox, gbc);

        // Create the button for changing the major frame line color
        majorFrameColorBox = new ColorCheckBox("Major Frame Line Color");
        gbc.insets.left = CHECK_BOX_HORIZONTAL_SPACING;
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridx = 0;
        gbc.gridy++;
        gbc.insets.bottom = LABEL_VERTICAL_SPACING * 2;
        allPanel.add(majorFrameColorBox, gbc);

        // Create the Step tab components -------------------------------------
        JPanel stepPanel = new JPanel(new GridBagLayout());

        // Use an outer panel so that the components can be easily forced to
        // the top of the tab area
        JPanel stepOuterPanel = new JPanel(new BorderLayout());
        stepOuterPanel.setBorder(tabBorder);
        stepOuterPanel.add(stepPanel, BorderLayout.PAGE_START);
        tabbedPane.addTab("Step",
                          null,
                          stepOuterPanel,
                          "Step plot preferences");
        tabbedPane.setMnemonicAt(1, 'S');

        // Set the grid layout starting points
        gbc.insets = new Insets(LABEL_VERTICAL_SPACING,
                                LABEL_HORIZONTAL_SPACING,
                                LABEL_VERTICAL_SPACING * 3,
                                LABEL_HORIZONTAL_SPACING);
        gbc.weightx = 0.0;
        gbc.gridwidth = 1;
        gbc.gridx = 0;
        gbc.gridy = 0;

        // Create the height slider label
        JLabel heightLabel = new JLabel("ID plot height");
        heightLabel.setFont(LABEL_FONT_BOLD);
        stepPanel.add(heightLabel, gbc);

        // Create a slider to adjust the step plot height
        heightSlider = new JSlider(MIN_STEP_HEIGHT, MAX_STEP_HEIGHT);

        // Set the slider appearance
        heightSlider.setMajorTickSpacing(10);
        heightSlider.setMinorTickSpacing(5);
        heightSlider.setPaintTicks(true);
        heightSlider.setPaintLabels(true);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.weightx = 1.0;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx++;
        stepPanel.add(heightSlider, gbc);

        // Adjust the height slider based on the mouse wheel movement
        heightSlider.addMouseWheelListener(new MouseWheelListener()
        {
            /******************************************************************
             * Handle a height slider mouse wheel movement event
             *****************************************************************/
            @Override
            public void mouseWheelMoved(MouseWheelEvent mwe)
            {
                // Adjust the new slider position based on mouse wheel rotation
                // direction. The slide "speed" is reduced by half to make
                // finer adjustment possible
                heightSlider.setValue(heightSlider.getValue()
                                      - (mwe.getUnitsToScroll() > 0
                                                                   ? 1
                                                                   : -1));
            }
        });

        // Create the spacing slider label
        JLabel spacingLabel = new JLabel("ID plot spacing");
        spacingLabel.setFont(LABEL_FONT_BOLD);
        gbc.fill = GridBagConstraints.NONE;
        gbc.weightx = 0.0;
        gbc.gridx = 0;
        gbc.gridy++;
        stepPanel.add(spacingLabel, gbc);

        // Create a slider to adjust the step plot spacing
        spacingSlider = new JSlider(MIN_STEP_SPACING, MAX_STEP_SPACING);
        spacingSlider.setSnapToTicks(true);

        // Change the labels for the spacing slider
        Dictionary<Integer, JLabel> dict = new Hashtable<Integer, JLabel>();
        dict.put(0, new JLabel("0"));
        dict.put(5, new JLabel("5"));
        dict.put(10, new JLabel("10"));
        dict.put(15, new JLabel("15"));
        spacingSlider.setLabelTable(dict);

        // Set the slider appearance and position
        spacingSlider.setMajorTickSpacing(5);
        spacingSlider.setMinorTickSpacing(1);
        spacingSlider.setPaintTicks(true);
        spacingSlider.setPaintLabels(true);
        gbc.insets.bottom = 0;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.weightx = 1.0;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx++;
        stepPanel.add(spacingSlider, gbc);

        // Adjust the spacing slider based on the mouse wheel movement
        spacingSlider.addMouseWheelListener(new MouseWheelListener()
        {
            /******************************************************************
             * Handle a spacing slider mouse wheel movement event
             *****************************************************************/
            @Override
            public void mouseWheelMoved(MouseWheelEvent mwe)
            {
                // Adjust the new slider position based on mouse wheel rotation
                // direction. The slide "speed" is reduced by half to make
                // finer adjustment possible
                spacingSlider.setValue(spacingSlider.getValue()
                                       - (mwe.getUnitsToScroll() > 0
                                                                    ? 1
                                                                    : -1));
            }
        });

        // Add a separator
        JSeparator stepSeparator = new JSeparator(JSeparator.HORIZONTAL);
        gbc.gridx = 0;
        gbc.gridy++;
        stepPanel.add(stepSeparator, gbc);
        gbc.fill = GridBagConstraints.NONE;

        // Create a checkbox for showing/hiding the y-axis tick labels
        labelCheckbox = createCheckBox("Show y-axis tick labels",
                                       true);
        gbc.insets.left = CHECK_BOX_HORIZONTAL_SPACING;
        gbc.gridy++;
        stepPanel.add(labelCheckbox, gbc);

        // Create a checkbox for showing/hiding the step line shading
        shadingCheckbox = createCheckBox("Shade area under step lines",
                                         true);
        gbc.gridy++;
        stepPanel.add(shadingCheckbox, gbc);

        // Create a checkbox for showing/hiding the step lines between exit and
        // enter events
        exitCheckbox = createCheckBox("Show lines between exit and enter events",
                                      true);
        gbc.gridy++;
        gbc.insets.bottom = LABEL_VERTICAL_SPACING * 2;
        stepPanel.add(exitCheckbox, gbc);

        // Create the Thread tab components -----------------------------------
        JPanel threadPanel = new JPanel(new GridBagLayout());

        // Use an outer panel so that the components can be easily forced to
        // the top of the tab area
        JPanel threadOuterPanel = new JPanel(new BorderLayout());
        threadOuterPanel.setBorder(tabBorder);
        threadOuterPanel.add(threadPanel, BorderLayout.PAGE_START);
        tabbedPane.addTab("Thread",
                          null,
                          threadOuterPanel,
                          "Thread plot preferences");
        tabbedPane.setMnemonicAt(2, 'T');

        // Set the grid layout starting points
        gbc.insets.bottom = 0;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx = 0;
        gbc.gridy = 0;

        // Create a checkbox for showing/hiding the horizontal grid lines
        horizontalGridCheckbox = createCheckBox("Show horizontal grid lines",
                                                true);
        threadPanel.add(horizontalGridCheckbox, gbc);

        // Create the Statistics tab components -------------------------------
        JPanel statisticsPanel = new JPanel(new GridBagLayout());

        // Use an outer panel so that the components can be easily forced to
        // the top of the tab area
        JPanel statisticsOuterPanel = new JPanel(new BorderLayout());
        statisticsOuterPanel.setBorder(tabBorder);
        statisticsOuterPanel.add(statisticsPanel, BorderLayout.PAGE_START);
        tabbedPane.addTab("Statistics",
                          null,
                          statisticsOuterPanel,
                          "Statistics plot preferences");
        tabbedPane.setMnemonicAt(3, 'A');

        // Set the grid layout starting points
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.insets = new Insets(LABEL_VERTICAL_SPACING,
                                LABEL_HORIZONTAL_SPACING,
                                0,
                                LABEL_HORIZONTAL_SPACING);

        // Create a sub-panel to hold the selection controls. This allows the
        // sub-panel's components to be positioned as desired relative to the
        // other components in the controls panel
        JPanel chartPanel = new JPanel(new GridBagLayout());

        // Set the sub-panel initial layout manager characteristics
        GridBagConstraints subgbc = new GridBagConstraints(0,
                                                           0,
                                                           1,
                                                           1,
                                                           0.0,
                                                           0.0,
                                                           GridBagConstraints.LINE_START,
                                                           GridBagConstraints.NONE,
                                                           new Insets(0, 0, 0, 0),
                                                           LABEL_HORIZONTAL_SPACING,
                                                           0);

        // Create radio buttons for displaying the statistics in pie or bar
        // chart format
        chartPieButton = new JRadioButton("Pie");
        chartBarButton = new JRadioButton("Bar");
        chartPieButton.setFont(LABEL_FONT_BOLD);
        chartBarButton.setFont(LABEL_FONT_BOLD);

        // Add the radio buttons to a group so that only one is active at a
        // time
        ButtonGroup chartGroup = new ButtonGroup();
        chartGroup.add(chartPieButton);
        chartGroup.add(chartBarButton);

        JLabel chartLabel = new JLabel("Chart type");
        chartLabel.setFont(LABEL_FONT_BOLD);
        chartPanel.add(chartLabel, subgbc);
        subgbc.weightx = 1.0;
        subgbc.gridx++;
        chartPanel.add(chartPieButton, subgbc);
        subgbc.gridy++;
        chartPanel.add(chartBarButton, subgbc);
        statisticsPanel.add(chartPanel, gbc);

        // Add a separator
        JSeparator statSeparator1 = new JSeparator(JSeparator.HORIZONTAL);
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridx = 0;
        gbc.gridy++;
        statisticsPanel.add(statSeparator1, gbc);
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridwidth = 2;

        // Create a checkbox for showing/hiding the CPU idle time
        cpuIdleCheckbox = createCheckBox("Show CPU idle time",
                                         true);
        gbc.gridy++;
        statisticsPanel.add(cpuIdleCheckbox, gbc);

        // Add a separator
        JSeparator statSeparator2 = new JSeparator(JSeparator.HORIZONTAL);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridy++;
        statisticsPanel.add(statSeparator2, gbc);
        gbc.fill = GridBagConstraints.NONE;

        // Create radio buttons for sorting the statistics plot by ID/name or
        // value
        sortNameButton = new JRadioButton("ID/name");
        sortValueButton = new JRadioButton("Value");
        sortNameButton.setFont(LABEL_FONT_BOLD);
        sortValueButton.setFont(LABEL_FONT_BOLD);

        // Add the radio buttons to a group so that only one is active at a
        // time
        ButtonGroup sortGroup = new ButtonGroup();
        sortGroup.add(sortNameButton);
        sortGroup.add(sortValueButton);

        // Create the sort buttons label, then position the label and buttons
        // relative to each other
        JLabel sortLabel = new JLabel("Arrange data by");
        sortLabel.setFont(LABEL_FONT_BOLD);
        gbc.insets.set(LABEL_VERTICAL_SPACING,
                       LABEL_HORIZONTAL_SPACING,
                       0,
                       0);
        gbc.weightx = 0.0;
        gbc.gridwidth = 1;
        gbc.gridy++;
        statisticsPanel.add(sortLabel, gbc);
        gbc.insets.set(LABEL_VERTICAL_SPACING,
                       0,
                       0,
                       LABEL_HORIZONTAL_SPACING);
        gbc.gridx++;
        statisticsPanel.add(sortNameButton, gbc);
        gbc.insets.set(0,
                       0,
                       LABEL_VERTICAL_SPACING,
                       LABEL_HORIZONTAL_SPACING);
        gbc.gridy++;
        statisticsPanel.add(sortValueButton, gbc);

        // Set the controls to the currently selected preferences values
        initializeControls(false);

        // Add a change listener to the keyboard focus manager so component
        // focus changes can be detected. This is needed to provide focus
        // indicators for the color selection check boxes
        KeyboardFocusManager.getCurrentKeyboardFocusManager().addPropertyChangeListener(new PropertyChangeListener()
        {
            Component focusOwner;

            /******************************************************************
             * Highlight a color check box if it has the keyboard focus and
             * remove the highlight when it loses focus
             *****************************************************************/
            @Override
            public void propertyChange(PropertyChangeEvent pce)
            {
                // Turn off color check box highlight when it no longer has
                // focus
                if (focusOwner != null && pce.getOldValue() == focusOwner)
                {
                    if (focusOwner.getClass().getSimpleName().equals("ColorCheckBox"))
                    {
                        ((ColorCheckBox) focusOwner).setIconFocus(false);
                    }

                    focusOwner = null;
                }

                // Turn on color check box highlight when it has focus
                if (pce.getNewValue() != null && pce.getNewValue() != focusOwner)
                {
                    focusOwner = (Component) pce.getNewValue();

                    if (focusOwner.getClass().getSimpleName().equals("ColorCheckBox"))
                    {
                        ((ColorCheckBox) focusOwner).setIconFocus(true);
                    }
                }

            }
        });

        // Create a panel to hold the dialog Okay, Default, and Cancel buttons
        buttonPanel = new JPanel();

        // Create the Okay button
        JButton btnOkay = new JButton("Okay",
                                      new ImageIcon(getClass().getResource(OK_ICON)));
        btnOkay.setFont(BUTTON_FONT);
        btnOkay.setMnemonic('O');
        buttonPanel.add(btnOkay);

        // Add a listener for the Okay button
        btnOkay.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Apply the preference changes
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set the button selected to Okay
                setButtonSelected(OK_OPTION);

                // Close the dialog
                closeDialog();
            }
        });

        // Create the Default button
        JButton btnDefault = new JButton("Default",
                                         new ImageIcon(getClass().getResource(RESET_ICON)));
        btnDefault.setFont(BUTTON_FONT);
        btnDefault.setMnemonic('D');
        buttonPanel.add(btnDefault);

        // Add a listener for the Default button
        btnDefault.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Set the preferences to the default values
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set the controls to the default preference values
                initializeControls(true);
            }
        });

        // Create the Cancel button
        JButton btnCancel = new JButton("Cancel",
                                        new ImageIcon(getClass().getResource(CANCEL_ICON)));
        btnCancel.setFont(BUTTON_FONT);
        btnCancel.setMnemonic('C');
        buttonPanel.add(btnCancel);

        // Add a listener for the Cancel button
        btnCancel.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Cancel changing the preferences
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set the button selected to Cancel
                setButtonSelected(CANCEL_OPTION);

                // Close the dialog
                closeDialog();
            }
        });
    }

    /**************************************************************************
     * Display the plot preferences dialog and update the preferences based on
     * the user's actions
     *************************************************************************/
    private void updatePreferences()
    {
        // Display the Plot Preferences dialog and wait for the user to select
        // the Okay or Cancel button. If Okay is chosen update the plots
        if (showOptionsDialog(perfMain.getMainFrame(),
                              mainPanel,
                              buttonPanel,
                              "Plot Preferences",
                              false) == OK_OPTION)
        {
            // Check for changes in the various controls and update as needed
            updateBackgroundColor();
            updateStepAppearance();
            updateDataGap();
            updateOverrun();
            updateVerticalGridLine();
            updateHorizontalGridLine();
            updateStatsChartType();
            updateCPUIdle();
            updateSortOrder();
            updateMinorMajorFrame();
        }
    }

    /**************************************************************************
     * Check for changes and update the step and thread of execution plot
     * background and grid line colors
     *************************************************************************/
    private void updateBackgroundColor()
    {
        // Check if the background or grid line colors changed
        if (backColorBox.getBackground() != plotPrefs.getBackgroundColor(false)
            || majorGridColorBox.getBackground() != plotPrefs.getMajorGridColor(false)
            || minorGridColorBox.getBackground() != plotPrefs.getMinorGridColor(false))
        {
            // Save the new color(s)
            plotPrefs.setBackgroundColor(backColorBox.getIconColor());
            plotPrefs.setMajorGridColor(majorGridColorBox.getIconColor());
            plotPrefs.setMinorGridColor(minorGridColorBox.getIconColor());

            // Update the plot colors if the plot exists
            if (stepPlot != null)
            {
                stepPlot.setPlotColors(stepPlot.isHasActivityLine());
            }

            if (threadPlot != null)
            {
                threadPlot.setPlotColors(true);
            }

            if (statisticsPlot != null)
            {
                statisticsPlot.setPlotColors();
            }
        }
    }

    /**************************************************************************
     * Check for changes and update the step plot's subplot height and spacing,
     * and display of the y-axis tick labels, shading, and lines between the
     * exit and enter events
     *************************************************************************/
    private void updateStepAppearance()
    {
        // Check if the step plot height or the spacing values changed
        if (plotPrefs.getStepPlotHeight(false) != heightSlider.getValue()
            || plotPrefs.getStepPlotSpacing(false) != spacingSlider.getValue()
            || plotPrefs.isStepPlotShowLabels(false) != labelCheckbox.isSelected()
            || plotPrefs.isStepPlotShowShading(false) != shadingCheckbox.isSelected()
            || plotPrefs.isStepPlotShowExit(false) != exitCheckbox.isSelected())
        {
            // Save the new value(s)
            plotPrefs.setStepPlotHeight(heightSlider.getValue());
            plotPrefs.setStepPlotSpacing(spacingSlider.getValue());
            plotPrefs.setStepPlotShowLabels(labelCheckbox.isSelected());
            plotPrefs.setStepPlotShowShading(shadingCheckbox.isSelected());
            plotPrefs.setStepPlotShowExit(exitCheckbox.isSelected());

            // Redraw the step plot if it's instantiated
            if (stepPlot != null)
            {
                // Set the step plot height and spacing
                stepPlot.adjustPlotHeightAndSpacing();

                // Force the plot to redraw using the new size
                stepPlot.getPlotChartPanel().revalidate();
            }
        }
    }

    /**************************************************************************
     * Check for changes and update the display of gaps between log files
     *************************************************************************/
    private void updateDataGap()
    {
        // Check if the data gap selection changed
        if (plotPrefs.isShowDataGaps(false) != showGapsCheckbox.isSelected())
        {
            plotPrefs.setShowDataGaps(showGapsCheckbox.isSelected());

            // Remove or restore the data gaps based on the check box
            // selection and update the plots and data dialogs
            perfLog.adjustDataGaps(plotPrefs.isShowDataGaps(false));
            perfMain.updatePlotsAndDialogs();
        }
    }

    /**************************************************************************
     * Check for changes and update the display of potential frame overruns
     *************************************************************************/
    private void updateOverrun()
    {
        // Check if the frame overrun selection changed
        if (plotPrefs.isShowOverruns(false) != showOverrunsCheckbox.isSelected())
        {
            plotPrefs.setShowOverruns(showOverrunsCheckbox.isSelected());

            // Remove or restore the overrun indicators based on the check box
            // selection and update the plots
            if (stepPlot != null)
            {
                stepPlot.showOverruns();
            }

            if (threadPlot != null)
            {
                threadPlot.showOverruns();
            }
        }
    }

    /**************************************************************************
     * Check for changes and update the display of vertical grid lines on the
     * step and thread of execution plots
     *************************************************************************/
    private void updateVerticalGridLine()
    {
        // Check if the vertical grid line selection changed
        if (plotPrefs.isShowPlotVerticalGrid(false) != verticalGridCheckbox.isSelected())
        {
            plotPrefs.setShowPlotVerticalGrid(verticalGridCheckbox.isSelected());

            // Draw or hide the vertical grid lines, if applicable, and if the
            // plot exists
            if (stepPlot != null)
            {
                stepPlot.setVerticalGridLines();
            }

            if (threadPlot != null)
            {
                threadPlot.setVerticalGridLines();
            }

            if (statisticsPlot != null)
            {
                statisticsPlot.setVerticalGridLines();
            }
        }
    }

    /**************************************************************************
     * Check for changes and update the display of horizontal grid lines on the
     * step and thread of execution plots
     *************************************************************************/
    private void updateHorizontalGridLine()
    {
        // Check if the horizontal grid line selection changed
        if (plotPrefs.isShowPlotHorizontalGrid(false) != horizontalGridCheckbox.isSelected())
        {
            plotPrefs.setShowPlotHorizontalGrid(horizontalGridCheckbox.isSelected());

            // Draw or hide the horizontal grid lines, if applicable, and if
            // the plot exists
            if (threadPlot != null)
            {
                threadPlot.setHorizontalGridLines();
            }
        }
    }

    /**************************************************************************
     * Check for changes and update the statistics chart type
     *************************************************************************/
    private void updateStatsChartType()
    {
        // Check if the statistics chart type selection changed
        int selectedButton = chartPieButton.isSelected()
                                                        ? PIE
                                                        : BAR;

        if (plotPrefs.getStatChartType(false) != selectedButton)
        {
            plotPrefs.setStatChartType(selectedButton);

            // Check if the statistics plot exists
            if (statisticsPlot != null)
            {
                // Select the statistics chart type
                statisticsPlot.selectStatsChartType(plotPrefs.getStatChartType(false));
            }
        }
    }

    /**************************************************************************
     * Check for changes and update the display of the CPU idle time on the
     * statistics pie charts
     *************************************************************************/
    private void updateCPUIdle()
    {
        // Check if the CPU idle time selection changed
        if (plotPrefs.isShowCPUIdle(false) != cpuIdleCheckbox.isSelected())
        {
            plotPrefs.setShowCPUIdle(cpuIdleCheckbox.isSelected());

            // Check if the statistics plot exists
            if (statisticsPlot != null)
            {
                // Show or hide the CPU idle time, if applicable
                statisticsPlot.setCPUIdleTime(plotPrefs.isShowCPUIdle(false));
            }
        }
    }

    /**************************************************************************
     * Check for changes and update the statistics chart sort order
     *************************************************************************/
    private void updateSortOrder()
    {
        // Check if the sort order selection changed
        int selectedButton = sortNameButton.isSelected()
                                                        ? BY_NAME
                                                        : BY_VALUE;

        if (plotPrefs.getSortOrder(false) != selectedButton)
        {
            plotPrefs.setSortOrder(selectedButton);

            // Check if the statistics plot exists
            if (statisticsPlot != null)
            {
                // Sort the data by ID/name or value
                statisticsPlot.setSortOrder(plotPrefs.getSortOrder(false));
            }
        }
    }

    /**************************************************************************
     * Check for changes and update the minor and major frame IDs and line
     * colors
     *************************************************************************/
    private void updateMinorMajorFrame()
    {
        // Frame property changed flag
        boolean frameChanged = false;

        // Check if the user changed the minor or major frame display
        // selections or IDs
        if (plotPrefs.isShowPlotMinorFrames(false) != minorFrameCheckbox.isSelected()
            || plotPrefs.isShowPlotMajorFrames(false) != majorFrameCheckbox.isSelected()
            || !compareStringToSelection(minorIDText,
                                         minorIDComboBox.getSelectedItem())
            || !compareStringToSelection(majorIDText,
                                         majorIDComboBox.getSelectedItem()))
        {
            // Save the new value(s)
            plotPrefs.setShowPlotMinorFrames(minorFrameCheckbox.isSelected());
            plotPrefs.setShowPlotMajorFrames(majorFrameCheckbox.isSelected());

            // Initialize to the default before comparing to the ID list
            plotPrefs.setMinorID(plotPrefs.getMinorID(true));
            plotPrefs.setMajorID(plotPrefs.getMajorID(true));

            // Step through the IDs and save the minor and major performance
            // IDs. Save the ID instead of the ID text since the text can be
            // changed
            for (PerformanceID pid : perfIDs.getPerfIDList())
            {
                if (compareStringToSelection(pid.getPerfIDName(true),
                                             minorIDComboBox.getSelectedItem()))
                {
                    plotPrefs.setMinorID(pid.getPerfID());
                }

                if (compareStringToSelection(pid.getPerfIDName(true),
                                             majorIDComboBox.getSelectedItem()))
                {
                    plotPrefs.setMajorID(pid.getPerfID());
                }
            }

            frameChanged = true;
        }

        // Check if the minor or major frame line color changed
        if (minorFrameColorBox.getBackground() != plotPrefs.getMinorFrameColor(false)
            || majorFrameColorBox.getBackground() != plotPrefs.getMajorFrameColor(false))
        {
            // Save the new line color(s)
            plotPrefs.setMinorFrameColor(minorFrameColorBox.getIconColor());
            plotPrefs.setMajorFrameColor(majorFrameColorBox.getIconColor());
            frameChanged = true;
        }

        // Check if a frame property changed
        if (frameChanged)
        {
            // Draw or hide the minor and major frame markers, if applicable,
            // and if the plot exists
            if (stepPlot != null)
            {
                stepPlot.adjustFrameLines();
            }

            if (threadPlot != null)
            {
                threadPlot.adjustFrameLines();
            }
        }
    }

    /**************************************************************************
     * Compare a string with an object and return true if they are the same
     * 
     * @param str
     *            first string to compare
     * 
     * @param obj
     *            second string, cast as an Object, to compare
     * 
     * @return true if the two strings are identical
     *************************************************************************/
    private boolean compareStringToSelection(String str, Object obj)
    {
        return (obj == null ? str == null : obj.toString().equals(str));
    }

    /**************************************************************************
     * Create a check box with label
     * 
     * @param title
     *            text string associated with the check box
     * 
     * @param isEnabled
     *            true is initially enabled
     * 
     * @return New check box with the specified characteristics
     *************************************************************************/
    private JCheckBox createCheckBox(String title,
                                     boolean isEnabled)
    {
        // Create a check box with label
        JCheckBox checkBox = new JCheckBox(title);
        checkBox.setHorizontalAlignment(JCheckBox.LEFT);
        checkBox.setFont(LABEL_FONT_BOLD);
        checkBox.setEnabled(isEnabled);

        return checkBox;
    }

    /**************************************************************************
     * Create a color selection component based on a check box. In place of the
     * check box a color box is displayed
     *************************************************************************/
    private class ColorCheckBox extends JCheckBox
    {
        private final String checkBoxText;
        private Color checkBoxColor;
        private ImageIcon noFocusIcon;
        private ImageIcon focusIcon;

        /**********************************************************************
         * Color check box constructor
         * 
         * @param text
         *            label to display along with the check box
         *********************************************************************/
        protected ColorCheckBox(String text)
        {
            // Store the check box label
            checkBoxText = text;

            // Create a label for the button. Adjust the label so that only the
            // initial character is capitalized. The initial caps version is
            // used when displaying the color selection dialog
            text = text.toLowerCase();
            text = Character.toUpperCase(text.charAt(0)) + text.substring(1);
            setHorizontalAlignment(JCheckBox.LEFT);
            setFont(LABEL_FONT_BOLD);
            setText(text);

            // Listen for selection events
            addActionListener(new ActionListener()
            {
                /**************************************************************
                 * Handle selection of the check box
                 *************************************************************/
                @Override
                public void actionPerformed(ActionEvent ae)
                {
                    // Display the color choice dialog
                    Color newColor = new CPMDialogHandler().showColorDialog(CPMPreferencesDialog.this,
                                                                            checkBoxText,
                                                                            OK_CANCEL_OPTION,
                                                                            checkBoxColor);

                    // Check if a valid color was selected
                    if (newColor != null)
                    {
                        // Update the button color
                        setIconColor(newColor);
                    }
                }
            });
        }

        /**********************************************************************
         * Return the color selected for the check box
         *********************************************************************/
        protected Color getIconColor()
        {
            return checkBoxColor;
        }

        /**********************************************************************
         * Create the icons to display for the check box in the selected color
         * 
         * @param color
         *            color in which to display the check box icon
         *********************************************************************/
        protected void setIconColor(Color color)
        {
            // Store the selected color
            checkBoxColor = color;

            // Create the icon for when the check box does not have the
            // keyboard focus and the mouse cursor is not positioned over the
            // check box
            noFocusIcon = createIcon(Color.BLACK);

            // Create the icon for when the check box has the keyboard focus
            // and the mouse cursor is not positioned over the check box
            focusIcon = createIcon(Color.WHITE);

            // Create the icon for when the mouse cursor is positioned over the
            // check box
            setRolloverIcon(createIcon(Color.LIGHT_GRAY));
            setRolloverSelectedIcon(createIcon(Color.LIGHT_GRAY));

            // Select the icons created for use with the check box. This
            // ensures the color check boxes are drawn as color check boxes and
            // not regular check boxes initially
            setIconFocus(false);
        }

        /**********************************************************************
         * Create a check box color icon
         * 
         * @param size
         *            width and height of the icon in pixels
         * 
         * @param borderColor
         *            color with which to outline the icon
         * 
         * @return Icon in the selected color with a border in the color
         *         specified
         *********************************************************************/
        private ImageIcon createIcon(Color borderColor)
        {
            // Create an image to work with
            BufferedImage image = new BufferedImage(LAF_CHECK_BOX_SIZE,
                                                    LAF_CHECK_BOX_SIZE,
                                                    BufferedImage.TYPE_INT_ARGB);

            // Create the graphics object from the image
            Graphics2D g2 = image.createGraphics();

            // Draw the icon image
            g2.setColor(borderColor);
            g2.drawRect(0, 0, LAF_CHECK_BOX_SIZE - 1, LAF_CHECK_BOX_SIZE - 1);
            g2.setColor(checkBoxColor);
            g2.fillRect(1, 1, LAF_CHECK_BOX_SIZE - 2, LAF_CHECK_BOX_SIZE - 2);

            // Delete the graphics object
            g2.dispose();

            return new ImageIcon(image);
        }

        /**********************************************************************
         * Update the color check box icon depending whether or not it has the
         * keyboard focus
         * 
         * @param isFocus
         *            true if the check box has the key board focus
         *********************************************************************/
        protected void setIconFocus(boolean isFocus)
        {
            // Select the icon to use based on the focus flag
            ImageIcon icon = isFocus ? focusIcon : noFocusIcon;

            // Set the icon to display
            setIcon(icon);
            setSelectedIcon(icon);
        }
    }
}
