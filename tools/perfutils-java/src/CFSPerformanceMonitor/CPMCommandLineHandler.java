/**
 * CFS Performance Monitor Command Line Argument Handler
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.BAR;
import static CFSPerformanceMonitor.CPMConstants.BY_NAME;
import static CFSPerformanceMonitor.CPMConstants.BY_VALUE;
import static CFSPerformanceMonitor.CPMConstants.EXIT_MASK;
import static CFSPerformanceMonitor.CPMConstants.MAX_STEP_HEIGHT;
import static CFSPerformanceMonitor.CPMConstants.MAX_STEP_SPACING;
import static CFSPerformanceMonitor.CPMConstants.MIN_STEP_HEIGHT;
import static CFSPerformanceMonitor.CPMConstants.MIN_STEP_SPACING;
import static CFSPerformanceMonitor.CPMConstants.MIN_WINDOW_HEIGHT;
import static CFSPerformanceMonitor.CPMConstants.MIN_WINDOW_WIDTH;
import static CFSPerformanceMonitor.CPMConstants.PIE;
import static CFSPerformanceMonitor.CPMConstants.STATISTICS_PLOT;
import static CFSPerformanceMonitor.CPMConstants.STEP_PLOT;
import static CFSPerformanceMonitor.CPMConstants.THREAD_PLOT;

import java.awt.Color;
import java.awt.Dimension;
import java.util.ArrayList;
import java.util.List;

import CFSPerformanceMonitor.CPMConstants.CmdType;

/******************************************************************************
 * CFS Performance Monitor command line argument handler class
 *****************************************************************************/
public class CPMCommandLineHandler
{
    // Variables that require access (via getters) outside of this class
    private Dimension mainSize;
    private String logFile;
    private String idFile;
    private String laf;
    private Integer plotType;

    // Text describing the command line error
    private String errorMessage;

    /**************************************************************************
     * Individual command line argument handler class
     *************************************************************************/
    private class CommandHandler
    {
        private final String command;
        private final CmdType type;
        private String[] options;
        private Object[] conditions;

        /**********************************************************************
         * Individual command line argument handler class constructor for
         * commands having no limit values or fixed set of options
         * 
         * @param command
         *            command name
         * 
         * @param type
         *            command type: CmdType.NAME, CmdType.COLOR, or CmdType.ID
         *********************************************************************/
        protected CommandHandler(String command, CmdType type)
        {
            this.command = command;
            this.type = type;
        }

        /**********************************************************************
         * Individual command line argument handler class constructor for
         * commands with value limits
         * 
         * @param command
         *            command name
         * 
         * @param type
         *            command type: CmdType.MINMAX or CmdType.SIZE
         * 
         * @param conditions
         *            array of limit values
         *********************************************************************/
        protected CommandHandler(String command,
                                 CmdType type,
                                 Object[] conditions)
        {
            this.command = command;
            this.type = type;
            this.conditions = conditions;
        }

        /**********************************************************************
         * Individual command line argument handler class constructor for
         * commands with a fixed set of valid options
         * 
         * @param command
         *            command name
         * 
         * @param type
         *            command type: CmdType.OPTION
         * 
         * @param options
         *            array of valid input options
         * 
         * @param conditions
         *            array of output parameters associated with each input
         *            option
         *********************************************************************/
        protected CommandHandler(String command,
                                 CmdType type,
                                 String[] options,
                                 Object[] conditions)
        {
            this.command = command;
            this.type = type;
            this.options = options;
            this.conditions = conditions;
        }

        /**********************************************************************
         * Command line argument handler
         * 
         * @param parm
         *            command parameter
         * 
         * @return true if the command does not succeed
         *********************************************************************/
        protected boolean handler(String parm)
        {
            Object parmVal = null;

            // Check and convert the command parameter value based on the
            // command type
            switch (type)
            {
                case NAME:
                    parmVal = parm;
                    break;

                case OPTION:
                    parmVal = handleOption(command, parm, options, conditions);
                    break;

                case MINMAX:
                    parmVal = handleMinMax(command, parm, conditions);
                    break;

                case COLOR:
                    parmVal = handleColor(command, parm);
                    break;

                case SIZE:
                    parmVal = handleSize(command, parm, conditions);
                    break;

                case ID:
                    parmVal = handleID(command, parm);
                    break;
            }

            // Check if a valid command line parameter was found
            if (parmVal != null)
            {
                // Implement the command
                doCommand(parmVal);
            }

            return parmVal == null;
        }

