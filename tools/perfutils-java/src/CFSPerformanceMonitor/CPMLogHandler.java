/**
 * CFS Performance Monitor Log Handler
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.APPEND_OPTION;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.CFE_FS_FILE_CONTENT_ID;
import static CFSPerformanceMonitor.CPMConstants.CLEAR_OPTION;
import static CFSPerformanceMonitor.CPMConstants.EXIT_MASK;
import static CFSPerformanceMonitor.CPMConstants.ID_MASK;
import static CFSPerformanceMonitor.CPMConstants.LOG_SEPARATOR;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.PERF_LOG_FILE;
import static CFSPerformanceMonitor.CPMConstants.READ_OPTION;
import static CFSPerformanceMonitor.CPMConstants.setTimePrecision;
import static CFSPerformanceMonitor.CPMConstants.ClearOption.ALLOW_APPEND;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

import javax.swing.JOptionPane;

import CFSPerformanceMonitor.CPMConstants.ClearOption;
import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;

/******************************************************************************
 * CFS Performance Monitor log handler class
 *****************************************************************************/
public class CPMLogHandler
{
    private CPMIDHandler perfIDs;
    private CPMMain perfMain;
    private CPMPreferencesHandler plotPrefs;

    private final List<PerfLog> perfLogData;
    private final List<LogHeader> logHeader;
    private final List<Integer> dataGapIndex;
    private final List<Double> dataGapLength;

    // Time stamp of the latest log data entry, seconds
    private double maximumTimeStamp;

    // Time span of the log data minus any gaps (caused by loading multiple log
    // files), seconds
    private double adjustedDataTimeSpan;

    // Total number of entry events logged
    private int totalOnCount;

    // Total number of exit events logged
    private int totalOffCount;

    // Average frequency of all events, entry/second
    private Double totalAvgFreq;

    // Total time, sec, that the IDs are active
    private Double totalTimeActive;

    // Minimum time on for all IDs, seconds
    private Double totalMinTimeActive;

    // Maximum time on for all IDs, seconds
    private Double totalMaxTimeActive;

    // Minimum interval between entry events for all IDs, seconds
    private Double totalMinInterval;

    // Maximum interval between entry events for all IDs, seconds
    private Double totalMaxInterval;

    // ID for the minimum time on for all IDs, seconds
    private long totalMinTimeActiveID;

    // ID for the maximum time on for all IDs, seconds
    private long totalMaxTimeActiveID;

    // ID for the minimum interval between entry events for all IDs, seconds
    private long totalMinIntervalID;

    // ID for the maximum interval between entry events for all IDs, seconds
    private long totalMaxIntervalID;

    // Minimum frame overrun for all IDs, seconds
    private Double totalMinOverrun;

    // Maximum frame overrun for all IDs, seconds
    private Double totalMaxOverrun;

    // ID for the minimum frame overrun for all IDs, seconds
    private long totalMinOverrunID;

    // ID for the maximum frame overrun for all IDs, seconds
    private long totalMaxOverrunID;

    // Log time stamp for the minimum time on for all IDs, seconds
    private Double totalMinTimeOnTimeStamp;

    // Log time stamp for the maximum time on for all IDs, seconds
    private Double totalMaxTimeOnTimeStamp;

    // Log time stamp for the minimum interval between entry events for all
    // IDs, seconds
    private Double totalMinIntervalTimeStamp;

    // Log time stamp for the maximum interval between entry events for all
    // IDs, seconds
    private Double totalMaxIntervalTimeStamp;

    // Log time stamp for the minimum frame overrun for all IDs, seconds
    private Double totalMinOverrunTimeStamp;

    // Log time stamp for the maximum frame overrun for all IDs, seconds
    private Double totalMaxOverrunTimeStamp;

    // Base time; from the time stamp of the first log file data entry. If
    // multiple files are being processed then this is the time stamp of the
    // first data entry in the earliest log file
    private long baseTimeUpper;
    private long baseTimeLower;

    // Data type byte sizes
    private final int uint8 = 1;
    private final int uint32 = 4;

    // Log file input stream
    private FileInputStream perfLogFile;

    // True if at least one log is successfully read
    private boolean logRead;

    // Maximum number of IDs. Equivalent to the maximum ID value + 1
    private long maximumIDs;

    // Maximum header description length, bytes
    private int hdrDescMaxLen;

    // Flag indicating if one or more frame overruns exists in the log
    private boolean overrunFlag;

    /**************************************************************************
     * CFS Performance Monitor log handler class constructor
     *************************************************************************/
    protected CPMLogHandler()
    {
        perfLogData = new ArrayList<PerfLog>();
        logHeader = new ArrayList<LogHeader>();
        dataGapIndex = new ArrayList<Integer>();
        dataGapLength = new ArrayList<Double>();
        logRead = false;
        maximumIDs = 0;
        baseTimeUpper = 0;
        baseTimeLower = 0;
    }

    /**************************************************************************
     * CFS Performance Monitor log header class
     *************************************************************************/
    protected class LogHeader implements Comparable<LogHeader>
    {
        // Note: Since Java doesn't have unsigned variable types (as of Java
        // 1.7), longs are used in place of ints to avoid sign issues that
        // would occur for large values (time stamps in particular)

        // Header - CFE_FS_Header_t
        private String ContentType;
        private long SubType;
        private long Length;
        private long SpacecraftID;
        private long ProcessorID;
        private long ApplicationID;
        private long TimeSeconds;
        private long TimeSubSeconds;
        private String Description;

        // Metadata - CFE_ES_PerfMetaData_t
        private byte Version;
        private byte Endian;
        private long TimerTicksPerSecond;
        private long TimerLow32Rollover;
        private long State;
        private long Mode;
        private long TriggerCount;
        private long DataStart;
        private long DataEnd;
        private long DataCount;
        private long InvalidMarkerReported;
        private long FilterTriggerMaskSize;
        private long FilterMask[];
        private long TriggerMask[];

        // Log file name; not part of the log file header information, but
        // needed when displaying the header data in the header dialog
        private String logFileName;

        /**********************************************************************
         * CFS Performance Monitor log header class constructor
         *********************************************************************/
        protected LogHeader()
        {
        }

        /**********************************************************************
         * Get the content type
         * 
         * @return Content type
         *********************************************************************/
        protected String getContentType()
        {
            return ContentType;
        }

        /**********************************************************************
         * Get the sub type
         * 
         * @return sub type
         *********************************************************************/
        protected long getSubType()
        {
            return SubType;
        }

        /**********************************************************************
         * Get the primary header length
         * 
         * @return Primary header length
         *********************************************************************/
        protected long getLength()
        {
            return Length;
        }

        /**********************************************************************
         * Get the spacecraft ID
         * 
         * @return Spacecraft ID
         *********************************************************************/
        protected long getSpacecraftID()
        {
            return SpacecraftID;
        }

        /**********************************************************************
         * Get the processor ID
         * 
         * @return Processor ID
         *********************************************************************/
        protected long getProcessorID()
        {
            return ProcessorID;
        }

        /**********************************************************************
         * Get the application ID
         * 
         * @return Application ID
         *********************************************************************/
        protected long getApplicationID()
        {
            return ApplicationID;
        }

        /**********************************************************************
         * Get the log file time stamp (seconds)
         * 
         * @return Log file time stamp, seconds
         *********************************************************************/
        protected long getTimeSeconds()
        {
            return TimeSeconds;
        }

        /**********************************************************************
         * Get the log file time stamp (subseconds)
         * 
         * @return Log file time stamp, subseconds
         *********************************************************************/
        protected long getTimeSubSeconds()
        {
            return TimeSubSeconds;
        }

        /**********************************************************************
         * Get the log file description
         * 
         * @return Log file description
         *********************************************************************/
        protected String getDescription()
        {
            return Description;
        }

        /**********************************************************************
         * Get the version
         * 
         * @return version
         *********************************************************************/
        protected int getVersion()
        {
            return Version;
        }

        /**********************************************************************
         * Get the log file endianess
         * 
         * @return Log file endianess
         *********************************************************************/
        protected int getEndian()
        {
            return Endian;
        }

        /**********************************************************************
         * Get the number of clock ticks per second
         * 
         * @return Number of clock ticks per second
         *********************************************************************/
        protected long getTimerTicksPerSecond()
        {
            return TimerTicksPerSecond;
        }

        /**********************************************************************
         * Get the timer rollover ticks
         * 
         * @return Timer rollover ticks
         *********************************************************************/
        protected long getTimerLow32Rollover()
        {
            return TimerLow32Rollover;
        }

        /**********************************************************************
         * Get the state
         * 
         * @return State
         *********************************************************************/
        protected long getState()
        {
            return State;
        }

        /**********************************************************************
         * Get the mode
         * 
         * @return Mode
         *********************************************************************/
        protected long getMode()
        {
            return Mode;
        }

        /**********************************************************************
         * Get the trigger count
         * 
         * @return Trigger count
         *********************************************************************/
        protected long getTriggerCount()
        {
            return TriggerCount;
        }

        /**********************************************************************
         * Get the index of the first log entry
         * 
         * @return Index of the first log entry
         *********************************************************************/
        protected long getDataStart()
        {
            return DataStart;
        }

        /**********************************************************************
         * Get the index of the last log entry
         * 
         * @return Index of the last log entry
         *********************************************************************/
        protected long getDataEnd()
        {
            return DataEnd;
        }

        /**********************************************************************
         * Get the number of log entries
         * 
         * @return Number of log entries
         *********************************************************************/
        protected long getDataCount()
        {
            return DataCount;
        }

        /**********************************************************************
         * Get the number of invalid markers reported
         * 
         * @return Number of invalid markers reported
         *********************************************************************/
        protected long getInvalidMarkerReported()
        {
            return InvalidMarkerReported;
        }

        /**********************************************************************
         * Get the trigger mask size
         * 
         * @return Trigger mask size
         *********************************************************************/
        protected long getFilterTriggerMaskSize()
        {
            return FilterTriggerMaskSize;
        }

        /**********************************************************************
         * Get the log file name
         * 
         * @return Log file name
         *********************************************************************/
        protected String getLogFileName()
        {
            return logFileName;
        }

