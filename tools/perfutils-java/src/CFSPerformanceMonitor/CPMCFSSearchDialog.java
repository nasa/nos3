/**
 * CFS Performance Monitor CFS Search Criteria Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.BUTTON_FONT;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_ICON;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.CFS_FILE_EXTENSION;
import static CFSPerformanceMonitor.CPMConstants.CFS_PERF_ID_SUFFIX;
import static CFSPerformanceMonitor.CPMConstants.CFS_SUBFOLDERS;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_CFS_SUBFOLDERS;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_FILE_EXTENSION;
import static CFSPerformanceMonitor.CPMConstants.DEFAULT_PERF_ID_SUFFIX;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LABEL_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LABEL_VERTICAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.OK_ICON;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.RESET_ICON;

import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JTextField;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;

/******************************************************************************
 * CFS Performance Monitor CFS search criteria dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMCFSSearchDialog extends CPMDialogHandler
{
    private final CPMMain perfMain;

    // Components that need to be accessed globally
    private JTextField subfoldersField;
    private JTextField extensionField;
    private JTextField suffixField;

    // Button selected by the user
    private int buttonSelected;

    /**************************************************************************
     * Search criteria dialog class constructor
     * 
     * @param perfMain
     *            main class
     *************************************************************************/
    protected CPMCFSSearchDialog(CPMMain perfMain)
    {
        this.perfMain = perfMain;

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

        // Set the layout manager characteristics
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.anchor = GridBagConstraints.LINE_START;
        gbc.insets = new Insets(0,
                                LABEL_HORIZONTAL_SPACING,
                                LABEL_VERTICAL_SPACING * 2,
                                LABEL_HORIZONTAL_SPACING);
        gbc.ipadx = 0;

        // Create the search criteria components ------------------------------
        JPanel searchPanel = new JPanel(new GridBagLayout());

        // Set up the subfolders label
        JLabel subfoldersLabel = new JLabel("Search subfolders");
        subfoldersLabel.setFont(LABEL_FONT_BOLD);
        gbc.fill = GridBagConstraints.NONE;
        gbc.gridwidth = GridBagConstraints.RELATIVE;
        gbc.gridx = 0;
        gbc.gridy = 0;
        searchPanel.add(subfoldersLabel, gbc);

        // Get the subfolders text field
        subfoldersField = new JTextField(" "
                                         + perfMain.getPreferences().get(CFS_SUBFOLDERS, ""),
                                         10);
        subfoldersField.setFont(LABEL_FONT_PLAIN);
        subfoldersField.setBorder(border);
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx++;
        searchPanel.add(subfoldersField, gbc);

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
                                        + perfMain.getPreferences().get(CFS_FILE_EXTENSION, ""));
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
                                     + perfMain.getPreferences().get(CFS_PERF_ID_SUFFIX, ""));
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
                    // Store the search criteria in the program preferences
                    // backing store
                    perfMain.getPreferences().put(CFS_SUBFOLDERS,
                                                  subfoldersField.getText().trim());
                    perfMain.getPreferences().put(CFS_FILE_EXTENSION,
                                                  extensionField.getText().trim());
                    perfMain.getPreferences().put(CFS_PERF_ID_SUFFIX,
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
                // Update the text fields to show the default values
                subfoldersField.setText(" " + DEFAULT_CFS_SUBFOLDERS);
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
                                           "CFS Search Criteria",
                                           false);
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
        String subfolders = subfoldersField.getText().trim();
        String extension = extensionField.getText().trim();
        String suffix = suffixField.getText().trim();

        // Check if any of the criteria fields are empty
        if (subfolders.equals("")
            || extension.equals("")
            || suffix.equals(""))
        {
            // Let the user know that the fields can't be empty
            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                     "<html><b>Search criteria fields cannot be blank",
                                                     "Invalid Value",
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
            new CPMDialogHandler().showMessageDialog(perfMain.getMainFrame(),
                                                     "<html><b>File extension and ID suffix<br>cannot contain spaces or commas",
                                                     "Invalid Value",
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