        /**********************************************************************
         * Placeholder for command type specific handler
         * 
         * @param parmVal
         *            command parameter
         *********************************************************************/
        protected void doCommand(Object parmVal)
        {
        }
    }

    /**************************************************************************
     * CFS Performance Monitor command line argument handler class constructor
     * 
     * @param args
     *            array of command line arguments and associated parameters
     * 
     * @param plotPrefs
     *            plot preferences
     *************************************************************************/
    CPMCommandLineHandler(String[] args,
                          final CPMPreferencesHandler plotPrefs)
    {
        mainSize = null;
        logFile = null;
        idFile = null;
        laf = null;
        plotType = null;
        errorMessage = null;

        // List to hold the valid command line arguments
        List<CommandHandler> argument = new ArrayList<CommandHandler>();

        // Main window size command
        argument.add(new CommandHandler("mainSize",
                                        CmdType.SIZE,
                                        new Object[] {MIN_WINDOW_WIDTH,
                                                      MIN_WINDOW_HEIGHT})
        {
            /******************************************************************
             * Set the main window size
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                mainSize = (Dimension) parmVal;
            }
        });

        // Log file(s) command
        argument.add(new CommandHandler("log", CmdType.NAME)
        {
            /******************************************************************
             * Set the log file(s)
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                logFile = (String) parmVal;
            }
        });

        // ID file command
        argument.add(new CommandHandler("id", CmdType.NAME)
        {
            /******************************************************************
             * Set the performance ID file
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                idFile = (String) parmVal;
            }
        });

        // Look & feel command
        argument.add(new CommandHandler("laf", CmdType.NAME)
        {
            /******************************************************************
             * Set the look & feel
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                laf = (String) parmVal;
            }
        });

        // Initially selected plot tab command
        argument.add(new CommandHandler("plot",
                                        CmdType.OPTION,
                                        new String[] {"step",
                                                      "thread",
                                                      "statistics"},
                                        new Object[] {STEP_PLOT,
                                                      THREAD_PLOT,
                                                      STATISTICS_PLOT})
        {
            /******************************************************************
             * Set the initially selected plot tab
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotType = (Integer) parmVal;
            }
        });

        // Step subplot height command
        argument.add(new CommandHandler("height",
                                        CmdType.MINMAX,
                                        new Object[] {MIN_STEP_HEIGHT,
                                                      MAX_STEP_HEIGHT})
        {
            /******************************************************************
             * Set the step plot height
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setStepPlotHeight((Integer) parmVal);
            }
        });

        // Step subplot spacing
        argument.add(new CommandHandler("spacing",
                                        CmdType.MINMAX,
                                        new Object[] {MIN_STEP_SPACING,
                                                      MAX_STEP_SPACING})
        {
            /******************************************************************
             * Set the step plot spacing
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setStepPlotSpacing((Integer) parmVal);
            }
        });

        // Show y-axis tick mark labels command
        argument.add(new CommandHandler("labels",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show y-axis tick mark labels flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setStepPlotShowLabels((Boolean) parmVal);
            }
        });

        // Show step plot shading command
        argument.add(new CommandHandler("shading",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show step plot shading flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setStepPlotShowShading((Boolean) parmVal);
            }
        });

        // Show step plot exit lines command
        argument.add(new CommandHandler("exit",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show step plot exit lines flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setStepPlotShowExit((Boolean) parmVal);
            }
        });

        // Show horizontal grid lines command
        argument.add(new CommandHandler("hGrid",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show horizontal grid lines flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setShowPlotHorizontalGrid((Boolean) parmVal);
            }
        });

        // Show vertical grid lines command
        argument.add(new CommandHandler("vGrid",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show vertical grid lines flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setShowPlotVerticalGrid((Boolean) parmVal);
            }
        });

        // Show minor frame lines command
        argument.add(new CommandHandler("minFrame",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show minor frame lines flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setShowPlotMinorFrames((Boolean) parmVal);
            }
        });

        // Show major frame lines command
        argument.add(new CommandHandler("majFrame",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show major frame lines flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setShowPlotMajorFrames((Boolean) parmVal);
            }
        });

        // Minor frame performance ID command
        argument.add(new CommandHandler("minID", CmdType.ID)
        {
            /******************************************************************
             * Set the minor frame performance ID
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setMinorID((Integer) parmVal);
            }
        });

        // Major frame performance ID command
        argument.add(new CommandHandler("majID", CmdType.ID)
        {
            /******************************************************************
             * Set the major frame performance ID
             * *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setMinorID((Integer) parmVal);
            }
        });

        // Show CPU idle time command
        argument.add(new CommandHandler("idle",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show CPU idle time flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setShowCPUIdle((Boolean) parmVal);
            }
        });

        // Statistics plot chart type command
        argument.add(new CommandHandler("type",
                                        CmdType.OPTION,
                                        new String[] {"pie", "bar"},
                                        new Object[] {PIE, BAR})
        {
            /******************************************************************
             * Set the statistics plot chart type
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setStatChartType((Integer) parmVal);
            }
        });

        // Statistics plot sort order command
        argument.add(new CommandHandler("order",
                                        CmdType.OPTION,
                                        new String[] {"name", "value"},
                                        new Object[] {BY_NAME, BY_VALUE})
        {
            /******************************************************************
             * Set the statistics plot sort order
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setSortOrder((Integer) parmVal);
            }
        });

        // Show data gaps command
        argument.add(new CommandHandler("gaps",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show data gaps flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setShowDataGaps((Boolean) parmVal);
            }
        });

        // Show frame overruns command
        argument.add(new CommandHandler("overruns",
                                        CmdType.OPTION,
                                        new String[] {"true", "false"},
                                        new Object[] {true, false})
        {
            /******************************************************************
             * Set the show frame overruns flag
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setShowOverruns((Boolean) parmVal);
            }
        });

        // Plot background color command
        argument.add(new CommandHandler("backColor", CmdType.COLOR)
        {
            /******************************************************************
             * Set the plot background color
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setBackgroundColor(new Color((Integer) parmVal));
            }
        });

        // Minor grid line color command
        argument.add(new CommandHandler("minGridColor", CmdType.COLOR)
        {
            /******************************************************************
             * Set the minor grid line color
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setMinorGridColor(new Color((Integer) parmVal));
            }
        });

        // Major grid line color command
        argument.add(new CommandHandler("majGridColor", CmdType.COLOR)
        {
            /******************************************************************
             * Set the major grid line color
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setMajorGridColor(new Color((Integer) parmVal));
            }
        });

        // Minor frame line color command
        argument.add(new CommandHandler("minFrameColor", CmdType.COLOR)
        {
            /******************************************************************
             * Set the minor frame line color
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setMinorFrameColor(new Color((Integer) parmVal));
            }
        });

        // Major frame line color command
        argument.add(new CommandHandler("majFrameColor", CmdType.COLOR)
        {
            /******************************************************************
             * Set the major frame line color
             *****************************************************************/
            @Override
            protected void doCommand(Object parmVal)
            {
                plotPrefs.setMajorFrameColor(new Color((Integer) parmVal));
            }
        });

        // Command line error detected flag
        boolean errorFlag = false;

        // Step through the command line arguments
        for (int index = 0; index < args.length && !errorFlag; index++)
        {
            // Assume an error exists
            errorFlag = true;

            // Get the next command line argument
            String arg = args[index];

            // Check if the command starts with a recognized delimiter
            if (arg.startsWith("-") || arg.startsWith("/"))
            {
                // Remove the delimiter
                arg = arg.substring(1);
            }

            // Increment the index to point at the argument's parameter
            index++;

            // Check that the argument's parameter is present
            if (index < args.length)
            {
                // Get the argument's parameter
                String parm = args[index];

                // Step through the valid commands
                for (CommandHandler cmd : argument)
                {
                    // Check if the command matches a valid command
                    if (arg.toLowerCase().equals(cmd.command.toLowerCase()))
                    {
                        // Handle the command
                        errorFlag = cmd.handler(parm);
                        break;
                    }
                }
            }
        }

        // Check if an invalid command argument or parameter was provided, or
        // if the parameter was missing
        if (errorFlag)
        {
            // Check if a bad parameter was detected
            if (errorMessage != null)
            {
                System.err.println("Error: " + errorMessage + "\n");
            }
            // Invalid command
            else
            {
                // Display the usage information
                System.out.println("usage:\n"
                                   + "java -jar CPM.jar [[<- or />]cmd parameter [...]]\n"
                                   + " Command line arguments:\n"
                                   + "  Description             Command       Value\n"
                                   + "  ----------------------- ------------- ---------------------------------------\n"
                                   + "  Plot background color   backColor     <[name] or [0xRRGGBB]>\n"
                                   + "  Show step exit lines    exit          <true or false>\n"
                                   + "  Show data gaps          gaps          <true or false>\n"
                                   + "  Step plot height        height        <step plot height>\n"
                                   + "  Show horizontal grid    hGrid         <true or false>\n"
                                   + "  Load ID file            id            <ID file name>\n"
                                   + "  Show CPU idle time      idle          <true or false>\n"
                                   + "  Show y-axis tick labels labels        <true or false>\n"
                                   + "  Load look & feel        laf           <look & feel>\n"
                                   + "  Load log file(s)        log           <log file name[;log file name 2[...]]>\n"
                                   + "  Main window size        mainSize      <widthxheight>\n"
                                   + "  Show major frame lines  majFrame      <true or false>\n"
                                   + "  Major frame line color  majFrameColor <[name] or [0xRRGGBB]>\n"
                                   + "  Major grid line color   majGridColor  <[name] or [0xRRGGBB]>\n"
                                   + "  Major frame ID          majID         <0x########>\n"
                                   + "  Show minor frame lines  minFrame      <true or false>\n"
                                   + "  Minor frame line color  minFrameColor <[name] or [0xRRGGBB]>\n"
                                   + "  Minor grid line color   minGridColor  <[name] or [0xRRGGBB]>\n"
                                   + "  Minor frame ID          minID         <0x########>\n"
                                   + "  Stats plot sort order   order         <name or value>\n"
                                   + "  Show frame overruns     overruns      <true or false>\n"
                                   + "  Selected plot tab       plot          <step or thread or statistics>\n"
                                   + "  Shade beneath steps     shading       <true or false>\n"
                                   + "  Step plot spacing       spacing       <step plot spacing>\n"
                                   + "  Stats chart type        type          <pie or bar>\n"
                                   + "  Show vertical grid      vGrid         <true or false>");
            }

            // Exit the application
            System.exit(0);
        }
    }