        /**********************************************************************
         * Calculate the length of the log file header and metadata
         * 
         * @return Length of the log file header and metadata
         *********************************************************************/
        private int getHeaderLength()
        {
            return uint32 * 18
                   + uint8 * (4 + hdrDescMaxLen)
                   + uint32 * (int) FilterTriggerMaskSize * 2;
        }

        /**********************************************************************
         * Return a text representation of the filter mask as hexadecimal
         * values
         * 
         * @return The filter mask in hexadecimal
         *********************************************************************/
        protected String getFilterMask()
        {
            return getMask(FilterMask);
        }

        /**********************************************************************
         * Return a text representation of the trigger mask as hexadecimal
         * values
         * 
         * @return The trigger mask in hexadecimal
         *********************************************************************/
        protected String getTriggerMask()
        {
            return getMask(TriggerMask);
        }

        /**********************************************************************
         * Return a text representation of the mask as hexadecimal values
         * 
         * @param maskValue
         *            array of mask values
         * 
         * @return The mask values in hexadecimal
         *********************************************************************/
        private String getMask(long[] maskValue)
        {
            String maskText = "";

            // Step through each value in the mask array in reverse order so
            // that the byte pattern of the displayed array is in the order of
            // highest bit to lowest bit
            for (int index = (int) FilterTriggerMaskSize - 1; index >= 0; index--)
            {
                // Add the mask value to the string, formatted as hexadecimal
                maskText += String.format("%08x ", maskValue[index]);
            }

            // Remove the trailing space
            // mask = mask.trim();

            return maskText;
        }

        /**********************************************************************
         * Compare the log header time stamps (for sorting the logs)
         * 
         * @param lh
         *            performance ID class
         * @return 1 if this log's file time is before the input log's file
         *         time; 0 if the log times are identical; -1 if this log's
         *         file time is after the input log's file time
         *********************************************************************/
        @Override
        public int compareTo(LogHeader lh)
        {
            // Assume the time stamps are equal
            int compare = 0;

            // Check if this log's seconds is later than the comparison log's
            // seconds
            if (this.TimeSeconds > lh.TimeSeconds)
            {
                // This log is later than the comparison log
                compare = 1;
            }
            // Check if this log's seconds is earlier than the comparison log's
            // seconds
            else if (this.TimeSeconds < lh.TimeSeconds)
            {
                // This log is earlier than the comparison log
                compare = -1;
            }
            // The logs' seconds are equal; compare subseconds
            else
            {
                // Check if this log's subseconds is later than the comparison
                // log's seconds
                if (this.TimeSubSeconds > lh.TimeSubSeconds)
                {
                    // This log is later than the comparison log
                    compare = 1;
                }
                // Check if this log's subseconds is earlier than the
                // comparison log's seconds
                else if (this.TimeSubSeconds < lh.TimeSubSeconds)
                {
                    // This log is earlier than the comparison log
                    compare = -1;
                }
            }

            return compare;
        }
    }

    /**************************************************************************
     * CFS Performance Monitor converted log data class (more useful format)
     *************************************************************************/
    protected class PerfLog
    {
        // Performance ID for this log entry (with the high bit zeroed)
        private long perfID;

        // Log entry time stamp converted to double, seconds
        private double timeStamp;

        // False for an exit; true for an entry
        private boolean isEntry;

        // = 1 if consecutive entry (exit) events occur for this event
        private boolean isSequenceError;

        // Time, in seconds, that an active period (time between entry and exit
        // events) exceeds the time allotted, based on the ID frequency
        private double overrun;

        // User-supplied annotation for this data point
        private String notes;

        /**********************************************************************
         * Performance log constructor (no initial parameters)
         *********************************************************************/
        PerfLog()
        {
        }

        /**********************************************************************
         * Performance log constructor
         * 
         * @param perfID
         *            performance ID
         * 
         * @param timeStamp
         *            event time stamp, seconds
         * 
         * @param isEntry
         *            true if entry event; false if exit event
         * 
         * @param sequenceError
         *            true if last event logged for this ID is the same as this
         *            event
         * 
         * @param overrun
         *            frame overrun, seconds
         *********************************************************************/
        private PerfLog(long perfID,
                        double timeStamp,
                        boolean isEntry,
                        boolean isSequenceError,
                        double overrun)
        {
            this.perfID = perfID;
            this.timeStamp = timeStamp;
            this.isEntry = isEntry;
            this.isSequenceError = isSequenceError;
            this.overrun = overrun;
            this.notes = "";
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
         * Get the event time stamp
         * 
         * @return Event time stamp
         *********************************************************************/
        protected double getTimeStamp()
        {
            return timeStamp;
        }

        /**********************************************************************
         * Return the event type
         * 
         * @return true for an entry event; false for an exit event
         *********************************************************************/
        protected boolean isEntryEvent()
        {
            return isEntry;
        }

        /**********************************************************************
         * Return the sequence error flag
         * 
         * @return true for a sequence error
         *********************************************************************/
        protected boolean isSequenceError()
        {
            return isSequenceError;
        }

        /**********************************************************************
         * Get the overrun time
         * 
         * @return Frame overrun time, seconds
         *********************************************************************/
        protected double getOverrun()
        {
            return overrun;
        }

        /**********************************************************************
         * Get the data point notes
         * 
         * @return Data point notes
         *********************************************************************/
        protected String getNotes()
        {
            return notes;
        }

        /**********************************************************************
         * Set the data point notes
         * 
         * @param notes
         *            user-supplied data point notes
         *********************************************************************/
        protected void setNotes(String notes)
        {
            this.notes = notes;
        }
    }

    /**************************************************************************
     * Set the performance ID, main, and plot preferences variables
     * 
     * @param perfLog
     *            performance ID
     * @param perfMain
     *            main class
     * @param plotPrefs
     *            plot preferences
     *************************************************************************/
    protected void setGlobals(CPMIDHandler perfIDs,
                              CPMMain perfMain,
                              CPMPreferencesHandler plotPrefs)
    {
        this.perfIDs = perfIDs;
        this.perfMain = perfMain;
        this.plotPrefs = plotPrefs;
    }

    /**************************************************************************
     * Get the performance log data list
     * 
     * @return Performance log data list
     *************************************************************************/
    protected final List<PerfLog> getPerfLogData()
    {
        return perfLogData;
    }

    /**************************************************************************
     * Get the log header list
     * 
     * @return Performance log header list
     *************************************************************************/
    protected final List<LogHeader> getLogHeader()
    {
        return logHeader;
    }

    /**************************************************************************
     * Get the data gap index list
     * 
     * @return Performance log data gap index list
     *************************************************************************/
    protected final List<Integer> getDataGapIndex()
    {
        return dataGapIndex;
    }

    /**************************************************************************
     * Return true if there are any events in the log list
     * 
     * @return true if there are any events in the log list
     *************************************************************************/
    protected boolean isLogData()
    {
        return !perfLogData.isEmpty();
    }

    /**************************************************************************
     * Return true if a log was successfully read
     * 
     * @return true if a log was successfully read
     *************************************************************************/
    protected boolean isLogRead()
    {
        return logRead;
    }

    /**************************************************************************
     * Get the latest log entry time stamp
     * 
     * @return Latest log entry's time stamp
     *************************************************************************/
    protected double getMaximumTimeStamp()
    {
        return maximumTimeStamp;
    }

    /**************************************************************************
     * Get the time span of log data minus any gaps
     * 
     * @return Time span of log data minus any gaps
     *************************************************************************/
    protected double getAdjustedDataTimeSpan()
    {
        return adjustedDataTimeSpan;
    }

    /**************************************************************************
     * Get the total number of entry events logged
     * 
     * @return Total number of entry events logged
     *************************************************************************/
    protected int getTotalOnCount()
    {
        return totalOnCount;
    }

    /**************************************************************************
     * Get the total number of exit events logged
     * 
     * @return Total number of exit events logged
     *************************************************************************/
    protected int getTotalOffCount()
    {
        return totalOffCount;
    }

    /**************************************************************************
     * Get the average frequency of all events
     * 
     * @return Average frequency of all events, entry events/second
     *************************************************************************/
    protected Double getTotalAvgFreq()
    {
        return totalAvgFreq;
    }

    /**************************************************************************
     * Get the total time that the IDs are active
     * 
     * @return Total time that the IDs are active, seconds
     *************************************************************************/
    protected Double getTotalTimeActive()
    {
        return totalTimeActive;
    }

    /**************************************************************************
     * Get the minimum time active for all IDs
     * 
     * @return Minimum time active for all IDs, seconds
     *************************************************************************/
    protected Double getTotalMinTimeActive()
    {
        return totalMinTimeActive;
    }

    /**************************************************************************
     * Get the maximum time active for all IDs
     * 
     * @return Maximum time active for all IDs, seconds
     *************************************************************************/
    protected Double getTotalMaxTimeActive()
    {
        return totalMaxTimeActive;
    }

    /**************************************************************************
     * Get the minimum interval between entry events for all IDs
     * 
     * @return Minimum interval between entry events for all IDs, seconds
     *************************************************************************/
    protected Double getTotalMinInterval()
    {
        return totalMinInterval;
    }

    /**************************************************************************
     * Get the maximum interval between entry events for all IDs
     * 
     * @return Maximum interval between entry events for all IDs, seconds
     *************************************************************************/
    protected Double getTotalMaxInterval()
    {
        return totalMaxInterval;
    }

    /**************************************************************************
     * Get the log time stamp for the minimum time active for all IDs
     * 
     * @return Log time stamp for the minimum time active for all IDs, seconds
     *************************************************************************/
    protected long getTotalMinTimeActiveID()
    {
        return totalMinTimeActiveID;
    }

    /**************************************************************************
     * Get the log time stamp for the maximum time active for all IDs
     * 
     * @return Log time stamp for the maximum time active for all IDs, seconds
     *************************************************************************/
    protected long getTotalMaxTimeActiveID()
    {
        return totalMaxTimeActiveID;
    }

    /**************************************************************************
     * Get the log time stamp for the minimum interval between entry events for
     * all IDs
     * 
     * @return Log time stamp for the minimum interval between entry events for
     *         all IDs, seconds
     *************************************************************************/
    protected long getTotalMinIntervalID()
    {
        return totalMinIntervalID;
    }

