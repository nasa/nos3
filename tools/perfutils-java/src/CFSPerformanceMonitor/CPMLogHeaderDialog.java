/**
 * CFS Performance Monitor Log Header Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.PRINT_OPTION;
import static CFSPerformanceMonitor.CPMConstants.TABLE_BACK_COLOR;
import static CFSPerformanceMonitor.CPMConstants.TRIGGER_MODE;
import static CFSPerformanceMonitor.CPMConstants.TRIGGER_STATE;

import java.awt.BorderLayout;
import java.io.File;

import javax.print.attribute.standard.OrientationRequested;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.ListSelectionModel;
import javax.swing.border.EtchedBorder;

import CFSPerformanceMonitor.CPMLogHandler.LogHeader;

/******************************************************************************
 * CFS Performance Monitor log header dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMLogHeaderDialog extends CPMDialogHandler
{
    private final CPMLogHandler perfLog;
    private final CPMMain perfMain;

    // Width, in pixels, needed to display a header tab's table
    private int tabWidth = -1;

    // Components that need to be accessed globally
    private final CPMTableHandler[] headerTable;
    private JTabbedPane tabbedPane;

    /**************************************************************************
     * Performance log header dialog class constructor
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
    protected CPMLogHeaderDialog(CPMLogHandler perfLog,
                                 CPMMain perfMain)
    {
        this.perfLog = perfLog;
        this.perfMain = perfMain;

        headerTable = new CPMTableHandler[perfLog.getLogHeader().size()];

        // Create the log header dialog
        initialize();
    }

    /**************************************************************************
     * Create the performance log header dialog
     *************************************************************************/
    private void initialize()
    {
        int index = 0;
        int totalWidth = 0;

        // Create a panel to hold the components of the dialog. Use a
        // BorderLayout so that the panel (and the tabbed pane within) fills
        // the dialog if the dialog is resized
        JPanel mainPanel = new JPanel(new BorderLayout());

        // Create a tabbed pane for the header information to appear in
        tabbedPane = new JTabbedPane(JTabbedPane.TOP);
        tabbedPane.setFont(LABEL_FONT_BOLD);
        mainPanel.add(tabbedPane);

        // For each log file header
        for (LogHeader lh : perfLog.getLogHeader())
        {
            // Index of the tab currently being constructed
            final int tabIndex = index;

            // Define the performance log header table
            headerTable[index] = new CPMTableHandler()
            {
                /**************************************************************
                 * Load the log data into the table and format the table cells
                 *************************************************************/
                @Override
                protected void loadAndFormatData()
                {
                    LogHeader lh = perfLog.getLogHeader().get(tabIndex);

                    // Get the header information for the table
                    Object[][] tableheaderList = new Object[][]
                    {
                     {"Content type", lh.getContentType()},
                     {"Sub type", lh.getSubType()},
                     {"Primary header length", lh.getLength()},
                     {"Spacecraft ID", lh.getSpacecraftID()},
                     {"Processor ID", lh.getProcessorID()},
                     {"Application ID", lh.getApplicationID()},
                     {"Time (seconds)", lh.getTimeSeconds()},
                     {"Time (subseconds)", lh.getTimeSubSeconds()},
                     {"Description", lh.getDescription()},
                     {"Version", lh.getVersion()},
                     {"Endian", lh.getEndian() == 1 ? "Big" : "Little"},
                     {"Ticks/second", lh.getTimerTicksPerSecond()},
                     {"Timer rollover", lh.getTimerLow32Rollover()},
                     {"Trigger state", TRIGGER_STATE[(int) lh.getState()]},
                     {"Trigger mode", TRIGGER_MODE[(int) lh.getMode()]},
                     {"Trigger count", lh.getTriggerCount()},
                     {"Data start index", lh.getDataStart()},
                     {"Data end index", lh.getDataEnd()},
                     {"Data count", lh.getDataCount()},
                     {"Invalid marker reported", lh.getInvalidMarkerReported() == 1
                                                                                   ? "Yes"
                                                                                   : "No"},
                     {"Filter/trigger mask size", lh.getFilterTriggerMaskSize()},
                     {"Filter mask", lh.getFilterMask()},
                     {"Trigger mask", lh.getTriggerMask()}
                    };

                    // Place the log header data into the table model, set up
                    // the renderers for the table cells, set up the table grid
                    // lines, and calculate the minimum width required to
                    // display the tab information
                    tabWidth = setUpdatableCharacteristics(tableheaderList,
                                                           new String[] {"", ""},
                                                           false,
                                                           true,
                                                           true);
                }
            };

            // Set the number of columns to at which to begin wrapping the
            // values column text based on the log description length. If the
            // description is very long the use a value that is equal to the
            // number of columns to display four 8-digit hexadecimal mask
            // values (= (8 chars + 1 space) * 4 mask values + 2 spaces for
            // padding)
            headerTable[index].setMinimumWrapColumn(Math.min(lh.getDescription().length() + 2,
                                                             38));

            // Set the number of rows to display in the table to show all but
            // the last two (filter and trigger mask) rows
            headerTable[index].setInitialViewableRows(21);

            // Set common table parameters and characteristics
            headerTable[index].setFixedCharacteristics(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION,
                                                       TABLE_BACK_COLOR,
                                                       false,
                                                       1,
                                                       new String[] {"Header parameter description",
                                                                     "Header parameter value"});

            // Turn of table sorting
            headerTable[index].setRowSorter(null);

            // Remove the table header
            headerTable[index].setTableHeader(null);

            // Check if this header table's width is the widest thus far
            if (tabWidth > totalWidth)
            {
                // Save the widest table width
                totalWidth = tabWidth;
            }

            // Place the header table into a scroll pane
            JScrollPane scrollPane = new JScrollPane(headerTable[index]);

            // Create the header table panel
            JPanel headerPanel = new JPanel();
            headerPanel.setLayout(new BoxLayout(headerPanel, BoxLayout.X_AXIS));
            headerPanel.setBorder(BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
            headerPanel.add(scrollPane);

            // Create a tab for each log file header
            File file = new File(lh.getLogFileName());
            tabbedPane.addTab(file.getName(),
                              null,
                              headerPanel,
                              file.getAbsoluteFile().toString());

            index++;
        }

        // Fit the dialog to the table and set the minimum size based on the
        // column widths
        setDialogMinimumSize(totalWidth);

        // Display the log header dialog box
        showOptionsDialog(perfMain.getMainFrame(),
                          mainPanel,
                          "Log Header",
                          PRINT_OPTION,
                          true);
    }

    /**************************************************************************
     * Print the log header table (in the currently selected tab if multiple
     * logs are loaded)
     * 
     * @return Always return false so that the dialog doesn't close
     *************************************************************************/
    @Override
    protected boolean verifySelection()
    {
        // Print the currently selected log header table
        headerTable[tabbedPane.getSelectedIndex()].printTable("Log Header: "
                                                              + tabbedPane.getTitleAt(tabbedPane.getSelectedIndex()),
                                                              this,
                                                              OrientationRequested.PORTRAIT);

        return false;
    }
}