    /**************************************************************************
     * Get the main window size
     * 
     * @return Main window size in Dimension format
     *************************************************************************/
    protected Dimension getMainSize()
    {
        return mainSize;
    }

    /**************************************************************************
     * Get the selected performance log file(s)
     * 
     * @return Selected performance log file(s). If more than one file is
     *         listed then the path+file names must be separated by semicolons
     *************************************************************************/
    protected String getLogFile()
    {
        return logFile;
    }

    /**************************************************************************
     * Get the selected performance ID file
     * 
     * @return Selected performance ID file
     *************************************************************************/
    protected String getIDFile()
    {
        return idFile;
    }

    /**************************************************************************
     * Get the selected look & feel
     * 
     * @return Selected look & feel
     *************************************************************************/
    protected String getLaF()
    {
        return laf;
    }

    /**************************************************************************
     * Get the initial plot tab to display
     * 
     * @return Selected initial plot tab (step if none selected)
     *************************************************************************/
    protected Integer getPlotType()
    {
        // Check if no plot was selected
        if (plotType == null)
        {
            // Default to showing the step plot tab
            plotType = STEP_PLOT;
        }

        return plotType;
    }

    /**************************************************************************
     * Get the main window size parameter
     * 
     * @param arg
     *            window size command line argument
     * 
     * @param parm
     *            window dimensions
     * 
     * @param min
     *            array containing the minimum main window width and minimum
     *            main window height
     * 
     * @return Main window dimension value (null if an error is found)
     *************************************************************************/
    private Dimension handleSize(String arg,
                                 String parm,
                                 Object[] min)
    {
        Dimension val = null;

        // Split out the width and height strings
        String[] parts = parm.split("x");

        // Check that only two values are provided and that both contain only
        // decimal digits
        if (parts.length == 2
            && parts[0].matches("\\d+")
            && parts[1].matches("\\d+"))
        {
            // Convert the width and height strings to a Dimension variable
            val = new Dimension(Integer.parseInt(parts[0]),
                                Integer.parseInt(parts[1]));

            // Check if the width is below the minimum allowed
            if (val.width < (Integer) min[0])
            {
                // Set the width to the minimum
                val.width = (Integer) min[0];
            }

            // Check if the height is below the minimum allowed
            if (val.height < (Integer) min[1])
            {
                // Set the height to the minimum
                val.height = (Integer) min[1];
            }
        }
        // Width or height string contains a non-numeral or the wrong number of
        // values
        else
        {
            errorMessage = arg
                           + " width or height not a number, or too "
                           + "many/few values";
        }

        return val;
    }