    /**************************************************************************
     * Get the log time stamp for the maximum interval between entry events for
     * all IDs
     * 
     * @return Log time stamp for the maximum interval between entry events for
     *         all IDs, seconds
     *************************************************************************/
    protected long getTotalMaxIntervalID()
    {
        return totalMaxIntervalID;
    }

    /**************************************************************************
     * Get the minimum frame overrun for all IDs
     * 
     * @return Minimum frame overrun for all IDs
     *************************************************************************/
    protected Double getTotalMinOverrun()
    {
        return totalMinOverrun;
    }

    /**************************************************************************
     * Get the maximum frame overrun for all IDs
     * 
     * @return Maximum frame overrun for all IDs
     *************************************************************************/
    protected Double getTotalMaxOverrun()
    {
        return totalMaxOverrun;
    }

    /**************************************************************************
     * Get the ID with the minimum frame overrun for all IDs
     * 
     * @return ID with the minimum frame overrun for all IDs
     *************************************************************************/
    protected long getTotalMinOverrunID()
    {
        return totalMinOverrunID;
    }

    /**************************************************************************
     * Get the ID with the maximum frame overrun for all IDs
     * 
     * @return ID with the maximum frame overrun for all IDs
     *************************************************************************/
    protected long getTotalMaxOverrunID()
    {
        return totalMaxOverrunID;
    }

    /**************************************************************************
     * Get the log time stamp for the minimum time active for all IDs
     * 
     * @return Log time stamp for the minimum time active for all IDs, seconds
     *************************************************************************/
    protected Double getTotalMinTimeOnTimeStamp()
    {
        return totalMinTimeOnTimeStamp;
    }

    /**************************************************************************
     * Get the log time stamp for the maximum time active for all IDs
     * 
     * @return Log time stamp for the maximum time active for all IDs, seconds
     *************************************************************************/
    protected Double getTotalMaxTimeOnTimeStamp()
    {
        return totalMaxTimeOnTimeStamp;
    }

    /**************************************************************************
     * Get the log time stamp for the minimum interval between entry events
     * 
     * @return Log time stamp for the minimum interval between entry events,
     *         seconds
     *************************************************************************/
    protected Double getTotalMinIntervalTimeStamp()
    {
        return totalMinIntervalTimeStamp;
    }

    /**************************************************************************
     * Get the log time stamp for the maximum interval between entry events
     * 
     * @return Log time stamp for the maximum interval between entry events,
     *         seconds
     *************************************************************************/
    protected Double getTotalMaxIntervalTimeStamp()
    {
        return totalMaxIntervalTimeStamp;
    }

    /**************************************************************************
     * Get the log time stamp for the minimum frame overrun for all IDs
     * 
     * @return Log time stamp for the minimum frame overrun for all IDs,
     *         seconds
     *************************************************************************/
    protected Double getTotalMinOverrunTimeStamp()
    {
        return totalMinOverrunTimeStamp;
    }

    /**************************************************************************
     * Get the log time stamp for the maximum frame overrun for all IDs
     * 
     * @return Log time stamp for the maximum frame overrun for all IDs,
     *         seconds
     *************************************************************************/
    protected Double getTotalMaxOverrunTimeStamp()
    {
        return totalMaxOverrunTimeStamp;
    }

    /**************************************************************************
     * Get the maximum number of IDs (equivalent to the maximum ID value + 1)
     * 
     * @return Maximum number of IDs/maximum ID value + 1
     *************************************************************************/
    protected long getMaximumIDs()
    {
        return maximumIDs;
    }

    /**************************************************************************
     * Get the status of the frame overrun flag
     * 
     * @return true if at least one frame overrun was detected
     *************************************************************************/
    protected boolean isOverrun()
    {
        return overrunFlag;
    }

    /**************************************************************************
     * Check the filter mask for the indicated performance ID and return true
     * if the ID is enabled in the mask in at least one log file
     * 
     * @param perfID
     *            performance ID
     * 
     * @return true if the ID is in the filter mask
     *************************************************************************/
    protected boolean isIDFilterMaskState(long perfID)
    {
        boolean inMask = false;

        // Step through the log headers
        for (LogHeader lh : logHeader)
        {
            // Check if the bit is set for this ID
            if (isIDInMask(perfID, lh.FilterMask))
            {
                // ID is in the mask; set the flag to true
                inMask = true;
                break;
            }
        }

        return inMask;
    }

    /**************************************************************************
     * Check the trigger mask for the indicated performance ID and return true
     * if the ID is enabled in the mask in at least one log file
     * 
     * @param perfID
     *            performance ID
     * 
     * @return true if the ID is in the trigger mask
     *************************************************************************/
    protected boolean isIDTriggerMaskState(long perfID)
    {
        boolean inMask = false;

        // Step through the log headers
        for (LogHeader lh : logHeader)
        {
            // Check if the bit is set for this ID
            if (isIDInMask(perfID, lh.TriggerMask))
            {
                // ID is in the mask; set the flag to true
                inMask = true;
                break;
            }
        }

        return inMask;
    }

    /**************************************************************************
     * Check the mask for the indicated performance ID and return true if the
     * ID is enabled in the mask
     * 
     * @param perfID
     *            performance ID
     * 
     * @param mask
     *            filter or trigger mask array
     * 
     * @return true if the ID is in the mask
     *************************************************************************/
    private boolean isIDInMask(long perfID, long[] mask)
    {
        boolean inMask;

        // Check if the ID is greater than the maximum ID value
        if (perfID > maximumIDs)
        {
            // The ID is cannot be in the filter mask
            inMask = false;
        }
        // The ID is in the range of the mask
        else
        {
            // Set the return flag to true if the mask bit is set for this ID
            inMask = ((mask[(int) (perfID / 32)]) & (1L << (perfID % 32))) != 0;
        }

        return inMask;
    }

