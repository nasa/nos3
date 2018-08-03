/**
 * CFS Performance Monitor Common constants
 */
package CFSPerformanceMonitor;

import java.awt.Color;
import java.awt.Font;

import javax.swing.JCheckBox;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JWindow;
import javax.swing.ScrollPaneConstants;
import javax.swing.UIManager;

/******************************************************************************
 * CFS Performance Monitor common constants class
 *****************************************************************************/
public class CPMConstants
{
    // CPM version number and version date
    protected static final String CPM_VERSION = "1.0.0";
    protected static final String CPM_DATE = "September 2014";
    protected static final String CPM_AUTHOR = "NASA JSC: ER6/Kevin McCluney";

    // Program preferences backing store keys
    protected static final String SEARCH_SOURCE_PATH = "SearchSourcePath";
    protected static final String SEARCH_SUBFOLDERS = "SearchSubfolders";
    protected static final String SEARCH_FILE_EXTENSION = "SearchFileExtension";
    protected static final String SEARCH_PERF_ID_SUFFIX = "SearchPerfIDSuffix";
    protected static final String PERF_ID_LIST_FILE = "PerfIDListFile";
    protected static final String PERF_LOG_FILE = "PerfLogFile";
    protected static final String LOOK_AND_FEEL = "LookAndFeel";
    protected static final String PLOT_PREFERENCES = "PlotPreferences";

    // 'Magic number' for cFE compliant files defined in cfe_fs.h. The
    // hexadecimal equivalent to 'cFE1'. A log file is identified by beginning
    // with this pattern
    protected static final int CFE_FS_FILE_CONTENT_ID = 0x63464531;

    // Trigger states
    protected static final String[] TRIGGER_STATE = {"Idle",
                                                     "Waiting",
                                                     "Triggered"};

    // Trigger modes
    protected static final String[] TRIGGER_MODE = {"Start",
                                                    "Center",
                                                    "End"};

    // Source code subfolders to search for performance IDs. The text case of
    // the names in this list don't affect the search since both this name and
    // the comparison folder name are forced to lower case prior to comparing
    // the names
    protected static final String DEFAULT_SUBFOLDERS = "apps,cfe";

    // Check for performance IDs only in files with this extension
    protected static final String DEFAULT_FILE_EXTENSION = ".h";

    // Only text in the targeted source code ending with this sequence are
    // considered performance IDs
    protected static final String DEFAULT_PERF_ID_SUFFIX = "PERF_ID";

    // Plot tab indices
    protected static final int STEP_PLOT = 0;
    protected static final int THREAD_PLOT = 1;
    protected static final int STATISTICS_PLOT = 2;

    // Help file name
    protected static final String HELP_FILE = "/docs/CPM_Users_Guide.pdf";

    // Main window minimum window size
    protected static final int MIN_WINDOW_WIDTH = 760;
    protected static final int MIN_WINDOW_HEIGHT = 400;

    // Table-containing dialog box minimum height
    protected static final int DIALOG_MINIMUM_HEIGHT = 300;

    // Masks to check for or remove the high bit, which (if set) indicates
    // this is an exit event (otherwise it's an entry event)
    protected static final long ID_MASK = 0x7fffffffL;
    protected static final long EXIT_MASK = 0x80000000L;

    // Log file name separator in the program preferences backing store if
    // multiple log files are stored
    protected static final String LOG_SEPARATOR = ";";

    // Step plot entry and exit event y-axis values
    protected static final double EXIT = 0.0;
    protected static final double ENTRY = 1.0;

    // Sort order of the data in the statistics plot (low to high); by
    // ID/name or value
    protected static final int BY_NAME = 0;
    protected static final int BY_VALUE = 1;

    // Statistics plot default chart type
    protected static final int PIE = 0;
    protected static final int BAR = 1;

    // Default performance ID name to use until the user changes it
    protected static final String NAME_NOT_DEFINED = "*** Undefined ***";

    // Column index value when the column does not exist in a table
    protected static final int COLUMN_NOT_DEFINED = -1;

    // Array of default ID colors
    protected static final Color[] ID_COLOR = {Color.GREEN,
                                               Color.YELLOW,
                                               Color.BLUE,
                                               Color.ORANGE,
                                               Color.MAGENTA,
                                               Color.CYAN,
                                               Color.LIGHT_GRAY,
                                               Color.PINK};

