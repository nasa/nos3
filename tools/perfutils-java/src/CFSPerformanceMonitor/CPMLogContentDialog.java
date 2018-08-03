/**
 * CFS Performance Monitor Log Content Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.COLUMN_NOT_DEFINED;
import static CFSPerformanceMonitor.CPMConstants.EXIT_MASK;
import static CFSPerformanceMonitor.CPMConstants.PERF_LOG_FILE;
import static CFSPerformanceMonitor.CPMConstants.RUNNING_ICON;
import static CFSPerformanceMonitor.CPMConstants.SEQERR_ICON;
import static CFSPerformanceMonitor.CPMConstants.STOPPED_ICON;
import static CFSPerformanceMonitor.CPMConstants.TABLE_BACK_COLOR;
import static CFSPerformanceMonitor.CPMConstants.TIME_FORMAT;
import static CFSPerformanceMonitor.CPMConstants.TIME_PRECISION;

import java.util.ArrayList;
import java.util.List;

import javax.print.attribute.standard.OrientationRequested;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.border.EtchedBorder;

import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;
import CFSPerformanceMonitor.CPMLogHandler.PerfLog;
import CFSPerformanceMonitor.CPMTableHandler.IconColumn;

/******************************************************************************
 * CFS Performance Monitor log content dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMLogContentDialog extends CPMDialogHandler
{
    private final CPMLogHandler perfLog;
    private final CPMIDHandler perfIDs;
    private final CPMMain perfMain;
    private final CPMPreferencesHandler plotPrefs;

    // Define the performance log table globally so that it can be accessed by
    // the action listeners, etc.
    private CPMTableHandler logTable;

    // Table column indices:
    // - Log event sequence index
    // - Performance ID in hexadecimal format
    // - Performance ID text name; '*** Undefined ***' if it doesn't have one
    // - Event time stamp relative to the beginning of the log, seconds
    // - Entry/exit event indicator
    // - Log event sequence error indicator
    // - Frame overrun time, seconds
    // - User-supplied data point annotations
    private final int INDEX = 0;
    private final int ID = 1;
    private final int NAME = 2;
    private final int TIME = 3;
    private final int ENT_EX = 4;
    private final int SEQ_ERR = 5;
    private final int OVER = 6;
    private final int NOTES = 7;
    private final int NUM_COLUMNS = 8;

    /**************************************************************************
     * Performance log content dialog class constructor
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
    protected CPMLogContentDialog(CPMLogHandler perfLog,
                                  CPMIDHandler perfIDs,
                                  CPMMain perfMain,
                                  CPMPreferencesHandler plotPrefs)
    {
        this.perfLog = perfLog;
        this.perfIDs = perfIDs;
        this.perfMain = perfMain;
        this.plotPrefs = plotPrefs;

        // Create the log content dialog
        initialize();
    }

    /**************************************************************************
     * Create the performance log content dialog
     *************************************************************************/
    private void initialize()
    {
        // Define the performance log content table
        logTable = new CPMTableHandler()
        {
            /******************************************************************
             * Override isCellEditable so that only the Notes column can be
             * edited
             *****************************************************************/
            @Override
            public boolean isCellEditable(int row, int column)
            {
                return column == NOTES;
            }

            /******************************************************************
             * Handle changes to the Notes column cells
             * 
             * @param row
             *            table row number
             * 
             * @param column
             *            table column number
             * 
             * @param oldValue
             *            original cell contents
             * 
             * @param newValue
             *            new cell contents
             ****************************************************************/
            @Override
            protected void handleCellContentChange(int row,
                                                   int column,
                                                   Object oldValue,
                                                   Object newValue)
            {
                // Store the log note with the log data and write it to the log
                // notes file
                perfLog.getPerfLogData().get(row).setNotes(newValue.toString());
                perfLog.writeLogNotes(row);
            }

            /******************************************************************
             * Load the log data into the table and format the table cells
             *****************************************************************/
            @Override
            protected void loadAndFormatData()
            {
                // Create an object to hold the log data
                final Object[][] tableLogList = new Object[perfLog.getPerfLogData().size()][NUM_COLUMNS];
                int row = 0;

                // Create the output format string for the time stamps so that
                // the decimals align with the text centered
                String timeFormat = "%"
                                    + (String.valueOf((int) (plotPrefs.isShowDataGaps(false)
                                                                                            ? perfLog.getMaximumTimeStamp()
                                                                                            : perfLog.getAdjustedDataTimeSpan())).length()
                                       + TIME_PRECISION + 1)
                                    + TIME_FORMAT.substring(1);

                // Collect the log data and format it for the table cells
                for (PerfLog log : perfLog.getPerfLogData())
                {
                    // Get the performance ID object associated with the log ID
                    PerformanceID pid = perfIDs.getPerfID(log.getPerfID());

                    tableLogList[row][INDEX] = row;
                    tableLogList[row][ID] = perfIDs.convertIDToHexString(log.isEntryEvent()
                                                                                           ? pid.getPerfID()
                                                                                           : pid.getPerfID()
                                                                                             | EXIT_MASK);
                    tableLogList[row][NAME] = pid.getPerfIDName(false);
                    tableLogList[row][TIME] = String.format(timeFormat, log.getTimeStamp());
                    tableLogList[row][ENT_EX] = log.isEntryEvent();
                    tableLogList[row][SEQ_ERR] = log.isSequenceError();
                    tableLogList[row][OVER] = log.getOverrun() == 0.0
                                                                     ? ""
                                                                     : String.format(TIME_FORMAT,
                                                                                     log.getOverrun());
                    tableLogList[row][NOTES] = log.getNotes();

                    row++;
                }

                // Place the log data into the table model along with the
                // column names, set up the editors and renderers for the table
                // cells, set up the table grid lines, and calculate the
                // minimum width required to display the table information
                int totalWidth = setUpdatableCharacteristics(tableLogList,
                                                             new String[] {"Index",
                                                                           "ID",
                                                                           "Name",
                                                                           "<html><center>Time Stamp<br>(sec)",
                                                                           "<html><center>Entry/<br>Exit",
                                                                           "<html><center>Seq<br>Err",
                                                                           "<html><center>Overrun<br>(sec)",
                                                                           "<html><center>Notes"},
                                                             true,
                                                             true,
                                                             true);

                // Fit the dialog to the table and set the minimum size based
                // on the column widths
                setDialogMinimumSize(totalWidth);

                // Set the dialog's title so that it includes the log file
                // name(s)
                setTitle("Log Data: "
                         + perfLog.getLogFileName(perfMain.getPreference(PERF_LOG_FILE)));
            }
        };

        // Create the list containing the columns that display icons and the
        // icons to display, based on the value of the cell
        List<IconColumn> iconColumns = new ArrayList<IconColumn>();
        iconColumns.add(logTable.new IconColumn(ENT_EX,
                                                new ImageIcon(getClass().getResource(RUNNING_ICON)),
                                                new ImageIcon(getClass().getResource(STOPPED_ICON))));
        iconColumns.add(logTable.new IconColumn(SEQ_ERR,
                                                new ImageIcon(getClass().getResource(SEQERR_ICON)),
                                                null));

        // Place the log data table into a scroll pane
        JScrollPane scrollPane = new JScrollPane(logTable);

        // Set common table parameters and characteristics
        logTable.setFixedCharacteristics(scrollPane,
                                         ListSelectionModel.MULTIPLE_INTERVAL_SELECTION,
                                         TABLE_BACK_COLOR,
                                         false,
                                         ID,
                                         NAME,
                                         NOTES,
                                         COLUMN_NOT_DEFINED,
                                         COLUMN_NOT_DEFINED,
                                         COLUMN_NOT_DEFINED,
                                         iconColumns,
                                         new String[] {"Event position in log",
                                                       "Performance ID (hexadecimal)",
                                                       "Performance ID name",
                                                       "Event relative time stamp (seconds)",
                                                       "Entry or exit event",
                                                       "Consecutive entry/exit event indicator",
                                                       "Frame overrun (seconds)",
                                                       "Annotations"});

        // Define log table panel
        JPanel logPanel = new JPanel();
        logPanel.setLayout(new BoxLayout(logPanel, BoxLayout.X_AXIS));
        logPanel.setBorder(BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
        logPanel.add(scrollPane);

        // Create an outer panel to put the log panel in. The border doesn't
        // appear without this
        JPanel outerPanel = new JPanel();
        outerPanel.setLayout(new BoxLayout(outerPanel, BoxLayout.X_AXIS));
        outerPanel.add(logPanel);
        outerPanel.setBorder(BorderFactory.createEmptyBorder());

        // Display the log content dialog box
        showOptionsDialog(perfMain.getMainFrame(),
                          outerPanel);
    }

    /**************************************************************************
     * Print the log contents table
     * 
     * @return Always return false so that the dialog doesn't close
     *************************************************************************/
    @Override
    protected boolean verifySelection()
    {
        // Print the log contents table
        logTable.printTable("Log Data: "
                            + perfLog.getLogFileName(perfMain.getPreference(PERF_LOG_FILE)),
                            this,
                            OrientationRequested.LANDSCAPE);

        return false;
    }

    /**************************************************************************
     * External method to reload the log data into the tables and reformat the
     * table cells
     *************************************************************************/
    protected void reloadAndReformatTableData()
    {
        logTable.loadAndFormatData();
    }
}
