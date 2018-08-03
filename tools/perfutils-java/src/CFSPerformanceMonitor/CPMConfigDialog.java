/**
 * CFS Performance Monitor Configs Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LABEL_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LABEL_VERTICAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LOG_SEPARATOR;
import static CFSPerformanceMonitor.CPMConstants.OK_CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.PERF_ID_LIST_FILE;
import static CFSPerformanceMonitor.CPMConstants.PERF_LOG_FILE;
import static CFSPerformanceMonitor.CPMConstants.ConfigType.SAVE;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.prefs.BackingStoreException;

import javax.swing.BorderFactory;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JTextArea;
import javax.swing.SwingUtilities;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;

import CFSPerformanceMonitor.CPMConstants.ConfigType;

/******************************************************************************
 * CFS Performance Monitor Config dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMConfigDialog extends CPMDialogHandler
{
    private final CPMMain perfMain;
    private final ConfigType dialogType;

    // Components that need to be accessed by other classes or listeners
    private JPanel configPanel;
    private JLabel configSelLabel;
    private JComboBox configComboBox;
    private JLabel configWhichLabel;
    private JTextArea configFilesTextArea;

    // True if the ID and log file names from the config selected in the combo
    // box should be displayed; false to display the currently loaded ID and
    // log file names
    private boolean useSelection;

    // Button selected by the user
    private int buttonSelected;

    /**************************************************************************
     * Configs dialog class constructor
     * 
     * @param perfMain
     *            main class
     * 
     * @param dialogType
     *            config dialog type: ConfigType.LOAD, ConfigType.Save, or
     *            ConfigType.Delete
     *************************************************************************/
    protected CPMConfigDialog(CPMMain perfMain,
                              ConfigType dialogType)
    {
        this.perfMain = perfMain;
        this.dialogType = dialogType;

        // Create the config dialog
        initialize();
    }

    /**************************************************************************
     * Return the button selected
     * 
     * @return Selected button (OK_OPTION or CANCEL_OPTION)
     *************************************************************************/
    protected int getButtonSelected()
    {
        return buttonSelected;
    }

    /**************************************************************************
     * Create the configs dialog
     *************************************************************************/
    private void initialize()
    {
        // Create an empty border
        Border emptyBorder = BorderFactory.createEmptyBorder();

        // Create the config dialog used by the Load Config, Save Config, and
        // Delete Config menu items
        configPanel = new JPanel(new GridBagLayout());
        configSelLabel = new JLabel();
        configSelLabel.setFont(LABEL_FONT_BOLD);
        configComboBox = new JComboBox();
        configComboBox.setFont(LABEL_FONT_PLAIN);
        configComboBox.setBorder(BorderFactory.createBevelBorder(BevelBorder.LOWERED,
                                                                 Color.LIGHT_GRAY,
                                                                 Color.GRAY));
        configWhichLabel = new JLabel();
        configWhichLabel.setFont(LABEL_FONT_BOLD);

        // Get the default background color; this is used to color the ID and
        // log file text areas
        Color backColor = new Color(configPanel.getBackground().getRGB());

        // Create text areas to hold the ID and log file names
        JTextArea configIDLogTextArea = new JTextArea("ID:\nLog:  ");
        configIDLogTextArea.setFont(LABEL_FONT_BOLD);
        configIDLogTextArea.setEditable(false);
        configIDLogTextArea.setBorder(emptyBorder);
        configIDLogTextArea.setBackground(backColor);
        configIDLogTextArea.setFocusable(false);
        configFilesTextArea = new JTextArea();
        configFilesTextArea.setFont(LABEL_FONT_PLAIN);
        configFilesTextArea.setEditable(false);
        configFilesTextArea.setBorder(emptyBorder);
        configFilesTextArea.setBackground(backColor);
        configFilesTextArea.setFocusable(false);

        // Create panels to hold the ID and log file name text areas
        JPanel configTextPanel = new JPanel(new BorderLayout());
        configTextPanel.setBorder(emptyBorder);
        configTextPanel.setFont(LABEL_FONT_BOLD);
        configTextPanel.add(configIDLogTextArea, BorderLayout.PAGE_START);
        JPanel configFilesPanel = new JPanel(new BorderLayout());
        configFilesPanel.setBorder(emptyBorder);
        configFilesPanel.setFont(LABEL_FONT_BOLD);
        configFilesPanel.add(configFilesTextArea, BorderLayout.PAGE_START);

        // Add the labels and file names to a panel
        JPanel configInfoPanel = new JPanel(new BorderLayout());
        configInfoPanel.add(configTextPanel, BorderLayout.LINE_START);
        configInfoPanel.add(configFilesPanel, BorderLayout.CENTER);

        // Set the initial layout manager characteristics
        GridBagConstraints gbc = new GridBagConstraints(0,
                                                        0,
                                                        1,
                                                        1,
                                                        1.0,
                                                        0.0,
                                                        GridBagConstraints.LINE_START,
                                                        GridBagConstraints.HORIZONTAL,
                                                        new Insets(0,
                                                                   LABEL_HORIZONTAL_SPACING,
                                                                   0,
                                                                   LABEL_HORIZONTAL_SPACING),
                                                        0,
                                                        0);

        // Add the dialog components
        configPanel.add(configSelLabel, gbc);
        gbc.gridy++;
        gbc.insets.set(LABEL_VERTICAL_SPACING,
                       LABEL_HORIZONTAL_SPACING * 2,
                       LABEL_VERTICAL_SPACING * 2,
                       LABEL_HORIZONTAL_SPACING);
        configPanel.add(configComboBox, gbc);
        gbc.gridy++;
        gbc.insets.set(LABEL_VERTICAL_SPACING,
                       LABEL_HORIZONTAL_SPACING,
                       0,
                       LABEL_HORIZONTAL_SPACING);
        configPanel.add(configWhichLabel, gbc);
        gbc.gridy++;
        gbc.insets.set(LABEL_VERTICAL_SPACING,
                       LABEL_HORIZONTAL_SPACING * 2,
                       LABEL_VERTICAL_SPACING * 2,
                       LABEL_HORIZONTAL_SPACING);
        configPanel.add(configInfoPanel, gbc);

        // Add a separator
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridy++;
        gbc.insets.set(0,
                       LABEL_HORIZONTAL_SPACING,
                       0,
                       LABEL_HORIZONTAL_SPACING);
        configPanel.add(new JSeparator(JSeparator.HORIZONTAL), gbc);

        // Dialog title
        String dialogTitle = "";

        // Modify the config dialog for load, save, or delete
        switch (dialogType)
        {
            case LOAD:
                dialogTitle = "Load Config";
                configSelLabel.setText("Select a config to load");
                configWhichLabel.setText("Selected config's files");
                useSelection = true;
                configComboBox.setEditable(false);
                break;

            case SAVE:
                dialogTitle = "Save Config";
                configSelLabel.setText("<html>Enter a new name or select<br>an existing config to save");
                configWhichLabel.setText("Files to save");
                useSelection = false;
                configComboBox.setEditable(true);
                break;

            case DELETE:
                dialogTitle = "Delete Config";
                configSelLabel.setText("Select a config to delete");
                configWhichLabel.setText("Selected config's files");
                useSelection = true;
                configComboBox.setEditable(false);
                break;
        }

        // Get the list of configs and add these to the combo box, and if a
        // config has been previously chosen then initially select it
        configComboBox.setModel(new DefaultComboBoxModel(getConfigList()));
        configComboBox.setSelectedItem(perfMain.getSelectedConfig());

        // Create the string containing the ID and log file names and display
        // it underneath the combo box
        configFilesTextArea.setText(parseConfigFileNames());

        // Listen for combo box selection changes
        configComboBox.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Update the config information based on the combo box selection
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Create the string containing the ID and log file names and
                // display it underneath the combo box
                configFilesTextArea.setText(parseConfigFileNames());

                // Resize the dialog, if needed, to fit the number and length
                // of the config file information
                SwingUtilities.windowForComponent(configFilesTextArea).pack();
            }
        });

        // Saved config file name error flag
        boolean errorFlag = true;

        // Keep showing the dialog as long as the saved config name is invalid
        // (if this is a Save Config dialog) or the user selects Cancel
        while (errorFlag)
        {
            // Assume the saved config name is valid (always true for the Load
            // and Delete Configs dialogs)
            errorFlag = false;

            // Display the Configs dialog and wait for the user to select the
            // Okay or Cancel button
            buttonSelected = showOptionsDialog(perfMain.getMainFrame(),
                                               configPanel,
                                               dialogTitle,
                                               OK_CANCEL_OPTION,
                                               false);

            // Check if the Okay button was selected
            if (buttonSelected == OK_OPTION)
            {
                // Get the config name and remove any white space surrounding
                // it
                perfMain.setSelectedConfig(configComboBox.getSelectedItem().toString().trim());
            }
        }
    }

    /**************************************************************************
     * Verify that the config name is valid
     * 
     * @return true if the config name is valid
     *************************************************************************/
    @Override
    protected boolean verifySelection()
    {
        // Assume the dialog input is valid
        boolean validFlag = true;

        // Only check if this is a Save Config dialog box
        if (dialogType == SAVE)
        {
            // Check if the config name, after removing any whitespace
            // surrounding it, is blank
            if (configComboBox.getSelectedItem().toString().trim().equals(""))
            {
                // Inform the user that the name is invalid
                new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                         "<html><b>Config name cannot be blank",
                                                         "Invalid Config Name",
                                                         JOptionPane.ERROR_MESSAGE,
                                                         OK_OPTION);

                // Set the flag to indicate an invalid input
                validFlag = false;
            }
        }

        return validFlag;
    }

    /**************************************************************************
     * Read the sorted list of configurations from the program preferences
     * backing store
     * 
     * @return Array of configs, or a single item array containing an empty
     *         string if no configs are found
     *************************************************************************/
    private String[] getConfigList()
    {
        List<Object> cfgList = new ArrayList<Object>();
        String[] cfgArray = null;

        try
        {
            // Check if any configs are stored in the program preferences
            // backing store
            for (String key : perfMain.getPreferences().keys())
            {
                // Only process keys starting with the text "CONFIG"
                if (key.startsWith("CONFIG"))
                {
                    // Config found; add to list
                    cfgList.add(key.substring(6));
                }
            }
        }
        catch (BackingStoreException bse)
        {
            // Inform the user that an error occurred accessing the preference
            // keys
            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                     "<html><b>Error accessing program preferences",
                                                     "File Warning",
                                                     JOptionPane.WARNING_MESSAGE,
                                                     OK_OPTION);
        }

        // Check if any configs were found
        if (cfgList.size() != 0)
        {
            int index = 0;
            cfgArray = new String[cfgList.size()];

            // Convert the list of configs to an array
            for (Object cfg : cfgList)
            {
                cfgArray[index] = cfg.toString();
                index++;
            }

            // Sort the config names alphabetically
            Arrays.sort(cfgArray);
        }
        // No configs were found
        else
        {
            // Create an array with an empty string so that the caller
            // has something to work with
            cfgArray = new String[1];
            cfgArray[0] = "";
        }

        return cfgArray;
    }

    /**************************************************************************
     * Parse the currently selected config string from the program preferences
     * backing store to extract the ID and log file names and return the names
     * to the caller, formatted for display in a column
     * 
     * @return String showing the ID file name and the log file name(s),
     *         separated by carriage returns
     *************************************************************************/
    private String parseConfigFileNames()
    {
        String fileNames;

        // Check if the combo box file names should be used
        if (useSelection)
        {
            // Get the config currently selected in the config dialog combo box
            fileNames = perfMain.getPreference("CONFIG"
                                               + configComboBox.getSelectedItem());
        }
        // Don't use the combo box file names
        else
        {
            // Use the file names from the program preferences backing store
            fileNames = perfMain.getPreference(PERF_ID_LIST_FILE)
                        + ","
                        + perfMain.getPreference(PERF_LOG_FILE);
        }

        // Divide the file name string into parts using ',' as the delimiter
        String[] parts = fileNames.split(",");

        // Get the ID file name, minus the path name
        String idFile = parts[0].substring(parts[0].lastIndexOf(File.separator) + 1);

        // There may be multiple log file names, so divide the log file portion
        // of the string into parts using the specified delimiter
        String[] logFile = parts[1].split(LOG_SEPARATOR);

        // Store the ID file name into the return string
        String files = idFile;

        // For each of the log file names
        for (int index = 0; index < logFile.length; index++)
        {
            // Remove the path name and store the log file name in the return
            // string
            files += "\n"
                     + logFile[index].substring(logFile[index].lastIndexOf(File.separator)
                         + 1);
        }

        return files;
    }
}