    // Text color for selected row's table text
    protected static final Color SELECTED_TEXT_COLOR = Color.WHITE;

    // Background color for selected row's table text
    protected static final Color SELECTED_BACK_COLOR = new Color(80, 120, 200);

    // Background color for alternating rows of non-selected table text
    protected static final Color ALTERNATE_COLOR = new Color(240, 245, 245);

    // Background color for the selected cell's table text
    protected static final Color FOCUS_COLOR = new Color(60, 100, 180);

    // Table text color (unselected)
    protected static final Color TABLE_TEXT_COLOR = Color.BLACK;

    // Table background color (unselected)
    protected static final Color TABLE_BACK_COLOR = Color.WHITE;

    // Totals table background color
    protected static final Color TOTALS_COLOR = new Color(240, 240, 240);

    // Color for table grid lines
    protected static final Color GRID_COLOR = new Color(230, 240, 240);

    // Dialog text label fonts
    protected static final Font LABEL_FONT_PLAIN = new Font("DejaVu Sans",
                                                            Font.PLAIN,
                                                            12);
    protected static final Font LABEL_FONT_BOLD = new Font("DejaVu Sans",
                                                           Font.BOLD,
                                                           12);

    // Table header and cell fonts
    protected static final Font HEADER_FONT = new Font("DejaVu Sans",
                                                       Font.BOLD,
                                                       12);
    protected static final Font CELL_FONT = new Font("Monospaced",
                                                     Font.PLAIN,
                                                     13);

    // Dialog box button font
    protected static final Font BUTTON_FONT = new Font("Dialog",
                                                       Font.BOLD,
                                                       12);

    // Plot axis label and tick mark fonts
    protected static final Font PLOT_LABEL_FONT = new Font("DejaVu Sans",
                                                           Font.PLAIN,
                                                           12);
    protected static final Font PLOT_TICK_FONT = new Font("DejaVu Sans",
                                                          Font.PLAIN,
                                                          10);
    protected static final Font STATS_Y_LABEL_FONT = new Font("Monospaced",
                                                              Font.PLAIN,
                                                              14);

    // Dialog box button padding, in pixels, between the group of buttons and
    // the edge of
    // the dialog window or upper dialog components)
    protected static final int BUTTON_PAD = 16;

    // Dialog box button padding, in pixels, between individual buttons
    protected static final int BUTTON_GAP = 4;

    // Horizontal and vertical spacing, in pixels, between a text label and
    // another component
    protected static final int LABEL_HORIZONTAL_SPACING = 10;
    protected static final int LABEL_VERTICAL_SPACING = 7;

    // Check box horizontal spacing, in pixels, between the left side of the
    // dialog and the check box
    protected static final int CHECK_BOX_HORIZONTAL_SPACING = 6;

    // Padding to the bottom (x-axis) or right side (y-axis) of the plot scroll
    // bars, in pixels
    protected static final int PLOT_SCROLL_BAR_PAD = 4;

    // Padding between the bottom of the plot and the x-axis label plot, in
    // pixels
    protected static final int PLOT_XAXIS_LABEL_PAD = 2;

    // Table header horizontal and vertical padding, in pixels. The horizontal
    // padding provides room for the column sort arrow
    protected static final int HEADER_HORIZONTAL_PADDING = 14;
    protected static final int HEADER_VERTICAL_PADDING = 20;

    // Table cell horizontal and vertical padding, in pixels
    protected static final int CELL_HORIZONTAL_PADDING = 5;
    protected static final int CELL_VERTICAL_PADDING = 3;

    // Padding between the dialog contents and the dialog's frame, and between
    // the dialog internal components, in pixels
    protected static final int DIALOG_BORDER_PAD = 10;
    protected static final int DIALOG_INNER_PAD = 10;

    // Minimum and maximum step subplot height and subplot spacing, in pixels
    protected static final int MIN_STEP_HEIGHT = 20;
    protected static final int MAX_STEP_HEIGHT = 100;
    protected static final int MIN_STEP_SPACING = 0;
    protected static final int MAX_STEP_SPACING = 15;

