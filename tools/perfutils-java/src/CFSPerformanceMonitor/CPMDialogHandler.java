/**
 * CFS Performance Monitor Custom dialog handler
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.APPEND_OPTION;
import static CFSPerformanceMonitor.CPMConstants.BUTTON_FONT;
import static CFSPerformanceMonitor.CPMConstants.BUTTON_GAP;
import static CFSPerformanceMonitor.CPMConstants.BUTTON_PAD;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_ICON;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.CLEAR_OPTION;
import static CFSPerformanceMonitor.CPMConstants.CLOSE_ICON;
import static CFSPerformanceMonitor.CPMConstants.CLOSE_OPTION;
import static CFSPerformanceMonitor.CPMConstants.DELETE_ICON;
import static CFSPerformanceMonitor.CPMConstants.DIALOG_BORDER_PAD;
import static CFSPerformanceMonitor.CPMConstants.DIALOG_INNER_PAD;
import static CFSPerformanceMonitor.CPMConstants.DIALOG_MINIMUM_HEIGHT;
import static CFSPerformanceMonitor.CPMConstants.ERROR_ICON;
import static CFSPerformanceMonitor.CPMConstants.INFORMATION_ICON;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_PLAIN;
import static CFSPerformanceMonitor.CPMConstants.LOAD_OPTION;
import static CFSPerformanceMonitor.CPMConstants.LOG_SEPARATOR;
import static CFSPerformanceMonitor.CPMConstants.NEW_ICON;
import static CFSPerformanceMonitor.CPMConstants.OK_ICON;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.PRINT_ICON;
import static CFSPerformanceMonitor.CPMConstants.PRINT_OPTION;
import static CFSPerformanceMonitor.CPMConstants.QUESTION_ICON;
import static CFSPerformanceMonitor.CPMConstants.READ_OPTION;
import static CFSPerformanceMonitor.CPMConstants.SAVE_OPTION;
import static CFSPerformanceMonitor.CPMConstants.SEARCH_OPTION;
import static CFSPerformanceMonitor.CPMConstants.WARNING_ICON;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.KeyboardFocusManager;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.File;

import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.UIManager;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.colorchooser.AbstractColorChooserPanel;

/******************************************************************************
 * CFS Performance Monitor dialog handler class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMDialogHandler extends JDialog
{
    // Panel containing the dialog's buttons
    private JPanel buttonPanel;

    // Button selected by the user
    private int buttonSelected;

    // Number of rows on which to arrange the buttons
    private int buttonRows;

    // Last button on the dialog's button panel. This button closes/cancels
    // the dialog
    private JButton exitButton;

    // Flag indicating if the dialog's minimum size has been set
    private boolean minimumSizeSet;

    // Minimum width, in pixels, required to display a table within a dialog
    private int tableWidth;

    /**************************************************************************
     * Dialog box handler constructor
     *************************************************************************/
    protected CPMDialogHandler()
    {
        buttonSelected = JOptionPane.CLOSED_OPTION;
        buttonRows = 1;
        minimumSizeSet = false;
        tableWidth = -1;
    }

    /**************************************************************************
     * Close the dialog box
     *************************************************************************/
    protected void closeDialog()
    {
        setVisible(false);
        dispose();
    }

    /**************************************************************************
     * Set the button selected
     * 
     * @param button
     *            dialog button selected by the user
     *************************************************************************/
    protected void setButtonSelected(int button)
    {
        buttonSelected = button;
    }

    /**************************************************************************
     * Set the number of rows occupied by the dialog's buttons
     * 
     * @param rows
     *            number of dialog button rows
     *************************************************************************/
    protected void setButtonRows(int rows)
    {
        buttonRows = rows;
    }

    /**************************************************************************
     * Display a non-resizable, modal message dialog with the provided icon and
     * return the button type selected
     * 
     * @param parent
     *            window to center the dialog over
     * 
     * @param message
     *            text message to display
     * 
     * @param title
     *            title to display in the dialog window frame
     * 
     * @param optionType
     *            dialog type: LOAD_OPTION, SAVE_OPTION, SEARCH_OPTION,
     *            READ_OPTION, PRINT_OPTION, CLOSE_OPTION, OK_OPTION, or
     *            OK_CANCEL_OPTION
     * 
     * @param icon
     *            icon to display to the left of the text message
     * 
     * @return Selected button type
     *************************************************************************/
    protected int showMessageDialog(Component parent,
                                    String message,
                                    String title,
                                    int optionType,
                                    Icon icon)
    {
        return createDialog(parent,
                            message,
                            null,
                            title,
                            optionType,
                            icon,
                            false,
                            true);
    }

    /**************************************************************************
     * Display a non-resizable, modal message dialog and return the button type
     * selected. The icon displayed is based on the message type
     * 
     * @param parent
     *            window to center the dialog over
     * 
     * @param message
     *            text message to display
     * 
     * @param title
     *            title to display in the dialog window frame
     * 
     * @param messageType
     *            message type: JOptionPane.PLAIN_MESSAGE,
     *            JOptionPane.INFORMATION_MESSAGE,
     *            JOptionPane.QUESTION_MESSAGE, JOptionPane.WARNING_MESSAGE, or
     *            JOptionPane.ERROR_MESSAGE
     * 
     * @param optionType
     *            dialog type: LOAD_OPTION, SAVE_OPTION, SEARCH_OPTION,
     *            READ_OPTION, PRINT_OPTION, CLOSE_OPTION, OK_OPTION, or
     *            OK_CANCEL_OPTION
     * 
     * @return Selected button type
     *************************************************************************/
    protected int showMessageDialog(Component parent,
                                    String message,
                                    String title,
                                    int messageType,
                                    int optionType)
    {
        Icon icon = null;

        // Get the icon to display beside the text message based on the current
        // look & feel. If the look & feel doesn't supply icons then revert to
        // a default set
        switch (messageType)
        {
            case JOptionPane.PLAIN_MESSAGE:
                icon = null;
                break;

            case JOptionPane.INFORMATION_MESSAGE:
                icon = getIcon("OptionPane.informationIcon", INFORMATION_ICON);
                break;

            case JOptionPane.QUESTION_MESSAGE:
                icon = getIcon("OptionPane.questionIcon", QUESTION_ICON);
                break;

            case JOptionPane.WARNING_MESSAGE:
                icon = getIcon("OptionPane.warningIcon", WARNING_ICON);
                break;

            case JOptionPane.ERROR_MESSAGE:
                icon = getIcon("OptionPane.errorIcon", ERROR_ICON);
                break;
        }

        return createDialog(parent,
                            message,
                            null,
                            title,
                            optionType,
                            icon,
                            false,
                            true);
    }

    /**************************************************************************
     * Get the icon associated with the UI key provided. If no icon is
     * available for the key then substitute a default icon
     * 
     * @param UIKey
     *            an Object specifying the icon
     * 
     * @param defaultIcon
     *            icon to use if the icon specified by UIKey cannot be found
     * 
     * @return Icon associated with UIKey
     *************************************************************************/
    private Icon getIcon(String UIKey, String defaultIcon)
    {
        // Get the icon associated with the UI key
        Icon icon = UIManager.getIcon(UIKey);

        // Check if the icon wasn't found
        if (icon == null)
        {
            // Use the default icon instead
            icon = new ImageIcon(getClass().getResource(defaultIcon));
        }

        return icon;
    }

    /**************************************************************************
     * Display a modal, user-interactive dialog using buttons defined by the
     * supplied option type. The dialog may be resized, based on the input
     * flag. Return the button type selected
     * 
     * @param parent
     *            window to center the dialog over
     * 
     * @param dialogPanel
     *            panel containing the dialog components
     * 
     * @param title
     *            title to display in the dialog window frame
     * 
     * @param optionType
     *            dialog type: LOAD_OPTION, SAVE_OPTION, SEARCH_OPTION,
     *            READ_OPTION, PRINT_OPTION, CLOSE_OPTION, OK_OPTION, or
     *            OK_CANCEL_OPTION
     * 
     * @param resizable
     *            true to allow the dialog to be resized
     * 
     * @return Selected button type
     *************************************************************************/
    protected int showOptionsDialog(Component parent,
                                    Component dialogPanel,
                                    String title,
                                    int optionType,
                                    boolean resizable)
    {
        // Used for the Appearance, Config, Log Header, and Set Bounds dialogs
        return createDialog(parent,
                            dialogPanel,
                            null,
                            title,
                            optionType,
                            null,
                            resizable,
                            true);
    }

    /**************************************************************************
     * Display a non-modal, resizable, user-interactive dialog with no title,
     * using buttons defined by the PRINT_OPTION option type. Return the button
     * type selected
     * 
     * @param parent
     *            window to center the dialog over
     * 
     * @param dialogPanel
     *            panel containing the dialog components
     * 
     * @return Selected button type
     *************************************************************************/
    protected int showOptionsDialog(Component parent,
                                    Component dialogPanel)
    {
        // Used by the Log Data and Log Statistics dialogs
        return createDialog(parent,
                            dialogPanel,
                            null,
                            null,
                            PRINT_OPTION,
                            null,
                            true,
                            false);
    }

    /**************************************************************************
     * Display a modal, user-interactive dialog using user-supplied buttons.
     * The dialog may be resized, based on the input flag. Return the button
     * type selected
     * 
     * @param parent
     *            window to center the dialog over
     * 
     * @param dialogPanel
     *            panel containing the dialog components
     * 
     * @param lowerPanel
     *            panel containing the dialog buttons
     * 
     * @param title
     *            title to display in the dialog window frame
     * 
     * @param resizable
     *            true to allow the dialog to be resized
     * 
     * @return Selected button type
     *************************************************************************/
    protected int showOptionsDialog(Component parent,
                                    Object dialogPanel,
                                    JPanel lowerPanel,
                                    String title,
                                    boolean resizable)
    {
        // Used for the ID Editor and Preferences dialogs
        return createDialog(parent,
                            dialogPanel,
                            lowerPanel,
                            title,
                            0,
                            null,
                            resizable,
                            true);
    }

    /**************************************************************************
     * Display a non-resizable, modal, user-interactive dialog with Append,
     * Clear, and Cancel buttons. Return the button type selected
     * 
     * @param parent
     *            window to center the dialog over
     * 
     * @param message
     *            text message to display
     * 
     * @param title
     *            title to display in the dialog window frame
     * 
     * @return Selected button type
     *************************************************************************/
    protected int showOptionsDialog(Component parent,
                                    String message,
                                    String title)
    {
        // Create a panel to hold the dialog's buttons
        JPanel lowerPanel = new JPanel();

        // Create the Append button
        JButton btnAppend = new JButton("Append",
                                        new ImageIcon(getClass().getResource(NEW_ICON)));
        btnAppend.setFont(BUTTON_FONT);
        btnAppend.setMnemonic('A');
        lowerPanel.add(btnAppend);

        // Add a listener for the Append button
        btnAppend.addActionListener(new ActionListener()
        {
            /******************************************************
             * Append to the list of IDs
             *****************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set the button selected to Append
                buttonSelected = APPEND_OPTION;

                // Close the dialog
                closeDialog();
            }
        });

        // Create the Clear button
        JButton btnClear = new JButton("Clear",
                                       new ImageIcon(getClass().getResource(DELETE_ICON)));
        btnClear.setFont(BUTTON_FONT);
        btnClear.setMnemonic('L');
        lowerPanel.add(btnClear);

        // Add a listener for the Clear button
        btnClear.addActionListener(new ActionListener()
        {
            /******************************************************
             * Delete the list of IDs
             *****************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set the button selected to Clear
                buttonSelected = CLEAR_OPTION;

                // Close the dialog
                closeDialog();
            }
        });

        // Create the Cancel button
        JButton btnCancel = new JButton("Cancel",
                                        new ImageIcon(getClass().getResource(CANCEL_ICON)));
        btnCancel.setFont(BUTTON_FONT);
        btnCancel.setMnemonic('C');
        lowerPanel.add(btnCancel);

        // Add a listener for the Cancel button
        btnCancel.addActionListener(new ActionListener()
        {
            /******************************************************
             * Cancel appending to or deleting the IDs
             *****************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Set the button selected to Cancel
                buttonSelected = CANCEL_OPTION;

                // Close the dialog
                closeDialog();
            }
        });

        // Display the append/delete dialog
        return createDialog(parent,
                            "<html><b>" + message,
                            lowerPanel,
                            title,
                            0,
                            getIcon("OptionPane.questionIcon", QUESTION_ICON),
                            false,
                            true);
    }

    /**************************************************************************
     * Display a non-resizable, modal, user-interactive color choice dialog
     * using buttons defined by the supplied option type and return the color
     * selected (null if the Cancel button is pressed)
     * 
     * @param parent
     *            window to center the dialog over
     * 
     * @param title
     *            title to display in the dialog window frame
     * 
     * @param optionType
     *            dialog type: LOAD_OPTION, SAVE_OPTION, SEARCH_OPTION,
     *            READ_OPTION, PRINT_OPTION, CLOSE_OPTION, OK_OPTION, or
     *            OK_CANCEL_OPTION
     * 
     * @param initialColor
     *            color initially selected when the dialog appears
     * 
     * @return Selected color (null if Cancel button pressed)
     *************************************************************************/
    protected Color showColorDialog(Component parent,
                                    String title,
                                    int optionType,
                                    Color initialColor)
    {
        Color selectedColor = null;

        // Create a color chooser and set the selected color to the initial
        // color
        final JColorChooser chooser = new JColorChooser(initialColor);

        // Create a panel to hold the color preview text and color boxes
        JPanel previewPanel = new JPanel(new BorderLayout());
        JLabel previewLabel = new JLabel("Preview", SwingConstants.CENTER);
        previewLabel.setFont(LABEL_FONT_BOLD);

        // Calculate the size of the color preview boxes based on the width of
        // the preview label text
        int previewWidth = (int) previewLabel.getFontMetrics(LABEL_FONT_BOLD).getStringBounds(previewLabel.getText(),
                                                                                              previewLabel.getGraphics()).getWidth() / 2 - 1;
        Dimension previewSize = new Dimension(previewWidth, previewWidth);

        // Create a panel to show the original color
        JPanel initialPanel = new JPanel();
        initialPanel.setOpaque(true);
        initialPanel.setBackground(initialColor);
        initialPanel.setPreferredSize(previewSize);

        // Create a panel to show the new color
        final JPanel newPanel = new JPanel();
        newPanel.setOpaque(true);
        newPanel.setBackground(initialColor);
        newPanel.setPreferredSize(previewSize);

        // Add the preview text and color boxes to the preview panel
        previewPanel.add(previewLabel, BorderLayout.PAGE_START);
        previewPanel.add(initialPanel, BorderLayout.LINE_START);
        previewPanel.add(newPanel, BorderLayout.LINE_END);

        // Add a listener for changes to the preview panel in order to capture
        // color change events. These are generated when the user changes the
        // color in the color chooser panel
        previewPanel.addPropertyChangeListener("foreground", new PropertyChangeListener()
        {
            /******************************************************************
             * Handle foreground color change events
             *****************************************************************/
            @Override
            public void propertyChange(PropertyChangeEvent pce)
            {
                // Update the preview color to the current choice
                newPanel.setBackground(chooser.getColor());
            }
        });

        // Set the color chooser's color preview panel
        chooser.setPreviewPanel(previewPanel);

        // Create the panel for holding the color chooser(s) and the
        // preview panel
        JPanel colorPanel = new JPanel(new FlowLayout(FlowLayout.CENTER,
                                                      20,
                                                      5));

        // Get the color chooser panels available to the current look & feel
        AbstractColorChooserPanel[] chooserPanel = chooser.getChooserPanels();

        // Check if there's only a single color chooser panel
        if (chooserPanel.length == 1)
        {
            // Add the color chooser panel to the dialog panel
            colorPanel.add(chooserPanel[0]);
        }
        // There's more than one color chooser panel
        else
        {
            // Create a tabbed pane
            JTabbedPane tabbedPane = new JTabbedPane(JTabbedPane.TOP);
            tabbedPane.setFont(LABEL_FONT_BOLD);
            colorPanel.add(tabbedPane);

            // Create a border for the tabbed panes
            Border border = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);

            // Step through the available color chooser panels
            for (int index = 0; index < chooserPanel.length; index++)
            {
                // Create a panel to hold the chooser so that it's centered in
                // the tab panel
                JPanel pnl = new JPanel(new GridBagLayout());
                pnl.setBorder(border);
                pnl.add(chooserPanel[index]);

                // Set the name, component, and tool tip text (if extant) of
                // the tab for each color chooser panel
                tabbedPane.addTab(chooserPanel[index].getDisplayName(),
                                  null,
                                  pnl,
                                  chooserPanel[index].getToolTipText());
            }

            // Add the color preview panel
            colorPanel.add(previewPanel);
        }

        // Display the color choice dialog and check if the user selected the
        // OK button
        if (createDialog(parent,
                         colorPanel,
                         null,
                         title,
                         optionType,
                         null,
                         false,
                         true) == OK_OPTION)
        {
            // Set the color to the one chosen
            selectedColor = chooser.getColor();
        }

        return selectedColor;
    }

    /**************************************************************************
     * Display a dialog that allows the user to select one or more files or a
     * folder
     * 
     * @param perfMain
     *            main class
     * 
     * @param fileName
     *            array of file names initially chosen (if any)
     * 
     * @param folderOnly
     *            true to allow only folders to be selected
     * 
     * @param multipleFiles
     *            true to allow selection of more than one file
     * 
     * @param fileTitle
     *            title to display in the dialog window frame
     * 
     * @param fileKey
     *            key to extract the file name(s) from the program preferences
     *            backing store
     * 
     * @param optionType
     *            dialog type: LOAD_OPTION, SAVE_OPTION, SEARCH_OPTION,
     *            READ_OPTION, PRINT_OPTION, CLOSE_OPTION, OK_OPTION, or
     *            OK_CANCEL_OPTION
     * 
     * @return Array containing the selected file handle(s) (null if the Cancel
     *         button is pressed or if the file name list is empty)
     *************************************************************************/
    protected File[] choosePathFile(CPMMain perfMain,
                                    String[] fileName,
                                    boolean folderOnly,
                                    boolean multipleFiles,
                                    String fileTitle,
                                    String fileKey,
                                    int optionType)
    {
        File[] file = new File[1];

        // Check if no file/path name is provided. If not then the user is
        // asked to choose one (or more, if multiple selection is set)
        if (fileName == null || fileName[0] == null)
        {
            String nameList = "";

            // Create the file chooser. Assume the default path initially; if a
            // path is provided it's set below
            final JFileChooser chooser = new JFileChooser();

            // Read the file/path from the program preferences backing store
            // (each file name is initialized to "" in case it isn't found in
            // the backing store; this prevents sending a null string to the
            // split method). Since this could be a multiple log file string,
            // use 'split' to break up the files and use the first file for the
            // path. This has no effect if only a single file/path is found,
            // and returns "" if the file name entry isn't found in the backing
            // store
            String[] filePath = perfMain.getPreferences().get(fileKey,
                                                              "").split(LOG_SEPARATOR);

            // True to allow multiple files to be selected
            chooser.setMultiSelectionEnabled(multipleFiles);

            // Check if the program preferences backing store has a specified
            // file/path for the key provided
            if (!filePath[0].isEmpty())
            {
                // Create an array of file handles for the current file/path
                // list
                File[] tempFile = new File[filePath.length];

                // Step through the current file/path list
                for (int index = 0; index < filePath.length; index++)
                {
                    // Create a file handle for each file
                    tempFile[index] = new File(filePath[index]);

                    // Append the folder name if folder-only is allowed, else
                    // the file name without the path, to the name list,
                    // bounded by quotes
                    nameList += "\""
                                + (folderOnly
                                             ? tempFile[index].getAbsolutePath()
                                             : tempFile[index].getName())
                                + "\" ";
                }

                // Set dialog's the starting path to that of the first file
                chooser.setCurrentDirectory(tempFile[0]);

                // Use the file handles to initialize the list of files shown
                // selected in the file chooser. This does not highlight the
                // files in all look & feels or operating systems
                chooser.setSelectedFiles(tempFile);
            }

            // Locate the file name input field in the file chooser dialog. In
            // order to use custom buttons in the file chooser dialog, the file
            // name input field must be located and the inputs to it listened
            // for so that the selected file can be updated. Otherwise, changes
            // made to the file name are ignored and when the Okay button is
            // pressed the file name used is the highlighted one in the file
            // list box
            final JTextField nameField = getFileChooserTextField(chooser);

            // Check if only a folder is allowed to be chosen
            if (folderOnly)
            {
                // Allow only the selection of a folder (not a file)
                chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
            }
            // Not folder-only
            else
            {
                // Allow only the selection of files
                chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

                // Add a listener for changes to the selected file(s)
                chooser.addPropertyChangeListener(new PropertyChangeListener()
                {
                    /**********************************************************
                     * Handle changes to the file(s) selected. Whenever a file
                     * selection change strip the path name from the file(s)
                     * and build a list of just the names. Insert this list
                     * into the file chooser's file name text field. This is
                     * done automatically for most look & feels, but not all
                     * (e.g., GTK+)
                     *********************************************************/
                    @Override
                    public void propertyChange(PropertyChangeEvent pce)
                    {
                        // Check if the file selection has changed
                        if (pce.getPropertyName().equals("SelectedFilesChangedProperty"))
                        {
                            String nameList = "";

                            // Step through the selected files
                            for (int index = 0; index < chooser.getSelectedFiles().length; index++)
                            {
                                // Append the file name without the path,
                                // surrounded by quotes, to the name list
                                nameList += "\""
                                            + chooser.getSelectedFiles()[index].getName()
                                            + "\" ";
                            }

                            // Insert the file name list into the file
                            // chooser's file name text field
                            nameField.setText(nameList);
                        }
                    }
                });
            }

            // Insert the file name list into the file chooser's file name text
            // field. Most look & feels do this automatically, but not all
            // (e.g., GTK+)
            nameField.setText(nameList);

            // Hide the file chooser's default buttons
            chooser.setControlButtonsAreShown(false);

            // Open the file chooser dialog and wait for a button click
            if (createDialog(perfMain.getMainFrame(),
                             chooser,
                             null,
                             fileTitle,
                             optionType,
                             null,
                             true,
                             true) == OK_OPTION)
            {
                // Extract the file name(s) from the file chooser name field.
                // Spaces may appear in the names only if the name is within
                // double quotes. The names are split on spaces outside of
                // quotes, at every second quote, or at commas. First any
                // leading or trailing whitespace characters are removed
                String names = nameField.getText().trim();

                // Create a flag to keep track of whether or not the character
                // being checked is inside or outside a pair of quotes
                boolean isQuoted = false;

                // Step through the string containing the file name(s)
                for (int index = 0; index < names.length(); index++)
                {
                    // Get the current character
                    char c = names.charAt(index);

                    // Check if the character is a quote
                    if (c == '"')
                    {
                        // Invert the quote on/off flag
                        isQuoted = !isQuoted;
                    }

                    // Check if the character is a space outside of a pair or
                    // quotes, or is the second quote of a pair of quotes
                    if ((c == ' ' || c == '"') && !isQuoted)
                    {
                        // Replace the character with a comma. Occurrences of
                        // double commas can result from this operation; these
                        // are accounted for later
                        names = names.substring(0, index)
                                + ","
                                + names.substring(index + 1);
                    }
                }

                // Replace every instance of back-to-back commas that may have
                // resulted in the replacement steps above with a single comma,
                // remove every remaining quote, then split the string at the
                // commas, which now delineate the separate file names
                fileName = names.replaceAll(",,+", ",").replaceAll("\"+", "").split(",");

                // Check if the file name text field isn't empty
                if (!fileName[0].isEmpty())
                {
                    // Create a file array
                    file = new File[fileName.length];

                    // Step through the file names/paths
                    for (int i = 0; i < fileName.length; i++)
                    {
                        // Create a file handle for each file name or the path
                        // name. If this is not a folder, prepend the file path
                        // to the name
                        file[i] = new File((folderOnly
                                                      ? ""
                                                      : chooser.getCurrentDirectory()
                                                        + File.separator)
                                           + fileName[i]);
                    }
                }
            }
        }
        // The file name(s) are already supplied
        else
        {
            // Create a file array
            file = new File[fileName.length];

            // Create a file handle for each file name
            for (int index = 0; index < fileName.length; index++)
            {
                file[index] = new File(fileName[index]);
            }
        }

        return file;
    }

    /**************************************************************************
     * Locate and return the file name input field in the file chooser dialog
     * 
     * @param cont
     *            container containing the JFileChooser
     * 
     * @return JFileChooser file name input field
     *************************************************************************/
    private JTextField getFileChooserTextField(Container cont)
    {
        JTextField tf = null;

        // Step through the file chooser components. The components are checked
        // in reverse to account for those look & feels (i.e., Motif) that have
        // a text field for the path in addition to one for the file name. By
        // looking in reverse the field with the file name is found first
        for (int i = cont.getComponentCount() - 1; i >= 0; i--)
        {
            // Get the file chooser component
            Component comp = cont.getComponent(i);

            // Check if the component is a text field
            if (comp instanceof JTextField && comp != null)
            {
                // Store the text field and exit the loop
                tf = (JTextField) comp;
                break;
            }
            // Component isn't a text field; check if the text field hasn't
            // been located (to prevent continued searching after the text
            // field is found) and if this is another container,
            else if (comp instanceof Container)
            {
                // Drill down into this container to look for the buttons
                tf = getFileChooserTextField((Container) comp);

                // Check if the text field was located
                if (tf != null)
                {
                    // Exit the loop
                    break;
                }
            }
        }

        return tf;
    }

    /**************************************************************************
     * Placeholder for method to verify the the dialog box selection(s) prior
     * to closing
     * 
     * @return true
     *************************************************************************/
    protected boolean verifySelection()
    {
        return true;
    }

    /**************************************************************************
     * Fit the dialog containing a table to the contents and set the minimum
     * size
     * 
     * @param totalWidth
     *            total width of the dialog in pixels
     *************************************************************************/
    protected void setDialogMinimumSize(int totalWidth)
    {
        // If the table already exists then check if its minimum total width
        // has grown larger. This is always true for the initial instantiation
        if (totalWidth > tableWidth)
        {
            setMinimumSize(new Dimension(totalWidth + DIALOG_BORDER_PAD * 2,
                                         DIALOG_MINIMUM_HEIGHT));
            minimumSizeSet = true;
        }

        // Save the table width. Use this to determine if the table needs to be
        // resized after it is already been made visible (i.e., if the minimum
        // width gets larger due to longer ID names or a look & feel change)
        tableWidth = totalWidth;
    }

    /**************************************************************************
     * Extract the dialog's button(s) from the supplied panel, then find the
     * widest button calculated from the button's text and icon
     *************************************************************************/
    protected void setButtonWidth()
    {
        int maxWidth = 0;
        int maxHeight = 0;

        // Get the number of buttons in the panel
        int numButtons = buttonPanel.getComponentCount();

        // Step through each button
        for (int index = 0; index < numButtons; index++)
        {
            // Create a pointer to the individual button for easier reference
            JButton button = (JButton) buttonPanel.getComponent(index);

            // Change the button padding between the button's icon/text and the
            // perimeter of the button
            button.setMargin(new Insets(1, 3, 1, 3));

            // Calculate the button width based on the width of the icon, gap
            // between the icon and text, width of the text, left and right
            // margins, and the button border
            int width = button.getIcon().getIconWidth()
                        + button.getIconTextGap()
                        + (int) button.getFontMetrics(BUTTON_FONT).getStringBounds(button.getText(),
                                                                                   button.getGraphics()).getWidth()
                        + button.getMargin().left
                        + button.getMargin().right
                        + button.getBorder().getBorderInsets(button).left
                        + button.getBorder().getBorderInsets(button).right;

            // Check if this is the widest button found in the group
            if (width > maxWidth)
            {
                // Store the width
                maxWidth = width;
            }

            // Get the height of the button text
            int height = (int) button.getFontMetrics(BUTTON_FONT).getStringBounds(button.getText(),
                                                                                  button.getGraphics()).getHeight();

            // Check if the button's icon height is greater than the text
            // height
            if (button.getIcon().getIconHeight() > height)
            {
                // Use the icon height since it's larger
                height = button.getIcon().getIconHeight();
            }

            // Calculate the button height based on the icon/text height, the
            // top and bottom margin, and the button border
            height += button.getMargin().top
                      + button.getMargin().bottom
                      + button.getBorder().getBorderInsets(button).top
                      + button.getBorder().getBorderInsets(button).bottom;

            // Check if this button has the greatest height
            if (height > maxHeight)
            {
                // Store the height
                maxHeight = height;
            }
        }

        // Step through each button again, now that the width and height are
        // known
        for (int index = 0; index < numButtons; index++)
        {
            // Set the size of the button(s)
            ((JButton) buttonPanel.getComponent(index)).setSize(maxWidth, maxHeight);
            ((JButton) buttonPanel.getComponent(index)).setPreferredSize(new Dimension(maxWidth, maxHeight));
        }

        // Size the panel containing the buttons based on the buttons' widths
        // and heights
        Dimension buttonPanelCoord = new Dimension(maxWidth
                                                   * numButtons / buttonRows
                                                   + BUTTON_PAD
                                                   * (numButtons / buttonRows - 1),
                                                   maxHeight * buttonRows
                                                       + BUTTON_GAP * (buttonRows - 1)
                                                       + BUTTON_PAD / 2);

        // Set the button panel to a fixed size so that the buttons stay
        // positioned correctly relative to each other
        buttonPanel.setPreferredSize(buttonPanelCoord);
        buttonPanel.setMaximumSize(buttonPanelCoord);
        buttonPanel.setMinimumSize(buttonPanelCoord);
    }

    /**************************************************************************
     * Create the buttons and button listeners for the dialog based on the
     * option type provided
     * 
     * @param parent
     *            parent window of the dialog; needed to get the button icons
     * 
     * @param optionType
     *            dialog type: LOAD_OPTION, SAVE_OPTION, SEARCH_OPTION,
     *            READ_OPTION, PRINT_OPTION, CLOSE_OPTION, OK_OPTION, or
     *            OK_CANCEL_OPTION
     *************************************************************************/
    private void createButtonPanel(Component parent,
                                   int optionType)
    {
        // Create a button panel to hold the button(s)
        buttonPanel = new JPanel();

        // Determine the button text, number of buttons, and icon based on
        // the type. Assume initially there's an Okay and a Cancel button
        String okButtonText = "Okay";
        char okButtonMnemonic = 'O';
        String cancelButtonText = "Cancel";
        Icon buttonIcon = new ImageIcon(parent.getClass().getResource(OK_ICON));
        int numButtons = 2;

        // Based on the dialog option type, adjust the button text and the
        // number of buttons to display
        switch (optionType)
        {
            case LOAD_OPTION:
                okButtonText = "Load";
                okButtonMnemonic = 'L';
                break;

            case SAVE_OPTION:
                okButtonText = "Save";
                okButtonMnemonic = 'S';
                break;

            case SEARCH_OPTION:
                okButtonText = "Search";
                okButtonMnemonic = 'S';
                break;

            case READ_OPTION:
                okButtonText = "Read";
                okButtonMnemonic = 'R';
                break;

            case OK_OPTION:
                numButtons = 1;
                break;

            case PRINT_OPTION:
                okButtonText = "Print";
                okButtonMnemonic = 'P';
                cancelButtonText = "Close";
                buttonIcon = new ImageIcon(parent.getClass().getResource(PRINT_ICON));
                break;

            case CLOSE_OPTION:
                okButtonText = "Close";
                okButtonMnemonic = 'C';
                buttonIcon = new ImageIcon(parent.getClass().getResource(CLOSE_ICON));
                numButtons = 1;
                break;
        }

        // Create the Okay button
        JButton btnOkButton = new JButton(okButtonText, buttonIcon);
        btnOkButton.setFont(BUTTON_FONT);
        btnOkButton.setMnemonic(okButtonMnemonic);
        buttonPanel.add(btnOkButton);

        // Add a listener for the Okay button
        btnOkButton.addActionListener(new ActionListener()
        {
            /**************************************************************
             * Set the selected button to indicate Okay and exit the dialog
             *************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Verify that the dialog box selection(s) are valid. This
                // is always true unless the verification method is
                // overridden
                if (verifySelection())
                {
                    buttonSelected = OK_OPTION;
                    closeDialog();
                }
            }
        });

        // Check if the dialog has more than one button
        if (numButtons == 2)
        {
            // Create the Cancel button
            JButton btnCancelButton = new JButton(cancelButtonText,
                                                  new ImageIcon(parent.getClass().getResource(CANCEL_ICON)));
            btnCancelButton.setFont(BUTTON_FONT);
            btnCancelButton.setMnemonic('C');
            buttonPanel.add(btnCancelButton);

            // Add a listener for the Cancel button
            btnCancelButton.addActionListener(new ActionListener()
            {
                /**********************************************************
                 * Set the selected button to indicate Cancel and exit the
                 * dialog
                 *********************************************************/
                @Override
                public void actionPerformed(ActionEvent ae)
                {
                    buttonSelected = CANCEL_OPTION;
                    closeDialog();
                }
            });
        }
    }

    /**************************************************************************
     * Create the dialog. If no buttons are provided (lower panel) then create
     * the buttons and button listeners needed based on the dialog type
     * 
     * @param parent
     *            window to center the dialog over
     * 
     * @param upperObject
     *            object containing the dialog components or message
     * 
     * @param lowerPanel
     *            panel containing the dialog buttons
     * 
     * @param title
     *            title to display in the dialog window frame
     * 
     * @param optionType
     *            dialog type: LOAD_OPTION, SAVE_OPTION, SEARCH_OPTION,
     *            READ_OPTION, PRINT_OPTION, CLOSE_OPTION, OK_OPTION, or
     *            OK_CANCEL_OPTION
     * 
     * @param icon
     *            icon to display to the left of the text message
     * 
     * @param resizable
     *            true to allow the dialog to be resized
     * 
     * @param modal
     *            false to allow the main application window to still be
     *            operated while the dialog is open
     * 
     * @return Selected button type
     *************************************************************************/
    private int createDialog(Component parent,
                             Object upperObject,
                             JPanel lowerPanel,
                             String title,
                             int optionType,
                             Icon icon,
                             boolean resizable,
                             boolean modal)
    {
        // Set the button panel to the supplied lower panel. buttonPanel is
        // needed to be global in order to resize the buttons for the case
        // where the look & feel changes while the dialog is open
        buttonPanel = lowerPanel;

        // Check if no buttons were provided
        if (buttonPanel == null)
        {
            // Create the button panel based on the option type provided
            createButtonPanel(parent, optionType);
        }

        // Size and position the dialog's button(s)
        setButtonWidth();

        // Add a change listener to the keyboard focus manager so component
        // focus changes can be detected
        KeyboardFocusManager.getCurrentKeyboardFocusManager().addPropertyChangeListener(new PropertyChangeListener()
        {
            Component focusOwner;

            /******************************************************************
             * Highlight a button if it has the keyboard focus and remove the
             * highlight when it loses focus
             *****************************************************************/
            @Override
            public void propertyChange(PropertyChangeEvent pce)
            {
                // Turn off button highlight when it no longer has focus
                if (focusOwner != null && pce.getOldValue() == focusOwner)
                {
                    if (focusOwner.getClass().getSimpleName().equals("JButton"))
                    {
                        getRootPane().setDefaultButton(null);
                    }

                    focusOwner = null;
                }

                // Turn on button highlight when it has focus
                if (pce.getNewValue() != null
                    && pce.getNewValue() != focusOwner)
                {
                    focusOwner = (Component) pce.getNewValue();

                    if (focusOwner.getClass().getSimpleName().equals("JButton"))
                    {
                        getRootPane().setDefaultButton((JButton) focusOwner);
                    }
                }
            }
        });

        // Create a component to point to the upper contents of the dialog
        Component upperComponent;

        // Check if a text message was provided instead of an upper panel
        if (upperObject instanceof String)
        {
            // Create a panel to hold the text message
            JPanel textPanel = new JPanel(new BorderLayout());
            textPanel.setBorder(BorderFactory.createEmptyBorder());

            // Create a panel to hold the icon. Add some padding between the
            // icon and the text message
            JPanel iconPanel = new JPanel();
            iconPanel.setBorder(BorderFactory.createEmptyBorder(0,
                                                                0,
                                                                0,
                                                                DIALOG_INNER_PAD));
            iconPanel.add(new JLabel(icon));

            // Create a label to hold the text message
            JLabel text = new JLabel(upperObject.toString(),
                                     SwingConstants.LEFT);
            text.setFont(LABEL_FONT_PLAIN);

            // Place the icon and text message into the upper panel
            textPanel.add(iconPanel, BorderLayout.CENTER);
            textPanel.add(text, BorderLayout.LINE_END);

            // Point to the panel containing the icon and message
            upperComponent = textPanel;
        }
        // Not a text message dialog
        else
        {
            // Point to the dialog components provided
            upperComponent = (Component) upperObject;
        }

        // Create a panel to contain the button panel. This is necessary so
        // that the button panel remains centered as the dialog is resized
        JPanel bpPanel = new JPanel();
        bpPanel.add(buttonPanel);

        // Add padding around the dialog upper component
        ((JComponent) upperComponent).setBorder(BorderFactory.createEmptyBorder(DIALOG_INNER_PAD,
                                                                                DIALOG_INNER_PAD,
                                                                                DIALOG_INNER_PAD,
                                                                                DIALOG_INNER_PAD));

        // Add padding to the bottom of the button panel
        bpPanel.setBorder(BorderFactory.createEmptyBorder(0,
                                                          0,
                                                          DIALOG_INNER_PAD,
                                                          0));

        // Set the layout manager for the dialog and and the upper and lower
        // components
        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(upperComponent, BorderLayout.CENTER);
        getContentPane().add(bpPanel, BorderLayout.PAGE_END);

        // Check if the title is provided
        if (title != null)
        {
            // Set the dialog's title
            setTitle(title);
        }

        // Set if operation of the main window is allowed while this dialog is
        // open
        setModalityType(modal
                             ? JDialog.DEFAULT_MODALITY_TYPE
                             : JDialog.ModalityType.MODELESS);

        // Set the default close operation so that the dialog frame's close
        // button doesn't automatically exit the dialog. Instead, if this close
        // button is pressed a button press event is sent to the last button on
        // the dialog's button panel
        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

        // Get the last button on the button panel. This is the button that
        // closes/cancels the dialog
        exitButton = (JButton) buttonPanel.getComponent(buttonPanel.getComponentCount() - 1);

        // Add a listener for dialog window events
        addWindowListener(new WindowAdapter()
        {
            /******************************************************************
             * Handle the dialog frame close button press event
             *****************************************************************/
            @Override
            public void windowClosing(WindowEvent we)
            {
                // Send a button press event for the dialog's close/cancel
                // button. This allows the frame's close button to perform the
                // same actions as the dialog's close/cancel button
                exitButton.doClick();
            }
        });

        // Set whether or not this dialog can be resized
        setResizable(resizable);

        // Size the dialog to fit its components and position the dialog frame
        pack();
        setLocationRelativeTo(parent);

        // Check if the dialog is resizable and if no minimum size has been set
        // for it
        if (resizable && !minimumSizeSet)
        {
            // Set the dialog minimum size to its current size. This prevents
            // having a dialog that can be resized too small to show its
            // components
            setMinimumSize(new Dimension(getSize().width,
                                         getSize().height));
        }

        // Display the dialog
        setVisible(true);

        return buttonSelected;
    }
}
