/**
 * CFS Performance Monitor Performance ID Handler
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.CLEAR_OPTION;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_FILE_EXTENSION;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_PERF_ID_SUFFIX;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_SUBFOLDERS;
import static CFSPerformanceMonitor.CPMConstants.ID_COLOR;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LOAD_OPTION;
import static CFSPerformanceMonitor.CPMConstants.NAME_NOT_DEFINED;
import static CFSPerformanceMonitor.CPMConstants.OK_CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.PERF_ID_LIST_FILE;
import static CFSPerformanceMonitor.CPMConstants.SAVE_OPTION;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_FILE_EXTENSION;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_OPTION;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_PERF_ID_SUFFIX;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_SOURCE_PATH;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_SUBFOLDERS;
import static CFSPerformanceMonitor.CPMConstants.ClearOption.ALLOW_APPEND;
import static CFSPerformanceMonitor.CPMConstants.ClearOption.CLEAR_ON_APPROVAL;
import static CFSPerformanceMonitor.CPMConstants.ClearOption.CLEAR_WITHOUT_ASKING;

import java.awt.Color;
import java.awt.Cursor;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.nio.CharBuffer;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JProgressBar;
import javax.swing.SwingWorker;

import CFSPerformanceMonitor.CPMConstants.ClearOption;

/******************************************************************************
 * CFS Performance Monitor performance ID handler class
 *****************************************************************************/
public class CPMIDHandler
{
    private CPMLogHandler perfLog;
    private CPMMain perfMain;
    private CPMPreferencesHandler plotPrefs;

    // List containing the individual performance IDs
    private List<PerformanceID> perfIDList;

    // String to hold the text announcing duplicate IDs were found in the CFS
    // source code
    private String duplicateID;

    // Number of files checked for performance IDs
    private int numFiles;

    // Number of performance IDs already in the ID list
    private int existingIDs;

    // Array index for assigning ID colors
    private int colorIndex;

    // Flag to indicate if IDs have been loaded from or saved to a file
    private boolean hasSavedIDs;

    // ID file parameter indices: The order of the parameters in the ID file is
    // Name, ID (in hexadecimal), color (in hexadecimal), frequency (in hz),
    // and annotation. Parameters are separated by commas
    private final int NAME = 0;
    private final int VALUE = 1;
    private final int COLOR = 2;
    private final int FREQ = 3;
    private final int NOTES = 4;
    private final int NUM_PARAMETERS = 5;

    /**************************************************************************
     * CFS Performance Monitor performance ID class
     *************************************************************************/
    protected class PerformanceID implements Comparable<PerformanceID>
    {
        // Performance ID (with the high bit zeroed)
        private final long perfID;

        // Performance ID name (or ID if no name is defined)
        private String perfIDName;

        // Show ID in plots if true, hide if false
        private boolean plotPerfID;

        // Performance ID plot color
        private Color perfIDColor;

        // Expected ID entry event frequency (events per second)
        private final double perfIDFreq;

        // User-supplied annotation for this ID
        private final String perfIDNotes;

        // Flag used to indicate if this ID has already been accounted for in
        // the current operation
        private Boolean perfIDIsUsed;

        // Number of ID exit events
        private int exitCount;

        // Number of ID entry events
        private int entryCount;

        // Average ID entry events per second
        private Double avgFreq;

        // ID total time active (i.e., between entry and exit events), seconds
        private Double timeActive;

        // ID's minimum active period, seconds
        private Double minTimeActive;

        // ID's maximum active period, seconds
        private Double maxTimeActive;

        // ID's minimum time between entry events, seconds
        private Double minInterval;

        // ID's maximum time between entry events, seconds
        private Double maxInterval;

        // ID's minimum frequency overrun time, seconds
        private Double minOverrun;

        // ID's maximum frequency overrun time, seconds
        private Double maxOverrun;

        // Time stamp for ID's minimum active period
        private Double minTimeActiveTimeStamp;

        // Time stamp for ID's maximum active period
        private Double maxTimeActiveTimeStamp;

        // Time stamp for ID's minimum time between entry events
        private Double minIntervalTimeStamp;

        // Time stamp for ID's maximum time between entry events
        private Double maxIntervalTimeStamp;

        // Time stamp for ID's minimum frequency overrun time
        private Double minOverrunTimeStamp;

        // Time stamp for ID's maximum frequency overrun time
        private Double maxOverrunTimeStamp;

        /**********************************************************************
         * Performance ID constructor
         * 
         * @param perfID
         *            performance ID
         * 
         * @param perdIDName
         *            ID name
         * 
         * @param plotPerfID
         *            true to allow the ID to be displayed on the plots
         * 
         * @param perfIDColor
         *            ID plot color
         * 
         * @param perfIDFreq
         *            frequency at which the ID is expected to be active, entry
         *            events/second
         * 
         * @param perfIDNotes
         *            user-supplied annotation for this ID
         *********************************************************************/
        private PerformanceID(long perfID,
                              String perfIDName,
                              boolean plotPerfID,
                              Color perfIDColor,
                              double perfIDFreq,
                              String perfIDNotes)
        {
            this.perfID = perfID;
            this.perfIDName = perfIDName;
            this.plotPerfID = plotPerfID;
            this.perfIDColor = perfIDColor;
            this.perfIDFreq = perfIDFreq;
            this.perfIDNotes = perfIDNotes;
        }

        /**********************************************************************
         * ID list sort comparison. Can't use s1.compareTo(s2) since it returns
         * a slightly different sort order than that for a table column sort
         * (A_ comes before AB_ in a table, but after using String's compareTo)
         * 
         * @param pid
         *            performance ID class
         * 
         * @return 1 if this ID's name is alphabetically before the input ID's
         *         name; 0 if the ID names are identical; -1 if this ID's name
         *         is alphabetically after the input ID's name
         *********************************************************************/
        @Override
        public int compareTo(PerformanceID pid)
        {
            // Assume the IDs are identical
            int compare = 0;
            int index;

            // Get the two IDs to compare
            String s1 = this.getPerfIDName(true);
            String s2 = pid.getPerfIDName(true);

            // Compare character by character
            for (index = 0; index < s1.length() && index < s2.length(); index++)
            {
                // Check if the characters don't match
                if (s1.charAt(index) != s2.charAt(index))
                {
                    // Assume the first ID is alphabetically before the second
                    // ID and exit the loop
                    compare = 1;
                    break;
                }
            }

            // Check if there was a mismatch
            if (compare == 1)
            {
                // Check if the first ID's character value is less than that of
                // the second ID
                if (Character.getNumericValue(s1.charAt(index)) < Character.getNumericValue(s2.charAt(index)))
                {
                    // The first ID is alphabetically after the second ID
                    compare = -1;
                }
            }

            return compare;
        }

