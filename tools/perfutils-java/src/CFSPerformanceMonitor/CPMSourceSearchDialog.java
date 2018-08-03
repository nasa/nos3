/**
 * CFS Performance Monitor Source Code Search Criteria Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.BUTTON_FONT;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_ICON;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_FILE_EXTENSION;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_PERF_ID_SUFFIX;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_SUBFOLDERS;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LABEL_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LABEL_VERTICAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.OK_ICON;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.RESET_ICON;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_FILE_EXTENSION;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_PERF_ID_SUFFIX;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_SUBFOLDERS;

import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JTextField;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;

/******************************************************************************
 * CFS Performance Monitor source code search criteria dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMSourceSearchDialog extends CPMDialogHandler
{
    private final CPMMain perfMain;

    // Components that need to be accessed globally
    private List<JCheckBox> subfolderCheckBox;
    private JTextField extensionField;
    private JTextField suffixField;

    // Folder containing the subfolders to search in for IDs
    private final File folder;

    // Button selected by the user
    private int buttonSelected;

    /**************************************************************************
     * Search criteria dialog class constructor
     * 
     * @param perfMain
     *            main class
     *************************************************************************/
    protected CPMSourceSearchDialog(CPMMain perfMain,
                                    File folder)
    {
        this.perfMain = perfMain;
        this.folder = folder;

        // Create the search criteria dialog
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
     * Create the search criteria dialog
     *************************************************************************/
    private void initialize()
    {
        // Create a border for the text fields
        Border border = BorderFactory.createBevelBorder(BevelBorder.LOWERED,
                                                        Color.LIGHT_GRAY,
                                                        Color.GRAY);

        // Set the initial layout manager characteristics
        GridBagConstraints gbc = new GridBagConstraints(0,
                                                        0,
                                                        GridBagConstraints.RELATIVE,
                                                        1,
                                                        0.0,
                                                        0.0,
                                                        GridBagConstraints.LINE_START,
                                                        GridBagConstraints.NONE,
                                                        new Insets(0,
                                                                   LABEL_HORIZONTAL_SPACING,
                                                                   LABEL_VERTICAL_SPACING * 2,
                                                                   LABEL_HORIZONTAL_SPACING),
                                                        0,
                                                        0);

        // Create the search criteria components ------------------------------
        JPanel searchPanel = new JPanel(new GridBagLayout());

        // Set up the subfolders label
        JLabel subfoldersLabel = new JLabel("Search subfolders");
        subfoldersLabel.setFont(LABEL_FONT_BOLD);
        searchPanel.add(subfoldersLabel, gbc);

        // Create a list of check boxes showing the target folder's subfolders.
        // Get the list of files and folders in the target folder and sort them
        // alphabetically
        File[] folderItems = folder.listFiles();
        Arrays.sort(folderItems);

        // Create a panel to contain the check boxes
        JPanel subPanel = new JPanel(new GridLayout(1, 0));

        // Create the check box array list
        subfolderCheckBox = new ArrayList<JCheckBox>();

        // Step through the list of files & folders in the target folder
        for (File item : folderItems)
        {
            // Check if this if a folder and not a file
            if (item.isDirectory())
            {
                // Create a check box with the subfolder name as the label and
                // add the check box to the panel
                subfolderCheckBox.add(new JCheckBox(item.getName()));
                subPanel.add(subfolderCheckBox.get(subfolderCheckBox.size() - 1));
            }
        }

        // Check if no subfolders exist in the target folder
        if (subfolderCheckBox.size() == 0)
        {
            // Indicate that the files within the target folder will be checked
            // for IDs
            JLabel allLabel = new JLabel("<html><i>all files in selected folder");
            allLabel.setFont(LABEL_FONT_PLAIN);
            subPanel.add(allLabel);
        }
        // At least one subfolder exists
        else
        {
            // Set the number of rows in which to group the subfolder check
            // boxes. This puts them into a more or less square grid
            ((GridLayout) subPanel.getLayout()).setColumns((int) Math.sqrt(subfolderCheckBox.size()));
            ((GridLayout) subPanel.getLayout()).setRows(0);
        }

        // Set the selection statuses of the check boxes based on the
        // preferences information
        setSubfolderSelected();

        // Place the subfolder search panel into the dialog
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx++;
        searchPanel.add(subPanel, gbc);

        // Set up the file extension label
        JLabel extensionLabel = new JLabel("File extension");
        extensionLabel.setFont(LABEL_FONT_BOLD);
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridwidth = GridBagConstraints.RELATIVE;
        gbc.gridx = 0;
        gbc.gridy++;
        searchPanel.add(extensionLabel, gbc);

        // Create the file extension text field
        extensionField = new JTextField(" "
                                        + perfMain.getPreferences().get(SEARCH_FILE_EXTENSION,
                                                                        DEFAULT_FILE_EXTENSION),
                                        10);
        extensionField.setFont(LABEL_FONT_PLAIN);
        extensionField.setBorder(border);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx++;
        searchPanel.add(extensionField, gbc);

        // Create the performance ID suffix label
        JLabel suffixLabel = new JLabel("Performance ID suffix");
        suffixLabel.setFont(LABEL_FONT_BOLD);
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridwidth = GridBagConstraints.RELATIVE;
        gbc.gridx = 0;
        gbc.gridy++;
        searchPanel.add(suffixLabel, gbc);

        // Create the suffix text field
        suffixField = new JTextField(" "
                                     + perfMain.getPreferences().get(SEARCH_PERF_ID_SUFFIX,
                                                                     DEFAULT_PERF_ID_SUFFIX));
        suffixField.setFont(LABEL_FONT_PLAIN);
        suffixField.setBorder(border);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx++;
        searchPanel.add(suffixField, gbc);

        // Add a separator
        gbc.gridx = 0;
        gbc.gridy++;
        gbc.insets.set(0,
                       LABEL_HORIZONTAL_SPACING,
                       0,
                       LABEL_HORIZONTAL_SPACING);
        searchPanel.add(new JSeparator(JSeparator.HORIZONTAL), gbc);

        // Create a panel to hold the dialog Okay, Default, and Cancel buttons
        JPanel buttonPanel = new JPanel();

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
             * Apply the search criteria changes
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set the button selected to Okay
                setButtonSelected(OK_OPTION);

                // Check that the search criteria are valid
                if (verifySelection())
                {
                    // Build the list of subfolders to search from the selected
                    // check boxes
                    String subfolderNames = "";

                    // Check if no subfolders exist in the target folder
                    if (subfolderCheckBox.size() == 0)
                    {
                        // Set the target folder an the subfolder to check;
                        // i.e., search through the files within the target
                        // folder
                        subfolderNames = "../" + folder.getName();
                    }
                    // At least one subfolder exists
                    else
                    {
                        // Step through the subfolder check boxes
                        for (JCheckBox box : subfolderCheckBox)
                        {
                            // Check if the subfolder is selected
                            if (box.isSelected())
                            {
                                // Add the subfolder to the search list
                                subfolderNames += box.getText() + ",";
                            }
                        }

                        // Remove the trailing comma
                        subfolderNames = subfolderNames.substring(0,
                                                                  subfolderNames.length() - 1);
                    }

                    // Store the search criteria in the program preferences
                    // backing store
                    perfMain.getPreferences().put(SEARCH_SUBFOLDERS,
                                                  subfolderNames);
                    perfMain.getPreferences().put(SEARCH_FILE_EXTENSION,
                                                  extensionField.getText().trim());
                    perfMain.getPreferences().put(SEARCH_PERF_ID_SUFFIX,
                                                  suffixField.getText().trim());

                    // Close the dialog
                    closeDialog();
                }
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
             * Set the search criteria to the default values
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Update the check boxes and text fields to show the default
                // values
                setSubfolderSelected();
                extensionField.setText(" " + DEFAULT_FILE_EXTENSION);
                suffixField.setText(" " + DEFAULT_PERF_ID_SUFFIX);
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
             * Cancel changing the search criteria
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set the button selected to Cancel
                setButtonSelected(CANCEL_OPTION);
                closeDialog();
            }
        });

        // Display the search criteria dialog and wait for the user to select
        // the Okay or Cancel button
        buttonSelected = showOptionsDialog(perfMain.getMainFrame(),
                                           searchPanel,
                                           buttonPanel,
                                           "Search Criteria",
                                           false);
    }

    /**************************************************************************
     * Set the state of the subfolder check boxes based on the stored
     * preference information
     *************************************************************************/
    private void setSubfolderSelected()
    {
        // Get the subfolder preference information and split it into separate
        // subfolder names
        String[] subfolderPrefs = perfMain.getPreferences().get(SEARCH_SUBFOLDERS,
                                                                DEFAULT_SUBFOLDERS).split(",");

        // Step through each subfolder check box
        for (JCheckBox box : subfolderCheckBox)
        {
            // Assume the box is not selected
            boolean selected = false;

            // Step through the preference subfolder names
            for (String subfolderName : subfolderPrefs)
            {
                // Check if the preference name matches the check box name
                if (subfolderName.toLowerCase().equals(box.getText().toLowerCase()))
                {
                    // Select the check box and exit the loop
                    selected = true;
                    break;
                }
            }

            // Set the state of this check box
            box.setSelected(selected);
        }
    }

    /**************************************************************************
     * Verify that the search criteria are valid
     * 
     * @return true if the search criteria are valid
     *************************************************************************/
    @Override
    protected boolean verifySelection()
    {
        // Assume the inputs contain an error
        boolean validInputs = false;

        // Get the criteria from the input fields, and remove leading and
        // trailing white space
        String extension = extensionField.getText().trim();
        String suffix = suffixField.getText().trim();

        // Assume none of the check boxes are selected
        boolean noSubfolderSelected = true;

        // Step through the subfolder check boxes
        for (JCheckBox box : subfolderCheckBox)
        {
            // Check if the box is selected
            if (box.isSelected())
            {
                // Clear the "none selected" flag and exit the loop
                noSubfolderSelected = false;
                break;
            }
        }

        // Check if the target folder contains one or more subfolders and none
        // is selected
        if (noSubfolderSelected && subfolderCheckBox.size() != 0)
        {
            // Let the user know that a subfolder must be selected
            new CPMDialogHandler().showMessageDialog(this,
                                                     "<html><b>At least one subfolder must be selected",
                                                     "Invalid Criteria",
                                                     JOptionPane.ERROR_MESSAGE,
                                                     OK_OPTION);
        }
        // Check if the extension or suffix field is empty
        else if (extension.equals("")
                 || suffix.equals(""))
        {
            // Let the user know that the fields can't be empty
            new CPMDialogHandler().showMessageDialog(this,
                                                     "<html><b>Search criteria fields cannot be blank",
                                                     "Invalid Criteria",
                                                     JOptionPane.ERROR_MESSAGE,
                                                     OK_OPTION);
        }
        // Check if the file extension or ID suffix fields have spaces or
        // commas
        else if (extension.contains(" ")
                 || extension.contains(",")
                 || suffix.contains(" ")
                 || suffix.contains(","))
        {
            // Let the user know that the extension and suffix fields can't
            // contain spaces or commas
            new CPMDialogHandler().showMessageDialog(this,
                                                     "<html><b>File extension and ID suffix<br>cannot contain spaces or commas",
                                                     "Invalid Criteria",
                                                     JOptionPane.ERROR_MESSAGE,
                                                     OK_OPTION);
        }
        // Inputs are valid
        else
        {
            // set the valid inputs flag
            validInputs = true;
        }

        return validInputs;
    }
}