    // Dialog box return options
    protected static final int OK_OPTION = JOptionPane.OK_OPTION;
    protected static final int CANCEL_OPTION = JOptionPane.CANCEL_OPTION;
    protected static final int OK_CANCEL_OPTION = JOptionPane.OK_CANCEL_OPTION;
    protected static final int APPEND_OPTION = 0xe0;
    protected static final int CLEAR_OPTION = 0xe1;

    // Dialog box option to display alternate text for the Okay button
    protected static final int CLOSE_OPTION = 0xfe;
    protected static final int LOAD_OPTION = 0xfd;
    protected static final int SAVE_OPTION = 0xfc;
    protected static final int SEARCH_OPTION = 0xfb;
    protected static final int READ_OPTION = 0xfa;
    protected static final int PRINT_OPTION = 0xf9;

    // Button and table icon file names
    protected static final String OK_ICON = "/images/ok.png";
    protected static final String CANCEL_ICON = "/images/cancel.png";
    protected static final String CLOSE_ICON = "/images/close.png";
    protected static final String NEW_ICON = "/images/new.png";
    protected static final String DELETE_ICON = "/images/delete.png";
    protected static final String UP_ICON = "/images/up.png";
    protected static final String DOWN_ICON = "/images/down.png";
    protected static final String SHOW_ICON = "/images/show.png";
    protected static final String HIDE_ICON = "/images/hide.png";
    protected static final String SAVE_ICON = "/images/save.png";
    protected static final String COLOR_ICON = "/images/color.png";
    protected static final String RESET_ICON = "/images/reset.png";
    protected static final String PRINT_ICON = "/images/print.png";
    protected static final String CLEAR_ICON = "/images/clear.png";
    protected static final String RUNNING_ICON = "/images/running.png";
    protected static final String STOPPED_ICON = "/images/stopped.png";
    protected static final String SEQERR_ICON = "/images/seqerr.png";
    protected static final String STEP_ICON = "/images/step.png";
    protected static final String THREAD_ICON = "/images/thread.png";
    protected static final String STATS_ICON = "/images/stats.png";
    protected static final String CPM_ICON = "/images/CPM.png";

    // Dialog box default icon file names
    protected static final String INFORMATION_ICON = "/images/information.png";
    protected static final String QUESTION_ICON = "/images/question.png";
    protected static final String WARNING_ICON = "/images/warning.png";
    protected static final String ERROR_ICON = "/images/error.png";

    // File name for the custom crosshair mouse pointer used in the plots
    protected static final String CURSOR_FILE = "/images/crosshair.png";

    // Step and thread of execution plot tool tip hot spot boundary, in pixels
    protected static final int HOTSPOT_SIZE = 3;

    // Width and height of the overrun indicator, in pixels
    protected static final int OVERRUN_INDICATOR_SIZE = 6;

    // Width and height of a thread of execution plot data point, in pixels
    protected static final int THREAD_POINT_SIZE = 4;

    // Number of rows of a table to initially display
    protected static final int INITIAL_VIEWABLE_TABLE_ROWS = 10;

    // Number of characters to word wrap at initially for table Notes column
    protected static final int NOTES_WORD_WRAP = 20;

    // Number of characters to word wrap at for tool tip text
    protected static final int TOOL_TIP_WORD_WRAP = 30;

    // Number of decimal places for displaying time and delta time values
    protected static int TIME_PRECISION = 6;

    // Calculate the minimum time delta displayed in a plot based on the time
    // precision
    protected static double MINIMUM_TIME_DELTA = Math.pow(10.0,
                                                          -TIME_PRECISION);

    // Format for the step and thread of execution plot, and bar chart x-axis
    // tick labels
    protected static String X_AXIS_LABEL_FORMAT = "0.0"
                                                  + new String(new char[TIME_PRECISION]).replace('\0', '#');

    // Output format for time and delta time values
    protected static String TIME_FORMAT = "%." + TIME_PRECISION + "f";

    // Set the plot horizontal (x-axis) and vertical (y-axis) scroll bar
    // fineness multipliers (a higher value allows for finer control of the
    // scroll bar as the plot is zoomed in)
    protected static final int X_SCROLLBAR_MULT = 100000000;
    protected static final int Y_SCROLLBAR_MULT = 10000;

    // Bias, in pixels, to align the statistics and totals tables in the
    // statistics dialog. The value is dependent on the look & feel
    protected static int LAF_BIAS = 0;