        /**********************************************************************
         * Get the performance ID
         * 
         * @return Performance ID
         *********************************************************************/
        protected long getPerfID()
        {
            return perfID;
        }

        /**********************************************************************
         * Get the performance ID name
         * 
         * @return ID name
         *********************************************************************/
        protected String getPerfIDName()
        {
            return perfIDName;
        }

        /**********************************************************************
         * Get the performance ID name. If useID is true and no name is defined
         * for the ID then return the ID instead. If useID is false and no name
         * is defined then return the default undefined text
         * 
         * @param useID
         *            true to allow the ID to be used in place of the name if
         *            no name is defined; false to use the default ID name if
         *            no name is defined
         * 
         * @return ID name (ID it no name defined and useID is true)
         *********************************************************************/
        protected String getPerfIDName(boolean useID)
        {
            String idText;

            // Check if no name has been assigned to this ID
            if (perfIDName.isEmpty())
            {
                // Check if the ID should be used for the name
                if (useID)
                {
                    // Convert the ID to text and use it for the name
                    idText = convertIDToHexString(perfID);
                }
                // The default name should be used
                else
                {
                    // Set the name to the default name
                    idText = NAME_NOT_DEFINED;
                }
            }
            // The ID has a name
            else
            {
                // Use the ID's name
                idText = perfIDName;
            }

            return idText;
        }

        /**********************************************************************
         * Return the status of the show performance ID on plot flag
         * 
         * @return true if the plot flag is enabled for this ID
         *********************************************************************/
        protected boolean isPlotPerfID()
        {
            return plotPerfID;
        }

        /**********************************************************************
         * Set the show performance ID on plot flag
         * 
         * @param true to allow displaying this ID on the plots
         *********************************************************************/
        private void setPlotPerfID(boolean plotID)
        {
            plotPerfID = plotID;
        }

        /**********************************************************************
         * Return the performance ID color
         * 
         * @return ID plot color
         *********************************************************************/
        protected Color getPerfIDColor()
        {
            return perfIDColor;
        }

        /**********************************************************************
         * Return the performance ID schedule frequency
         * 
         * @return Expected ID entry event frequency
         *********************************************************************/
        protected double getPerfIDFreq()
        {
            return perfIDFreq;
        }

        /**********************************************************************
         * Get the ID notes
         * 
         * @return ID notes
         *********************************************************************/
        protected String getPerfIDNotes()
        {
            return perfIDNotes;
        }

        /**********************************************************************
         * Set the ID used flag state
         * 
         * @param usedState
         *            state of the ID used flag: true, false, or null
         *********************************************************************/
        protected void setUsed(Boolean usedState)
        {
            perfIDIsUsed = usedState;
        }

        /**********************************************************************
         * Get the state of the ID used flag
         * 
         * @return State of the ID used flag: true, false, or null
         *********************************************************************/
        protected Boolean isUsed()
        {
            return perfIDIsUsed;
        }

        /**********************************************************************
         * Get the exit event count
         * 
         * @return Number of ID exit events
         *********************************************************************/
        protected int getExitCount()
        {
            return exitCount;
        }

        /**********************************************************************
         * Increment the exit event count
         *********************************************************************/
        protected void incrementExitCount()
        {
            exitCount++;
        }

        /**********************************************************************
         * Get the entry event count
         * 
         * @return Number of ID entry events
         *********************************************************************/
        protected int getEntryCount()
        {
            return entryCount;
        }

        /**********************************************************************
         * Increment the entry event count
         *********************************************************************/
        protected void incrementEntryCount()
        {
            entryCount++;
        }

        /**********************************************************************
         * Get the average entry event frequency
         * 
         * @return ID average entry event frequency, entry events/seconds
         *********************************************************************/
        protected Double getAvgFreq()
        {
            return avgFreq;
        }

        /**********************************************************************
         * Set the average entry event frequency
         * 
         * @param frequency
         *            ID average entry event frequency, entry events/seconds
         *********************************************************************/
        protected void setAvgFreq(double frequency)
        {
            avgFreq = frequency;
        }

        /**********************************************************************
         * Get the active time
         * 
         * @return ID active time, seconds
         *********************************************************************/
        protected Double getTimeActive()
        {
            return timeActive;
        }

        /**********************************************************************
         * Add to the active time count
         * 
         * @param addTime
         *            amount of time to add to the active time count, seconds
         *********************************************************************/
        protected void addToActiveTime(double addTime)
        {
            timeActive += addTime;
        }

        /**********************************************************************
         * Get the minimum active time
         * 
         * @return ID minimum active time, seconds
         *********************************************************************/
        protected Double getMinTimeActive()
        {
            return minTimeActive;
        }

        /**********************************************************************
         * Set the minimum active time
         * 
         * @param time
         *            ID minimum active time, seconds
         *********************************************************************/
        protected void setMinTimeActive(double time)
        {
            minTimeActive = time;
        }

        /**********************************************************************
         * Get the maximum active time
         * 
         * @return ID maximum active time, seconds
         *********************************************************************/
        protected Double getMaxTimeActive()
        {
            return maxTimeActive;
        }

        /**********************************************************************
         * Set the maximum active time
         * 
         * @param time
         *            ID maximum active time, seconds
         *********************************************************************/
        protected void setMaxTimeActive(double time)
        {
            maxTimeActive = time;
        }

        /**********************************************************************
         * Get the minimum interval time between entry events
         * 
         * @return ID minimum interval time, seconds
         *********************************************************************/
        protected Double getMinInterval()
        {
            return minInterval;
        }

        /**********************************************************************
         * Set the minimum interval time between entry events
         * 
         * @param interval
         *            ID minimum interval time, seconds
         *********************************************************************/
        protected void setMinInterval(double interval)
        {
            minInterval = interval;
        }

        /**********************************************************************
         * Get the maximum interval time between entry events
         * 
         * @return ID maximum interval time, seconds
         *********************************************************************/
        protected Double getMaxInterval()
        {
            return maxInterval;
        }

        /**********************************************************************
         * Set the maximum interval time between entry events
         * 
         * @param interval
         *            ID maximum interval time, seconds
         *********************************************************************/
        protected void setMaxInterval(double interval)
        {
            maxInterval = interval;
        }

        /**********************************************************************
         * Get the minimum frame overrun time
         * 
         * @return ID minimum frame overrun time, seconds
         *********************************************************************/
        protected Double getMinOverrun()
        {
            return minOverrun;
        }