    /**************************************************************************
     * Get an integer value parameter that has minimum and maximum limits
     * 
     * @param arg
     *            integer value command line argument
     * 
     * @param parm
     *            integer value
     * 
     * @param limit
     *            array containing the lower limit value (inclusive) and the
     *            upper limit value (inclusive)
     * 
     * @return Integer value (null if an error is found)
     *************************************************************************/
    private Integer handleMinMax(String arg,
                                 String parm,
                                 Object[] limit)
    {
        Integer val = null;

        // Check if the parameter contains only decimal digits
        if (parm.matches("\\d+"))
        {
            // Convert the string to an integer
            val = Integer.parseInt(parm);

            // Check if the value falls outside the limits
            if (val < (Integer) limit[0] || val > (Integer) limit[1])
            {
                errorMessage = arg
                               + " must be >= "
                               + limit[0]
                               + " and <= "
                               + limit[1];
                val = null;
            }
        }
        // String contains a non-numeral
        else
        {
            errorMessage = arg + " not a number";
        }

        return val;
    }

    /**************************************************************************
     * Get a color parameter
     * 
     * @param arg
     *            color command line argument
     * 
     * @param parm
     *            color in hexadecimal format or a color name
     * 
     * @return Selected color in hexadecimal format (null if an error is found)
     *************************************************************************/
    private Integer handleColor(String arg, String parm)
    {
        Integer val = null;

        // Remove the leading '0x' if present (not required)
        if (parm.startsWith("0x"))
        {
            parm = parm.substring(2);
        }

        // Check if the parameter has the allowed number of hexadecimal digits
        if (parm.length() == 6 && parm.matches("[0-9A-Fa-f]+"))
        {
            // Convert the string to an integer
            val = Integer.parseInt(parm, 16);
        }
        // Not a hexadecimal value
        else
        {
            try
            {
                // Check if the parameter is a valid color name; if so, get the
                // RGB value (mask out the alpha portion)
                val = ((Color) Color.class.getField(parm).get(null)).getRGB()
                      & 0xffffff;
            }
            catch (Exception e)
            {
                // Parameter is not a hexadecimal color or color name
                errorMessage = arg
                               + " must be a color name or in the format "
                               + "'0x######' where '#' is a hexadecimal digit";
            }
        }

        return val;
    }

