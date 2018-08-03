/**
 * CFS Performance Monitor Performance Log Statistics Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.CELL_FONT;
import static CFSPerformanceMonitor.CPMConstants.CELL_HORIZONTAL_PADDING;
import static CFSPerformanceMonitor.CPMConstants.COLUMN_NOT_DEFINED;
import static CFSPerformanceMonitor.CPMConstants.HEADER_FONT;
import static CFSPerformanceMonitor.CPMConstants.LABEL_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LAF_BIAS;
import static CFSPerformanceMonitor.CPMConstants.PERF_LOG_FILE;
import static CFSPerformanceMonitor.CPMConstants.TABLE_BACK_COLOR;
import static CFSPerformanceMonitor.CPMConstants.TABLE_TEXT_COLOR;
import static CFSPerformanceMonitor.CPMConstants.TIME_FORMAT;
import static CFSPerformanceMonitor.CPMConstants.TOTALS_COLOR;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.Comparator;

import javax.print.attribute.standard.OrientationRequested;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableModel;
import javax.swing.table.TableRowSorter;

import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;

/******************************************************************************
 * CFS Performance Monitor log statistics dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMLogStatisticsDialog extends CPMDialogHandler
{
    private final CPMLogHandler perfLog;
    private final CPMIDHandler perfIDs;
    private final CPMMain perfMain;

    // Define globally so that the components can be accessed by the action
    // listeners, etc.
    private CPMTableHandler statsTable;
    private CPMTableHandler totalsTable;

    // These components must be defined globally so they can be accessed by
    // the dialog window resize listener
    private JPanel totalsPanel;
    private JLabel totalsLabel;

    // Table column indices:
    // - Performance ID in hexadecimal format
    // - Performance ID text name; '*** Undefined ***' if it doesn't have one
    // - Number of entry events
    // - Number of exit events
    // - Average entry event frequency, events/second
    // - Total time active (between entry and exit events), seconds
    // - Total time inactive (between exit and entry events), seconds
    // - Total time active (between entry and exit events), percent
    // - Total time inactive (between exit and entry events), percent
    // - Minimum time active, seconds
    // - Maximum time active, seconds
    // - Minimum time between entry events, seconds
    // - Maximum time between entry events, seconds
    // - Minimum frame overrun time, seconds
    // - Maximum frame overrun time, seconds
    private final int ID = 0;
    private final int NAME = 1;
    private final int ENTRY = 2;
    private final int EXIT = 3;
    private final int AVG_FREQ = 4;
    private final int ACT_SEC = 5;
    private final int INACT_SEC = 6;
    private final int ACT_PCT = 7;
    private final int INACT_PCT = 8;
    private final int MIN_ACT = 9;
    private final int MAX_ACT = 10;
    private final int MIN_INT = 11;
    private final int MAX_INT = 12;
    private final int MIN_OVER = 13;
    private final int MAX_OVER = 14;
    private final int NUM_COLUMNS = 15;

    // Display text for cells that have no value
    private final String NO_DATA = "n/a";

    /**************************************************************************
     * Performance log statistics dialog class constructor
     * 
     * @param perfLog
     *            performance log
     * 
     * @param perfIDs
     *            performance IDs
     * 
     * @param perfMain
     *            main class
     *************************************************************************/
    protected CPMLogStatisticsDialog(CPMLogHandler perfLog,
                                     CPMIDHandler perfIDs,
                                     CPMMain perfMain)
    {
        this.perfLog = perfLog;
        this.perfIDs = perfIDs;
        this.perfMain = perfMain;

        // Create the ID editor dialog
        initialize();
    }

    /**************************************************************************
     * Create the performance log statistics dialog
     *************************************************************************/
    private void initialize()
    {
        // Define the performance ID statistics table
        statsTable = new CPMTableHandler()
        {
            /******************************************************************
             * Override isCellEditable so that no column can be edited
             *****************************************************************/
            @Override
            public boolean isCellEditable(int row, int column)
            {
                return false;
            }

            /******************************************************************
             * Override getToolTipText so that the time stamp for the minimum
             * and maximum statistic values can be displayed
             *****************************************************************/
            @Override
            public String getToolTipText(MouseEvent me)
            {
                String toolTipText = null;
                Point point = me.getPoint();
                int rowIndex = rowAtPoint(point);
                int colIndex = columnAtPoint(point);

                for (PerformanceID pid : perfIDs.getPerfIDList())
                {
                    if (pid.getPerfID() == perfIDs.convertStringToIntID(getValueAt(rowIndex,
                                                                                   ID).toString()))
                    {
                        // Create the tool tip text based on the table column
                        // over which the mouse pointer is located
                        switch (colIndex)
                        {
                            case MIN_ACT:
                                toolTipText = buildToolTipText(pid.getMinTimeActiveTimeStamp());
                                break;

                            case MAX_ACT:
                                toolTipText = buildToolTipText(pid.getMaxTimeActiveTimeStamp());
                                break;

                            case MIN_INT:
                                toolTipText = buildToolTipText(pid.getMinIntervalTimeStamp());
                                break;

                            case MAX_INT:
                                toolTipText = buildToolTipText(pid.getMaxIntervalTimeStamp());
                                break;

                            case MIN_OVER:
                                toolTipText = buildToolTipText(pid.getMinOverrunTimeStamp());
                                break;

                            case MAX_OVER:
                                toolTipText = buildToolTipText(pid.getMaxOverrunTimeStamp());
                                break;
                        }

                        break;
                    }
                }

                return toolTipText;
            }

            /**************************************************************************
             * Statistics table loader placeholder. The statistics and totals
             * tables are loaded and formatted together in the totals table
             * override for this method
             *************************************************************************/
            @Override
            protected void loadAndFormatData()
            {
            }
        };

        // Place the statistics table into a scroll pane
        JScrollPane scrollPane = new JScrollPane(statsTable);

        // Set common table parameters and characteristics
        statsTable.setFixedCharacteristics(scrollPane,
                                           ListSelectionModel.MULTIPLE_INTERVAL_SELECTION,
                                           TABLE_BACK_COLOR,
                                           false,
                                           ID,
                                           NAME,
                                           COLUMN_NOT_DEFINED,
                                           COLUMN_NOT_DEFINED,
                                           COLUMN_NOT_DEFINED,
                                           COLUMN_NOT_DEFINED,
                                           null,
                                           new String[] {"Performance ID (hexadecimal)",
                                                         "Performance ID name",
                                                         "Total number of Entry events",
                                                         "Total number of Exit events",
                                                         "Average frequency of entry events (events / second)",
                                                         "Total time active (seconds)",
                                                         "Total time inactive (seconds)",
                                                         "Total time active (percent)",
                                                         "Total time inactive (percent)",
                                                         "Minimum time between entry & exit events (seconds)",
                                                         "Maximum time between entry & exit events (seconds)",
                                                         "Minimum interval between entry events (seconds)",
                                                         "Maximum interval between entry events (seconds)",
                                                         "Minimum frame overrun (seconds)",
                                                         "Maximum frame overrun (seconds)"});

        // Define the performance ID overall totals table
        totalsTable = new CPMTableHandler()
        {
            /******************************************************************
             * Override getToolTipText so that the time stamp for the overall
             * minimum and maximum statistic values can be displayed
             *****************************************************************/
            @Override
            public String getToolTipText(MouseEvent me)
            {
                String toolTipText = null;

                switch (columnAtPoint(me.getPoint()))
                {
                    case MIN_ACT - 2:
                        toolTipText = buildToolTipText(perfLog.getTotalMinTimeActiveID(),
                                                       perfLog.getTotalMinTimeOnTimeStamp());
                        break;

                    case MAX_ACT - 2:
                        toolTipText = buildToolTipText(perfLog.getTotalMaxTimeActiveID(),
                                                       perfLog.getTotalMaxTimeOnTimeStamp());
                        break;

                    case MIN_INT - 2:
                        toolTipText = buildToolTipText(perfLog.getTotalMinIntervalID(),
                                                       perfLog.getTotalMinIntervalTimeStamp());
                        break;

                    case MAX_INT - 2:
                        toolTipText = buildToolTipText(perfLog.getTotalMaxIntervalID(),
                                                       perfLog.getTotalMaxIntervalTimeStamp());
                        break;

                    case MIN_OVER - 2:
                        toolTipText = buildToolTipText(perfLog.getTotalMinOverrunID(),
                                                       perfLog.getTotalMinOverrunTimeStamp());
                        break;

                    case MAX_OVER - 2:
                        toolTipText = buildToolTipText(perfLog.getTotalMaxOverrunID(),
                                                       perfLog.getTotalMaxOverrunTimeStamp());
                        break;
                }

                return toolTipText;
            }

            /******************************************************************
             * Load the ID statistics into the table and format the table
             * cells. Both the statistics and totals tables are loaded in this
             * one method for simplicity
             *****************************************************************/
            @Override
            protected void loadAndFormatData()
            {
                // Copy the performance ID statistics values to an object array
                // for display in the dialog box statistics table
                Object[][] statsTableData = new Object[perfIDs.getPerfIDList().size()][NUM_COLUMNS];
                int row = 0;

                // Step through the ID list
                for (PerformanceID pid : perfIDs.getPerfIDList())
                {
                    // Convert the ID to a hexadecimal string
                    statsTableData[row][ID] = perfIDs.convertIDToHexString(pid.getPerfID());

                    // ID name
                    statsTableData[row][NAME] = pid.getPerfIDName(false);

                    // Statistics
                    statsTableData[row][ENTRY] = pid.getEntryCount();
                    statsTableData[row][EXIT] = pid.getExitCount();
                    statsTableData[row][AVG_FREQ] = formatStatistic("%.2f",
                                                                    pid.getAvgFreq());
                    statsTableData[row][ACT_SEC] = formatStatistic(TIME_FORMAT,
                                                                   pid.getTimeActive());
                    statsTableData[row][INACT_SEC] = formatStatistic(TIME_FORMAT,
                                                                     perfLog.getAdjustedDataTimeSpan()
                                                                         - pid.getTimeActive());
                    statsTableData[row][ACT_PCT] = formatStatistic("%.3f",
                                                                   pid.getTimeActive()
                                                                       / perfLog.getAdjustedDataTimeSpan()
                                                                       * 100.0);
                    statsTableData[row][INACT_PCT] = formatStatistic("%.3f",
                                                                     (perfLog.getAdjustedDataTimeSpan()
                                                                         - pid.getTimeActive())
                                                                         / perfLog.getAdjustedDataTimeSpan()
                                                                         * 100.0);
                    statsTableData[row][MIN_ACT] = formatStatistic(TIME_FORMAT,
                                                                   pid.getMinTimeActive());
                    statsTableData[row][MAX_ACT] = formatStatistic(TIME_FORMAT,
                                                                   pid.getMaxTimeActive());
                    statsTableData[row][MIN_INT] = formatStatistic(TIME_FORMAT,
                                                                   pid.getMinInterval());
                    statsTableData[row][MAX_INT] = formatStatistic(TIME_FORMAT,
                                                                   pid.getMaxInterval());
                    statsTableData[row][MIN_OVER] = formatStatistic(TIME_FORMAT,
                                                                    pid.getMinOverrun());
                    statsTableData[row][MAX_OVER] = formatStatistic(TIME_FORMAT,
                                                                    pid.getMaxOverrun());

                    row++;
                }

                // Copy the overall statistics values to an object array for
                // display in the dialog box totals table
                Object[][] totalsTableData = new Object[1][NUM_COLUMNS - ENTRY];

                totalsTableData[0][ENTRY - ENTRY] = perfLog.getTotalOnCount();
                totalsTableData[0][EXIT - ENTRY] = perfLog.getTotalOffCount();
                totalsTableData[0][AVG_FREQ - ENTRY] = formatStatistic("%.2f",
                                                                       perfLog.getTotalAvgFreq());
                totalsTableData[0][ACT_SEC - ENTRY] = String.format(TIME_FORMAT,
                                                                    perfLog.getTotalTimeActive());
                totalsTableData[0][INACT_SEC - ENTRY] = String.format(TIME_FORMAT,
                                                                      perfLog.getAdjustedDataTimeSpan()
                                                                          - perfLog.getTotalTimeActive());
                totalsTableData[0][ACT_PCT - ENTRY] = formatStatistic("%.3f",
                                                                      perfLog.getTotalTimeActive()
                                                                          / perfLog.getAdjustedDataTimeSpan()
                                                                          * 100.0);
                totalsTableData[0][INACT_PCT - ENTRY] = formatStatistic("%.3f",
                                                                        (perfLog.getAdjustedDataTimeSpan()
                                                                            - perfLog.getTotalTimeActive())
                                                                            / perfLog.getAdjustedDataTimeSpan()
                                                                            * 100.0);
                totalsTableData[0][MIN_ACT - ENTRY] = formatStatistic(TIME_FORMAT,
                                                                      perfLog.getTotalMinTimeActive());
                totalsTableData[0][MAX_ACT - ENTRY] = formatStatistic(TIME_FORMAT,
                                                                      perfLog.getTotalMaxTimeActive());
                totalsTableData[0][MIN_INT - ENTRY] = formatStatistic(TIME_FORMAT,
                                                                      perfLog.getTotalMinInterval());
                totalsTableData[0][MAX_INT - ENTRY] = formatStatistic(TIME_FORMAT,
                                                                      perfLog.getTotalMaxInterval());
                totalsTableData[0][MIN_OVER - ENTRY] = formatStatistic(TIME_FORMAT,
                                                                       perfLog.getTotalMinOverrun());
                totalsTableData[0][MAX_OVER - ENTRY] = formatStatistic(TIME_FORMAT,
                                                                       perfLog.getTotalMaxOverrun());

                // Store the row number (equals the total number of rows in the
                // table)
                int maxRow = row;

                // Create an array to hold the maximum width, in characters, of
                // each column
                int maxLen[] = new int[NUM_COLUMNS];

                // Pad the values in the cells with leading spaces so that the
                // decimals align. Step through the data columns
                for (int column = 0; column < NUM_COLUMNS; column++)
                {
                    // Check if this is a column containing floating point
                    // values
                    if (column >= AVG_FREQ)
                    {
                        // Step through each row in this column
                        for (row = 0; row < maxRow; row++)
                        {
                            // Check if the width of the text is the longest
                            // found
                            if (statsTableData[row][column].toString().length() > maxLen[column])
                            {
                                // Store the longest text width
                                maxLen[column] = statsTableData[row][column].toString().length();
                            }
                        }

                        // Check if the totals text width for this column is
                        // longer
                        if (totalsTableData[0][column - ENTRY].toString().length() > maxLen[column])
                        {
                            // Store the longest text width
                            maxLen[column] = totalsTableData[0][column - ENTRY].toString().length();
                        }

                        // For each row in this column
                        for (row = 0; row < maxRow; row++)
                        {
                            // Check if the statistics array contains a value
                            if (!statsTableData[row][column].equals(NO_DATA))
                            {
                                // Pad the text with spaces (to the left) to
                                // make it the maximum width. This aligns the
                                // values by their decimal points
                                statsTableData[row][column] = String.format(("%"
                                                                             + maxLen[column]
                                                                             + "s"),
                                                                            statsTableData[row][column]);
                            }
                        }

                        // Check if the totals array contains a value
                        if (!totalsTableData[0][column - ENTRY].equals(NO_DATA))
                        {
                            // Pad the text with spaces (to the left) to make
                            // it the maximum width. This aligns the value by
                            // its decimal point with the values in the
                            // statistics table
                            totalsTableData[0][column - ENTRY] = String.format(("%"
                                                                                + maxLen[column]
                                                                                + "s"),
                                                                               totalsTableData[0][column - ENTRY]);
                        }
                    }
                }

                // Place the statistics data into the table model along with
                // the column names, set up the renderers for the table cells,
                // set up the table grid lines, and calculate the minimum width
                // required to display the statistics information
                int totalWidth = statsTable.setUpdatableCharacteristics(statsTableData,
                                                                        new String[] {"ID",
                                                                                      "Name",
                                                                                      "<html><center>Entry<br>Events",
                                                                                      "<html><center>Exit<br>Events",
                                                                                      "<html><center>Avg Freq<br>(evt/sec)",
                                                                                      "<html><center>Time Act<br>(sec)",
                                                                                      "<html><center>Time Inact<br>(sec)",
                                                                                      "<html><center>Time Act<br>(%)",
                                                                                      "<html><center>Time Inact<br>(%)",
                                                                                      "<html><center>Min Act<br>(sec)",
                                                                                      "<html><center>Max Act<br>(sec)",
                                                                                      "<html><center>Min Int<br>(sec)",
                                                                                      "<html><center>Max Int<br>(sec)",
                                                                                      "<html><center>Min Over<br>(sec)",
                                                                                      "<html><center>Max Over<br>(sec)"},
                                                                        true,
                                                                        true,
                                                                        true);

                // Place the statistics totals data into the table model. The
                // column names aren't used but an array of the correct size
                // must be supplied. Set up the renderers for the table cells
                // and the table grid lines
                totalsTable.setUpdatableCharacteristics(totalsTableData,
                                                        new String[NUM_COLUMNS - ENTRY],
                                                        true,
                                                        false,
                                                        false);

                // Set the widths of the totals table columns
                totalsTable.setColumnWidths(ENTRY, statsTable.getMinWidth());

                // Fit the dialog to the table and set the minimum size based
                // on the column widths
                setDialogMinimumSize(totalWidth);

                // Set the dialog's title so that it includes the log file
                // name(s)
                setTitle("Log Statistics: "
                         + perfLog.getLogFileName(perfMain.getPreference(PERF_LOG_FILE)));
            }
        };

        // Set common table parameters and characteristics
        totalsTable.setFixedCharacteristics(ListSelectionModel.SINGLE_INTERVAL_SELECTION,
                                            TOTALS_COLOR,
                                            false,
                                            COLUMN_NOT_DEFINED,
                                            new String[] {"", "", "", "",
                                                          "", "", "", "",
                                                          "", "", "", "",
                                                          ""});

        // Get the statistics table's row sorter
        TableRowSorter<TableModel> sorter = new TableRowSorter<TableModel>(statsTable.getModel());
        statsTable.setRowSorter(sorter);

        // For each of the columns in the table that can be a floating point
        // value
        for (int index = AVG_FREQ; index < NUM_COLUMNS; index++)
        {
            // Override the sort comparator to convert the strings to numbers,
            // if applicable, and then compare them. Otherwise these table
            // entries would be sorted alphabetically
            sorter.setComparator(index, new Comparator<String>()
            {
                /**************************************************************
                 * Compare two strings converted to doubles
                 *************************************************************/
                @Override
                public int compare(String strg1, String strg2)
                {
                    // Sort direction:
                    // 1 => strg1 > strg2
                    // 0 => strg1 = strg2
                    // - 1 => strg1 < strg2
                    int sortDirection;

                    // Check if the strings representations are the same
                    if (strg1.equals(strg2))
                    {
                        // No need to sort
                        sortDirection = 0;
                    }
                    // Check if either string is "n/a"; if so, treat it as the
                    // greater value when sorting
                    else if (strg1.equals(NO_DATA))
                    {
                        sortDirection = 1;
                    }
                    else if (strg2.equals(NO_DATA))
                    {
                        sortDirection = -1;
                    }
                    // The string represents a number
                    else
                    {
                        // Parse the strings as numbers and compare; sort based
                        // on the values
                        if (Double.parseDouble(strg1)
                            - Double.parseDouble(strg2) >= 0.0)
                        {
                            sortDirection = 1;
                        }
                        else
                        {
                            sortDirection = -1;
                        }
                    }

                    return sortDirection;
                }
            });
        }

        // Create a border for the panels
        Border border = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);

        // Define the statistics table panel with a scroll pane
        JPanel statsPanel = new JPanel();
        statsPanel.setLayout(new BoxLayout(statsPanel, BoxLayout.X_AXIS));
        statsPanel.setBorder(border);
        statsPanel.add(scrollPane);

        // Define the overall statistics text fields for the lower panel
        int textHeight = getFontMetrics(HEADER_FONT).getHeight() + 19;

        // Create a label for the totals table
        totalsLabel = new JLabel("Overall");
        totalsLabel.setFont(HEADER_FONT);
        totalsLabel.setHorizontalAlignment(JLabel.RIGHT);
        totalsLabel.setVerticalAlignment(JLabel.CENTER);
        totalsLabel.setOpaque(true);
        totalsLabel.setForeground(TABLE_TEXT_COLOR);

        // Create the totals table panel with fixed dimensions. The width value
        // is unused
        totalsPanel = new JPanel();
        totalsPanel.setLayout(new BoxLayout(totalsPanel, BoxLayout.X_AXIS));
        totalsPanel.setPreferredSize(new Dimension(1, textHeight));
        totalsPanel.setMaximumSize(new Dimension(1, textHeight));
        totalsPanel.setMinimumSize(new Dimension(1, textHeight));
        totalsPanel.setBorder(border);
        totalsPanel.add(totalsLabel);
        totalsPanel.add(totalsTable);

        // Define a panel for the statistics table and the totals table
        JPanel tablePanel = new JPanel(new BorderLayout());
        tablePanel.add(statsPanel, BorderLayout.CENTER);
        tablePanel.add(totalsPanel, BorderLayout.PAGE_END);

        // Add a component listener to the dialog box to handle resize events
        addComponentListener(new ComponentAdapter()
        {
            /******************************************************************
             * Handle the dialog window resize event so that the totals table
             * can be positioned if the window is resized using the mouse
             *****************************************************************/
            @Override
            public void componentResized(ComponentEvent ce)
            {
                // Reposition the totals table
                repositionTotalsTable();
            }
        });

        // Add a listener to the dialog box to handle resize property changes
        addPropertyChangeListener(new PropertyChangeListener()
        {
            /******************************************************************
             * Handle the dialog resize due to a look & feel change event
             *****************************************************************/
            @Override
            public void propertyChange(PropertyChangeEvent pce)
            {
                // Check if this is the property event to handle
                if (pce.getPropertyName() == "resize")
                {
                    // Reposition the totals table
                    repositionTotalsTable();
                }
            }
        });

        // Position the totals table initially
        repositionTotalsTable();

        // Display the log content dialog box
        showOptionsDialog(perfMain.getMainFrame(),
                          tablePanel);
    }

    /**************************************************************************
     * Print the statistics table
     * 
     * @return Always return false so that the dialog doesn't close
     *************************************************************************/
    @Override
    protected boolean verifySelection()
    {
        // Get the table models
        DefaultTableModel statsModel = (DefaultTableModel) statsTable.getModel();
        DefaultTableModel totalsModel = (DefaultTableModel) totalsTable.getModel();

        // Calculate the number of spaces with which to pad the "Overall" text
        // in the table in order to right-justify it in the Name column. This
        // is equal to the column's pixel width minus the cell padding and the
        // width of the "Overall" text, divided by the width of a single
        // character (the cells use a fixed width font)
        int spc = (statsTable.getMinWidth()[NAME]
                   - (CELL_HORIZONTAL_PADDING * 2) - 1
                   - (int) getFontMetrics(CELL_FONT).getStringBounds("Overall", null).getWidth())
                  / (int) getFontMetrics(CELL_FONT).getStringBounds("X", null).getWidth();

        String pad = "";

        // Build a string of spaces for the padding
        for (int index = 0; index < spc; index++)
        {
            pad += " ";
        }

        // Since the data consists of two tables (statistics and totals), to
        // print these together requires combining the tables temporarily.
        // Insert an empty row, then insert the totals table contents into the
        // statistics table
        statsModel.addRow(new Object[] {"", "", "", "", "",
                                        "", "", "", "", "",
                                        "", "", "", "", ""});
        statsModel.addRow(new Object[] {"",
                                        "<html><pre>" + pad + "<b>Overall",
                                        totalsModel.getValueAt(0, ENTRY - 2),
                                        totalsModel.getValueAt(0, EXIT - 2),
                                        totalsModel.getValueAt(0, AVG_FREQ - 2),
                                        totalsModel.getValueAt(0, ACT_SEC - 2),
                                        totalsModel.getValueAt(0, INACT_SEC - 2),
                                        totalsModel.getValueAt(0, ACT_PCT - 2),
                                        totalsModel.getValueAt(0, INACT_PCT - 2),
                                        totalsModel.getValueAt(0, MIN_ACT - 2),
                                        totalsModel.getValueAt(0, MAX_ACT - 2),
                                        totalsModel.getValueAt(0, MIN_INT - 2),
                                        totalsModel.getValueAt(0, MAX_INT - 2),
                                        totalsModel.getValueAt(0, MIN_OVER - 2),
                                        totalsModel.getValueAt(0, MAX_OVER - 2)});

        // Print the combined (statistics + totals) table
        statsTable.printTable("Log Statistics: "
                              + perfLog.getLogFileName(perfMain.getPreference(PERF_LOG_FILE)),
                              this,
                              OrientationRequested.LANDSCAPE);

        // Remove the two rows added to the statistics table
        ((DefaultTableModel) statsTable.getModel()).removeRow(statsTable.getRowCount() - 1);
        ((DefaultTableModel) statsTable.getModel()).removeRow(statsTable.getRowCount() - 1);

        return false;
    }

    /**************************************************************************
     * Reposition the totals table when the dialog box changes size, including
     * when changes are made to the look & feel
     *************************************************************************/
    private void repositionTotalsTable()
    {
        // Calculate the padding, in pixels, required to align the columns in
        // the totals table with those in the statistics table. The position is
        // dependent on the current size of the ID name column (which is
        // resizable)
        int leftPadding = statsTable.getMinWidth()[ID]
                          + statsTable.getTableHeader().getColumnModel().getColumn(NAME).getWidth()
                          - (int) getFontMetrics(HEADER_FONT).getStringBounds("Overall", null).getWidth()
                          - LABEL_HORIZONTAL_SPACING
                          + LAF_BIAS;

        // (Re)position the totals table
        totalsLabel.setBorder(BorderFactory.createEmptyBorder(0,
                                                              leftPadding,
                                                              0,
                                                              LABEL_HORIZONTAL_SPACING));
    }

    /**************************************************************************
     * Create the maximum/minimum statistics value tool tip text string
     *************************************************************************/
    private String buildToolTipText(Double timeStamp)
    {
        String tooltip = null;

        // Check if a value exists for this cell
        if (timeStamp != null)
        {
            tooltip = "@ time stamp "
                      + String.format(TIME_FORMAT, timeStamp);
        }

        return tooltip;
    }

    /**************************************************************************
     * Create the overall maximum/minimum statistics value tool tip text string
     * 
     * @param perfID
     *            performance ID value
     * 
     * @param timeStamp
     *            event time stamp, seconds
     *************************************************************************/
    private String buildToolTipText(long perfID, Double timeStamp)
    {
        String tooltip = null;

        // Check if a value exists for this cell
        if (timeStamp != null)
        {
            tooltip = "<html>"
                      + perfIDs.getPerfID(perfID).getPerfIDName(true)
                      + "<br>"
                      + buildToolTipText(timeStamp);
        }

        return tooltip;
    }

    /**************************************************************************
     * Format a statistic value. If the value hasn't been calculated then
     * return "n/a"
     * 
     * @param format
     *            output string format
     * 
     * @param statValue
     *            statistics datum to be formatted
     *************************************************************************/
    private String formatStatistic(String format, Double statValue)
    {
        return statValue == null
                                ? NO_DATA
                                : String.format(format, statValue);
    }

    /**************************************************************************
     * External method to reload the statistics into the tables and reformat
     * the table cells
     *************************************************************************/
    protected void reloadAndReformatTableData()
    {
        statsTable.loadAndFormatData();
        totalsTable.loadAndFormatData();
    }
}
