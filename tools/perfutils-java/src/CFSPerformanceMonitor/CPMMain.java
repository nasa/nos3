/**
 * CFS Performance Monitor Main class
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.CPM_AUTHOR;
import static CFSPerformanceMonitor.CPMConstants.CPM_DATE;
import static CFSPerformanceMonitor.CPMConstants.CPM_ICON;
import static CFSPerformanceMonitor.CPMConstants.CPM_VERSION;
import static CFSPerformanceMonitor.CPMConstants.HELP_FILE;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LOG_SEPARATOR;
import static CFSPerformanceMonitor.CPMConstants.LOOK_AND_FEEL;
import static CFSPerformanceMonitor.CPMConstants.MIN_WINDOW_HEIGHT;
import static CFSPerformanceMonitor.CPMConstants.MIN_WINDOW_WIDTH;
import static CFSPerformanceMonitor.CPMConstants.OK_CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.PERF_ID_LIST_FILE;
import static CFSPerformanceMonitor.CPMConstants.PERF_LOG_FILE;
import static CFSPerformanceMonitor.CPMConstants.PLOT_PREFERENCES;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_SOURCE_PATH;
import static CFSPerformanceMonitor.CPMConstants.STATISTICS_PLOT;
import static CFSPerformanceMonitor.CPMConstants.STATS_ICON;
import static CFSPerformanceMonitor.CPMConstants.STEP_ICON;
import static CFSPerformanceMonitor.CPMConstants.STEP_PLOT;
import static CFSPerformanceMonitor.CPMConstants.THREAD_ICON;
import static CFSPerformanceMonitor.CPMConstants.THREAD_PLOT;
import static CFSPerformanceMonitor.CPMConstants.setLaFAdjustments;
import static CFSPerformanceMonitor.CPMConstants.ArrowFocusOption.HANDLE_ALL_ARROWS;
import static CFSPerformanceMonitor.CPMConstants.ArrowFocusOption.HANDLE_DOWN_ARROW;
import static CFSPerformanceMonitor.CPMConstants.ArrowFocusOption.HANDLE_UP_AND_DOWN_ARROWS;
import static CFSPerformanceMonitor.CPMConstants.ArrowFocusOption.HANDLE_UP_ARROW;
import static CFSPerformanceMonitor.CPMConstants.ArrowFocusOption.IGNORE_UP_AND_DOWN_ARROWS;
import static CFSPerformanceMonitor.CPMConstants.ArrowFocusOption.USE_DEFAULT_HANDLER;
import static CFSPerformanceMonitor.CPMConstants.ClearOption.ALLOW_APPEND;
import static CFSPerformanceMonitor.CPMConstants.ClearOption.CLEAR_ON_APPROVAL;
import static CFSPerformanceMonitor.CPMConstants.ClearOption.CLEAR_WITHOUT_ASKING;

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.KeyEventDispatcher;
import java.awt.KeyboardFocusManager;
import java.awt.LayoutManager;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import javax.imageio.ImageIO;
import javax.swing.AbstractButton;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;
import javax.swing.JViewport;
import javax.swing.SwingUtilities;
import javax.swing.ToolTipManager;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import CFSPerformanceMonitor.CPMConstants.ArrowFocusOption;
import CFSPerformanceMonitor.CPMConstants.ClearOption;
import CFSPerformanceMonitor.CPMConstants.ConfigType;

/******************************************************************************
 * CFS Performance Monitor main class
 *****************************************************************************/
public class CPMMain
{
    // Create and initialize the user-adjustable preferences for plots
    private final CPMPreferencesHandler plotPrefs;

    // Create and instantiate the performance log handler
    private final CPMIDHandler perfIDs;

    // Create and instantiate the performance ID handler
    private final CPMLogHandler perfLog;

    // Create the references to the dialogs and plots
    private CPMLogContentDialog logContent;
    private CPMLogStatisticsDialog logStatistics;
    private CPMStepPlot stepPlot;
    private CPMThreadPlot threadPlot;
    private CPMStatisticsPlot statisticsPlot;

    // Program preferences backing store node
    private final Preferences progPrefs;

    // Components that need to be accessed by other classes or listeners
    private JFrame frameCPM;
    private JTabbedPane tabbedPane;
    private JPanel stepPlotPanel;
    private JPanel threadPlotPanel;
    private JPanel statsPlotPanel;

    // Menu items that need to be accessed by multiple methods
    private JMenuItem mntmLoadConfig;
    private JMenuItem mntmDeleteConfig;
    private JMenuItem mntmSaveConfig;
    private JMenuItem mntmExit;
    private JMenuItem mntmShowHeader;
    private JMenuItem mntmShowData;
    private JMenuItem mntmShowStats;
    private JMenuItem mntmShowCount;
    private JMenuItem mntmSaveIDsToFile;
    private JMenuItem mntmClearIDList;
    private JMenuItem mntmStepPlot;
    private JMenuItem mntmThreadPlot;
    private JMenu mnStatisticsPlot;
    private JMenuItem mntmSetBounds;

    // Look and feel currently selected by the user
    private String selectedLaF;

    // Config currently selected by the user from the program preferences
    // backing store
    private String selectedConfig;

    // Window event handler for non-modal dialogs
    private final NonModalDialogHandler nonModalDialogHandler;

    /**************************************************************************
     * Window event listener for non-modal dialogs. When the dialog window is
     * closed set the dialog pointer to null so that a new dialog will be
     * created if the menu command to open the dialog is issued
     *************************************************************************/
    private class NonModalDialogHandler extends WindowAdapter
    {
        /**********************************************************************
         * Handle a window closed event
         *********************************************************************/
        @Override
        public void windowClosed(WindowEvent we)
        {
            // Set to null to indicate that the dialog is closed
            we.setSource(null);
        }
    }

    /**************************************************************************
     * Special JPanel class for the plot panels; displays the CPM logo on the
     * panel until a plot is displayed
     *************************************************************************/
    @SuppressWarnings("serial")
    private class ImagePanel extends JPanel
    {
        // CPM logo graphic
        private BufferedImage image;

        /**********************************************************************
         * Handle a window closed event
         *********************************************************************/
        protected ImagePanel(LayoutManager layout)
        {
            // Set the panel's layout manager
            setLayout(layout);

            image = null;

            try
            {
                // Create the CPM logo graphic
                image = ImageIO.read(getClass().getResource(CPM_ICON));
            }
            catch (IOException ioe)
            {
                // Ignore the error if the logo cannot be created
            }
        }

        /**********************************************************************
         * Handle a paint component event
         *********************************************************************/
        @Override
        protected void paintComponent(Graphics g)
        {
            super.paintComponent(g);

            // Check that the logo image was successfully loaded
            if (image != null)
            {
                // Make the logo image semi-transparent
                Graphics2D g2 = (Graphics2D) g;
                g2.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER,
                                                           0.15f));