    /**************************************************************************
     * Get a minor or major frame performance ID parameter
     * 
     * @param arg
     *            minor/major frame ID command line argument
     * 
     * @param parm
     *            performance ID in hexadecimal format
     * 
     * @return Performance ID in hexadecimal format (null if an error is found)
     *************************************************************************/
    private Integer handleID(String arg, String parm)
    {
        Integer val = null;

        // Remove the leading '0x' if present (not required)
        if (parm.startsWith("0x"))
        {
            parm = parm.substring(2);
        }

        // Check if the parameter has the allowed number of hexadecimal digits
        if (parm.length() <= 8 && parm.matches("[0-9A-Fa-f]+"))
        {
            // Convert the string to an integer
            val = Integer.parseInt(parm, 16);

            // Check that the high bit isn't set. The high bit is reserved for
            // indicating exit events
            if ((val | EXIT_MASK) != 0)
            {
                errorMessage = arg + " high bit cannot be set";
                val = null;
            }
        }
        // Invalid parameter length or characters
        else
        {
            errorMessage = arg
                           + " must be in the format '0x########' where '#' "
                           + "is a hexadecimal digit";
        }

        return val;
    }

    /**************************************************************************
     * Get the selected option which must match one of a predefined list of
     * options
     * 
     * @param arg
     *            command line argument
     * 
     * @param parm
     *            command option
     * 
     * @param inputOptions
     *            array of valid inputs
     * 
     * @param outputValues
     *            array of output values associated with each input option
     * 
     * @return Output value for the option selected
     *************************************************************************/
    private Object handleOption(String arg,
                                String parm,
                                String[] inputOptions,
                                Object[] outputValues)
    {
        Object val = null;
        int index = 0;
        String valid = "";

        // Step through the valid options
        for (Object option : inputOptions)
        {
            // Check if the command parameter matches the valid option
            if (parm.equals(option))
            {
                // Get the associated output value for this option
                val = outputValues[index];
                break;
            }

            // Build the option list for the error message text, in case it's
            // needed
            valid += (index != 0 ? " | " : " ") + inputOptions[index];

            index++;
        }

        // Check if no valid option was found
        if (index == inputOptions.length)
        {
            // Build the error message
            errorMessage = arg + " must be one of the following:" + valid;
        }

        return val;
    }
}