    /**************************************************************************
     * Read one or more log files and concatenate the data if needed
     * 
     * @param fileNames
     *            array of log file path + file name(s)
     * 
     * @param clearOrAppend
     *            CLEAR_WITHOUT_ASKING to clear the currently loaded log data
     *            without asking; ALLOW_APPEND to allow appending the new Logs
     *            to the current log data
     * 
     * @return Selected log file path + file name(s)
     *************************************************************************/
    protected String[] readLog(String[] fileNames, ClearOption clearOrAppend)
    {
        List<String> selectedFileName = new ArrayList<String>();
        long maxTicksPerSec = 1;
        int buttonPressed = CLEAR_OPTION;

        // Assume that the log file hasn't been successfully read until it is
        // successfully read
        logRead = false;

        // Check if the user has the option to retain the log data, if it
        // already exists
        if (clearOrAppend == ALLOW_APPEND && !perfLogData.isEmpty())
        {
            // Ask the user to append to or clear the current log data
            buttonPressed = new CPMDialogHandler().showOptionsDialog(perfMain.getMainFrame(),
                                                                     "<html><b>Append to or clear existing log data?",
                                                                     "Append/Clear Log");
        }

        // Check if the Cancel button wasn't pressed
        if (buttonPressed != CANCEL_OPTION)
        {
            // If a performance log file name list is supplied then use it,
            // else allow the user to choose one or more log files
            File[] logFile = new CPMDialogHandler().choosePathFile(perfMain,
                                                                   fileNames,
                                                                   false,
                                                                   true,
                                                                   "Read Performance Log(s)",
                                                                   PERF_LOG_FILE,
                                                                   READ_OPTION);

            // Check if at least one file was selected
            if (logFile[0] != null)
            {
                // Check if the user elected to append the new log data to the
                // existing data
                if (buttonPressed == APPEND_OPTION)
                {
                    // Create file handle(s) for the existing log file(s). By
                    // providing a file name list only the file handles are
                    // created and no dialog is displayed
                    File[] previousLogFile = new CPMDialogHandler().choosePathFile(perfMain,
                                                                                   perfMain.getPreferences().get(PERF_LOG_FILE, "").split(LOG_SEPARATOR),
                                                                                   false,
                                                                                   true,
                                                                                   "",
                                                                                   "",
                                                                                   0);

                    // Create a collection in order to combine the previous and
                    // new log file arrays into a single array, then add the
                    // previous log files to the collection
                    Collection<File> allLogFiles = new ArrayList<File>();
                    allLogFiles.addAll(Arrays.asList(previousLogFile));

                    // Step through the new log(s) to be added
                    for (int index = 0; index < logFile.length; index++)
                    {
                        // Check that the log isn't already in the collection
                        // so that the same log isn't loaded in multiple times
                        if (!allLogFiles.contains(logFile[index]))
                        {
                            // Add the log file to the collection
                            allLogFiles.add(logFile[index]);
                        }
                    }

                    // Convert the collection of log files into an array
                    logFile = allLogFiles.toArray(new File[] {});
                }

                // Remove the log header information, if it exists from reading
                // a previous log
                logHeader.removeAll(logHeader);

                // Sort the log files based on their time stamps
                for (int logIndex = 0; logIndex < logFile.length; logIndex++)
                {
                    try
                    {
                        // Open the log file
                        perfLogFile = new FileInputStream(logFile[logIndex]);

                        // Read the log file header and add it to the list of
                        // log headers
                        int maxIDs = readHeader(logFile[logIndex].getAbsolutePath());

                        // Check if no error occurred reading the log header
                        if (maxIDs != -1)
                        {
                            // Check if this log has more ID space than
                            // previous ones (if any)
                            if (maxIDs > maximumIDs)
                            {
                                // Save the new maximum IDs number
                                maximumIDs = maxIDs;
                            }

                            // Check if the ticks per second for this log are
                            // greater than the largest found so far. The list
                            // size is used instead of the loop index in case a
                            // log is skipped (due to an error reading the
                            // header)
                            if (logHeader.get(logHeader.size() - 1).TimerTicksPerSecond > maxTicksPerSec)
                            {
                                // Save the ticks per second
                                maxTicksPerSec = logHeader.get(logHeader.size() - 1).TimerTicksPerSecond;
                            }

                            try
                            {
                                // Close the log file
                                perfLogFile.close();
                            }
                            catch (IOException ioe)
                            {
                                // Inform the user that the file cannot be
                                // closed
                                new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                         "<html><b>Cannot close log file<br></b>"
                                                                             + logFile[logIndex].getAbsolutePath(),
                                                                         "File Warning",
                                                                         JOptionPane.WARNING_MESSAGE,
                                                                         OK_OPTION);
                            }
                        }
                    }
                    catch (FileNotFoundException fnfe)
                    {
                        // Inform the user that the log file cannot be found
                        new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                 "<html><b>Cannot locate log file<br></b>"
                                                                     + logFile[logIndex].getAbsolutePath(),
                                                                 "File Error",
                                                                 JOptionPane.ERROR_MESSAGE,
                                                                 OK_OPTION);
                    }
                }

                // Sort the log headers by their time stamps so that they'll be
                // read in the correct order and so that they'll appear in the
                // sorted order in the header dialog
                Collections.sort(logHeader);

                // Read the sorted list of log files
                for (int logIndex = 0; logIndex < logHeader.size(); logIndex++)
                {
                    // Get the log entry to shorten the calls later
                    LogHeader log = logHeader.get(logIndex);

                    // Create a handle to the log file. This is done on top of
                    // the original file array
                    logFile[logIndex] = new File(log.logFileName);

                    try
                    {
                        // Check if this is the first pass
                        if (logIndex == 0)
                        {
                            // Remove the log data that was previously opened
                            // (if any) before reading the first file
                            perfLogData.removeAll(perfLogData);

                            // Remove the log data gap list (if extant)
                            dataGapIndex.removeAll(dataGapIndex);
                            dataGapLength.removeAll(dataGapLength);
                        }
                        // This isn't the first log file read in this series
                        else if (!perfLogData.isEmpty())
                        {
                            // Store the index of the first entry from the
                            // current log. The gap is from this entry to the
                            // last entry of the previous log
                            dataGapIndex.add(perfLogData.size());
                        }

                        // Open the log file
                        perfLogFile = new FileInputStream(logFile[logIndex]);

                        // Read the log entries
                        boolean errorFlag = readDataEntryBuffer(log,
                                                                perfLogData.isEmpty());

                        // Check that no error was detected
                        if (!errorFlag)
                        {
                            // Strip the path from the log file name stored in
                            // the header data for display in the header dialog
                            log.logFileName = getLogFileName(log.logFileName);

                            // Save the log file name
                            selectedFileName.add(logFile[logIndex].getAbsolutePath());

                            // Read the notes file associated with this log
                            readLogNotes(logIndex);

                            // Set the flag indicating that at least one log
                            // was successfully read
                            logRead = true;

                        }
                        // An error occurred reading the log file
                        else
                        {
                            // Inform the user that an error occurred reading
                            // the log file
                            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                     "<html><b>Cannot read log file<br></b>"
                                                                         + logFile[logIndex].getAbsolutePath(),
                                                                     "File Error",
                                                                     JOptionPane.ERROR_MESSAGE,
                                                                     OK_OPTION);
                        }

                        try
                        {
                            // Close the log file
                            perfLogFile.close();
                        }
                        catch (IOException ioe)
                        {
                            // Inform the user that the log file didn't close
                            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                     "<html><b>Cannot close log file<br></b>"
                                                                         + logFile[logIndex].getAbsolutePath(),
                                                                     "File Warning",
                                                                     JOptionPane.WARNING_MESSAGE,
                                                                     OK_OPTION);
                        }
                    }
                    catch (FileNotFoundException fnfe)
                    {
                        // Inform the user that the log file cannot be found
                        new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                                 "<html><b>Cannot locate log file<br></b>"
                                                                     + logFile[logIndex].getAbsolutePath(),
                                                                 "File Error",
                                                                 JOptionPane.ERROR_MESSAGE,
                                                                 OK_OPTION);
                    }
                }

                // Check if any logs were successfully read
                if (logRead)
                {
                    // Save the last (latest) time stamp
                    maximumTimeStamp = perfLogData.get(perfLogData.size() - 1).timeStamp;
                    adjustedDataTimeSpan = maximumTimeStamp;

                    // Calculate the precision for time and delta time output
                    setTimePrecision(String.valueOf(maxTicksPerSec - 1).length());

                    // Step through the data gap list
                    for (int index = 0; index < dataGapIndex.size(); index++)
                    {
                        // Calculate the time difference between the end of the
                        // last log and the beginning of the current log. An
                        // additional amount is included equal to the length of
                        // a single clock tick in order to space the previous
                        // log's last entry and the current log's first entry
                        double gapSpan = perfLogData.get(dataGapIndex.get(index)).timeStamp
                                         - perfLogData.get(dataGapIndex.get(index) - 1).timeStamp
                                         - 1.0 / logHeader.get(0).TimerTicksPerSecond;

                        // Store the gap length
                        dataGapLength.add(gapSpan);

                        // Subtract the data gap from the total time span
                        adjustedDataTimeSpan -= gapSpan;
                    }

                    // Count the number of IDs referenced in the log and
                    // display the result
                    countIDsInLog();

                    // Match log entries to the performance ID list
                    perfIDs.mapLogToIDList(true, false);

                    // Check the log data for inconsistencies
                    checkLogConsistency();

                    // Check if the data gaps are to be hidden initially
                    if (!plotPrefs.isShowDataGaps(false))
                    {
                        // Remove the data gaps
                        adjustDataGaps(false);
                    }
                }
            }
        }

        return selectedFileName.toArray(new String[0]);
    }

    /**************************************************************************
     * Count the number of unique performance IDs referenced in the log and
     * display a dialog showing the result
     *************************************************************************/
    protected void countIDsInLog()
    {
        int idCount = 0;

        // Create a list to contain the IDs as they're found
        List<Long> foundID = new ArrayList<Long>();

        // Step through the log
        for (PerfLog log : perfLogData)
        {
            // Check if this ID hasn't been counted
            if (!foundID.contains(log.perfID))
            {
                // Increment the ID counter and flag this ID as 'found' by
                // adding it to the list
                idCount++;
                foundID.add(log.perfID);
            }
        }

        // Inform the user of the number of IDs referenced in the log
        new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                 "<html><b>"
                                                     + idCount
                                                     + " IDs referenced in the log(s)",
                                                 "Log ID Count",
                                                 JOptionPane.INFORMATION_MESSAGE,
                                                 OK_OPTION);
    }

    /**************************************************************************
     * Extract the log file name(s) from the log file path+name string. If a
     * single path+name is provided then the log file minus the path is
     * returned. If multiple path+names, separated by delimiters, are provided
     * then the list of logs, minus the paths and separated by commas, is
     * returned
     * 
     * @param pathName
     *            log file path + file names, separated by commas
     * 
     * @return Log file names (no path), separated by commas
     *************************************************************************/
    protected String getLogFileName(String pathName)
    {
        // Split up the list of log names into an array
        String[] logNames = pathName.split(LOG_SEPARATOR);
        String logFileName = "";

        // For each log file name
        for (int index = 0; index < logNames.length; index++)
        {
            // Check if this isn't the first file if more than one was provided
            if (index != 0)
            {
                // Append the file name to the string, separated by a comma
                logFileName += ", ";
            }

            // Extract the name, minus the path, and append it to the log list
            logFileName += logNames[index].substring(logNames[index].lastIndexOf(File.separator) + 1);
        }

        // Return the file name(s)
        return logFileName;
    }

    /**************************************************************************
     * Read a single parameter from the log
     * 
     * @param numBytes
     *            number of bytes to read from the file
     * 
     * @return Array of bytes read
     *************************************************************************/
    private byte[] readParameter(int numBytes)
    {
        byte[] bData = null;

        try
        {
            // Check that the number of bytes needed are present in the file,
            // otherwise the call will block (the program will wait forever)
            if (perfLogFile.available() >= numBytes)
            {
                // Read the parameter
                bData = new byte[numBytes];
                perfLogFile.read(bData, 0, numBytes);
            }
        }
        catch (IOException ioe)
        {
            // The error message is annunciated if this method returns a null,
            // so no message is needed here
        }

        return bData;
    }

    /**************************************************************************
     * Read the performance log header and ad it to the list of headers
     * 
     * @param fileName
     *            log path + file name
     * 
     * @return Maximum ID value for this log file or -1 if the header cannot be
     *         read
     *************************************************************************/
    private int readHeader(String fileName)
    {
        String errorType = "";
        int maxIDs = -1;

        // Create a new log header
        LogHeader lh = new LogHeader();

        // Create storage for the raw header parameters
        byte[][] bHdrData = {new byte[uint32],
                             new byte[uint32],
                             new byte[uint32],
                             new byte[uint32],
                             new byte[uint32],
                             new byte[uint32],
                             new byte[uint32],
                             new byte[uint32]};

        // For each parameter in the log header
        for (int index = 0; index < bHdrData.length && errorType.isEmpty(); index++)
        {
            // Read the header parameter
            if ((bHdrData[index] = readParameter(bHdrData[index].length)) == null)
            {
                // Reading the parameter was unsuccessful; note the error and
                // stop reading
                errorType = "Error reading header data";
                break;
            }
        }

        // Check that the header parameters were successfully read from the
        // log file
        if (errorType.isEmpty())
        {
            // Check if the first four bytes don't match the pattern that cFE
            // uses to delineate cFE compliant log files
            if (byteArrayToLong(bHdrData[0],
                                ByteOrder.BIG_ENDIAN) != CFE_FS_FILE_CONTENT_ID)
            {
                // The content type doesn't match the expected pattern; note
                // the error
                errorType = "Content type mismatch";
            }
            // The pattern matches
            else
            {
                // Header - convert the data from raw bytes to the proper
                // format. cFE always stores the header values in big endian
                // order
                lh.ContentType = new String(bHdrData[0]);
                lh.SubType = byteArrayToLong(bHdrData[1], ByteOrder.BIG_ENDIAN);
                lh.Length = byteArrayToLong(bHdrData[2], ByteOrder.BIG_ENDIAN);
                lh.SpacecraftID = byteArrayToLong(bHdrData[3],
                                                  ByteOrder.BIG_ENDIAN);
                lh.ProcessorID = byteArrayToLong(bHdrData[4],
                                                 ByteOrder.BIG_ENDIAN);
                lh.ApplicationID = byteArrayToLong(bHdrData[5],
                                                   ByteOrder.BIG_ENDIAN);
                lh.TimeSeconds = byteArrayToLong(bHdrData[6],
                                                 ByteOrder.BIG_ENDIAN);
                lh.TimeSubSeconds = byteArrayToLong(bHdrData[7],
                                                    ByteOrder.BIG_ENDIAN);

                try
                {
                    // Calculate the header description length using the total
                    // header size minus the size of the other header
                    // parameters
                    hdrDescMaxLen = (int) lh.Length - uint32 * bHdrData.length;

                    // Create storage for the raw header description
                    byte[] bHdrDesc = new byte[uint8 * hdrDescMaxLen];

                    // Read the header description
                    if ((bHdrDesc = readParameter(bHdrDesc.length)) == null)
                    {
                        // Reading the parameter was unsuccessful; note the
                        // error
                        errorType = "Error reading description";
                    }
                    // No error occurred reading the header description
                    else
                    {
                        // Convert the header description from raw bytes
                        lh.Description = new String(bHdrDesc).replaceAll("[^\\p{Print}]",
                                                                         "");

                        // Create storage for the raw metadata parameters
                        byte[][] bMetaData = {new byte[uint8],
                                              new byte[uint8],
                                              new byte[uint8 * 2], // Spare
                                              new byte[uint32],
                                              new byte[uint32],
                                              new byte[uint32],
                                              new byte[uint32],
                                              new byte[uint32],
                                              new byte[uint32],
                                              new byte[uint32],
                                              new byte[uint32],
                                              new byte[uint32],
                                              new byte[uint32]};

                        // For each parameter in the log header
                        for (int index = 0; index < bMetaData.length && errorType.isEmpty(); index++)
                        {
                            // Read the header parameter
                            if ((bMetaData[index] = readParameter(bMetaData[index].length)) == null)
                            {
                                // Reading the parameter was unsuccessful; note
                                // the error and stop reading
                                errorType = "Error reading metadata";
                                break;
                            }
                        }

                        // Check that the metadata parameters were successfully
                        // read from the log file
                        if (errorType.isEmpty())
                        {
                            // Metadata - convert the data from raw bytes to
                            // the proper format
                            lh.Version = ByteBuffer.wrap(bMetaData[0]).get();
                            lh.Endian = ByteBuffer.wrap(bMetaData[1]).get();
                            // bMetaData[2] - Skip the spare bytes
                            lh.TimerTicksPerSecond = byteArrayToLong(bMetaData[3],
                                                                     getEndianess(lh.Endian));
                            lh.TimerLow32Rollover = byteArrayToLong(bMetaData[4],
                                                                    getEndianess(lh.Endian));
                            lh.State = byteArrayToLong(bMetaData[5],
                                                       getEndianess(lh.Endian));
                            lh.Mode = byteArrayToLong(bMetaData[6],
                                                      getEndianess(lh.Endian));
                            lh.TriggerCount = byteArrayToLong(bMetaData[7],
                                                              getEndianess(lh.Endian));
                            lh.DataStart = byteArrayToLong(bMetaData[8],
                                                           getEndianess(lh.Endian));
                            lh.DataEnd = byteArrayToLong(bMetaData[9],
                                                         getEndianess(lh.Endian));
                            lh.DataCount = byteArrayToLong(bMetaData[10],
                                                           getEndianess(lh.Endian));
                            lh.InvalidMarkerReported = byteArrayToLong(bMetaData[11],
                                                                       getEndianess(lh.Endian));
                            lh.FilterTriggerMaskSize = byteArrayToLong(bMetaData[12],
                                                                       getEndianess(lh.Endian));

                            // Calculate the maximum IDs that can be handled
                            // based on the size allotted for the filter &
                            // trigger masks. Each bit represents an ID. The
                            // calculation is: maximum # of IDs = # of elements
                            // in the mask array * # of bytes/element * # of
                            // bits/byte
                            maxIDs = (int) lh.FilterTriggerMaskSize * uint32 * 8;

                            // With the mask size known the storage for the raw
                            // bytes for the filter and trigger mask parameters
                            // can be assigned
                            byte[] bFilterMask = new byte[uint32
                                                          * (int) lh.FilterTriggerMaskSize];
                            byte[] bTriggerMask = new byte[uint32
                                                           * (int) lh.FilterTriggerMaskSize];

                            // Read the filter and trigger mask parameters
                            if ((bFilterMask = readParameter(bFilterMask.length)) == null
                                || (bTriggerMask = readParameter(bTriggerMask.length)) == null)
                            {
                                // Reading the parameter was unsuccessful; note
                                // the error
                                maxIDs = -1;
                                errorType = "Error reading masks";
                            }
                            // No error occurred reading the mask parameters
                            else
                            {
                                // Create storage for the mask value arrays
                                lh.FilterMask = new long[(int) lh.FilterTriggerMaskSize];
                                lh.TriggerMask = new long[(int) lh.FilterTriggerMaskSize];

                                // Step through each array index
                                for (int index = 0; index < lh.FilterTriggerMaskSize; index++)
                                {
                                    // Convert the mask data from raw bytes to
                                    // the proper format
                                    lh.FilterMask[index] = byteArrayToLong(Arrays.copyOfRange(bFilterMask,
                                                                                              uint32 * index,
                                                                                              uint32 * (index + 1)),
                                                                           getEndianess(lh.Endian));
                                    lh.TriggerMask[index] = byteArrayToLong(Arrays.copyOfRange(bTriggerMask,
                                                                                               uint32 * index,
                                                                                               uint32 * (index + 1)),
                                                                            getEndianess(lh.Endian));
                                }

                                // Store the log file name
                                lh.logFileName = fileName;

                                // Add this log header to the list of headers
                                logHeader.add(lh);
                            }
                        }
                    }
                }
                catch (OutOfMemoryError ome)
                {
                    // The header length parameter is too large, possibly due
                    // to a corrupted file
                    errorType = "Out of memory error";
                }
            }
        }

        // Check if the header wasn't read successfully
        if (!errorType.isEmpty())
        {
            // Inform the user that the log file header cannot be read
            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                     "<html><b>"
                                                         + errorType
                                                         + " for log file<br></b>"
                                                         + fileName,
                                                     "Header Error",
                                                     JOptionPane.ERROR_MESSAGE,
                                                     OK_OPTION);
        }

        return maxIDs;
    }

    /**************************************************************************
     * Convert a byte array, accounting for endianess, to an unsigned integer
     * 
     * @param byteArray
     *            array of bytes
     * 
     * @param endianess
     *            byte order for the data (ByteOrder.BIG_ENDIAN or
     *            ByteOrder.LITTLE_ENDIAN)
     * 
     * @return Integer representation of the byte array
     *************************************************************************/
    private long byteArrayToLong(byte[] byteArray, ByteOrder endianess)
    {
        // Convert the byte array to a buffer and arrange the buffer's byte
        // order based on the endianess
        ByteBuffer byteBuffer = ByteBuffer.wrap(byteArray).order(endianess);

        // The input value is an unsigned 32-bit integer. A large enough value
        // would show incorrectly since the high bit would be interpreted as
        // the sign. To prevent the sign rollover the value is converted to a
        // long. This is done by first converting the buffer to an integer,
        // then to a binary in string format. The binary value is converted to
        // a long integer, eliminating the sign
        return Long.parseLong(Integer.toBinaryString(byteBuffer.getInt()), 2);
    }

    /**************************************************************************
     * Convert the endianess flag in the header to a ByteOrder flag
     * 
     * @param endian
     *            1 for big endian; 0 for little endian
     * 
     * @return Byte order for the data (ByteOrder.BIG_ENDIAN or
     *         ByteOrder.LITTLE_ENDIAN)
     *************************************************************************/
    private ByteOrder getEndianess(int endian)
    {
        return endian == 1
                          ? ByteOrder.BIG_ENDIAN
                          : ByteOrder.LITTLE_ENDIAN;
    }

    /**************************************************************************
     * Read the performance log data entries
     * 
     * @param lh
     *            log header
     * 
     * @param firstFile
     *            true if this is the first file read
     * 
     * @return true if an error occurred
     *************************************************************************/
    private boolean readDataEntryBuffer(LogHeader lh, boolean firstFile)
    {
        boolean errorFlag = false;
        int index = 0;
        FileChannel fc = perfLogFile.getChannel();

        try
        {
            // Locate the starting data entry to get the base time
            fc.position(lh.getHeaderLength() + uint32 * 3 * lh.DataStart);

            // To get the data entries sorted in order, first read from the
            // DataStart position to the end of the file
            while (lh.DataCount - lh.DataStart > index)
            {
                // Read the data entry from the log file. Store the base time
                // if this is the first log file's first data entry
                errorFlag = readSingleDataEntry(firstFile && index == 0, lh);

                index++;
            }

            // Reset the file pointer back to the beginning of the file
            // (after the header & metadata)
            fc.position(lh.getHeaderLength());

            // Finish reading the file from the beginning of the file (after
            // the header) to the DataEnd position
            while (index < lh.DataCount)
            {
                // Read the data entry from the log file
                errorFlag = readSingleDataEntry(firstFile && index == 0, lh);
                index++;
            }
        }
        catch (IOException ioe)
        {
            // Error occurred when repositioning the file pointer
            errorFlag = true;
        }

        return errorFlag;
    }

    /**************************************************************************
     * Read a single performance log data entry and convert it into a
     * performance data entry. Store the log entry's time stamp as the base
     * time if this is the first log entry processed
     * 
     * @param firstTimeStamp
     *            true if this is the first log entry time stamp read
     * 
     * @param lh
     *            log header
     * 
     * @return true if an error occurred
     *************************************************************************/
    private boolean readSingleDataEntry(boolean firstTimeStamp, LogHeader lh)
    {
        boolean errorCode = false;

        // Create a new log entry to hold the data
        PerfLog perfEntry = new PerfLog();
        long TimerUpper32;
        long TimerLower32;

        // Array to store the raw bytes for each event entry parameter, based
        // on each parameter's size
        byte[][] bData = {new byte[uint32],
                          new byte[uint32],
                          new byte[uint32]};

        // For each parameter in the event entry
        for (int index = 0; index < bData.length; index++)
        {
            // Read the event entry parameter
            if ((bData[index] = readParameter(bData[index].length)) == null)
            {
                // Reading the parameter was unsuccessful; flag the error
                errorCode = true;
            }
        }

        // Check that event entry parameters were successfully read from the
        // log file
        if (!errorCode)
        {
            // Convert the data from raw bytes to the proper format. Cast the
            // performance ID to an integer (4-bytes); since the ID is always
            // used as a hexadecimal value the sign issue is avoided
            perfEntry.perfID = (int) byteArrayToLong(bData[0],
                                                     getEndianess(lh.Endian));
            TimerUpper32 = byteArrayToLong(bData[1],
                                           getEndianess(lh.Endian));
            TimerLower32 = byteArrayToLong(bData[2],
                                           getEndianess(lh.Endian));

            // Save the time stamp as the base time if this is the initial data
            // entry in the first log file processed
            if (firstTimeStamp)
            {
                baseTimeUpper = TimerUpper32;
                baseTimeLower = TimerLower32;
            }

            // Adjust the time stamp using the base time as 0 seconds. The base
            // time is the time stamp of the first data entry in the log file
            // (if multiple files then the earliest file determines the base
            // time)
            if (TimerLower32 < baseTimeLower)
            {
                TimerLower32 += lh.TimerTicksPerSecond;
                TimerUpper32--;
            }

            TimerLower32 -= baseTimeLower;
            TimerUpper32 -= baseTimeUpper;

            // If high bit it set then this is an exit (off) condition;
            // otherwise it's an entry (on) condition
            perfEntry.isEntry = (perfEntry.perfID & EXIT_MASK) == 0;

            // Convert the time stamp to floating point seconds
            perfEntry.timeStamp = TimerUpper32 +
                                  (TimerLower32 /
                                  (double) lh.TimerTicksPerSecond);

            // Add the log entry to the data list. Store the ID without the bit
            // that designates the event as an entry or exit event - this
            // information is stored in a separate parameter
            perfLogData.add(new PerfLog(perfEntry.perfID & ID_MASK,
                                        perfEntry.timeStamp,
                                        perfEntry.isEntry,
                                        false,
                                        0.0));
        }

        return errorCode;
    }

    /**************************************************************************
     * Check the log for possible inconsistencies such as multiple entry events
     * without an exit in between or vice versa or negative delta time values
     *************************************************************************/
    private void checkLogConsistency()
    {
        int negativeTimeCount = 0;
        int consecutiveEventCount = 0;
        double previousTime = 0.0;

        // Null the ID used flags. This flag is used to track the entry/exit
        // state of the IDs. The value is initially set to null so that the
        // test is skipped on the first instance of the ID
        perfIDs.resetIDUsed(null);

        // Step through the log data. Since sequenceError can be modified,
        // can't use an enhanced for loop
        for (int index = 0; index < perfLogData.size(); index++)
        {
            // Use this to shorten the variable references later in the loop
            PerfLog log = perfLogData.get(index);

            // Check if the time stamp precedes the time stamp for the
            // previous event
            if (log.timeStamp < previousTime)
            {
                // Backwards jump detected; increment the counter
                negativeTimeCount++;
            }

            // Save the current time stamp for the next pass
            previousTime = log.timeStamp;

            // Get the performance ID associated with the log ID
            PerformanceID pid = perfIDs.getPerfID(log.perfID);

            // Check if the previous event (entry or exit) for this ID is the
            // same as the current event for this ID
            if (pid.isUsed() != null && pid.isUsed() == log.isEntry)
            {
                // Consecutive entry events or exits for the same ID; set the
                // error flag and increment the counter
                log.isSequenceError = true;
                consecutiveEventCount++;
            }

            // Save the current state for the next pass for this ID
            pid.setUsed(log.isEntry);
        }

        // Check if any inconsistencies were detected
        if (negativeTimeCount != 0
            || consecutiveEventCount != 0)
        {
            // Build the warning message
            String warningMessage = "<html><b>Log inconsistencies detected:</b><br><br>";

            if (negativeTimeCount != 0)
            {
                String inner = "";

                // Account for singular versus plural
                if (negativeTimeCount > 1)
                {
                    inner = "s";
                }

                warningMessage += String.format("- %d instance"
                                                + inner
                                                + " of a time<br>&#160&#160&#160stamp occurring out of order<br>",
                                                negativeTimeCount);
            }

            if (consecutiveEventCount != 0)
            {
                String inner = "";

                // Account for singular versus plural
                if (consecutiveEventCount > 1)
                {
                    inner = "s";
                }

                warningMessage += String.format("- %d instance"
                                                + inner
                                                + " of an ID with<br>&#160&#160&#160consecutive entry or exit events<br>",
                                                consecutiveEventCount);
            }

            // Inform the user if any inconsistencies were detected
            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                     warningMessage,
                                                     "Log Inconsistency",
                                                     JOptionPane.WARNING_MESSAGE,
                                                     OK_OPTION);
        }
    }

    /**************************************************************************
     * Count the number of IDs of those selected to be plotted that are active
     * at the beginning of a log data file. The first log file is indicated by
     * an index of -1; indices of 0 and above indicate subsequent log files
     * 
     * @param gapIndex
     *            index into the data gap list. Use -1 to indicate the
     *            beginning of the first log file's data
     * 
     * @return Number of active, selected IDs
     *************************************************************************/
    protected int getActiveIDCount(int gapIndex)
    {
        // Assume all IDs are inactive
        int activeIDCount = 0;

        // Set the log start and stop indices to check, assuming that only a
        // single log data file is loaded
        int startIndex = 0;
        int stopIndex = perfLogData.size();

        // Clear the ID used flags. This flag will be set if an ID has been
        // matched to an ID in the log in order to prevent reusing an
        // ID that has already had its activity status accounted for
        perfIDs.resetIDUsed(false);

        // Check if this is not the first log data file
        if (gapIndex != -1)
        {
            // Set the start index to the one indicated in the data gap list
            startIndex = dataGapIndex.get(gapIndex);

            // Go to the next gap
            gapIndex++;

            // Check that another gap exists. If not, the stop index is already
            // set above to the last log entry
            if (gapIndex < dataGapIndex.size())
            {
                // Set the stop index to the last entry for the previous log
                // file
                stopIndex = dataGapIndex.get(gapIndex);
            }
        }

        // For each log entry in the selected log file
        for (int logIndex = startIndex; logIndex < stopIndex; logIndex++)
        {
            // Initialize the ID index
            int idIndex = 0;

            // Step through the performance IDs
            for (PerformanceID pid : perfIDs.getPerfIDList())
            {
                // Check if this ID is selected to be plotted
                if (pid.isPlotPerfID())
                {
                    // Check if this ID matches the current one in the log
                    if (pid.getPerfID() == perfLogData.get(logIndex).perfID)
                    {
                        // Check if this ID has not already been checked for
                        // its initial state
                        if (!perfIDs.getPerfIDList().get(idIndex).isUsed())
                        {
                            // Indicate that this ID has been checked so that
                            // it won't be checked again
                            perfIDs.getPerfIDList().get(idIndex).setUsed(true);

                            // Check if this is an exit event. It's assumed
                            // that if the first event is an exit then the ID
                            // is initially active
                            if (!perfLogData.get(logIndex).isEntry)
                            {
                                // Increment the active ID counter
                                activeIDCount++;
                            }
                        }

                        // Since this ID matches the one in the log there's no
                        // need to continue through the ID list
                        break;
                    }
                }

                // Increment the ID index to the next ID
                idIndex++;
            }
        }

        return activeIDCount;
    }

    /**************************************************************************
     * Initialize the statistics values for the logged events
     *************************************************************************/
    private void clearStatistics()
    {
        // Step through the performance ID list
        for (int index = 0; index < perfIDs.getPerfIDList().size(); index++)
        {
            // Use this to shorten the references below
            PerformanceID pid = perfIDs.getPerfIDList().get(index);

            // Clear this ID's statistics values
            pid.clearPerfIDStats();
        }

        // Clear the overall statistic values
        totalOnCount = 0;
        totalOffCount = 0;
        totalAvgFreq = 0.0;
        totalTimeActive = 0.0;
        totalMinTimeActive = null;
        totalMaxTimeActive = null;
        totalMinInterval = null;
        totalMaxInterval = null;
        totalMinOverrun = null;
        totalMaxOverrun = null;
        totalMinTimeOnTimeStamp = null;
        totalMaxTimeOnTimeStamp = null;
        totalMinIntervalTimeStamp = null;
        totalMaxIntervalTimeStamp = null;
        totalMinOverrunTimeStamp = null;
        totalMaxOverrunTimeStamp = null;
    }

    /**************************************************************************
     * Calculate statistics on the logged events. If multiple log files are
     * read then do not factor in the gap between files in the calculations
     *************************************************************************/
    protected void calcEventStatistics()
    {
        double previousTimeStamp;
        double deltaTime;
        double interval;
        int dataIndex;
        int gapCount;
        int gapIndex = -1;

        // Clear out any existing statistics values
        clearStatistics();

        // Step through the performance ID list
        for (int index = 0; index < perfIDs.getPerfIDList().size(); index++)
        {
            // Use this to shorten the references below
            PerformanceID pid = perfIDs.getPerfIDList().get(index);

            // Initialize the previous time stamp
            previousTimeStamp = 0.0;

            // Initialize flag used to determine if the ID was active when the
            // log ends
            boolean idActive = false;

            dataIndex = 0;
            gapCount = 0;

            // Check if there are any data gaps
            if (!dataGapIndex.isEmpty())
            {
                // Get the index into the log of the first gap
                gapIndex = dataGapIndex.get(gapCount);
            }

            // Step through the log data
            for (int logIndex = 0; logIndex < perfLogData.size(); logIndex++)
            {
                // Get a pointer to the log entry to shorten the calls below
                PerfLog log = perfLogData.get(logIndex);

                // Check if the log has reached the current data gap
                if (dataIndex == gapIndex)
                {
                    // Update the previous time stamp in order to skip the data
                    // gap
                    previousTimeStamp = log.timeStamp;

                    // Update the index to the next data gap
                    gapCount++;

                    if (gapCount < dataGapIndex.size())
                    {
                        gapIndex = dataGapIndex.get(gapCount);
                    }
                }

                // Check if the log entry ID matches the list ID
                if (log.perfID == pid.getPerfID())
                {
                    // Calculate the time difference between this event and the
                    // previous entry event for this ID
                    deltaTime = log.timeStamp - previousTimeStamp;

                    // Check if this is an entry event
                    if (log.isEntry)
                    {
                        // Increment the entry event counter
                        pid.incrementEntryCount();

                        // Check that there aren't consecutive entry events for
                        // this entry. Otherwise the time between the two
                        // consecutive entry events wouldn't be counted in the
                        // "time on" total and the interval check could be
                        // incorrect
                        if (!log.isSequenceError)
                        {
                            idActive = true;

                            // Only calculate the min/max interval if another
                            // entry event has occurred for this ID
                            if (pid.getEntryCount() > 1)
                            {
                                // Calculate the time between consecutive entry
                                // events for this ID
                                interval = log.timeStamp - previousTimeStamp;

                                // Check if this is the minimum time between
                                // entry events for this ID
                                if (pid.getMinInterval() == null
                                    || interval < pid.getMinInterval())
                                {
                                    // Save the minimum interval and the time
                                    // when it occurred
                                    pid.setMinInterval(interval);
                                    pid.setMinIntervalTimeStamp(previousTimeStamp);
                                }

                                // Check if this is the maximum time between
                                // entry events for this ID
                                if (pid.getMaxInterval() == null
                                    || interval > pid.getMaxInterval())
                                {
                                    // Save the maximum interval and the time
                                    // when it occurred
                                    pid.setMaxInterval(interval);
                                    pid.setMaxIntervalTimeStamp(previousTimeStamp);
                                }
                            }

                            // Save the time stamp
                            previousTimeStamp = log.timeStamp;
                        }
                    }
                    // It's an exit event
                    else
                    {
                        // Increment the exit event counter
                        pid.incrementExitCount();

                        // Check that there aren't consecutive exit events for
                        // this entry. Otherwise the time between two
                        // consecutive exit events would be added to the total
                        if (!log.isSequenceError)
                        {
                            idActive = false;

                            // Add to the accumulated time for this ID
                            pid.addToActiveTime(deltaTime);

                            // Only calculate the min/max time on if an entry
                            // event has occurred for this ID
                            if (pid.getEntryCount() != 0)
                            {
                                // Check if this is the minimum on time found
                                // for this ID
                                if (pid.getMinTimeActive() == null
                                    || deltaTime < pid.getMinTimeActive())
                                {
                                    // Store the minimum on time for this ID
                                    // and the time when it occurred
                                    pid.setMinTimeActive(deltaTime);
                                    pid.setMinTimeActiveTimeStamp(previousTimeStamp);
                                }

                                // Check if this is the maximum on time found
                                // for this ID
                                if (pid.getMaxTimeActive() == null
                                    || deltaTime > pid.getMaxTimeActive())
                                {
                                    // Store the maximum on time for this ID
                                    // and the time when it occurred
                                    pid.setMaxTimeActive(deltaTime);
                                    pid.setMaxTimeActiveTimeStamp(previousTimeStamp);
                                }
                            }
                        }
                    }

                    // Check if the ID has a frequency value assigned and if
                    // the time difference exceeds the allotted time, which is
                    // the inverse of the ID frequency. The comparison is made
                    // between consecutive enter events and between enter-exit
                    // event pairs
                    if (pid.getPerfIDFreq() != 0.0
                        && deltaTime > 1.0 / pid.getPerfIDFreq())
                    {
                        // Calculate the amount of the overrun in seconds
                        log.overrun = deltaTime - 1.0 / pid.getPerfIDFreq();

                        // Set the overrun detected flag
                        overrunFlag = true;

                        // Check if this is the minimum frame overrun for
                        // this ID
                        if (pid.getMinOverrun() == null
                            || log.overrun < pid.getMinOverrun())
                        {
                            // Save the minimum frame overrun and the time when
                            // it occurred
                            pid.setMinOverrun(log.overrun);
                            pid.setMinOverrunTimeStamp(log.timeStamp);
                        }

                        // Check if this is the maximum frame overrun for this
                        // ID
                        if (pid.getMaxOverrun() == null
                            || log.overrun > pid.getMaxOverrun())
                        {
                            // Save the maximum frame overrun and the time when
                            // it occurred
                            pid.setMaxOverrun(log.overrun);
                            pid.setMaxOverrunTimeStamp(log.timeStamp);
                        }
                    }
                    // The events occur within the allotted time
                    else
                    {
                        // Set the overrun to zero
                        log.overrun = 0.0;
                    }
                }

                // Increment the log data index
                dataIndex++;
            }

            // Check if the ID was active when the log ended
            if (idActive)
            {
                // Add the last active time period for this ID to the total
                pid.addToActiveTime(maximumTimeStamp - previousTimeStamp);
            }

            // Calculate the average frequency for this ID. Don't count the
            // data gap period(s)
            pid.setAvgFreq(pid.getEntryCount() / adjustedDataTimeSpan);

            // Update the overall statistics values
            calcTotalStats(pid);
        }

        // Calculate the average frequency for all IDs. Don't count the data
        // gap period(s)
        totalAvgFreq = totalOnCount / adjustedDataTimeSpan;

        // Calculate the total time on (and off)
        calcTotalTimeOn();
    }

    /**************************************************************************
     * Update the overall statistics using the specified performance ID's
     * statistics values
     * 
     * @param pid
     *            performance ID
     *************************************************************************/
    private void calcTotalStats(PerformanceID pid)
    {
        // Update the time counter that tracks the total entry events, the
        // total exit events, and the total time the IDs are active
        totalOnCount += pid.getEntryCount();
        totalOffCount += pid.getExitCount();

        // Check if this is the minimum on time found for all IDs
        if (pid.getMinTimeActive() != null
            && (totalMinTimeActive == null
            || pid.getMinTimeActive() < totalMinTimeActive))
        {
            // Store the minimum on time for all IDs and the time when it
            // occurred
            totalMinTimeActive = pid.getMinTimeActive();
            totalMinTimeActiveID = pid.getPerfID();
            totalMinTimeOnTimeStamp = pid.getMinTimeActiveTimeStamp();
        }

        // Check if this is the maximum on time found for all IDs
        if (pid.getMaxTimeActive() != null
            && (totalMaxTimeActive == null
            || pid.getMaxTimeActive() > totalMaxTimeActive))
        {
            // Store the maximum on time for all IDs and the time when it
            // occurred
            totalMaxTimeActive = pid.getMaxTimeActive();
            totalMaxTimeActiveID = pid.getPerfID();
            totalMaxTimeOnTimeStamp = pid.getMaxTimeActiveTimeStamp();
        }

        // Check if this is the minimum interval found for all IDs
        if (pid.getMinInterval() != null
            && (totalMinInterval == null
            || pid.getMinInterval() < totalMinInterval))
        {
            // Store the minimum interval for all IDs and the time when it
            // occurred
            totalMinInterval = pid.getMinInterval();
            totalMinIntervalID = pid.getPerfID();
            totalMinIntervalTimeStamp = pid.getMinIntervalTimeStamp();
        }

        // Check if this is the maximum interval found for all IDs
        if (pid.getMaxInterval() != null
            && (totalMaxInterval == null
            || pid.getMaxInterval() > totalMaxInterval))
        {
            // Store the maximum interval for all IDs and the time when it
            // occurred
            totalMaxInterval = pid.getMaxInterval();
            totalMaxIntervalID = pid.getPerfID();
            totalMaxIntervalTimeStamp = pid.getMaxIntervalTimeStamp();
        }

        // Check if this is the minimum frame overrun found for all IDs
        if (pid.getMinOverrun() != null
            && (totalMinOverrun == null
            || pid.getMinOverrun() < totalMinOverrun))
        {
            // Store the minimum frame overrun for all IDs and the time when it
            // occurred
            totalMinOverrun = pid.getMinOverrun();
            totalMinOverrunID = pid.getPerfID();
            totalMinOverrunTimeStamp = pid.getMinOverrunTimeStamp();
        }

        // Check if this is the maximum frame overrun found for all IDs
        if (pid.getMaxOverrun() != null
            && (totalMaxOverrun == null
            || pid.getMaxOverrun() < totalMaxOverrun))
        {
            // Store the maximum frame overrun for all IDs and the time when it
            // occurred
            totalMaxOverrun = pid.getMaxOverrun();
            totalMaxOverrunID = pid.getPerfID();
            totalMaxOverrunTimeStamp = pid.getMaxOverrunTimeStamp();
        }
    }

    /**************************************************************************
     * Calculate the total time that any ID is active. Since active IDs can
     * overlap (i.e., two or more may be between entry and exit events
     * simultaneously) the total active time isn't necessarily the sum of the
     * individual ID active times; therefore, a separate check of the log data
     * must be done to prevent counting the overlap periods more than once
     *************************************************************************/
    private void calcTotalTimeOn()
    {
        double previousTimeStamp = 0.0;
        int activeIDCount = 0;

        // Clear the ID used flags. This flag will be set if an ID has been
        // matched to an ID in the log in order to prevent reusing an
        // ID that has already been accounted for
        perfIDs.resetIDUsed(false);

        // Step through the performance log entries in order to count the
        // number of IDs that are active initially (i.e., between an entry and
        // an exit event). This is necessary to set the counter for use later
        for (PerfLog log : perfLogData)
        {
            // Get the performance ID associated with the log ID
            PerformanceID pid = perfIDs.getPerfID(log.perfID);

            // Check that this ID hasn't already been found
            if (!pid.isUsed())
            {
                // Set the "already checked" flag for this ID
                pid.setUsed(true);

                // Check if this is an exit event. If the first event for this
                // ID is an exit, then it must be active initially
                if (!log.isEntry)
                {
                    // Increment the active ID counter
                    activeIDCount++;
                }
            }
        }

        int dataIndex = 0;
        int gapCount = 0;
        int gapIndex = -1;

        // Check if there are any data gaps
        if (!dataGapIndex.isEmpty())
        {
            // Get the index into the log of the first gap
            gapIndex = dataGapIndex.get(gapCount);
        }

        // Step through the performance log entries
        for (PerfLog log : perfLogData)
        {
            // Check if the log has reached the current data gap
            if (dataIndex == gapIndex)
            {
                // Update the previous time stamp in order to skip the
                // data gap
                previousTimeStamp = log.timeStamp;

                // Update the index to the next data gap
                gapCount++;

                if (gapCount < dataGapIndex.size())
                {
                    gapIndex = dataGapIndex.get(gapCount);
                }
            }

            // Check that there aren't consecutive entry or exit events for
            // this entry. This prevents possibly fouling the active ID counter
            if (!log.isSequenceError)
            {
                // Check if this is an entry event
                if (log.isEntry)
                {
                    // Increment the active ID counter
                    activeIDCount++;

                    // Check if this is the first entry event to follow an exit
                    // event
                    if (activeIDCount == 1)
                    {
                        // Save the event time to mark the beginning of the
                        // current active period
                        previousTimeStamp = log.timeStamp;
                    }

                }
                // It's an exit event
                else
                {
                    // Decrement the active ID counter
                    activeIDCount--;

                    // Check if all IDs are now inactive
                    if (activeIDCount == 0)
                    {
                        // Calculate the time between the first event becoming
                        // active and the last event becoming inactive. This is
                        // the active time for the current period; add this to
                        // the total active time
                        totalTimeActive += log.timeStamp - previousTimeStamp;
                    }
                }
            }

            // Increment the log data index
            dataIndex++;
        }

        // Check if one or more IDs were active when the log ended
        if (activeIDCount > 0)
        {
            // Add the last active time period to the total
            totalTimeActive += maximumTimeStamp - previousTimeStamp;
        }
    }

    /**************************************************************************
     * Remove or restore the gaps in the data caused when reading in multiple
     * log files
     * 
     * @param showGaps
     *            true to show the gaps between files; false to hide the gaps
     *************************************************************************/
    protected void adjustDataGaps(boolean showGaps)
    {
        // Check if there are any data gaps
        if (!dataGapIndex.isEmpty())
        {
            int gapCount = 0;
            double timeAdjust = 0.0;

            // Get the index into the log of the first gap
            int gapIndex = dataGapIndex.get(gapCount);

            // Step through the log data
            for (int dataIndex = 0; dataIndex < perfLogData.size(); dataIndex++)
            {
                // Check if the log has reached the current data gap
                if (dataIndex == gapIndex)
                {
                    // Update the previous time stamp in order to skip the data
                    // gap
                    timeAdjust += dataGapLength.get(gapCount);

                    // Update the index to the next data gap
                    gapCount++;

                    if (gapCount < dataGapIndex.size())
                    {
                        gapIndex = dataGapIndex.get(gapCount);
                    }
                }

                // Check if this is not the first log file's data
                if (gapCount != 0)
                {
                    // Check if gaps are to be shown
                    if (showGaps)
                    {
                        // Adjust the time stamp to restore the gap
                        perfLogData.get(dataIndex).timeStamp += timeAdjust;
                    }
                    // Hide the gaps
                    else
                    {
                        // Adjust the time stamp to remove the gap
                        perfLogData.get(dataIndex).timeStamp -= timeAdjust;
                    }
                }
            }

            // Check if gaps are to be shown
            if (showGaps)
            {
                // Adjust the maximum time stamp value to restore the gap
                maximumTimeStamp += timeAdjust;
            }
            // Hide the gaps
            else
            {
                // Adjust the maximum time stamp value to remove the gap
                maximumTimeStamp -= timeAdjust;
            }
        }
    }

    /**************************************************************************
     * Read the log notes file and store the notes with the log data
     * 
     * @param logNum
     *            index of the log in the log header list
     *************************************************************************/
    private void readLogNotes(int logNum)
    {
        int offset = 0;

        // Check if this is not the first of multiple loaded logs
        if (logNum != 0)
        {
            // Set the log index offset using the gap index
            offset = dataGapIndex.get(logNum - 1);
        }

        // Get the log notes file
        File notesFile = new File(getNotesFileName(logNum));

        try
        {
            // Check if the file can accessed
            if (notesFile.canRead())
            {
                // Open the notes file
                BufferedReader br = new BufferedReader(new FileReader(notesFile));

                // Read in the first line of the notes file
                String line = br.readLine();

                // Continue to read the file until EOF is reached or an error
                // is detected
                while (line != null)
                {
                    // Separate the log index from the note
                    String[] part = line.split(",", 2);

                    // Store the note in the log data. If the index references
                    // the second or subsequent set of log data (when multiple
                    // logs are loaded) then add the offset so that the correct
                    // combined log index is used
                    perfLogData.get(Integer.parseInt(part[0]) + offset).setNotes(part[1]);

                    // Read the next line in the notes file
                    line = br.readLine();
                }

                // Close the notes file
                br.close();
            }
        }
        catch (Exception e)
        {
            // Inform the user that an error occurred while reading the log
            // notes file
            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                     "<html><b>Cannot read log notes file<br></b>"
                                                         + notesFile.getAbsolutePath(),
                                                     "File Warning",
                                                     JOptionPane.ERROR_MESSAGE,
                                                     OK_OPTION);
        }
    }

    /**************************************************************************
     * Add or update a log notes file annotation
     * 
     * @param logIndex
     *            index of the log entry with the log note to add/update
     *************************************************************************/
    protected void writeLogNotes(int logIndex)
    {
        int logNum;
        int offset = 0;

        // If multiple log files are loaded then step through the list of log
        // data gaps to determine which log the note belongs with
        for (logNum = 0; logNum < dataGapIndex.size(); logNum++)
        {
            // Check if the index of the note to insert is after the index
            // where the next gap occurs
            if (logIndex >= dataGapIndex.get(logNum))
            {
                // Save the gap index as the amount to bias the log index
                offset = dataGapIndex.get(logNum);
            }
            // The index falls within the current gap indices
            else
            {
                // Exit the loop
                break;
            }
        }

        // Get the log notes file and temporary file names
        String fileName = getNotesFileName(logNum);
        File notesFile = new File(fileName);
        File tempFile = new File(System.getProperty("java.io.tmpdir"),
                                 fileName + ".tmp");

        // Notes file error type
        String errorType = "";

        try
        {
            // Check if the notes file doesn't exist
            if (!notesFile.exists())
            {
                // Create an empty notes file
                notesFile.createNewFile();
            }

            // Create the temporary file
            tempFile.createNewFile();

            // Check if the files cannot be read from or written to
            if (!(notesFile.canRead() && tempFile.canWrite()))
            {
                // Set the error type to indicate that the files can't be
                // accessed as needed
                errorType = "Cannot write temporary file or read";
            }
            // The notes file can be read and the temporary file can be written
            else
            {
                BufferedReader br = null;
                PrintWriter pw = null;

                try
                {
                    // Open the notes and temporary files
                    br = new BufferedReader(new FileReader(notesFile));
                    pw = new PrintWriter(tempFile);

                    try
                    {
                        // Read in the first line of the notes file
                        String line;
                        line = br.readLine();

                        // Continue to read the file until EOF is reached or an
                        // error is detected
                        while (line != null)
                        {
                            // Separate the log index from the note
                            String[] part = line.split(",", 2);

                            // Check if the index doesn't match the new note's
                            // row
                            if (logIndex - offset != Integer.parseInt(part[0]))
                            {
                                // Copy the note from the original file to the
                                // temporary file. If the row numbers match the
                                // note is skipped
                                pw.printf("%s\n", line);
                            }

                            // Read the next line in the notes file
                            line = br.readLine();
                        }

                        // Check if the note contains any text. If not, the
                        // note is ignored, which deletes the existing note
                        // with this index, if present
                        if (!perfLogData.get(logIndex).getNotes().isEmpty())
                        {
                            // Append the new/updated note to the notes file.
                            // Subtract the offset so that the index is
                            // relative to the individual log's entries, which
                            // makes the notes file independent of having
                            // single or multiple logs loaded
                            pw.printf("%d,%s\n",
                                      logIndex - offset,
                                      perfLogData.get(logIndex).getNotes());
                        }

                        // Close the notes and temporary files
                        br.close();
                        pw.close();

                        // Delete the original notes file
                        if (!notesFile.delete())
                        {
                            // Error occurred deleting the notes file
                            errorType = "Cannot delete original";
                        }
                        // Check if the new notes file is empty
                        else if (tempFile.length() == 0)
                        {
                            // Delete the new notes file
                            if (!tempFile.delete())
                            {
                                // Error occurred deleting the new, empty notes
                                // file
                                errorType = "Cannot delete temporary file for";
                            }
                        }
                        // Rename the new notes file to the original file's
                        // name
                        else if (!tempFile.renameTo(notesFile))
                        {
                            // Error occurred renaming the notes file
                            errorType = "Cannot rename temporary file to";
                        }
                    }
                    catch (IOException ioe)
                    {
                        // I/O error occurred reading the notes file or writing
                        // the temporary file
                        errorType = "Error writing temporary file or reading";
                    }
                }
                catch (FileNotFoundException fnfe)
                {
                    // Error occurred opening the files
                    errorType = "Cannot locate temporary file or";
                }
            }
        }
        catch (IOException ioe)
        {
            // Error occurred creating the notes or temporary files
            errorType = "Cannot create temporary file or";
        }

        // Check if an error occurred
        if (!errorType.isEmpty())
        {
            // Inform the user that an error occurred while inserting the note
            // into the log notes file
            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                     "<html><b>"
                                                         + errorType
                                                         + " log notes file<br></b>"
                                                         + notesFile.getAbsolutePath(),
                                                     "File Warning",
                                                     JOptionPane.ERROR_MESSAGE,
                                                     OK_OPTION);
        }
    }

    /**************************************************************************
     * Build the log notes file name based on the log file's time stamp
     * 
     * @param logNum
     *            index of the log in the log header list
     * 
     * @return Notes file name
     *************************************************************************/
    private String getNotesFileName(int logNum)
    {
        return "CPM."
               + logHeader.get(logNum).getTimeSeconds()
               + "."
               + logHeader.get(logNum).getTimeSubSeconds()
               + ".notes";
    }
}