        /**********************************************************************
         * Set the minimum frame overrun time
         * 
         * @param overrun
         *            ID minimum frame overrun time, seconds
         *********************************************************************/
        protected void setMinOverrun(double overrun)
        {
            minOverrun = overrun;
        }

        /**********************************************************************
         * Get the maximum frame overrun time
         * 
         * @return ID maximum frame overrun time, seconds
         *********************************************************************/
        protected Double getMaxOverrun()
        {
            return maxOverrun;
        }

        /**********************************************************************
         * Set the maximum frame overrun time
         * 
         * @param overrun
         *            ID maximum frame overrun time, seconds
         *********************************************************************/
        protected void setMaxOverrun(double overrun)
        {
            maxOverrun = overrun;
        }

        /**********************************************************************
         * Get the minimum time on time stamp
         * 
         * @return ID minimum active time stamp, seconds
         *********************************************************************/
        protected Double getMinTimeActiveTimeStamp()
        {
            return minTimeActiveTimeStamp;
        }

        /**********************************************************************
         * Set the minimum active time stamp
         * 
         * @param timeStamp
         *            ID minimum active time stamp, seconds
         *********************************************************************/
        protected void setMinTimeActiveTimeStamp(double timeStamp)
        {
            minTimeActiveTimeStamp = timeStamp;
        }

        /**********************************************************************
         * Get the maximum time on time stamp
         * 
         * @return ID maximum active time stamp, seconds
         *********************************************************************/
        protected Double getMaxTimeActiveTimeStamp()
        {
            return maxTimeActiveTimeStamp;
        }

        /**********************************************************************
         * Set the maximum active time stamp
         * 
         * @param timeStamp
         *            ID maximum active time stamp, seconds
         *********************************************************************/
        protected void setMaxTimeActiveTimeStamp(double timeStamp)
        {
            maxTimeActiveTimeStamp = timeStamp;
        }

        /**********************************************************************
         * Get the minimum interval time between entry events time stamp
         * 
         * @return ID minimum interval time, seconds
         *********************************************************************/
        protected Double getMinIntervalTimeStamp()
        {
            return minIntervalTimeStamp;
        }

        /**********************************************************************
         * Set the minimum interval time between entry events time stamp
         * 
         * @param timeStamp
         *            ID minimum interval time stamp, seconds
         *********************************************************************/
        protected void setMinIntervalTimeStamp(double timeStamp)
        {
            minIntervalTimeStamp = timeStamp;
        }

        /**********************************************************************
         * Get the maximum interval time between entry events time stamp
         * 
         * @return ID maximum interval time, seconds
         *********************************************************************/
        protected Double getMaxIntervalTimeStamp()
        {
            return maxIntervalTimeStamp;
        }

        /**********************************************************************
         * Set the maximum interval time between entry events time stamp
         * 
         * @param timeStamp
         *            ID maximum interval time stamp, seconds
         *********************************************************************/
        protected void setMaxIntervalTimeStamp(double timeStamp)
        {
            maxIntervalTimeStamp = timeStamp;
        }

        /**********************************************************************
         * Get the minimum frame overrun time stamp
         * 
         * @return ID minimum frame overrun time stamp, seconds
         *********************************************************************/
        protected Double getMinOverrunTimeStamp()
        {
            return minOverrunTimeStamp;
        }

        /**********************************************************************
         * Set the minimum frame overrun time stamp
         * 
         * @param timeStamp
         *            ID minimum frame overrun time stamp, seconds
         *********************************************************************/
        protected void setMinOverrunTimeStamp(double timeStamp)
        {
            minOverrunTimeStamp = timeStamp;
        }

        /**********************************************************************
         * Get the maximum frame overrun time stamp
         * 
         * @return ID maximum frame overrun time stamp, seconds
         *********************************************************************/
        protected Double getMaxOverrunTimeStamp()
        {
            return maxOverrunTimeStamp;
        }

        /**********************************************************************
         * Set the maximum frame overrun time stamp
         * 
         * @param timeStamp
         *            ID maximum frame overrun time stamp, seconds
         *********************************************************************/
        protected void setMaxOverrunTimeStamp(double timeStamp)
        {
            maxOverrunTimeStamp = timeStamp;
        }

        /**********************************************************************
         * Clear the performance ID statistics values
         *********************************************************************/
        protected void clearPerfIDStats()
        {
            exitCount = 0;
            entryCount = 0;
            timeActive = 0.0;
            avgFreq = 0.0;
            minTimeActive = null;
            maxTimeActive = null;
            minInterval = null;
            maxInterval = null;
            minTimeActiveTimeStamp = null;
            maxTimeActiveTimeStamp = null;
            minIntervalTimeStamp = null;
            maxIntervalTimeStamp = null;
            minOverrun = null;
            maxOverrun = null;
            minOverrunTimeStamp = null;
            maxOverrunTimeStamp = null;
        }
    }

    /**************************************************************************
     * Set the performance log, main, and plot preferences variables
     * 
     * @param perfLog
     *            performance log
     * 
     * @param perfMain
     *            main class
     * 
     * @param plotPrefs
     *            plot preferences
     *************************************************************************/
    protected void setGlobals(CPMLogHandler perfLog,
                              CPMMain perfMain,
                              CPMPreferencesHandler plotPrefs)
    {
        this.perfLog = perfLog;
        this.perfMain = perfMain;
        this.plotPrefs = plotPrefs;

        perfIDList = new ArrayList<PerformanceID>();

        hasSavedIDs = false;
    }

    /**************************************************************************
     * Get the performance ID list
     * 
     * @return Performance ID list
     *************************************************************************/
    protected List<PerformanceID> getPerfIDList()
    {
        return perfIDList;
    }

    /**************************************************************************
     * Return the ID list loaded from a file or saved to a file status
     * 
     * @return true if an ID list has been loaded or saved
     *************************************************************************/
    protected boolean isSavedIDs()
    {
        return hasSavedIDs;
    }

    /**************************************************************************
     * Reset the auto-assigned color index to zero
     *************************************************************************/
    protected void resetColorIndex()
    {
        colorIndex = 0;
    }

    /**************************************************************************
     * Reset the ID used flags to a specified state
     * 
     * @param resetValue
     *            state with which to initialize the ID used flags: true,
     *            false, or null
     *************************************************************************/
    protected void resetIDUsed(Boolean resetValue)
    {
        // Step through each ID in the list
        for (int index = 0; index < perfIDList.size(); index++)
        {
            // Set the ID used flag to false
            perfIDList.get(index).perfIDIsUsed = resetValue;
        }
    }