    // Used in the ID editor when rendering the desired background color for
    // the Show cell (check boxes). The value is dependent on the look & feel,
    // and if not set correctly the background color may be wrong
    protected static boolean LAF_OPAQUE = true;

    // Width, in pixels, of a vertical scroll bar. This is used when sizing
    // tables in dialogs
    protected static int LAF_SCROLL_BAR_WIDTH = 38;

    // Size of a check box icon (width and height) in pixels. Used to size the
    // color selection check boxes in the Preferences dialog
    protected static int LAF_CHECK_BOX_SIZE = UIManager.getIcon("CheckBox.icon").getIconHeight();

    // Options for handling currently loaded log data and ID list
    protected static enum ClearOption
    {
        CLEAR_ON_APPROVAL,
        CLEAR_WITHOUT_ASKING,
        ALLOW_APPEND
    }

    // Config dialog types - used to determine which config dialog to display
    protected static enum ConfigType
    {
        LOAD,
        SAVE,
        DELETE
    }

    // Mouse cursor modes - used to determine which plot cursor field to update
    protected static enum CursorMode
    {
        START,
        END,
        DELTA
    }

    // Arrow keys focus options - used for determining keyboard traversal
    // actions in response to arrow key presses
    protected static enum ArrowFocusOption
    {
        USE_DEFAULT_HANDLER,
        HANDLE_UP_ARROW,
        HANDLE_DOWN_ARROW,
        HANDLE_UP_AND_DOWN_ARROWS,
        HANDLE_ALL_ARROWS,
        IGNORE_UP_AND_DOWN_ARROWS
    }

    // Command line command types
    protected static enum CmdType
    {
        NAME,
        MINMAX,
        SIZE,
        COLOR,
        ID,
        OPTION
    }

    /**************************************************************************
     * Update the time and delta time value precision and output format
     * 
     * @param precision
     *            number of decimal places to display time values
     *************************************************************************/
    protected static void setTimePrecision(int precision)
    {
        TIME_PRECISION = precision;
        TIME_FORMAT = "%."
                      + String.valueOf(precision)
                      + "f";
    }

    /**************************************************************************
     * Set GUI adjustment(s) based on the selected look & feel
     * 
     * @param lookAndFeel
     *            name of the look & feel in effect
     *************************************************************************/
    protected static void setLaFAdjustments(String lookAndFeel)
    {
        // Based on the selected look & feel, adjust the bias that's needed to
        // align the two tables in the statistics dialog, and set the opaque
        // flag used to adjust the rendering of the check boxes in the ID table
        if (lookAndFeel.equals("Metal"))
        {
            LAF_BIAS = 1;
            LAF_OPAQUE = true;
        }
        else if (lookAndFeel.equals("GTK+"))
        {
            LAF_BIAS = 4;
            LAF_OPAQUE = true;
        }
        else if (lookAndFeel.equals("Nimbus"))
        {
            LAF_BIAS = 3;
            LAF_OPAQUE = false;
        }
        else if (lookAndFeel.equals("Windows"))
        {
            LAF_BIAS = 1;
            LAF_OPAQUE = true;
        }
        else if (lookAndFeel.equals("Windows Classic"))
        {
            LAF_BIAS = 2;
            LAF_OPAQUE = true;
        }
        else if (lookAndFeel.equals("CDE/Motif"))
        {
            LAF_BIAS = 1;
            LAF_OPAQUE = true;
        }

        // Find the width, in pixels, needed to display a vertical scroll bar
        // by creating a dummy scroll bar. Set the scroll bar to null
        // afterwards to free up its memory
        JScrollPane sp = new JScrollPane(null,
                                         ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
                                         ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        LAF_SCROLL_BAR_WIDTH = sp.getPreferredSize().width * 2 + 2;
        sp = null;

        // Determine the height, in pixels, of a check box icon. The correct
        // height is not returned by the UI manager for all look & feels unless
        // the check box is first realized, so an invisible window is created,
        // a check box instantiated, then the window removed. The check box
        // size returned by the UI manager is now correct
        JWindow wndw = new JWindow();
        wndw.add(new JCheckBox());
        wndw.pack();
        wndw.dispose();
        LAF_CHECK_BOX_SIZE = UIManager.getIcon("CheckBox.icon").getIconHeight();
    }
}