                // Display the CPM logo, centered in the panel
                g2.drawImage(image,
                             (getWidth() - image.getWidth()) / 2,
                             (getHeight() - image.getHeight()) / 2,
                             null);
            }
        }
    };

    /**************************************************************************
     * Create the application
     *************************************************************************/
    private CPMMain(String[] args)
    {
        plotPrefs = new CPMPreferencesHandler();
        perfIDs = new CPMIDHandler();
        perfLog = new CPMLogHandler();

        // Get the backing store node for storing the program preference keys
        // and values. These are stored by user so that different users can
        // have their own preferences
        progPrefs = Preferences.userNodeForPackage(this.getClass());

        logContent = null;
        logStatistics = null;
        stepPlot = null;
        threadPlot = null;
        statisticsPlot = null;

        selectedConfig = "";

        nonModalDialogHandler = new NonModalDialogHandler();

        // Build the main window
        initialize();

        // Adjust the response to the Enter key to act like the Space key to
        // activate certain control types. Adjust the response to the arrow
        // keys based on the component with the keyboard focus
        setKeyboardHandler();

        // Set the variables used globally within these classes. This can't be
        // done in the respective constructors since each class requires the
        // reference to the other class (i.e., the second class doesn't exist
        // yet when the first class is instantiated)
        perfLog.setGlobals(perfIDs, this, plotPrefs);
        perfIDs.setGlobals(perfLog, this, plotPrefs);

        // Get the program preferences
        getProgramPreferences();

        // Read the command line arguments and make adjustments as needed
        CPMCommandLineHandler perfCmd = new CPMCommandLineHandler(args,
                                                                  plotPrefs);

        // Check if a main window size was specified
        if (perfCmd.getMainSize() != null)
        {
            // Set the main window size
            frameCPM.setSize(perfCmd.getMainSize());
        }

        // Set the initially selected plot tab
        tabbedPane.setSelectedIndex(perfCmd.getPlotType());

        // Set the selected look & feel using the one provided on the command
        // line (if present)
        setLookAndFeel(perfCmd.getLaF());

        // Make the main application window visible
        frameCPM.setVisible(true);

        // Force tool tip pop-ups to effectively remain visible until the mouse
        // pointer moves away from the object
        ToolTipManager.sharedInstance().setDismissDelay(Integer.MAX_VALUE);

        // Read in the log and ID files, then update the plots and dialogs
        readPerfFiles(perfCmd.getLogFile(), perfCmd.getIDFile());
    }

    /**************************************************************************
     * Launch the application
     * 
     * @param args
     *            array of command line arguments
     *************************************************************************/
    public static void main(final String[] args)
    {
        EventQueue.invokeLater(new Runnable()
        {
            /******************************************************************
             * Execute the main class
             *****************************************************************/
            @Override
            public void run()
            {
                try
                {
                    new CPMMain(args);
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
            }
        });
    }

    /**************************************************************************
     * Get the main application frame
     * 
     * @return Main application frame
     *************************************************************************/
    protected JFrame getMainFrame()
    {
        return frameCPM;
    }

    /**************************************************************************
     * Get the step plot panel
     * 
     * @return Step plot panel
     *************************************************************************/
    protected JPanel getStepPlotPanel()
    {
        return stepPlotPanel;
    }

    /**************************************************************************
     * Get the thread of execution plot panel
     * 
     * @return Thread of execution plot panel
     *************************************************************************/
    protected JPanel getThreadPlotPanel()
    {
        return threadPlotPanel;
    }

    /**************************************************************************
     * Get the statistics plot panel
     * 
     * @return Statistics plot panel
     *************************************************************************/
    protected JPanel getStatsPlotPanel()
    {
        return statsPlotPanel;
    }

    /**************************************************************************
     * Set the mouse pointer for the main application window
     * 
     * @param cursor
     *            application default mouse pointer
     *************************************************************************/
    protected void setMainCursor(int cursor)
    {
        frameCPM.setCursor(Cursor.getPredefinedCursor(cursor));
    }

    /**************************************************************************
     * Activate/deactivate the main menu and plot tabs by setting the component
     * enable flags appropriately. While disabled these components are grayed
     * out and do not respond to inputs
     *************************************************************************/
    protected void setGUIActivated(boolean activate)
    {
        // Step through the menu bar items
        for (int index = 0; index < frameCPM.getJMenuBar().getComponentCount(); index++)
        {
            // Enable/disable the item based on the input flag
            frameCPM.getJMenuBar().getMenu(index).setEnabled(activate);
        }

        // Enable/disable the plot tabbed pane based on the input flag
        tabbedPane.setEnabled(activate);
    }

    /**************************************************************************
     * Initialize the contents of the main application frame
     *************************************************************************/
    private void initialize()
    {
        // Create the main application frame and set its characteristics
        frameCPM = new JFrame();
        frameCPM.setTitle("CFS Performance Monitor  " + CPM_VERSION);
        frameCPM.setBounds(100, 100, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
        frameCPM.setMinimumSize(new Dimension(MIN_WINDOW_WIDTH,
                                              MIN_WINDOW_HEIGHT));

        // Set the default close operation so that the main window frame's
        // close button doesn't automatically exit the program. Instead, if
        // this close button is pressed a 'click' event is sent to the File |
        // Exit command
        frameCPM.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

        // Add a listener for main window events
        frameCPM.addWindowListener(new WindowAdapter()
        {
            /******************************************************************
             * Handle the main window frame close button press event
             *****************************************************************/
            @Override
            public void windowClosing(WindowEvent we)
            {
                // Send a 'click' event for the main window frame's File | Exit
                // menu item. This allows the frame's close button to perform
                // the same actions as the File | Exit command; i.e., asks the
                // user to confirm exiting the program
                mntmExit.doClick();
            }
        });

        // Create panels to contain the plots
        stepPlotPanel = new ImagePanel(new GridLayout(0, 1, 0, 0));
        threadPlotPanel = new ImagePanel(new GridLayout(0, 1, 0, 0));
        statsPlotPanel = new ImagePanel(new GridLayout(0, 1, 0, 0));

        // Create a tabbed pane for the plots to appear in
        tabbedPane = new JTabbedPane(JTabbedPane.TOP);
        tabbedPane.setFont(LABEL_FONT_PLAIN);

        // Create a tab for each plot type
        tabbedPane.addTab("Step",
                          new ImageIcon(getClass().getResource(STEP_ICON)),
                          stepPlotPanel,
                          "Step plot");
        tabbedPane.addTab("Thread",
                          new ImageIcon(getClass().getResource(THREAD_ICON)),
                          threadPlotPanel,
                          "Thread of execution plot");
        tabbedPane.addTab("Statistics",
                          new ImageIcon(getClass().getResource(STATS_ICON)),
                          statsPlotPanel,
                          "Statistics plot");
        tabbedPane.setMnemonicAt(0, 'S');
        tabbedPane.setMnemonicAt(1, 'T');
        tabbedPane.setMnemonicAt(2, 'A');

        // Listen for tab selection changes
        tabbedPane.addChangeListener(new ChangeListener()
        {
            /******************************************************************
             * Display the plot associated with the selected tab
             *****************************************************************/
            @Override
            public void stateChanged(ChangeEvent ce)
            {
                // Show the plot determined by which tab is selected
                showSelectedPlot();
            }
        });

        frameCPM.getContentPane().add(tabbedPane);

        // Create the main menu bar
        JMenuBar menuBar = new JMenuBar();
        frameCPM.setJMenuBar(menuBar);

        // Create the File menu and menu items
        JMenu mnFile = new JMenu("File");
        mntmLoadConfig = new JMenuItem("Load Config");
        mntmDeleteConfig = new JMenuItem("Delete Config");
        mntmSaveConfig = new JMenuItem("Save Config");
        mntmExit = new JMenuItem("Exit");
        JMenuItem mntmAppearance = new JMenuItem("Appearance");
        mnFile.setFont(LABEL_FONT_PLAIN);
        mntmLoadConfig.setFont(LABEL_FONT_PLAIN);
        mntmSaveConfig.setFont(LABEL_FONT_PLAIN);
        mntmDeleteConfig.setFont(LABEL_FONT_PLAIN);
        mntmAppearance.setFont(LABEL_FONT_PLAIN);
        mntmExit.setFont(LABEL_FONT_PLAIN);
        mnFile.setMnemonic('F');
        mntmLoadConfig.setMnemonic('L');
        mntmSaveConfig.setMnemonic('S');
        mntmDeleteConfig.setMnemonic('D');
        mntmAppearance.setMnemonic('A');
        mntmExit.setMnemonic('X');
        menuBar.add(mnFile);
        mnFile.add(mntmLoadConfig);
        mnFile.add(mntmSaveConfig);
        mnFile.add(mntmDeleteConfig);
        mnFile.addSeparator();
        mnFile.add(mntmAppearance);
        mnFile.add(mntmExit);

        // Create the Log menu and menu items
        JMenu mnLog = new JMenu("Log");
        JMenuItem mntmReadLog = new JMenuItem("Read Log");
        mntmShowHeader = new JMenuItem("Show Header");
        mntmShowData = new JMenuItem("Show Data");
        mntmShowStats = new JMenuItem("Show Statistics");
        mntmShowCount = new JMenuItem("Show ID Count");
        mnLog.setFont(LABEL_FONT_PLAIN);
        mntmReadLog.setFont(LABEL_FONT_PLAIN);
        mntmShowHeader.setFont(LABEL_FONT_PLAIN);
        mntmShowData.setFont(LABEL_FONT_PLAIN);
        mntmShowStats.setFont(LABEL_FONT_PLAIN);
        mntmShowCount.setFont(LABEL_FONT_PLAIN);
        mnLog.setMnemonic('L');
        mntmReadLog.setMnemonic('R');
        mntmShowHeader.setMnemonic('H');
        mntmShowHeader.setDisplayedMnemonicIndex(5);
        mntmShowData.setMnemonic('D');
        mntmShowStats.setMnemonic('S');
        mntmShowStats.setDisplayedMnemonicIndex(5);
        mntmShowCount.setMnemonic('C');
        menuBar.add(mnLog);
        mnLog.add(mntmReadLog);
        mnLog.add(mntmShowHeader);
        mnLog.add(mntmShowData);
        mnLog.add(mntmShowStats);
        mnLog.add(mntmShowCount);

        // Create the ID menu and menu items
        JMenu mnIDs = new JMenu("IDs");
        JMenuItem mntmLoadIDsFromFile = new JMenuItem("Load from File");
        mntmSaveIDsToFile = new JMenuItem("Save to File");
        JMenuItem mntmEditIDList = new JMenuItem("Edit List");
        mntmClearIDList = new JMenuItem("Clear List");
        JMenuItem mntmSearchSource = new JMenuItem("Search Source");
        mnIDs.setFont(LABEL_FONT_PLAIN);
        mntmLoadIDsFromFile.setFont(LABEL_FONT_PLAIN);
        mntmSaveIDsToFile.setFont(LABEL_FONT_PLAIN);
        mntmEditIDList.setFont(LABEL_FONT_PLAIN);
        mntmClearIDList.setFont(LABEL_FONT_PLAIN);
        mntmSearchSource.setFont(LABEL_FONT_PLAIN);
        mnIDs.setMnemonic('I');
        mntmLoadIDsFromFile.setMnemonic('L');
        mntmSaveIDsToFile.setMnemonic('S');
        mntmEditIDList.setMnemonic('E');
        mntmClearIDList.setMnemonic('C');
        mntmSearchSource.setMnemonic('C');
        mntmSearchSource.setDisplayedMnemonicIndex(7);
        menuBar.add(mnIDs);
        mnIDs.add(mntmLoadIDsFromFile);
        mnIDs.add(mntmSaveIDsToFile);
        mnIDs.add(mntmEditIDList);
        mnIDs.add(mntmClearIDList);
        mnIDs.add(mntmSearchSource);

        // Create the Plot menu and menu items
        JMenu mnPlot = new JMenu("Plot");
        mntmStepPlot = new JMenuItem("Step");
        mntmThreadPlot = new JMenuItem("Thread");
        mnStatisticsPlot = new JMenu("Statistics");
        JMenuItem mntmPlotPreferences = new JMenuItem("Preferences");
        mntmSetBounds = new JMenuItem("Set Bounds");
        mnPlot.setFont(LABEL_FONT_PLAIN);
        mntmStepPlot.setFont(LABEL_FONT_PLAIN);
        mntmThreadPlot.setFont(LABEL_FONT_PLAIN);
        mnStatisticsPlot.setFont(LABEL_FONT_PLAIN);
        mntmPlotPreferences.setFont(LABEL_FONT_PLAIN);
        mntmSetBounds.setFont(LABEL_FONT_PLAIN);
        mnPlot.setMnemonic('P');
        mntmStepPlot.setMnemonic('S');
        mntmThreadPlot.setMnemonic('T');
        mnStatisticsPlot.setMnemonic('A');
        mntmPlotPreferences.setMnemonic('P');
        mntmSetBounds.setMnemonic('B');
        menuBar.add(mnPlot);
        mnPlot.add(mntmStepPlot);
        mnPlot.add(mntmThreadPlot);
        mnPlot.add(mnStatisticsPlot);
        mnPlot.addSeparator();
        mnPlot.add(mntmPlotPreferences);
        mnPlot.add(mntmSetBounds);

        // Create the Statistics sub-menu items
        JMenuItem[] mntmStatsPlot = new JMenuItem[CPMStatisticsPlot.NUM_PLOTS];

        // For each statistics plot
        for (int index = 0; index < CPMStatisticsPlot.NUM_PLOTS; index++)
        {
            mntmStatsPlot[index] = new JMenuItem(CPMStatisticsPlot.PLOT_NAMES[index]);
            mntmStatsPlot[index].setFont(LABEL_FONT_PLAIN);
            mnStatisticsPlot.add(mntmStatsPlot[index]);
        }

        // Create the Help menu and menu items
        JMenu mnHelp = new JMenu("Help");
        JMenuItem mntmGuide = new JMenuItem("Guide");
        JMenuItem mntmAbout = new JMenuItem("About");
        mnHelp.setFont(LABEL_FONT_PLAIN);
        mntmGuide.setFont(LABEL_FONT_PLAIN);
        mntmAbout.setFont(LABEL_FONT_PLAIN);
        mnHelp.setMnemonic('H');
        mntmGuide.setMnemonic('G');
        mntmAbout.setMnemonic('A');
        menuBar.add(mnHelp);
        mnHelp.add(mntmGuide);
        mnHelp.add(mntmAbout);

        // Disable invalid menu items initially
        mntmLoadConfig.setEnabled(false);
        mntmSaveConfig.setEnabled(false);
        mntmDeleteConfig.setEnabled(false);
        mntmShowHeader.setEnabled(false);
        mntmShowData.setEnabled(false);
        mntmShowStats.setEnabled(false);
        mntmShowCount.setEnabled(false);
        mntmSaveIDsToFile.setEnabled(false);
        mntmClearIDList.setEnabled(false);
        mntmStepPlot.setEnabled(false);
        mntmThreadPlot.setEnabled(false);
        mnStatisticsPlot.setEnabled(false);
        mntmSetBounds.setEnabled(false);

        // Add a listener for the Load Config menu item
        mntmLoadConfig.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Load in a saved configuration from the preferences storage
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Display the Load Config dialog
                CPMConfigDialog configDialog = new CPMConfigDialog(CPMMain.this,
                                                                   ConfigType.LOAD);

                // Check if the Okay button was selected
                if (configDialog.getButtonSelected() == OK_OPTION)
                {
                    // Parse the file names from the chosen configuration
                    // retrieved from the backing store
                    String[] parts = progPrefs.get("CONFIG"
                                                   + selectedConfig,
                                                   "").split(",");

                    // Read in the log and ID files, then update the plots and
                    // dialogs
                    readPerfFiles(parts[1], parts[0]);
                }
            }
        });

        // Add a listener for the Save Config menu item
        mntmSaveConfig.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Save the current configuration in the preferences storage
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Display the Save Config dialog
                CPMConfigDialog configDialog = new CPMConfigDialog(CPMMain.this,
                                                                   ConfigType.SAVE);

                // Check if the Okay button was selected
                if (configDialog.getButtonSelected() == OK_OPTION)
                {
                    // Write the config, using the current log and ID file
                    // names, to the program preferences backing store
                    progPrefs.put("CONFIG" + selectedConfig,
                                  progPrefs.get(PERF_ID_LIST_FILE, "")
                                      + ","
                                      + progPrefs.get(PERF_LOG_FILE, ""));

                    // Enable the config menu items
                    setConfigMenuItems();
                }
            }
        });

        // Add a listener for the Delete Config menu item
        mntmDeleteConfig.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Delete the selected configuration from the preferences storage
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Display the Delete Config dialog
                CPMConfigDialog configDialog = new CPMConfigDialog(CPMMain.this,
                                                                   ConfigType.DELETE);

                // Check if the Okay button was selected
                if (configDialog.getButtonSelected() == OK_OPTION)
                {
                    // Remove the selected config from the program preferences
                    // backing store
                    progPrefs.remove("CONFIG" + selectedConfig);

                    // Erase the selected config text since it was deleted
                    selectedConfig = "";

                    // Disable the config menu items if the last one is deleted
                    setConfigMenuItems();
                }
            }
        });

        // Add a listener for the program Appearance menu item
        mntmAppearance.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Show the program appearance dialog
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                new CPMAppearanceDialog(CPMMain.this);

                // Store the selected look & feel in the program preferences
                // backing store
                progPrefs.put(LOOK_AND_FEEL, selectedLaF);
            }
        });

        // Add a listener for the Exit menu item
        mntmExit.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Exit the program
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Ask the user to verify if exiting is okay
                if (new CPMDialogHandler().showMessageDialog(frameCPM,
                                                             "<html><b>Exit application?",
                                                             "Exit CPM",
                                                             JOptionPane.QUESTION_MESSAGE,
                                                             OK_CANCEL_OPTION) == OK_OPTION)
                {
                    // Exit the program
                    System.exit(0);
                }
            }
        });

        // Add a listener for the Read Log menu item
        mntmReadLog.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Select a performance log and read the contents
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set up a dummy, null file name list so that the user will
                // get a dialog from which to choose one or more log files
                String[] fileName = {null};

                // Read the contents of the log file(s) selected by the user
                if (readPerfLogFile(fileName, ALLOW_APPEND))
                {
                    // Update the plots and/or the log content dialog
                    updatePlotsAndDialogs();
                }
            }
        });

        // Add a listener for the Show Header menu item
        mntmShowHeader.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display performance log header information
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                new CPMLogHeaderDialog(perfLog, CPMMain.this);
            }
        });

        // Add a listener for the Show Data menu item
        mntmShowData.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display the performance log contents
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Display the log content dialog if not already open
                if (logContent == null
                    || !logContent.isVisible())
                {
                    // Create the log content dialog
                    logContent = new CPMLogContentDialog(perfLog,
                                                         perfIDs,
                                                         CPMMain.this,
                                                         plotPrefs);

                    // Delete the log content dialog on a window close event
                    logContent.addWindowListener(nonModalDialogHandler);
                }
                // The log content dialog is already open
                else
                {
                    // Bring the log content dialog to the foreground
                    logContent.toFront();
                }
            }
        });

        mntmShowStats.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display the performance ID statistics dialog
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Display the ID statistics dialog if not already open
                if (logStatistics == null
                    || !logStatistics.isVisible())
                {
                    // Create the log statistics dialog
                    logStatistics = new CPMLogStatisticsDialog(perfLog,
                                                               perfIDs,
                                                               CPMMain.this);

                    // Delete the ID statistics dialog on a window close event
                    logStatistics.addWindowListener(nonModalDialogHandler);
                }
                // The ID statistics dialog is already open
                else
                {
                    // Bring the ID statistics dialog to the foreground
                    logStatistics.toFront();
                }
            }
        });

        // Add a listener for the Show ID Count menu item
        mntmShowCount.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display performance log ID count
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                perfLog.countIDsInLog();
            }
        });

        // Add a listener for the Load IDs from File menu item
        mntmLoadIDsFromFile.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Load the list of performance IDs from a previously saved file
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Ask the user for an ID file and check if it's valid
                if (readPerfIDFile(null, ALLOW_APPEND))
                {
                    // Update the plots and/or the log content dialog
                    updatePlotsAndDialogs();
                }
            }
        });

        // Add a listener for the Save IDs to File menu item
        mntmSaveIDsToFile.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Save the list of performance IDs to a file for later reuse
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                String fileName;

                if ((fileName = perfIDs.saveIDs()) != null)
                {
                    // Store the ID list file name in the program preferences
                    // backing store
                    progPrefs.put(PERF_ID_LIST_FILE, fileName);

                    // Check if log data has been read
                    if (perfLog.isLogRead())
                    {
                        // A valid log file and a valid ID file are loaded;
                        // enable the Save Config menu item
                        mntmSaveConfig.setEnabled(true);
                    }
                }
            }
        });

        // Add a listener for the Edit ID List menu item
        mntmEditIDList.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Allow user to edit the list of performance IDs
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Create the ID editor and allow the user to edit the ID list
                new CPMIDEditorDialog(perfLog,
                                      perfIDs,
                                      CPMMain.this);

                // Set the flag to true if there are any IDs in the list
                boolean enableItem = !perfIDs.getPerfIDList().isEmpty();

                // Enable the menu items that rely on having IDs if any IDs
                // exist; otherwise disable these menu items
                mntmSaveIDsToFile.setEnabled(enableItem);
                mntmClearIDList.setEnabled(enableItem);
            }
        });

        // Add a listener for the Clear ID List menu item
        mntmClearIDList.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Delete the list of performance IDs
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Check if the user confirmed deleting the ID list
                if (perfIDs.clearPerformanceIDList(CLEAR_ON_APPROVAL))
                {
                    // Initialize the performance IDs and map these to the log
                    perfIDs.mapLogToIDList(false, false);

                    // Update the plots and/or the log content dialog
                    updatePlotsAndDialogs();

                    // Disable the Save IDs, Clear IDs, Save Config, and Set
                    // Frames menu items
                    mntmSaveIDsToFile.setEnabled(false);
                    mntmClearIDList.setEnabled(false);
                    mntmSaveConfig.setEnabled(false);
                }
            }
        });

        // Add a listener for the Search Source menu item
        mntmSearchSource.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Get the list of performance IDs from the source code
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                perfIDs.findPerformanceIDs();
            }
        });

        // Add a listener for the Step plot menu item
        mntmStepPlot.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display the performance log step plot
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                showStepPlot();
            }
        });

        // Add a listener for the Thread plot menu item
        mntmThreadPlot.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display the thread of execution plot
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                showThreadPlot();
            }
        });

        // For each of the Statistics plot menu items
        for (int index = 0; index < CPMStatisticsPlot.NUM_PLOTS; index++)
        {
            // Must create this index as final so that it can be used in the
            // listener below
            final int plotIndex = index;

            // Add a listener for the Statistics plot menu item
            mntmStatsPlot[index].addActionListener(new ActionListener()
            {
                /**************************************************************
                 * Display the statistics plot
                 *************************************************************/
                @Override
                public void actionPerformed(ActionEvent ae)
                {
                    // Set the selected plot index. This is used when creating
                    // or updating the statistics plot
                    CPMStatisticsPlot.setSelectedPlot(plotIndex);
                    showStatisticsPlot();
                }
            });
        }

        // Add a listener for the Preferences menu item
        mntmPlotPreferences.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display the plot Preferences dialog
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Display the plot preferences dialog
                new CPMPreferencesDialog(perfLog,
                                         perfIDs,
                                         stepPlot,
                                         threadPlot,
                                         statisticsPlot,
                                         CPMMain.this,
                                         plotPrefs);

                // Store the plot preferences in the program preferences
                // backing store
                progPrefs.put(PLOT_PREFERENCES, plotPrefs.getPreferences());
            }
        });

        // Add a listener for the Set Bounds menu item
        mntmSetBounds.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display the plot Set Bounds dialog
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                if (stepPlotPanel.isVisible())
                {
                    // Display the plot set bounds dialog
                    new CPMSetBoundsDialog(stepPlot, CPMMain.this);
                }
                else if (threadPlotPanel.isVisible())
                {
                    // Display the plot set bounds dialog
                    new CPMSetBoundsDialog(threadPlot, CPMMain.this);
                }
            }
        });

        // Add a listener for the Guide menu item
        mntmGuide.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Extract the user's guide from the .jar file and display it
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                String errorType = "";

                // Extract the help file name from it .jar package name
                String fileName = HELP_FILE.substring(HELP_FILE.lastIndexOf("/") + 1);

                // Check if the Desktop class is supporting by the platform
                if (Desktop.isDesktopSupported())
                {
                    try
                    {
                        // Create a file with the same name as the help file in
                        // the temporary folder
                        File file = new File(System.getProperty("java.io.tmpdir"),
                                             fileName);

                        // Check if the user's guide hasn't already been
                        // extracted from the .jar file
                        if (!file.exists())
                        {
                            // Display the wait cursor while the file is
                            // extracted
                            setMainCursor(Cursor.WAIT_CURSOR);

                            // Delete the extracted file when the application
                            // exits
                            file.deleteOnExit();

                            // Open an input stream using the user's guide
                            // within the .jar file and create an output stream
                            // to write the contents to
                            InputStream is = getClass().getResourceAsStream(HELP_FILE);
                            FileOutputStream fos = new FileOutputStream(file);

                            // Create a buffer to read the guide. The larger
                            // the buffer the faster the copy speed
                            byte[] data = new byte[2048];

                            // Read in the first part of the file
                            int bytesRead = is.read(data);

                            // Continue to read as long as there are bytes in
                            // the input file
                            while (bytesRead != -1)
                            {
                                // Write the data to the output file and read
                                // in the next portion
                                fos.write(data, 0, bytesRead);
                                bytesRead = is.read(data);
                            }

                            // Close the input and output files
                            is.close();
                            fos.close();

                            // Restore the default cursor
                            setMainCursor(Cursor.DEFAULT_CURSOR);
                        }

                        // Check if the help file exists
                        if (file.exists())
                        {
                            // Display the help file
                            Desktop.getDesktop().open(file);
                        }
                        // Help file cannot be found
                        else
                        {
                            // Set the error type message
                            errorType = "missing";
                        }
                    }
                    catch (Exception e)
                    {
                        // The help file type has no supporting application to
                        // open it or an IO error occurred; set the error type
                        // message
                        errorType = "no application registered to open ."
                                    + HELP_FILE.split("\\.")[1]
                                    + " files";
                    }
                }
                // The Desktop class isn't supported
                else
                {
                    // Set the error type message
                    errorType = "Desktop class unsupported";
                }

                // Check if an error occurred opening the help file
                if (!errorType.isEmpty())
                {
                    // Inform the user that the help file can't be opened
                    new CPMDialogHandler().showMessageDialog(frameCPM,
                                                             "<html><b>Help file "
                                                                 + fileName
                                                                 + " cannot be opened;<br>"
                                                                 + errorType,
                                                             "File Error",
                                                             JOptionPane.ERROR_MESSAGE,
                                                             OK_OPTION);
                }
            }
        });

        // Add a listener for the About menu item
        mntmAbout.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Display the help About dialog
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Create the icon to display in the dialog by scaling the CPM
                // logo
                int iconWidth = 110;
                ImageIcon icon = new ImageIcon(getClass().getResource(CPM_ICON));
                Image image = icon.getImage().getScaledInstance(iconWidth,
                                                                icon.getIconHeight()
                                                                    * iconWidth
                                                                    / icon.getIconWidth(),
                                                                Image.SCALE_SMOOTH);
                icon = new ImageIcon(image);

                // Display the application name, author, and version
                new CPMDialogHandler().showMessageDialog(frameCPM,
                                                         "<html><b>CFS Performance Monitor</b><br>"
                                                             + CPM_AUTHOR
                                                             + "<br>Version "
                                                             + CPM_VERSION
                                                             + "&#160&#160&#160"
                                                             + CPM_DATE,
                                                         "About CPM",
                                                         OK_OPTION,
                                                         icon);
            }
        });
    }

    /**************************************************************************
     * Perform the steps needed following a search of the source code for
     * performance IDs
     * 
     * @param pathName
     *            path name to the source code selected by the user to search
     *************************************************************************/
    protected void findIDsCleanup(String pathName)
    {
        // Check if the user provided a valid path to the source code and IDs
        // were found
        if (pathName != null)
        {
            // Store the source code path and search criteria in the program
            // preferences backing store
            progPrefs.put(SEARCH_SOURCE_PATH, pathName);

            // Update the plots and/or the log content dialog
            updatePlotsAndDialogs();

            // Enable the Save IDs and Clear IDs menu items
            mntmSaveIDsToFile.setEnabled(true);
            mntmClearIDList.setEnabled(true);
        }
    }

    /**************************************************************************
     * Display the performance ID step plot
     *************************************************************************/
    private void showStepPlot()
    {
        tabbedPane.setSelectedIndex(STEP_PLOT);

        // Check if the step plot hasn't been created
        if (stepPlot == null)
        {
            // Display the performance log plot
            stepPlot = new CPMStepPlot(perfLog,
                                       perfIDs,
                                       CPMMain.this,
                                       plotPrefs);
        }
        // The step plot is already instantiated
        else
        {
            // Restore focus to the plot so that key presses can be detected
            stepPlot.getPlotChartPanel().requestFocusInWindow();
        }

        // Enable the Set Bounds menu item
        mntmSetBounds.setEnabled(true);
    }

    /**************************************************************************
     * Display the thread of execution plot
     *************************************************************************/
    private void showThreadPlot()
    {
        // If the menu command was selected then this ensures that the tab
        // matches the plot
        tabbedPane.setSelectedIndex(THREAD_PLOT);

        // Check if the thread of execution plot hasn't been created
        if (threadPlot == null)
        {
            // Display the thread of execution plot
            threadPlot = new CPMThreadPlot(perfLog,
                                           perfIDs,
                                           CPMMain.this,
                                           plotPrefs);
        }
        // The thread of execution plot is already instantiated
        else
        {
            // Restore focus to the plot so that key presses can be detected
            threadPlot.getPlotChartPanel().requestFocusInWindow();
        }

        // Enable the Set Bounds menu item
        mntmSetBounds.setEnabled(true);
    }

    /**************************************************************************
     * Display the statistics plot
     *************************************************************************/
    private void showStatisticsPlot()
    {
        // If the menu command was selected then this ensures that the tab
        // matches the plot
        tabbedPane.setSelectedIndex(STATISTICS_PLOT);

        // Check if the statistics plot hasn't been created
        if (statisticsPlot == null)
        {
            // Display the statistics plot
            statisticsPlot = new CPMStatisticsPlot(perfLog,
                                                   perfIDs,
                                                   CPMMain.this,
                                                   plotPrefs);
        }
        // The statistics plot already exists
        else
        {
            // If necessary, change the plot to the selected one
            statisticsPlot.selectStatsPlot(CPMStatisticsPlot.getSelectedPlot());
        }

        // Disable the Set Bounds menu item
        mntmSetBounds.setEnabled(false);
    }

    /**************************************************************************
     * Update the plots and/or the log content dialog
     *************************************************************************/
    private void showSelectedPlot()
    {
        // Check that there is log data read in
        if (perfLog.isLogData())
        {
            // Show the plot, determined by which tab is selected
            switch (tabbedPane.getSelectedIndex())
            {
                case STEP_PLOT:
                    showStepPlot();
                    break;

                case THREAD_PLOT:
                    showThreadPlot();
                    break;

                case STATISTICS_PLOT:
                    showStatisticsPlot();
                    break;
            }
        }
    }

    /**************************************************************************
     * Update the plots, log content dialog, and statistics dialog
     *************************************************************************/
    protected void updatePlotsAndDialogs()
    {
        // Check if log data has been read. Without log data there can be no
        // plots or data-related dialogs
        if (perfLog.isLogData())
        {
            // Remove the existing plots, if extant
            stepPlotPanel.removeAll();
            threadPlotPanel.removeAll();
            statsPlotPanel.removeAll();

            // "Delete" the plots so new ones can be created
            stepPlot = null;
            threadPlot = null;
            statisticsPlot = null;

            // Calculate statistics on the logged events
            perfLog.calcEventStatistics();

            // Show the plot, determined by which tab is selected, using the
            // updated data/IDs
            showSelectedPlot();

            // Check if the log content dialog is open
            if (logContent != null)
            {
                // Reload the table with the new log data and reformat the
                // table
                logContent.reloadAndReformatTableData();
            }

            // Check if the ID statistics dialog is open
            if (logStatistics != null)
            {
                // Reload the table with the new statistics data and reformat
                // the table
                logStatistics.reloadAndReformatTableData();
            }

            // Check if an ID list has been loaded from or saved to a file
            if (perfIDs.isSavedIDs())
            {
                // A valid log file and a valid ID file are loaded; enable the
                // Save Config menu item
                mntmSaveConfig.setEnabled(true);
            }
        }
    }

    /**************************************************************************
     * Get the application look & feel
     * 
     * @return name of the selected look & feel
     *************************************************************************/
    protected String getLookAndFeel()
    {
        return selectedLaF;
    }

    /**************************************************************************
     * Set the application look & feel
     * 
     * @param laf
     *            name of the selected look & feel
     * 
     * @return true if an error occurred
     *************************************************************************/
    protected void setLookAndFeel(String laf)
    {
        // Set the default to the cross-platform look & feel
        selectedLaF = "Metal";
        String lafClass = UIManager.getCrossPlatformLookAndFeelClassName();

        // Check if the look & feel was not provided
        if (laf == null)
        {
            // Get the look & feel from the program preferences backing store
            laf = progPrefs.get(LOOK_AND_FEEL, selectedLaF);
        }

        // Step through the look & feels available
        for (LookAndFeelInfo lafInfo : UIManager.getInstalledLookAndFeels())
        {
            // Check if the desired look & feel matches one of the available
            // ones
            if (laf.equals(lafInfo.getName()))
            {
                // Store the selected look & feel
                selectedLaF = laf;
                lafClass = lafInfo.getClassName();
                break;
            }
        }

        try
        {
            // Set the selected look & feel of the GUI
            UIManager.setLookAndFeel(lafClass);

            // Save the selected L&F in the program preferences backing store
            progPrefs.put(LOOK_AND_FEEL, selectedLaF);

            // Set the adjustments to the selected look & feel
            setLaFAdjustments(selectedLaF);

            // Force the application to redraw
            SwingUtilities.updateComponentTreeUI(frameCPM);

            // Check if log data has been read. Without log data there can be
            // no data there can be no plots or data-related dialogs
            if (perfLog.isLogData())
            {
                // Delete the plots so new ones can be created using the new
                // look & feel. This is necessary since changing the L&F causes
                // the components to be resized and repositioned
                if (stepPlot != null)
                {
                    stepPlotPanel.removeAll();
                    stepPlot = null;
                }

                if (threadPlot != null)
                {
                    threadPlotPanel.removeAll();
                    threadPlot = null;
                }

                if (statisticsPlot != null)
                {
                    statsPlotPanel.removeAll();
                    statisticsPlot = null;
                }

                // Redisplay the currently selected plot
                showSelectedPlot();

                // Check if the log content dialog is open
                if (logContent != null)
                {
                    // Update the log content table to the new look & feel
                    SwingUtilities.updateComponentTreeUI(logContent);
                    logContent.setButtonWidth();
                    logContent.validate();
                    logContent.reloadAndReformatTableData();
                }

                // Check if the ID statistics dialog is open
                if (logStatistics != null)
                {
                    // Update the statistics table to the new look & feel. The
                    // property change event is necessary in order for the
                    // table to update after the look & feel change
                    SwingUtilities.updateComponentTreeUI(logStatistics);
                    logStatistics.setButtonWidth();
                    logStatistics.firePropertyChange("resize", 0, 1);
                    logStatistics.validate();
                    logStatistics.reloadAndReformatTableData();
                }
            }
        }
        // Look & feel failed to load
        catch (Exception e)
        {
            // Inform the user that there was an error setting the look & feel
            new CPMDialogHandler().showMessageDialog(frameCPM,
                                                     "<html><b>Problem occurred when setting the look & feel to </b>"
                                                         + selectedLaF,
                                                     "L&F Warning",
                                                     JOptionPane.WARNING_MESSAGE,
                                                     OK_OPTION);
        }
    }

    /**************************************************************************
     * Get the currently selected configuration
     * 
     * @return Selected configuration
     *************************************************************************/
    protected String getSelectedConfig()
    {
        return selectedConfig;
    }

    /**************************************************************************
     * Set the selected configuration
     * 
     * @param config
     *            selected configuration
     *************************************************************************/
    protected void setSelectedConfig(String config)
    {
        selectedConfig = config;
    }

    /**************************************************************************
     * Check if any configs are stored in the program preferences backing store
     * and set the menu items to enabled/disabled accordingly
     *************************************************************************/
    private void setConfigMenuItems()
    {
        boolean foundFlag = false;

        try
        {
            // Check if any configs are stored in the program preferences
            // backing store
            for (String key : progPrefs.keys())
            {
                if (key.startsWith("CONFIG"))
                {
                    // Config found; set the flag and quit searching
                    foundFlag = true;
                    break;
                }
            }
        }
        catch (BackingStoreException bse)
        {
            // Inform the user that an error occurred accessing the preference
            // keys
            new CPMDialogHandler().showMessageDialog(frameCPM,
                                                     "<html><b>Error accessing program preferences",
                                                     "File Warning",
                                                     JOptionPane.WARNING_MESSAGE,
                                                     OK_OPTION);
        }

        // Enable the Load Config and Delete Config menu items
        mntmLoadConfig.setEnabled(foundFlag);
        mntmDeleteConfig.setEnabled(foundFlag);
    }

    /**************************************************************************
     * Get the program preferences backing store node
     * 
     * @return Program preferences backing store node
     *************************************************************************/
    protected Preferences getPreferences()
    {
        return progPrefs;
    }

    /**************************************************************************
     * Get the specified program preference value from the backing store
     * 
     * @param key
     *            program preference key
     * 
     * @return Specified program preference value
     *************************************************************************/
    protected String getPreference(String key)
    {
        return progPrefs.get(key, null);
    }

    /**************************************************************************
     * Get the programs preferences from the backing store
     *************************************************************************/
    private void getProgramPreferences()
    {
        try
        {
            // Attempt to store a test preference value to determine if access
            // to the backing store is available
            progPrefs.putBoolean("PreferenceStorageAvailable", true);
            progPrefs.remove("PreferenceStorageAvailable");
            progPrefs.flush();
        }
        catch (BackingStoreException bse)
        {
            // Inform the user that there the program preferences can't be
            // stored
            new CPMDialogHandler().showMessageDialog(frameCPM,
                                                     "<html><b>Cannot store program preference values",
                                                     "File Warning",
                                                     JOptionPane.WARNING_MESSAGE,
                                                     OK_OPTION);
        }

        // Set the user-selectable plot preferences
        plotPrefs.setPreferences(progPrefs.get(PLOT_PREFERENCES,
                                               plotPrefs.getPreferences()).split(","));

        // Set the config menu items
        setConfigMenuItems();
    }

    /**************************************************************************
     * Read one or more performance logs
     * 
     * @param logNames
     *            array of log path + file names
     * 
     * @param clearOrAppend
     *            CLEAR_ON_APPROVAL to clear the loaded IDs on user approval;
     *            CLEAR_WITHOUT_ASKING to clear the currently loaded IDs
     *            without asking; ALLOW_APPEND to allow appending the new IDs
     *            to the current list
     * 
     * @return true if at least one log file is read successfully
     *************************************************************************/
    private boolean readPerfLogFile(String[] logNames,
                                    ClearOption clearOrAppend)
    {
        // Attempt to read the log file(s)
        logNames = perfLog.readLog(logNames, clearOrAppend);

        // Check that at least one log file was successfully read
        if (perfLog.isLogRead())
        {
            // Save the first log file name
            String logNameString = logNames[0];

            // For any additional file names
            for (int index = 1; index < logNames.length; index++)
            {
                // Append the file name to the string, separated by a delimiter
                logNameString += LOG_SEPARATOR + logNames[index];
            }

            // Store the selected log file name(s) in the program preferences
            progPrefs.put(PERF_LOG_FILE, logNameString);

            // Enable the menu items that are available now that log data has
            // been read
            mntmShowHeader.setEnabled(true);
            mntmShowData.setEnabled(true);
            mntmShowStats.setEnabled(true);
            mntmShowCount.setEnabled(true);
            mntmStepPlot.setEnabled(true);
            mntmThreadPlot.setEnabled(true);
            mnStatisticsPlot.setEnabled(true);

            // Check if a ID file has been read
            if (mntmClearIDList.isEnabled())
            {
                // Enable the Save Config menu item
                mntmSaveConfig.setEnabled(true);
            }
        }

        // Return "true" if the log was successfully read; otherwise false
        return perfLog.isLogRead();
    }

    /**************************************************************************
     * Read a list of performance IDs from a file
     * 
     * @param fileName
     *            ID path + file name
     * 
     * @param clearOrAppend
     *            CLEAR_ON_APPROVAL to clear the loaded IDs on user approval;
     *            CLEAR_WITHOUT_ASKING to clear the currently loaded IDs
     *            without asking; ALLOW_APPEND to allow appending the new IDs
     *            to the current list
     * 
     * @return true if the ID file is read successfully
     *************************************************************************/
    private boolean readPerfIDFile(String fileName, ClearOption clearOrAppend)
    {
        boolean successFlag = false;

        // Attempt to load the IDs from the file
        if ((fileName = perfIDs.loadIDs(fileName, clearOrAppend)) != null)
        {
            // Store the selected ID list file name in the program preferences
            progPrefs.put(PERF_ID_LIST_FILE, fileName);

            // Enable the menu items that are available now that an ID list has
            // been read
            mntmSaveIDsToFile.setEnabled(true);
            mntmClearIDList.setEnabled(true);

            // Check if a log file has been read
            if (mntmShowData.isEnabled())
            {
                // Enable the Save Config menu item
                mntmSaveConfig.setEnabled(true);
            }

            successFlag = true;
        }

        return successFlag;
    }

    /**************************************************************************
     * Read the performance log file(s) and performance ID file
     * 
     * @param logFiles
     *            log path + file names, separated by a delimiter
     * 
     * @param idFile
     *            ID path + file name
     * 
     * @return true if the ID file is read successfully
     *************************************************************************/
    private void readPerfFiles(String logFiles, String idFile)
    {
        boolean idResult = false;
        boolean logResult = false;

        // Check if an ID file was provided
        if (idFile != null)
        {
            // Read the performance ID file, clearing any existing IDs
            idResult = readPerfIDFile(idFile, CLEAR_WITHOUT_ASKING);
        }

        // Check if a log file was provided
        if (logFiles != null)
        {
            // Parse the string of log file names into an array
            String[] logNames = logFiles.split(LOG_SEPARATOR);

            // Read the log file(s), clearing any existing log data
            logResult = readPerfLogFile(logNames, CLEAR_WITHOUT_ASKING);
        }

        // If reading either the ID or log files is successful then the
        // currently visible plot and log content dialog need to be updated
        if (idResult || logResult)
        {
            // Display one of the plots (which one to display depends on which
            // plot tab is selected). Update the log content dialog if visible
            updatePlotsAndDialogs();
        }
    }

    /**************************************************************************
     * Handle Enter and space key press events in a table in order to activate
     * check box and color button controls, and initiate editing for editable
     * fields. The space key would do these without this method; it is included
     * in order to eliminate the cell background color 'flash' that occurs when
     * a check box is toggled
     * 
     * @param table
     *            reference to the table where the key press event occurred
     * 
     * @return true if the key press event is handled by this method; false
     *         otherwise
     *************************************************************************/
    private boolean tableEditCellHandler(CPMTableHandler table)
    {
        boolean handled = false;

        // Get the row and column in the table with the focus
        int row = table.getSelectedRow();
        int column = table.getSelectedColumn();

        // Check that a cell is selected
        if (row != -1 && column != -1)
        {
            // Get the contents of the selected cell
            Object cellObject = table.getValueAt(row, column);

            // Check if this is a check box
            if (table.getCellRenderer(row, column).getClass().getSimpleName().equals("BooleanRenderer"))
            {
                // Invert the value of the check box (true <-> false) and
                // indicate the event has been handled
                table.setValueAt(!((Boolean) cellObject), row, column);
                handled = true;
            }
            // Check if this cell contains a color button, string, or integer
            else if (cellObject instanceof Color
                     || cellObject instanceof String
                     || cellObject instanceof Integer)
            {
                // Initiate editing on the cell and indicate that the event has
                // been handled. Note that this has no effect on non-editable
                // cells
                table.editCellAt(row, column);
                handled = true;
            }
        }
        // No cell is selected; check if the table contains at least one row so
        // that a row can be selected
        else if (table.getRowCount() > 0)
        {
            // Assume the top row will be selected
            row = 0;

            // Get the table's scroll pane viewport
            JViewport vp = table.getViewport();

            // Verify that the table has a scroll pane
            if (vp != null)
            {
                // Set the row to select to the topmost visible one
                row = table.rowAtPoint(vp.getViewPosition());
            }

            // Select the topmost visible row and set the focus to the first
            // column in the selected row
            table.setRowSelectionInterval(row, 0);
            table.requestFocusInWindow();
            table.changeSelection(row, 0, false, false);
        }

        return handled;
    }

    /**************************************************************************
     * Adjust the handling of Enter and space key inputs in order to activate
     * dialog controls, and keyboard focus changes in order to use the arrow
     * keys like Tab keys
     *************************************************************************/
    private void setKeyboardHandler()
    {
        // Get the keyboard focus manager
        final KeyboardFocusManager focusManager = KeyboardFocusManager.getCurrentKeyboardFocusManager();

        // Listen for key presses
        focusManager.addKeyEventDispatcher(new KeyEventDispatcher()
        {
            /******************************************************************
             * Alter the response to the Enter key to act like the Space key to
             * activate button and check box controls, and the arrow keys so as
             * to mimic the Tab and Shift+Tab keys, unless the key press
             * originates within a table or combo box. For a tabbed pane the
             * left/right arrows are left unchanged so that these are used for
             * traversing the tabbed panes, but the down and up arrows act like
             * Tab and Shift+Tab respectively
             *****************************************************************/
            @Override
            public boolean dispatchKeyEvent(KeyEvent ke)
            {
                // Flag that indicates if the key event has been handled by
                // this method (true) or that it still needs to be processed
                // (false)
                boolean handled = false;

                // Check if this is a key press event, and the Ctrl, Shift, or
                // Alt key is not pressed
                if (ke.getID() == KeyEvent.KEY_PRESSED
                    && !ke.isControlDown()
                    && !ke.isShiftDown()
                    && !ke.isAltDown())
                {
                    // Get a reference to the component with the focus in order
                    // to shorten the subsequent calls
                    Component comp = ke.getComponent();

                    // Check if the Enter key is pressed
                    if (ke.getKeyCode() == KeyEvent.VK_ENTER)
                    {
                        // Check if this is a button (including a color button,
                        // radio button, or check box
                        if (comp instanceof JButton
                            || comp instanceof JRadioButton
                            || comp instanceof JCheckBox)
                        {
                            // Activate the control (same as if Space key is
                            // pressed)
                            ((AbstractButton) comp).doClick();
                            handled = true;
                        }
                        // Check if this is a table
                        else if (comp instanceof CPMTableHandler)
                        {
                            // Handle the Enter key in the table
                            handled = tableEditCellHandler((CPMTableHandler) comp);
                        }
                    }
                    // Check if the space key is pressed while in a table
                    else if (ke.getKeyCode() == KeyEvent.VK_SPACE
                             && comp instanceof CPMTableHandler)
                    {
                        // Handle the space key in the table
                        handled = tableEditCellHandler((CPMTableHandler) comp);
                    }
                    // Check if the key pressed is an "action" key (i.e., a key
                    // that doesn't produce a character and is not a modifier;
                    // this covers the arrow keys)
                    else if (ke.isActionKey())
                    {
                        // Assume that the default handling will be used with
                        // an arrow key
                        ArrowFocusOption arrowResponse = USE_DEFAULT_HANDLER;

                        // Check if the focus is on a tabbed pane's tab or on a
                        // slider
                        if (comp instanceof JTabbedPane
                            || comp instanceof JSlider)
                        {
                            // The left and right arrows traverse the tabs, and
                            // the up and down arrows behave like (Shift+)Tab
                            arrowResponse = HANDLE_UP_AND_DOWN_ARROWS;
                        }
                        // Check if the focus is in a text field within a table
                        else if (comp instanceof JTextField
                                 && comp.getParent() instanceof CPMTableHandler)
                        {
                            // The up and down arrows are ignored. This
                            // prevents accidently exiting edit mode on a table
                            // cell and losing any changes
                            arrowResponse = IGNORE_UP_AND_DOWN_ARROWS;
                        }
                        // Check if the focus is on a button (including a color
                        // button), radio button, or check box
                        else if (comp instanceof JButton
                                 || comp instanceof JRadioButton
                                 || comp instanceof JCheckBox)
                        {
                            // The up and left arrow keys behave as Shift+Tab,
                            // and the down and right arrow keys behave as Tab
                            arrowResponse = HANDLE_ALL_ARROWS;
                        }
                        // Check if the focus is within a table
                        else if (comp instanceof CPMTableHandler)
                        {
                            // Get the reference to the table with the focus
                            CPMTableHandler table = (CPMTableHandler) comp;

                            // Get the row containing the cell with the focus
                            // in order to shorten the subsequent calls
                            int row = table.getSelectedRow();

                            // Check if the table has no rows
                            if (row == -1)
                            {
                                // Treat the table as if it wasn't there; i.e.,
                                // the left and right arrows behave like the
                                // (Shift+)Tab key
                                arrowResponse = HANDLE_ALL_ARROWS;
                            }
                            // Check if the table has only a single row
                            else if (table.getRowCount() == 1)
                            {
                                // The up and down arrows behave like the
                                // (Shift+)Tab key
                                arrowResponse = HANDLE_UP_AND_DOWN_ARROWS;
                            }
                            // Check if the top row is selected
                            else if (row == 0)
                            {
                                // The up arrow behaves like the Shift+Tab key
                                arrowResponse = HANDLE_UP_ARROW;
                            }
                            // Check if the bottom row is selected
                            else if (row == table.getRowCount() - 1)
                            {
                                // The down arrow behaves like the Tab key
                                arrowResponse = HANDLE_DOWN_ARROW;
                            }
                        }

                        // Check if the key pressed is an arrow key and if so
                        // adjust its behavior
                        switch (ke.getKeyCode())
                        {
                            case KeyEvent.VK_LEFT:
                            case KeyEvent.VK_KP_LEFT:
                                // Check if the left arrow key should be
                                // handled
                                if (arrowResponse == HANDLE_ALL_ARROWS)
                                {
                                    // Treat the left arrow as a Shift+Tab key
                                    // and indicate that the key has been
                                    // handled
                                    focusManager.focusPreviousComponent();
                                    handled = true;
                                }

                                break;

                            case KeyEvent.VK_UP:
                            case KeyEvent.VK_KP_UP:
                                // Check if the up arrow key should be handled
                                if (arrowResponse == HANDLE_ALL_ARROWS
                                    || arrowResponse == HANDLE_UP_ARROW
                                    || arrowResponse == HANDLE_UP_AND_DOWN_ARROWS)
                                {
                                    // Treat the up arrow as a Shift+Tab key
                                    // and indicate that the key has been
                                    // handled
                                    focusManager.focusPreviousComponent();
                                    handled = true;
                                }
                                // Check if the up arrow should be ignored
                                else if (arrowResponse == IGNORE_UP_AND_DOWN_ARROWS)
                                {
                                    // Indicate that the key has been handled
                                    handled = true;
                                }

                                break;

                            case KeyEvent.VK_RIGHT:
                            case KeyEvent.VK_KP_RIGHT:
                                // Check if the right arrow key should be
                                // handled
                                if (arrowResponse == HANDLE_ALL_ARROWS)
                                {
                                    // Treat the right arrow as a Tab key and
                                    // indicate that the key has been handled
                                    focusManager.focusNextComponent();
                                    handled = true;
                                }

                                break;

                            case KeyEvent.VK_DOWN:
                            case KeyEvent.VK_KP_DOWN:
                                // Check if the down arrow key should be
                                // handled
                                if (arrowResponse == HANDLE_ALL_ARROWS
                                    || arrowResponse == HANDLE_DOWN_ARROW
                                    || arrowResponse == HANDLE_UP_AND_DOWN_ARROWS)
                                {
                                    // Treat the down arrow as a Tab key and
                                    // indicate that the key has been handled
                                    focusManager.focusNextComponent();
                                    handled = true;
                                }
                                // Check if the down arrow should be ignored
                                else if (arrowResponse == IGNORE_UP_AND_DOWN_ARROWS)
                                {
                                    // Indicate that the key has been handled
                                    handled = true;
                                }

                                break;
                        }
                    }
                }

                return handled;
            }
        });
    }
}