    /**************************************************************************
     * Search the user-selected source code for performance IDs. Since the
     * search can take an extended amount of time it is performed in the
     * background on a separate thread, which allows a wait cursor and progress
     * bar to be displayed and updated during the search. The user interface is
     * disabled until the search completes in order to prevent possible error
     * conditions occurring that may arise if the user initiates other commands
     * during the search
     *************************************************************************/
    protected void findPerformanceIDs()
    {
        // Make a shallow copy of the original ID list in case the original
        // list needs to be restored
        final List<PerformanceID> originalList = new ArrayList<PerformanceID>(perfIDList);

        // Ask the user if it's okay to delete or append to the list if it
        // currently exists
        if (clearPerformanceIDList(ALLOW_APPEND))
        {
            final File[] file;

            // Get the path to the source code
            file = new CPMDialogHandler().choosePathFile(perfMain,
                                                         null,
                                                         true,
                                                         false,
                                                         "Select Source Code Folder",
                                                         SEARCH_SOURCE_PATH,
                                                         SEARCH_OPTION);

            // Check if a path was selected
            if (file[0] != null)
            {
                // Display the search criteria dialog. Check if the Okay button
                // is pressed
                if (new CPMSourceSearchDialog(perfMain,
                                              file[0]).getButtonSelected() == OK_OPTION)
                {
                    // Since this could take a while, show the "wait" mouse
                    // pointer to alert the user. The pointer is restored to
                    // the default when the search completes
                    perfMain.setMainCursor(Cursor.WAIT_CURSOR);

                    // Create a progress bar to indicate the progress of the ID
                    // search. The bar is placed within a frame and positioned
                    // centered in the main window. The text displayed in the
                    // bar is updated as each new folder is searched
                    final JProgressBar searchBar = new JProgressBar();
                    searchBar.setIndeterminate(true);
                    searchBar.setString(" Searching for IDs... ");
                    searchBar.setFont(LABEL_FONT_BOLD);
                    searchBar.setStringPainted(true);
                    final JFrame searchFrame = new JFrame();
                    searchFrame.add(searchBar);
                    searchFrame.setUndecorated(true);
                    searchFrame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
                    searchFrame.pack();
                    searchFrame.setLocationRelativeTo(perfMain.getMainFrame());
                    searchFrame.setVisible(true);

                    // Create a SwingWorker to perform the ID search on a
                    // separate thread
                    SwingWorker<?, ?> idSearchWorker = new SwingWorker<Void, Integer>()
                    {
                        // Search error flag
                        boolean errorFlag = false;

                        /******************************************************
                         * Perform ID search
                         *****************************************************/
                        @Override
                        protected Void doInBackground()
                        {
                            // Deactivate the main menu and plot tabs
                            perfMain.setGUIActivated(false);

                            // Store the number of IDs already in the list (if
                            // appending)
                            existingIDs = perfIDList.size();

                            // Clear the string that keeps track of any
                            // duplicate IDs
                            duplicateID = "";

                            // Initialize the files checked counter
                            numFiles = 0;

                            // Set up the pattern to match
                            Pattern pattern = Pattern.compile(perfMain.getPreferences().get(SEARCH_PERF_ID_SUFFIX,
                                                                                            DEFAULT_PERF_ID_SUFFIX)
                                                              + "\\s");

                            // Get the list of subfolders to search
                            String[] selectedSubfolders = perfMain.getPreferences().get(SEARCH_SUBFOLDERS,
                                                                                        DEFAULT_SUBFOLDERS).split(",");

                            // Step through the folders potentially containing
                            // performance IDs
                            for (String subfolder : selectedSubfolders)
                            {
                                // Update the progress bar to show the current
                                // search folder. Use pack(0 to fit the bar to
                                // the text and re-center it in the main window
                                searchBar.setString(" Searching " + subfolder + " for IDs... ");
                                searchFrame.pack();
                                searchFrame.setLocationRelativeTo(perfMain.getMainFrame());

                                // Check selected subfolders for header files;
                                // true is returned if an error occurs
                                if (checkTargetFiles(file[0].getAbsolutePath()
                                                     + File.separator
                                                     + subfolder,
                                                     pattern))
                                {
                                    // An error occurred; set the flag and exit
                                    // the loop
                                    errorFlag = true;
                                    break;
                                }
                            }

                            return null;
                        }

                        /******************************************************
                         * Perform ID search completed steps
                         *****************************************************/
                        @Override
                        protected void done()
                        {
                            String pathName = null;

                            // Restore the default mouse pointer and remove the
                            // progress bar
                            perfMain.setMainCursor(Cursor.DEFAULT_CURSOR);
                            searchFrame.dispose();

                            // Check that no error occurred obtaining the
                            // header file list
                            if (!errorFlag)
                            {
                                // Remap the performance IDs to the log
                                mapLogToIDList(true, false);

                                // Get the source code path name selected
                                pathName = file[0].getAbsolutePath();

                                // Check if the path name doesn't end with a
                                // period
                                if (!pathName.endsWith("."))
                                {
                                    // Append "/." (or "\.") to the path name
                                    // so that next time the file chooser will
                                    // go all the way into the selected path
                                    pathName += File.separator + ".";
                                }

                                // Create the results dialog message
                                String message = "<html><b>Completed creating performance ID list from source code at<br>"
                                                 + file[0].getAbsoluteFile()
                                                 + "<br><br></b> - ";

                                // Check if no header files were located in the
                                // file path provided
                                if (numFiles == 0)
                                {
                                    // Indicate that no header files were found
                                    message += "No header file found; no IDs added";
                                }
                                // At least one header file was found
                                else
                                {
                                    // Indicate the number of header files
                                    // searched, the number of new IDs found
                                    // (i.e., in addition to those already in
                                    // the ID list, if appending), and any IDs
                                    // duplicated within the header files
                                    message += perfIDList.size()
                                               + " IDs found in "
                                               + numFiles
                                               + " header files"
                                               + numberOfAddedIDs()
                                               + duplicateID;
                                }

                                // Inform the user that the search is complete,
                                // and display the results
                                new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                         message,
                                                                         "Search Results",
                                                                         JOptionPane.PLAIN_MESSAGE,
                                                                         OK_OPTION);
                            }
                            // An error occurred
                            else
                            {
                                // Restore the original list
                                perfIDList = originalList;

                                // Inform the user that an error occurred
                                // during the search
                                new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                         "<html><b>Error searching for IDs",
                                                                         "Search Error",
                                                                         JOptionPane.ERROR_MESSAGE,
                                                                         OK_OPTION);
                            }

                            // Reactivate the main menu and plot tabs
                            perfMain.setGUIActivated(true);

                            // Perform the search completion actions based on
                            // if the search was successful
                            perfMain.findIDsCleanup(pathName);
                        }
                    };

                    // Initiate the ID search. The application does not wait
                    // for this thread to complete
                    idSearchWorker.execute();
                }
                // Cancel selected in Search Criteria dialog
                else
                {
                    // Restore the original list
                    perfIDList = originalList;
                }
            }
            // Cancel selected in Source Code dialog
            else
            {
                // Restore the original list
                perfIDList = originalList;
            }
        }
    }

    /**************************************************************************
     * Check the files with the target extension for IDs
     * 
     * @param folderName
     *            folder to search
     * 
     * @param pattern
     *            text pattern that identifies a performance ID
     * 
     * @return true if an error occurred
     *************************************************************************/
    private boolean checkTargetFiles(String folderName, Pattern pattern)
    {
        boolean errorFlag = false;
        File folder = new File(folderName);

        // Get all the files from a folder
        File[] fileList = folder.listFiles();

        if (fileList != null)
        {
            // Valid subfolder
            for (File file : fileList)
            {
                // Check that no error has occurred
                if (!errorFlag)
                {
                    // Check that this is a file and that the file name ends in
                    // the target extension. The file name and extension are
                    // both put in lower case for the comparison in order to
                    // eliminate case sensitivity
                    if (file.isFile()
                        && file.getName().toLowerCase().endsWith(perfMain.getPreferences().get(SEARCH_FILE_EXTENSION,
                                                                                               DEFAULT_FILE_EXTENSION).toLowerCase()))
                    {
                        // Increment the files checked counter
                        numFiles++;

                        // File with target extension found; look for any
                        // performance IDs in the file
                        try
                        {
                            // Search this file for the pattern
                            checkFileForPerfID(file, pattern);
                        }
                        catch (IOException ioe)
                        {
                            // I/O error - report the results and set the
                            // error flag
                            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                     "<html><b>I/O error obtaining header file list",
                                                                     "I/O Error",
                                                                     JOptionPane.ERROR_MESSAGE,
                                                                     OK_OPTION);
                            errorFlag = true;
                        }
                    }
                    // Check that this is a path name
                    else if (file.isDirectory())
                    {
                        // It's a subfolder; search its files for matches
                        errorFlag = checkTargetFiles(file.getAbsolutePath(),
                                                     pattern);
                    }
                }
            }
        }

        return errorFlag;
    }

    /**************************************************************************
     * Search for occurrences of the input pattern in the given file
     * 
     * @param file
     *            file to search
     * 
     * @param pattern
     *            text pattern that identifies a performance ID
     *************************************************************************/
    private void checkFileForPerfID(File file,
                                    Pattern pattern) throws IOException
    {
        // Set up Charset and CharsetDecoder for ISO-8859-15
        Charset charset = Charset.forName("ISO-8859-15");
        CharsetDecoder decoder = charset.newDecoder();

        // Pattern used to detect separate lines
        Pattern linePattern = Pattern.compile(".*\r?\n");

        // Open the file and then get a channel from the stream
        FileInputStream fis = new FileInputStream(file);
        FileChannel fc = fis.getChannel();

        // Get the file's size and then map it into memory
        MappedByteBuffer byteBuffer = fc.map(FileChannel.MapMode.READ_ONLY,
                                             0,
                                             fc.size());

        // Decode the file into a char buffer
        CharBuffer charBuffer = decoder.decode(byteBuffer);

        // Create the line and pattern matchers, then perform the search
        Matcher lineMatch = linePattern.matcher(charBuffer);
        Matcher patternMatch = null;

        // For each line in the file
        while (lineMatch.find())
        {
            // Get the line from the file
            CharSequence charSeq = lineMatch.group();

            // Check if this is the first pass through the loop for this file
            if (patternMatch == null)
            {
                // Create a pattern matcher for the target pattern using the
                // current line information
                patternMatch = pattern.matcher(charSeq);
            }
            // Not the first pass
            else
            {
                // Reset the matcher with the new line information
                patternMatch.reset(charSeq);
            }

            // Check if the line contains the target pattern
            if (patternMatch.find())
            {
                // Line potentially contains performance ID information;
                // extract it
                extractPerformanceID(charSeq);
            }

            // Check if the end of the file has been reached
            if (lineMatch.end() == charBuffer.limit())
            {
                // Exit the loop
                break;
            }
        }

        // Close the channel and the stream
        fc.close();
        fis.close();
    }

    /**************************************************************************
     * Parse the performance ID found in the header file
     * 
     * @param charSeq
     *            character sequence from a file potentially containing a
     *            performance ID definition
     *************************************************************************/
    private void extractPerformanceID(CharSequence charSeq)
    {
        // Strip any leading or trailing whitespace from the line
        String line = charSeq.toString().trim();

        // Line must start with #define to be a performance ID definition
        if (line.startsWith("#define"))
        {
            // (1) Remove the #define keyword (first 7 characters),
            // (2) Remove the leading space(s),
            // (3) Replace instances of one or more spaces/tabs with a comma,
            // (4) Split the resulting string into parts at the commas
            String[] part = line.substring(7).trim().replaceAll("\\s+", ",").split(",");

            // Convert the ID into an integer
            long perfID = convertStringToIntID(part[1]);

            // Get the ID's index in the ID list
            Integer perfIndex = getPerfIDIndex(perfID);

            // Check if this is a new ID
            if (perfIndex == null)
            {
                // Add the performance ID information to array. Initialize the
                // "show in plot" flag to false; this flag is set automatically
                // to true if the ID is present in the log file (the user can
                // override this flag in the ID editor dialog)
                addPerformanceID(perfID, part[0], false, null, 0, "");
            }
            // The ID is already in the list; check if it's a duplicate of an
            // ID in the current search
            else if (perfIndex > existingIDs)
            {
                // Check if this is the first duplicate found
                if (duplicateID.equals(""))
                {
                    // Create a lead in
                    duplicateID = "<br><br><b>Duplicate performance ID(s) found (and ignored):</b>";
                }

                // Append the duplicate's information to the string
                duplicateID += String.format("<br>- %s, %s (same as %s)",
                                             convertIDToHexString(perfID),
                                             part[0],
                                             perfIDList.get(perfIndex).getPerfIDName(true));
            }
        }
    }

    /**************************************************************************
     * Convert the performance ID from a string into an equivalent decimal
     * value
     * 
     * @param idString
     *            text string representation of the performance ID
     * 
     * @return Decimal representation of the performance ID (null if conversion
     *         fails)
     *************************************************************************/
    protected Long convertStringToIntID(String idString)
    {
        Long value = null;
        int radix;

        // Determine if the ID is hexadecimal
        if (idString.startsWith("0x") || idString.startsWith("0X"))
        {
            // Remove the leading '0x'
            idString = idString.substring(2);
            radix = 16;
        }
        // Not hex, so it must be decimal
        else
        {
            radix = 10;
        }

        try
        {
            // Convert the string representation to a decimal value
            value = Long.parseLong(idString, radix);
        }
        // The conversion failed (invalid characters)
        catch (NumberFormatException nfe)
        {
        }

        return value;
    }

    /**************************************************************************
     * Convert an integer performance ID into an equivalent hexadecimal string
     * 
     * @param perfID
     *            integer representation of the performance ID
     * 
     * @return Hexadecimal string representation of the performance ID
     *************************************************************************/
    protected String convertIDToHexString(long perfID)
    {
        return String.format("0x%08x", perfID);
    }

    /**************************************************************************
     * Add a performance ID to the ID list
     * 
     * @param perfID
     *            performance ID
     * 
     * @param perfIDName
     *            performance ID name
     * 
     * @param plotPerfID
     *            show ID in plot flag
     * 
     * @param perfIDColor
     *            performance ID color
     * 
     * @param perfIDFreq
     *            performance ID frequency
     * 
     * @param perfIDNotes
     *            user-supplied annotation for this ID
     *************************************************************************/
    protected void addPerformanceID(long perfID,
                                    String perfIDName,
                                    boolean plotPerfID,
                                    Color perfIDColor,
                                    double perfIDFreq,
                                    String perfIDNotes)
    {
        // Add the ID to the list
        perfIDList.add(new PerformanceID(perfID,
                                         perfIDName,
                                         plotPerfID,
                                         perfIDColor,
                                         perfIDFreq,
                                         perfIDNotes));
    }

    /**************************************************************************
     * Clear the list of performance IDs
     * 
     * @param clearOrAppend
     *            CLEAR_ON_APPROVAL to clear the loaded IDs on user approval;
     *            CLEAR_WITHOUT_ASKING to clear the currently loaded IDs
     *            without asking; ALLOW_APPEND to allow appending the new IDs
     *            to the current list
     * 
     * @return false if the user elected to cancel clearing the ID list
     *************************************************************************/
    protected boolean clearPerformanceIDList(ClearOption clearOrAppend)
    {
        boolean status = true;
        int buttonPressed = CANCEL_OPTION;

        // Check that the list isn't empty
        if (perfIDList.size() != 0)
        {
            // Check if it's okay to clear the existing ID list
            if (clearOrAppend == CLEAR_ON_APPROVAL)
            {
                // Display the delete ID confirmation dialog
                buttonPressed = new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                         "<html><b>Clear existing performance ID list?",
                                                                         "Clear IDs",
                                                                         JOptionPane.QUESTION_MESSAGE,
                                                                         OK_CANCEL_OPTION);
            }
            // Check if the existing list should be appended to or cleared
            else if (clearOrAppend == ALLOW_APPEND)
            {
                // Display the append/delete ID list dialog
                buttonPressed = new CPMDialogHandler().showOptionsDialog(perfMain.getMainFrame(),
                                                                         "<html><b>Append to or clear existing performance ID list?",
                                                                         "Append/Clear IDs");
            }

            // Check if force clear flag is set or if the user elected to
            // delete the IDs
            if (clearOrAppend == CLEAR_WITHOUT_ASKING
                || (clearOrAppend == CLEAR_ON_APPROVAL
                && buttonPressed == OK_OPTION)
                || (clearOrAppend == ALLOW_APPEND
                && buttonPressed == CLEAR_OPTION))
            {
                // Delete the existing IDs
                perfIDList.clear();

                // Clear the selected minor and major frame marker IDs
                plotPrefs.setShowPlotMinorFrames(false);
                plotPrefs.setShowPlotMajorFrames(false);
            }
            // Check if the user selected Cancel
            else if (buttonPressed == CANCEL_OPTION)
            {
                status = false;
            }
        }

        return status;
    }

    /**************************************************************************
     * Load the performance IDs from the user-selected ID list file. If append
     * is selected then only add IDs not already in the list. Return the file
     * name if successful, otherwise return null
     * 
     * @param fileName
     *            performance ID list path+file name
     * 
     * @param clearOrAppend
     *            CLEAR_ON_APPROVAL to clear the loaded IDs on user approval;
     *            CLEAR_WITHOUT_ASKING to clear the currently loaded IDs
     *            without asking; ALLOW_APPEND to allow appending the new IDs
     *            to the current list
     * 
     * @return Selected ID list path + file name
     *************************************************************************/
    protected String loadIDs(String fileName, ClearOption clearOrAppend)
    {
        String selectedFileName = null;

        // Create a flag that indicates if the original list should be restored
        boolean restoreList = true;

        // Make a shallow copy of the original ID list in case the original
        // list needs to be restored
        List<PerformanceID> originalList = new ArrayList<PerformanceID>(perfIDList);

        // Check if okay to overwrite or append to the current list
        if (clearPerformanceIDList(clearOrAppend))
        {
            File perfIDFile[];

            // Store the ID file name into an array since an array is required
            // by the path choosing method
            String[] fileNameArray = {fileName};

            // Allow the user to select the ID file path + name to load from
            perfIDFile = new CPMDialogHandler().choosePathFile(perfMain,
                                                               fileNameArray,
                                                               false,
                                                               false,
                                                               "Load Performance IDs",
                                                               PERF_ID_LIST_FILE,
                                                               LOAD_OPTION);

            // Check if a file was chosen
            if (perfIDFile[0] != null)
            {
                // Check if the file doesn't exist
                if (!perfIDFile[0].exists())
                {
                    // Inform the user that the ID file cannot be located
                    new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                             "<html><b>Cannot locate ID file<br></b>" +
                                                                 perfIDFile[0].getAbsolutePath(),
                                                             "File Error",
                                                             JOptionPane.ERROR_MESSAGE,
                                                             OK_OPTION);
                }
                // The file exists
                else
                {
                    try
                    {
                        // Get the number of IDs currently in the list
                        existingIDs = perfIDList.size();

                        // Read the performance names and IDs from the
                        // selected file
                        BufferedReader br = new BufferedReader(new FileReader(perfIDFile[0]));

                        // File format error flag; assume no errors
                        boolean formatError = false;

                        // Number of IDs that exceed the maximum ID value
                        int badIDs = 0;

                        // Read first line in file
                        String line = br.readLine();

                        // Continue to read the file until EOF is reached or an
                        // error is detected
                        while (line != null && formatError == false)
                        {
                            // Parse performance ID parameters
                            String[] part = line.split(",", NUM_PARAMETERS);

                            // Check that the line has the correct number of
                            // parameters
                            if (part.length == NUM_PARAMETERS)
                            {
                                // Convert the ID into an integer
                                Long perfID = convertStringToIntID(part[VALUE]);

                                // Check if the ID is not a valid value
                                if (perfID == null)
                                {
                                    // Set the error flag
                                    formatError = true;
                                }
                                // The ID is a valid value
                                else
                                {
                                    // Check if the ID value exceeds the
                                    // maximum ID value supported by the
                                    // current log
                                    if (perfLog.getMaximumIDs() != 0
                                        && perfID >= perfLog.getMaximumIDs())
                                    {
                                        // Increment the number of bad IDs
                                        badIDs++;
                                    }

                                    // Get the performance ID object associated
                                    // with the ID
                                    PerformanceID pid = getPerfID(perfID);

                                    // Check if the ID isn't already in the
                                    // list
                                    if (pid == null)
                                    {
                                        // Add the ID to the list. Initialize
                                        // the "show in plot" flag to false;
                                        // this flag is set automatically to
                                        // true if the ID is present in the log
                                        // file (the user can override this
                                        // flag in the ID editor dialog)
                                        addPerformanceID(perfID,
                                                         part[NAME],
                                                         false,
                                                         Color.decode(part[COLOR]),
                                                         Double.valueOf(part[FREQ]),
                                                         part[NOTES]);
                                    }
                                    // The ID already exists. Check if no name
                                    // has been assigned to it
                                    else if (pid.perfIDName.isEmpty())
                                    {
                                        // Use the name from the file for the
                                        // ID
                                        pid.perfIDName = part[NAME];
                                    }

                                    // Read next line in file
                                    line = br.readLine();
                                }
                            }
                            // The file line format is incorrect
                            else
                            {
                                // Set the error flag
                                formatError = true;
                            }
                        }

                        try
                        {
                            // Close the ID file
                            br.close();
                        }
                        catch (IOException ioe)
                        {
                            // Inform the user that the file cannot be closed
                            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                     "<html><b>Cannot close ID file<br></b>"
                                                                         + perfIDFile[0].getAbsolutePath(),
                                                                     "File Warning",
                                                                     JOptionPane.WARNING_MESSAGE,
                                                                     OK_OPTION);
                        }

                        // Check that no errors were detected in the file
                        if (!formatError)
                        {
                            // Clear the flag for restoring the original list
                            // since the new one was successfully read
                            restoreList = false;

                            // Remap the performance IDs to the log
                            mapLogToIDList(false, false);

                            // Create a string to show the number of IDs that
                            // are greater than the maximum supported value
                            String numberOfBadIDs = "";

                            // Check if there are any IDs exceeding the maximum
                            // ID value
                            if (badIDs != 0)
                            {
                                String inner;

                                // Make the message read correctly for singular
                                // or plural
                                if (badIDs == 1)
                                {
                                    inner = " exceeds";
                                }
                                else
                                {
                                    inner = "s exceed";
                                }

                                // Build the string showing the number that
                                // exceed the maximum supported ID value
                                numberOfBadIDs = "<br><br>- "
                                                 + badIDs
                                                 + " ID value"
                                                 + inner
                                                 + " the maximum<br>&#160&#160&#160supported ("
                                                 + convertIDToHexString(perfLog.getMaximumIDs() - 1)
                                                 + ")";
                            }

                            // Save the ID list file name
                            selectedFileName = perfIDFile[0].getAbsolutePath();

                            // Set flag to indicate that IDs have been loaded
                            hasSavedIDs = true;

                            // Let the user know how many IDs are loaded
                            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                     "<html><b>"
                                                                         + perfIDList.size()
                                                                         + " performance IDs loaded"
                                                                         + numberOfAddedIDs()
                                                                         + numberOfBadIDs,
                                                                     "IDs Loaded",
                                                                     badIDs == 0
                                                                                ? JOptionPane.INFORMATION_MESSAGE
                                                                                : JOptionPane.WARNING_MESSAGE,
                                                                     OK_OPTION);
                        }
                        // An error occurred
                        else
                        {
                            // Inform the user that the ID file is invalid
                            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                     "<html><b>Format invalid for ID file<br></b>" +
                                                                         perfIDFile[0].getAbsolutePath(),
                                                                     "File Error",
                                                                     JOptionPane.ERROR_MESSAGE,
                                                                     OK_OPTION);
                        }
                    }
                    catch (IOException ioe)
                    {
                        // Inform the user that the ID file cannot be read
                        new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                 "<html><b>Cannot read ID file<br></b>" +
                                                                     perfIDFile[0].getAbsolutePath(),
                                                                 "File Error",
                                                                 JOptionPane.ERROR_MESSAGE,
                                                                 OK_OPTION);
                    }
                }
            }

            // Check if the flag for restoring the original list remains set,
            // which indicates the user canceled the load IDs operation or an
            // error occurred
            if (restoreList)
            {
                // Restore the original list
                perfIDList = originalList;
            }
        }

        return selectedFileName;
    }

    /**************************************************************************
     * Save performance IDs to a file
     * 
     * @return Performance ID list path+file name
     *************************************************************************/
    protected String saveIDs()
    {
        String fileName = null;
        File perfIDFile[];

        // Allow the user to select the ID file path + name to save to
        perfIDFile = new CPMDialogHandler().choosePathFile(perfMain,
                                                           null,
                                                           false,
                                                           false,
                                                           "Save Performance IDs",
                                                           PERF_ID_LIST_FILE,
                                                           SAVE_OPTION);

        // Check if a file was chosen
        if (perfIDFile[0] != null)
        {
            // Save error type message; assume no error, unless one occurs
            String errorType = "";

            // Check if the ID file exists
            if (perfIDFile[0].exists())
            {
                // Allow the user to select the ID path + file
                if (new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                             "<html><b>Overwrite existing performance ID file?",
                                                             "Overwrite File",
                                                             JOptionPane.QUESTION_MESSAGE,
                                                             OK_CANCEL_OPTION) == OK_OPTION)
                {
                    // Check if the file can be deleted
                    if (!perfIDFile[0].delete())
                    {
                        // Set the error type message
                        errorType = "Cannot replace";
                    }
                }
            }

            // Check that no errors occurred
            if (errorType.isEmpty())
            {
                try
                {
                    // Check if the ID file is successfully created
                    if (perfIDFile[0].createNewFile())
                    {
                        // Output performance ID names and IDs to the selected
                        // file
                        PrintWriter pw = new PrintWriter(perfIDFile[0]);

                        // Step through the performance IDs
                        for (PerformanceID pid : perfIDList)
                        {
                            // Output the ID information to the file
                            pw.printf("%s,%s,%s,%f,%s\n",
                                      pid.perfIDName,
                                      convertIDToHexString(pid.perfID),
                                      String.format("0x%06x",
                                                    pid.perfIDColor.getRGB()
                                                    & 0xffffff),
                                      pid.perfIDFreq,
                                      pid.perfIDNotes);
                        }

                        // Close the ID file
                        pw.close();

                        // Save the ID list file name
                        fileName = perfIDFile[0].getAbsolutePath();

                        // Set flag to indicate IDs have been saved
                        hasSavedIDs = true;
                    }
                    // ID file cannot be created
                    else
                    {
                        // Set the error type message
                        errorType = "Cannot create";
                    }
                }
                catch (IOException ioe)
                {
                    // I/O error - set the error type message
                    errorType = "Error writing to";
                }
            }

            // Check if an error occurred writing the ID file
            if (!errorType.isEmpty())
            {
                // Inform the user that the ID file cannot be created
                new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                         "<html><b>"
                                                             + errorType
                                                             + " ID file<br></b>" +
                                                             perfIDFile[0].getAbsolutePath(),
                                                         "File Error",
                                                         JOptionPane.ERROR_MESSAGE,
                                                         OK_OPTION);
            }
        }

        return fileName;
    }

    /**************************************************************************
     * Check if a performance ID is in the ID list and return its index if
     * found, or null if the ID isn't in the list
     * 
     * @param perfID
     *            performance ID
     * 
     * @return Performance ID's index in the ID list (null if not found)
     *************************************************************************/
    private Integer getPerfIDIndex(long perfID)
    {
        Integer index = null;

        // Step through the ID list
        for (PerformanceID pid : perfIDList)
        {
            // Check if the list ID matches the target ID
            if (pid.perfID == perfID)
            {
                // Save the index of the ID and quit searching
                index = perfIDList.indexOf(pid);
                break;
            }
        }

        return index;
    }

    /**************************************************************************
     * Return the performance ID object associated with the specified ID value
     * 
     * @param perfID
     *            performance ID
     * 
     * @return Performance ID object (null if the ID is not found)
     *************************************************************************/
    protected PerformanceID getPerfID(long perfID)
    {
        PerformanceID pid = null;

        // Get the index of the ID in the list
        Integer index = getPerfIDIndex(perfID);

        // Check that the ID was found
        if (index != null)
        {
            // Get the performance ID object associated with this ID
            pid = perfIDList.get(index);
        }

        return pid;
    }

    /**************************************************************************
     * Create a string indicating the number of IDs added to the current list
     * 
     * @return String showing the number of new IDs
     *************************************************************************/
    private String numberOfAddedIDs()
    {
        String addedIDs = "";

        // Check if IDs have been appended to the list
        if (existingIDs != 0)
        {
            // Calculate the number of new IDs added
            addedIDs = " ("
                       + (perfIDList.size() - existingIDs)
                       + " added)";
        }

        return addedIDs;
    }

    /**************************************************************************
     * Map the performance log entries to the performance ID list. If the log
     * has an ID that's not already in the list then add it to the list
     * 
     * @param sortFlag
     *            true to sort the ID list alphabetically (by name, or ID if no
     *            name exists)
     * @param keepPlotStatusFlag
     *            true to keep the ID's plot flag; false to enable the plot
     *            flag if the ID is present in the log
     *************************************************************************/
    protected void mapLogToIDList(boolean sortFlag,
                                  boolean keepPlotStatusFlag)
    {
        // Check that the flag to maintain the current plot ID status isn't set
        if (!keepPlotStatusFlag)
        {
            // For each ID in the ID list
            for (int perfIndex = 0; perfIndex < perfIDList.size(); perfIndex++)
            {
                // Reset the "show ID in plot" flag. This flag is reenabled for
                // the IDs that are present in the log
                perfIDList.get(perfIndex).setPlotPerfID(false);
            }
        }

        // Step through the log data
        for (int logIndex = 0; logIndex < perfLog.getPerfLogData().size(); logIndex++)
        {
            // Get the performance ID
            long perfID = perfLog.getPerfLogData().get(logIndex).getPerfID();

            // Check if this ID doesn't exceed the maximum ID value. IDs that
            // exceed the maximum are ignored
            if (perfID < perfLog.getMaximumIDs())
            {
                // Get the performance ID in the list. If the ID isn't in the
                // list then the null returned
                PerformanceID pid = getPerfID(perfID);

                // Check if the ID isn't already in the list
                if (pid == null)
                {
                    // Add this ID to the list with a dummy name. Set the "show
                    // in plot" flag to true since data exists for this ID
                    // (therefore it must be enabled in the filter mask). The
                    // user can override this flag in the ID editor dialog. Set
                    // the color to null in order to use a color from the
                    // default color array. Initialize the frequency to 0 so
                    // that it's ignored
                    addPerformanceID(perfID,
                                     "",
                                     true,
                                     null,
                                     0.0,
                                     "");
                }
                // Check that the flag to maintain the current plot ID status
                // isn't set
                else if (!keepPlotStatusFlag)
                {
                    // Enable this ID for plotting since it's in the log
                    // (therefore it must be enabled in the filter mask). Skip
                    // this when performing the Save operation in the ID Editor
                    // to prevent altering the user's Show enable/disable
                    // selection
                    pid.setPlotPerfID(true);
                }
            }
        }

        // Check if the ID list is to be sorted
        if (sortFlag)
        {
            // Sort the list of IDs by ID value
            Collections.sort(perfIDList, null);
        }

        // Reset the color array index to the beginning of the array
        resetColorIndex();

        // Step through the ID list
        for (int index = 0; index < perfIDList.size(); index++)
        {
            // Check if the ID doesn't have a color assigned
            if (perfIDList.get(index).perfIDColor == null)
            {
                // Select the next color in the color array and increment
                // the index
                perfIDList.get(index).perfIDColor = getNextColor();
            }
        }
    }

    /**************************************************************************
     * Get the next color from the predefined color array and increment the
     * array index
     * 
     * @return Next color in the color array
     *************************************************************************/
    protected Color getNextColor()
    {
        Color nextColor = ID_COLOR[colorIndex];

        colorIndex++;

        // Check if the index has reached the end of the array
        if (colorIndex == ID_COLOR.length)
        {
            // Reset the index back to the start of the array
            colorIndex = 0;
        }

        return nextColor;
    }
}
