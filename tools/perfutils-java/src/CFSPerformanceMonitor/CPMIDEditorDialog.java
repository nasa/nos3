/**
 * CFS Performance Monitor Performance ID Editor Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.BUTTON_FONT;
import static CFSPerformanceMonitor.CPMConstants.CANCEL_ICON;
import static CFSPerformanceMonitor.CPMConstants.CLEAR_ICON;
import static CFSPerformanceMonitor.CPMConstants.CLOSE_ICON;
import static CFSPerformanceMonitor.CPMConstants.COLOR_ICON;
import static CFSPerformanceMonitor.CPMConstants.DELETE_ICON;
import static CFSPerformanceMonitor.CPMConstants.DOWN_ICON;
import static CFSPerformanceMonitor.CPMConstants.HIDE_ICON;
import static CFSPerformanceMonitor.CPMConstants.ID_COLOR;
import static CFSPerformanceMonitor.CPMConstants.NEW_ICON;
import static CFSPerformanceMonitor.CPMConstants.OK_CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.OK_ICON;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.PRINT_ICON;
import static CFSPerformanceMonitor.CPMConstants.RESET_ICON;
import static CFSPerformanceMonitor.CPMConstants.SAVE_ICON;
import static CFSPerformanceMonitor.CPMConstants.SHOW_ICON;
import static CFSPerformanceMonitor.CPMConstants.TABLE_BACK_COLOR;
import static CFSPerformanceMonitor.CPMConstants.UP_ICON;
import static CFSPerformanceMonitor.CPMConstants.ClearOption.CLEAR_WITHOUT_ASKING;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

import javax.print.attribute.standard.OrientationRequested;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.border.EtchedBorder;
import javax.swing.table.DefaultTableModel;

import CFSPerformanceMonitor.CPMIDHandler.PerformanceID;
import CFSPerformanceMonitor.CPMTableHandler.IconColumn;

/******************************************************************************
 * CFS Performance Monitor ID editor dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMIDEditorDialog extends CPMDialogHandler
{
    private final CPMLogHandler perfLog;
    private final CPMIDHandler perfIDs;
    private final CPMMain perfMain;

    // Define globally so that it can be accessed by the action listeners, etc.
    private CPMTableHandler idTable;
    private JButton btnNew;
    private JButton btnDelete;
    private JButton btnMoveUp;
    private JButton btnMoveDown;
    private JButton btnShowAll;
    private JButton btnHideAll;
    private JButton btnPrint;
    private JButton btnColor;
    private JButton btnClear;
    private JButton btnReset;
    private JButton btnSave;
    private JButton btnClose;

    // Array to set up the contents of the performance ID table
    Object[][] tablePIDList;

    // Flag used to indicate if changes to the ID list have been saved
    private boolean changesSaved;

    // Table column indices:
    // - Row number; editing error messages reference the row number(s)
    // - Show (if checked) or hide (if not checked) this ID in the plots
    // - ID is in filter mask (true) or not in mask (false)
    // - ID is in trigger mask (true) or not in mask (false)
    // - Performance ID in hexadecimal format
    // - Performance ID text name; '*** Undefined ***' if it doesn't have one
    // - Performance ID plot color
    // - Performance ID schedule frequency, hz
    // - Number of logged entry events (0 if no log is loaded); use for sorting
    private final int ROW = 0;
    private final int SHOW = 1;
    private final int F_MASK = 2;
    private final int T_MASK = 3;
    private final int ID = 4;
    private final int NAME = 5;
    private final int COLOR = 6;
    private final int FREQ = 7;
    private final int EVENTS = 8;
    private final int NOTES = 9;
    private final int NUM_COLUMNS = 10;

    // Create the column name text
    private final String[] columnNames = new String[] {"Row",
                                                       "Show",
                                                       "<html><center>Filter<br>Mask",
                                                       "<html><center>Trig<br>Mask",
                                                       "ID",
                                                       "Name",
                                                       "Color",
                                                       "Freq",
                                                       "Events",
                                                       "Notes"};

    /**************************************************************************
     * Performance ID editor dialog class constructor
     * 
     * @param perfLog
     *            performance log
     * 
     * @param perfIDs
     *            performance IDs
     * 
     * @param perfMain
     *            main class
     *************************************************************************/
    protected CPMIDEditorDialog(CPMLogHandler perfLog,
                                CPMIDHandler perfIDs,
                                CPMMain perfMain)
    {
        this.perfLog = perfLog;
        this.perfIDs = perfIDs;
        this.perfMain = perfMain;

        changesSaved = false;

        // Create the ID editor dialog
        initialize();
    }

    /**************************************************************************
     * Create the performance ID editor dialog
     *************************************************************************/
    private void initialize()
    {
        // Define the performance ID editor table
        idTable = new CPMTableHandler()
        {
            /******************************************************************
             * Override isCellEditable so that only the Show, ID, Name, Color,
             * Frequency, and Notes columns can be edited
             *****************************************************************/
            @Override
            public boolean isCellEditable(int row, int column)
            {
                return column == SHOW
                       || column == ID
                       || column == NAME
                       || column == COLOR
                       || column == FREQ
                       || column == NOTES;
            }

            /******************************************************************
             * Handle changes to the editable column cells (ID, Name, etc.)
             * 
             * @param row
             *            table row number
             * 
             * @param column
             *            table column number
             * 
             * @param oldValue
             *            original cell contents
             * 
             * @param newValue
             *            new cell contents
             ****************************************************************/
            @Override
            protected void handleCellContentChange(int row,
                                                   int column,
                                                   Object oldValue,
                                                   Object newValue)
            {
                boolean errorFlag = false;

                // Convert the model row number to the view row number
                int viewRow = convertRowIndexToView(row);

                // Create a string version of newValue
                String newValueS = newValue.toString();

                // Check if the ID has been changed
                if (column == ID)
                {
                    Long perfID;

                    // Check if the ID can be converted from a string to an
                    // integer
                    if ((perfID = perfIDs.convertStringToIntID(newValueS)) == null)
                    {
                        // Inform the user that the ID is in the wrong format
                        new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                 "<html><b>ID \""
                                                                     + newValueS
                                                                     + "\" invalid (not a number)<br>[row "
                                                                     + (viewRow + 1)
                                                                     + "]",
                                                                 "Invalid ID",
                                                                 JOptionPane.ERROR_MESSAGE,
                                                                 OK_OPTION);
                        errorFlag = true;
                    }
                    // The ID is in a valid format
                    else
                    {
                        // Check if the ID exceeds the maximum value supported
                        // by the current log
                        if (perfLog.getMaximumIDs() != 0
                            && perfID >= perfLog.getMaximumIDs())
                        {
                            // Inform the user that the ID is larger than the
                            // maximum supported
                            new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                     "<html><b>ID \""
                                                                         + newValueS
                                                                         + "\" value exceeds<br>maximum supported ("
                                                                         + perfIDs.convertIDToHexString(perfLog.getMaximumIDs() - 1)
                                                                         + ")<br>[row "
                                                                         + (viewRow + 1)
                                                                         + "]",
                                                                     "Invalid ID",
                                                                     JOptionPane.WARNING_MESSAGE,
                                                                     OK_OPTION);
                        }

                        // Compare this ID to the others in the table in order
                        // to avoid creating a duplicate
                        for (int otherRow = 0; otherRow < getRowCount(); otherRow++)
                        {
                            // Only compare if this row isn't the one being
                            // edited
                            if (otherRow != viewRow)
                            {
                                if (perfIDs.convertIDToHexString(perfID).equals(String.valueOf(getValueAt(otherRow, ID))))
                                {
                                    // Inform the user that this ID matches one
                                    // in another row
                                    new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                             "<html><b>Duplicate ID \""
                                                                                 + newValueS
                                                                                 + "\" - matches row "
                                                                                 + (otherRow + 1)
                                                                                 + "<br>[row "
                                                                                 + (viewRow + 1)
                                                                                 + "]",
                                                                             "Duplicate ID",
                                                                             JOptionPane.ERROR_MESSAGE,
                                                                             OK_OPTION);
                                    errorFlag = true;
                                    break;
                                }
                            }
                        }
                    }

                    // Check if the ID passed all of the tests
                    if (!errorFlag)
                    {
                        // Format the new ID and store it back into the table
                        idTable.setValueAt(perfIDs.convertIDToHexString(perfID),
                                           viewRow,
                                           ID);

                        // Set the filter and trigger mask columns based on the
                        // log file masks
                        idTable.setValueAt(perfLog.isIDFilterMaskState(perfID),
                                           viewRow,
                                           F_MASK);
                        idTable.setValueAt(perfLog.isIDTriggerMaskState(perfID),
                                           viewRow,
                                           T_MASK);
                    }
                    // An error occurred
                    else
                    {
                        // Restore the ID to its original value
                        idTable.setValueAt(oldValue, viewRow, ID);
                    }
                }
                // Check if the ID name has been changed
                else if (column == NAME)
                {
                    // Check if the ID name isn't blank. It's okay to have
                    // multiple IDs with no name, so only perform the duplicate
                    // name test if the user has entered a name
                    if (!newValue.equals(""))
                    {
                        // Compare this ID name to the others in the table in
                        // order to avoid creating a duplicate
                        for (int otherRow = 0; otherRow < getRowCount(); otherRow++)
                        {
                            // Only compare if this row isn't the one being
                            // edited, and if the ID name isn't the default
                            // name
                            if (otherRow != viewRow
                                && newValue.equals(getValueAt(otherRow, NAME)))
                            {
                                // Inform the user that this ID name matches
                                // one in another row
                                new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                         "<html><b>Duplicate name \""
                                                                             + newValueS
                                                                             + "\" - matches row "
                                                                             + (otherRow + 1)
                                                                             + "<br>[row "
                                                                             + (viewRow + 1)
                                                                             + "]",
                                                                         "Duplicate Name",
                                                                         JOptionPane.ERROR_MESSAGE,
                                                                         OK_OPTION);
                                errorFlag = true;
                                break;
                            }
                        }
                    }

                    // Check if any errors were detected
                    if (!errorFlag)
                    {
                        // Store 'cleaned' ID text back into table for use when
                        // saving to the list
                        idTable.setValueAt(newValue, viewRow, NAME);
                    }
                    // An error occurred
                    else
                    {
                        // Restore the ID name to its original value
                        idTable.setValueAt(oldValue, viewRow, NAME);
                    }
                }
                // Check if the ID frequency has been changed
                else if (column == FREQ)
                {
                    // Check if the cell is blank
                    if (newValueS.isEmpty())
                    {
                        // Set the value to the default (0)
                        newValue = 0;
                        newValueS = "0";
                    }

                    // Check if the frequency contains other than a positive
                    // decimal number. Values can be in the format #.#, #., .#,
                    // -#, -#., -.#, or -#.#
                    if (!newValueS.matches("(\\d*\\.?\\d+)|(\\d+\\.?\\d*)"))
                    {
                        // Inform the user that the frequency is in the wrong
                        // format
                        new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                 "<html><b>Frequency invalid (blank, negative, or<br>not a number) [row "
                                                                     + (viewRow + 1)
                                                                     + "]",
                                                                 "Invalid Frequency",
                                                                 JOptionPane.ERROR_MESSAGE,
                                                                 OK_OPTION);
                        errorFlag = true;
                    }

                    // Check if any errors were detected
                    if (!errorFlag)
                    {
                        // Parse the frequency and format it, removing any
                        // excess leading or trailing zeroes
                        newValueS = formatFrequency(Double.valueOf(newValueS));

                        // Store 'cleaned' frequency back into table for use
                        // when saving to the list
                        setValueAt(newValueS, viewRow, FREQ);
                    }
                    // An error occurred
                    else
                    {
                        // Restore the ID frequency to its original value
                        setValueAt(oldValue, viewRow, FREQ);
                    }
                }
            }

            /******************************************************************
             * Load the IDs into the table and format the table cells
             *****************************************************************/
            @Override
            protected void loadAndFormatData()
            {
                // Copy the performance ID list contents to an object array for
                // display and manipulation in the dialog box table
                tablePIDList = new Object[perfIDs.getPerfIDList().size()][NUM_COLUMNS];

                int row = 0;

                // Step through the ID list
                for (PerformanceID pid : perfIDs.getPerfIDList())
                {
                    // Store the row number
                    tablePIDList[row][ROW] = row + 1;

                    // This flag indicates if the ID is to be plotted or not
                    // (status is shown in a user-selectable checkbox)
                    tablePIDList[row][SHOW] = pid.isPlotPerfID();

                    // Compare the ID to the log's filter and trigger masks; if
                    // the ID's mask bit is set then indicate it's in the mask
                    tablePIDList[row][F_MASK] = perfLog.isIDFilterMaskState(pid.getPerfID());
                    tablePIDList[row][T_MASK] = perfLog.isIDTriggerMaskState(pid.getPerfID());

                    // Convert the ID to an 8-character hexadecimal string with
                    // '0x' prepended
                    tablePIDList[row][ID] = perfIDs.convertIDToHexString(pid.getPerfID());

                    // ID name
                    tablePIDList[row][NAME] = pid.getPerfIDName();

                    // ID schedule frequency. Format the value, removing any
                    // excess trailing zeroes
                    tablePIDList[row][FREQ] = formatFrequency(pid.getPerfIDFreq());

                    // Number of logged entry and exit events. This is included
                    // so that the IDs can be sorted by this attribute
                    tablePIDList[row][EVENTS] = perfIDs.getPerfIDList().get(row).getEntryCount()
                                                + perfIDs.getPerfIDList().get(row).getExitCount();

                    // ID annotation
                    tablePIDList[row][NOTES] = pid.getPerfIDNotes();

                    // ID plot color
                    tablePIDList[row][COLOR] = pid.getPerfIDColor();

                    row++;
                }

                // Place the ID data into the table model along with the column
                // names, set up the editors and renderers for the table cells,
                // set up the table grid lines, and calculate the minimum width
                // required to display the table information
                int totalWidth = setUpdatableCharacteristics(tablePIDList,
                                                             columnNames,
                                                             true,
                                                             true,
                                                             true);

                // Set the minimum size based on the column widths
                setDialogMinimumSize(totalWidth);
            }
        };

        // Create the list containing the columns that display icons and the
        // icons to display, based on the value of the cell
        List<IconColumn> iconColumns = new ArrayList<IconColumn>();
        iconColumns.add(idTable.new IconColumn(COLOR, null, null));
        iconColumns.add(idTable.new IconColumn(F_MASK,
                                               new ImageIcon(getClass().getResource(OK_ICON)),
                                               new ImageIcon(getClass().getResource(CANCEL_ICON))));
        iconColumns.add(idTable.new IconColumn(T_MASK,
                                               new ImageIcon(getClass().getResource(OK_ICON)),
                                               new ImageIcon(getClass().getResource(CANCEL_ICON))));

        // Place the ID table into a scroll pane
        JScrollPane scrollPane = new JScrollPane(idTable);

        // Set common table parameters and characteristics
        idTable.setFixedCharacteristics(scrollPane,
                                        ListSelectionModel.SINGLE_INTERVAL_SELECTION,
                                        TABLE_BACK_COLOR,
                                        true,
                                        ID,
                                        NAME,
                                        NOTES,
                                        SHOW,
                                        COLOR,
                                        ROW,
                                        iconColumns,
                                        new String[] {"Table row number",
                                                      "Show or hide ID when plotting",
                                                      "ID in filter mask indicator",
                                                      "ID in trigger mask indicator",
                                                      "Performance ID (hexadecimal)",
                                                      "Performance ID name",
                                                      "Performance ID plot color",
                                                      "Performance ID schedule frequency",
                                                      "Total number of events for ID",
                                                      "Annotations"});

        // Add a key listener to handle Insert, Delete, and Ctrl-X key presses
        idTable.addKeyListener(new KeyAdapter()
        {
            /******************************************************************
             * Handle key press events
             *****************************************************************/
            @Override
            public void keyPressed(KeyEvent ke)
            {
                // Check if the Insert key was pressed
                if (ke.getKeyCode() == KeyEvent.VK_INSERT)
                {
                    // Insert a row into the table at the selected location
                    insertRow();
                }
                // Check if the Delete or Ctrl+X key was pressed
                else if (ke.getKeyCode() == KeyEvent.VK_DELETE
                         || (ke.getKeyCode() == KeyEvent.VK_X
                         && ke.isControlDown()))
                {
                    // Delete the selected row(s) from the table
                    deleteRow();
                }
                // Check if the Ctrl+up arrow key is pressed
                else if ((ke.getKeyCode() == KeyEvent.VK_UP
                         || ke.getKeyCode() == KeyEvent.VK_KP_UP)
                         && ke.isControlDown())
                {
                    // Move the selected row(s) up
                    moveRowUp();
                }
                // Check if the Ctrl+down arrow key is pressed
                else if ((ke.getKeyCode() == KeyEvent.VK_DOWN
                         || ke.getKeyCode() == KeyEvent.VK_KP_DOWN)
                         && ke.isControlDown())
                {
                    // Move the selected row(s) down
                    moveRowDown();
                }
            }
        });

        // Define the editor panel to contain the ID table
        JPanel editorPanel = new JPanel();
        editorPanel.setLayout(new BoxLayout(editorPanel, BoxLayout.X_AXIS));
        editorPanel.setBorder(BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
        editorPanel.add(scrollPane);

        // Create an outer panel to put the editor panel in. The border doesn't
        // appear without this
        JPanel outerPanel = new JPanel();
        outerPanel.setLayout(new BoxLayout(outerPanel, BoxLayout.X_AXIS));
        outerPanel.add(editorPanel);
        outerPanel.setBorder(BorderFactory.createEmptyBorder());

        // Create the lower (button) panel
        JPanel buttonPanel = new JPanel();

        // Define the buttons for the lower panel:
        // New button
        btnNew = new JButton("New",
                             new ImageIcon(getClass().getResource(NEW_ICON)));
        btnNew.setFont(BUTTON_FONT);
        btnNew.setMnemonic('N');

        // Add a listener for the New button
        btnNew.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Insert a new row into the ID table at the selected location
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                insertRow();
            }
        });

        // Delete button
        btnDelete = new JButton("Delete",
                                new ImageIcon(getClass().getResource(DELETE_ICON)));
        btnDelete.setFont(BUTTON_FONT);
        btnDelete.setMnemonic('D');

        // Add a listener for the Delete button
        btnDelete.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Delete the selected row(s) from the ID table
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                deleteRow();
            }
        });

        // Move Up button
        btnMoveUp = new JButton("Up",
                                new ImageIcon(getClass().getResource(UP_ICON)));
        btnMoveUp.setFont(BUTTON_FONT);
        btnMoveUp.setMnemonic('U');

        // Add a listener for the Move Up button
        btnMoveUp.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Move the selected row(s) up in the ID table
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                moveRowUp();
            }
        });

        // Move Down button
        btnMoveDown = new JButton("Down",
                                  new ImageIcon(getClass().getResource(DOWN_ICON)));
        btnMoveDown.setFont(BUTTON_FONT);
        btnMoveDown.setMnemonic('W');

        // Add a listener for the Move Down button
        btnMoveDown.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Move the selected row(s) down in the ID table
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                moveRowDown();
            }
        });

        // Show All button
        btnShowAll = new JButton("Show All",
                                 new ImageIcon(getClass().getResource(SHOW_ICON)));
        btnShowAll.setFont(BUTTON_FONT);
        btnShowAll.setMnemonic('O');

        // Add a listener for the Show All button
        btnShowAll.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Enable the Show Plot flag for all IDs in the ID table
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Step through each row in the table
                for (int index = 0; index < idTable.getRowCount(); index++)
                {
                    // Set the flag to plot an ID for every ID in the list
                    idTable.setValueAt(true, index, SHOW);
                }
            }
        });

        // Hide All button
        btnHideAll = new JButton("Hide All",
                                 new ImageIcon(getClass().getResource(HIDE_ICON)));
        btnHideAll.setFont(BUTTON_FONT);
        btnHideAll.setMnemonic('I');

        // Add a listener for the Hide All button
        btnHideAll.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Disable the Show Plot flag for all IDs in the ID table
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Step through each row in the table
                for (int index = 0; index < idTable.getRowCount(); index++)
                {
                    // Reset the flag to plot an ID for every ID in the list
                    idTable.setValueAt(false, index, SHOW);
                }
            }
        });

        // Auto-color button
        btnColor = new JButton("Color",
                               new ImageIcon(getClass().getResource(COLOR_ICON)));
        btnColor.setFont(BUTTON_FONT);
        btnColor.setMnemonic('L');

        // Add a listener for the Color button
        btnColor.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Auto-assign ID colors based on the current order in the editor
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Check if the colors will change if auto-assigned
                if (colorsWillChange())
                {
                    // Confirm auto-assignment of the ID colors
                    if (new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                 "<html><b>Auto-assign colors to IDs?",
                                                                 "Auto Color",
                                                                 JOptionPane.QUESTION_MESSAGE,
                                                                 OK_CANCEL_OPTION) == OK_OPTION)
                    {
                        int colorIndex = 0;

                        // Step through the ID table
                        for (int index = 0; index < idTable.getRowCount(); index++)
                        {
                            // Select the next color in the color array and
                            // increment the index
                            idTable.setValueAt(ID_COLOR[colorIndex],
                                               index,
                                               COLOR);
                            colorIndex++;

                            // Check if the index has reached the end of the
                            // array
                            if (colorIndex == ID_COLOR.length)
                            {
                                // Reset the index back to the start of the
                                // array
                                colorIndex = 0;
                            }
                        }
                    }
                }
            }
        });

        // Print button
        btnPrint = new JButton("Print",
                               new ImageIcon(getClass().getResource(PRINT_ICON)));
        btnPrint.setFont(BUTTON_FONT);
        btnPrint.setMnemonic('P');

        // Add a listener for the Print button
        btnPrint.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Output the ID table to the printer
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Print the ID editor table
                idTable.printTable("ID List",
                                   CPMIDEditorDialog.this,
                                   OrientationRequested.LANDSCAPE);
            }
        });

        // Clear button
        btnClear = new JButton("Clear",
                               new ImageIcon(getClass().getResource(CLEAR_ICON)));
        btnClear.setFont(BUTTON_FONT);
        btnClear.setMnemonic('A');

        // Add a listener for the Print button
        btnClear.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Clear the ID list
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                {
                    // Get the table model
                    DefaultTableModel model = (DefaultTableModel) idTable.getModel();

                    // Check if there are any rows to clear
                    if (model.getRowCount() != 0)
                    {
                        // Confirm clearing the ID table values
                        if (new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                     "<html><b>Clear the ID list?",
                                                                     "Clear Table",
                                                                     JOptionPane.QUESTION_MESSAGE,
                                                                     OK_CANCEL_OPTION) == OK_OPTION)
                        {
                            // Remove all of the rows from the table
                            removeRows(0, model.getRowCount() - 1);
                        }
                    }
                }
            }
        });

        // Reset button
        btnReset = new JButton("Reset",
                               new ImageIcon(getClass().getResource(RESET_ICON)));
        btnReset.setFont(BUTTON_FONT);
        btnReset.setMnemonic('R');

        // Add a listener for the Reset button
        btnReset.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Restore the table to the initial values
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Check if the table contents has changed
                if (tableHasChanged())
                {
                    // Confirm resetting the ID table values
                    if (new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                 "<html><b>Restore table to original values?",
                                                                 "Reset Table",
                                                                 JOptionPane.QUESTION_MESSAGE,
                                                                 OK_CANCEL_OPTION) == OK_OPTION)
                    {
                        // Restore the original ID data into the table model,
                        // reset the editors and renderers for the table cells,
                        // and reset the table grid lines
                        idTable.setUpdatableCharacteristics(tablePIDList,
                                                            columnNames,
                                                            true,
                                                            true,
                                                            true);
                    }
                }
            }
        });

        // Save button
        btnSave = new JButton("Save",
                              new ImageIcon(getClass().getResource(SAVE_ICON)));
        btnSave.setFont(BUTTON_FONT);
        btnSave.setMnemonic('S');

        // Add a listener for the Save button
        btnSave.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Save the ID table, if changes were made
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Only save the IDs if a cell's content has changed and that
                // every row with an ID name entered also has an ID
                if (tableHasChanged() && !checkForMissingID())
                {
                    // Clear the current ID list
                    perfIDs.clearPerformanceIDList(CLEAR_WITHOUT_ASKING);

                    // Step through the rows in the table
                    for (int row = 0; row < idTable.getRowCount(); row++)
                    {
                        // Skip rows in the table that have no ID and name
                        row = skipEmptyTableRows(row);

                        // Check that the end of the table hasn't been reached
                        if (row < idTable.getRowCount())
                        {
                            // Copy the table contents into the performance ID
                            // list
                            perfIDs.addPerformanceID(Integer.parseInt(idTable.getValueAt(row, ID).toString().substring(2), 16),
                                                     idTable.getValueAt(row, NAME).toString().trim(),
                                                     Boolean.parseBoolean(idTable.getValueAt(row, SHOW).toString()),
                                                     (Color) idTable.getValueAt(row, COLOR),
                                                     Double.parseDouble(idTable.getValueAt(row, FREQ).toString()),
                                                     idTable.getValueAt(row, NOTES).toString());
                        }
                    }

                    // Set the "changes saved" flag so that remapping the ID
                    // list will be performed on closing the dialog
                    changesSaved = true;
                }
            }
        });

        // Close button
        btnClose = new JButton("Close",
                               new ImageIcon(getClass().getResource(CLOSE_ICON)));
        btnClose.setFont(BUTTON_FONT);
        btnClose.setMnemonic('C');

        // Add a listener for the Close button
        btnClose.addActionListener(new ActionListener()
        {
            /******************************************************************
             * Close the ID editor
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Check if the table contents hasn't changed, or if it has
                // (due to unsaved changes) then confirm discarding the changes
                if (!tableHasChanged()
                    || new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                                "<html><b>Discard changes?",
                                                                "Discard Changes",
                                                                JOptionPane.QUESTION_MESSAGE,
                                                                OK_CANCEL_OPTION) == OK_OPTION)
                {
                    // Close the dialog
                    closeDialog();

                    // Check if changes to the ID list have been saved
                    if (changesSaved)
                    {
                        // Remap the performance IDs to the log. This can't be
                        // done during the save operation since, if the user
                        // changes an ID that's in the log, the ID from the log
                        // is reloaded, which changes the list and causes a
                        // miscompare on close
                        perfIDs.mapLogToIDList(false, true);

                        // Update the plots and/or the log content dialog
                        perfMain.updatePlotsAndDialogs();
                    }
                }
            }
        });

        // Add buttons in the order in which they'll appear (left to right, top
        // to bottom)
        buttonPanel.add(btnNew);
        buttonPanel.add(btnMoveUp);
        buttonPanel.add(btnShowAll);
        buttonPanel.add(btnColor);
        buttonPanel.add(btnClear);
        buttonPanel.add(btnSave);
        buttonPanel.add(btnDelete);
        buttonPanel.add(btnMoveDown);
        buttonPanel.add(btnHideAll);
        buttonPanel.add(btnPrint);
        buttonPanel.add(btnReset);
        buttonPanel.add(btnClose);

        // Distribute the buttons across two rows
        setButtonRows(2);

        // Display the ID editor dialog
        showOptionsDialog(perfMain.getMainFrame(),
                          outerPanel,
                          buttonPanel,
                          "Performance ID Editor",
                          true);
    }

    /**************************************************************************
     * Insert a row into the table at the selection point
     *************************************************************************/
    private void insertRow()
    {
        int firstRow;

        // Check if the table is empty
        if (idTable.getRowCount() == 0)
        {
            // Set the row index to 0 (the first row)
            firstRow = 0;
        }
        // The table isn't empty
        else
        {
            // Set the row index to the row selected by the user
            firstRow = idTable.getSelectedRow();
        }

        // Ensure at least one row is selected
        if (firstRow != -1)
        {
            // Update the table model to match the view
            updateTableModelToView();

            // Insert a blank line above the selected row
            ((DefaultTableModel) idTable.getModel()).insertRow(firstRow,
                                                               new Object[] {"",
                                                                             false,
                                                                             false,
                                                                             false,
                                                                             "",
                                                                             "",
                                                                             Color.WHITE,
                                                                             0,
                                                                             "",
                                                                             ""});

            // Set the table sort capability based on if any rows exist
            idTable.setTableSortable();

            // Select the new row
            idTable.setRowSelectionInterval(firstRow, firstRow);

            // Check if the table has more than one row
            if (idTable.getRowCount() > 1)
            {
                // Deselect the originally selected row
                idTable.removeRowSelectionInterval(firstRow + 1,
                                                   firstRow + 1);
            }
        }
    }

    /**************************************************************************
     * Delete the selected row(s) from the table
     *************************************************************************/
    private void deleteRow()
    {
        // Get the first selected row
        int firstRow = idTable.getSelectedRow();

        // Check if at least one row is selected
        if (firstRow != -1)
        {
            // Get the number of selected rows
            int numRows = idTable.getSelectedRowCount();

            // Update the table model to match the view
            updateTableModelToView();

            // Remove the selected rows
            removeRows(firstRow, firstRow + numRows - 1);
        }
    }

    /**************************************************************************
     * Move the selected row(s) up one row
     *************************************************************************/
    private void moveRowUp()
    {
        int firstRow = idTable.getSelectedRow();

        // Check if at least one row is selected and it doesn't include the
        // topmost row
        if (firstRow > 0)
        {
            int endRow = firstRow + idTable.getSelectedRowCount() - 1;

            // Update the table model to match the view
            updateTableModelToView();

            // Move the selected row(s) up one row
            ((DefaultTableModel) idTable.getModel()).moveRow(firstRow,
                                                             endRow,
                                                             firstRow - 1);

            // Update the selected row(s) since they have moved
            idTable.addRowSelectionInterval(firstRow - 1, endRow - 1);
        }
    }

    /**************************************************************************
     * Move the selected row(s) down one row
     *************************************************************************/
    private void moveRowDown()
    {
        int firstRow = idTable.getSelectedRow();
        int endRow = firstRow + idTable.getSelectedRowCount() - 1;

        // Check if at least one row is selected and it doesn't include the
        // topmost row
        if (firstRow != -1 && endRow < idTable.getRowCount() - 1)
        {
            // Update the table model to match the view
            updateTableModelToView();

            // Move the selected row(s) down one row
            ((DefaultTableModel) idTable.getModel()).moveRow(firstRow,
                                                             endRow,
                                                             firstRow + 1);

            // Update the selected row(s) since they have moved
            idTable.addRowSelectionInterval(firstRow + 1, endRow + 1);
        }
    }

    /**************************************************************************
     * Remove one or more rows from the table
     * 
     * @param startRow
     *            row index marking the start of the selected row(s) to remove
     * 
     * @param endRow
     *            row index marking the end of the selected row(s) to remove
     *************************************************************************/
    private void removeRows(int startRow, int endRow)
    {
        // Get the table model
        DefaultTableModel model = (DefaultTableModel) idTable.getModel();

        // Check if there are any rows to clear
        if (model.getRowCount() != 0)
        {
            // Counter for the number of rows skipped due to the IDs having
            // logged events
            int numRowsSkipped = 0;

            // Step through each row in the table, starting at the end. Going
            // in reverse simplifies maintaining the correct index if rows are
            // retained due to having events
            while (endRow >= startRow)
            {
                // Retrieve the contents of the Events, ID, and mask columns
                // for the current row
                String numEvents = model.getValueAt(endRow, EVENTS).toString();
                String perfID = model.getValueAt(endRow, ID).toString();
                boolean filter = (Boolean) model.getValueAt(endRow, F_MASK);
                boolean trigger = (Boolean) model.getValueAt(endRow, T_MASK);

                // Delete the row
                model.removeRow(endRow);

                // Check if the ID in this row has events logged
                if (!numEvents.equals("") && Integer.parseInt(numEvents) != 0)
                {
                    // Events exist for this ID. These would be automatically
                    // restored when the log is remapped to the IDs, so insert
                    // the ID back into the table with the user-adjustable
                    // parameters reset to the defaults
                    model.insertRow(endRow,
                                    new Object[] {"",
                                                  false,
                                                  filter,
                                                  trigger,
                                                  perfID,
                                                  "",
                                                  Color.WHITE,
                                                  0,
                                                  numEvents,
                                                  ""});

                    // Increment the the skip counter. This is used to
                    // determine if a warning message should be annunciated
                    numRowsSkipped++;
                }

                // Go to the next row to remove
                endRow--;
            }

            // Set the table sort capability based on if any rows remain
            idTable.setTableSortable();

            // Check if any rows remain in the table. If so, select the row at
            // the starting index. Also select any rows that were not deleted
            // (due to having logged events)
            if (idTable.getRowCount() != 0)
            {
                // Check if the selected row is now beyond the end of the table
                if (startRow >= idTable.getRowCount())
                {
                    // Set the selected row number to the last row
                    startRow = idTable.getRowCount() - 1;
                }

                // Determine the ending selected row based on the number of
                // skipped rows
                endRow = startRow
                         + (numRowsSkipped == 0
                                               ? 0
                                               : numRowsSkipped - 1);

                // Set the selected row(s)
                idTable.setRowSelectionInterval(startRow, endRow);
            }

            // Check if any rows were skipped
            if (numRowsSkipped != 0)
            {
                // Inform the user that one or more rows were not deleted
                new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                         "<html><b>One or more IDs were not deleted<br>due to having logged events",
                                                         "ID Not Deleted",
                                                         JOptionPane.INFORMATION_MESSAGE,
                                                         OK_OPTION);
            }
        }
    }

    /**************************************************************************
     * Change the table model based to match the arrangement of the data in the
     * table view
     *************************************************************************/
    private void updateTableModelToView()
    {
        // Create a new data array for the table model
        Object[][] tableData = new Object[idTable.getRowCount()][NUM_COLUMNS];

        // Step through each of the table's rows
        for (int row = 0; row < idTable.getRowCount(); row++)
        {
            // Step through each of the table's columns
            for (int column = 0; column < idTable.getColumnCount(); column++)
            {
                // Get the value in the table and save it in the new data array
                tableData[row][column] = idTable.getValueAt(row, column);
            }
        }

        // Replace the original ID data in the table model with the new
        // (sorted) model data, and reset the editors and renderers for the
        // table cells
        idTable.setUpdatableCharacteristics(tableData,
                                            columnNames,
                                            true,
                                            true,
                                            true);
    }

    /**************************************************************************
     * Determine if any changes have been made compared to the original ID list
     * 
     * @return true if any cell in the table has been changed
     *************************************************************************/
    private boolean tableHasChanged()
    {
        boolean hasChanges = false;
        int tableRow = 0;

        for (PerformanceID pid : perfIDs.getPerfIDList())
        {
            // Skip rows in the table that have no ID and no name
            tableRow = skipEmptyTableRows(tableRow);

            // Check if the show flag, ID, name, color, frequency, or notes has
            // been changed, or if there are more entries in the list than in
            // the table
            if (tableRow == idTable.getRowCount()
                || !idTable.getValueAt(tableRow, SHOW).toString().
                    equals(Boolean.toString(pid.isPlotPerfID()))
                || !idTable.getValueAt(tableRow, ID).
                    equals(perfIDs.convertIDToHexString(pid.getPerfID()))
                || !idTable.getValueAt(tableRow, NAME).toString().
                    equals(pid.getPerfIDName())
                || !idTable.getValueAt(tableRow, COLOR).
                    equals(pid.getPerfIDColor())
                || !idTable.getValueAt(tableRow, FREQ).toString().
                    equals(formatFrequency(pid.getPerfIDFreq()))
                || !idTable.getValueAt(tableRow, NOTES).
                    equals(String.valueOf(pid.getPerfIDNotes())))
            {
                // Something didn't match; set the change flag
                hasChanges = true;
                break;
            }

            tableRow++;
        }

        // Check if there are any additional non-empty rows at the end of the
        // table
        if (!hasChanges && tableRow < idTable.getRowCount())
        {
            // Skip rows in the table that have no ID and no name
            tableRow = skipEmptyTableRows(tableRow);

            // Check if the row counter has reached the end of the table
            if (tableRow != idTable.getRowCount())
            {
                // Since the end of the table was not reached, additional
                // non-empty rows exist; set the change flag
                hasChanges = true;
            }
        }

        return hasChanges;
    }

    /**************************************************************************
     * Check that a row with an ID name entered has an ID as well
     * 
     * @return true if a row has an ID name but no ID
     *************************************************************************/
    private boolean checkForMissingID()
    {
        boolean idIsMissing = false;

        // Step through each row in the table
        for (int row = 0; row < idTable.getRowCount(); row++)
        {
            // Skip rows in the table that have no ID and no name
            row = skipEmptyTableRows(row);

            // Check if the ID column is empty
            if (idTable.getValueAt(row, ID).toString().trim().isEmpty())
            {
                // Set the flag indicating a missing ID
                idIsMissing = true;

                // Inform the user that a row is missing its ID
                new CPMDialogHandler().showMessageDialog(CPMIDEditorDialog.this,
                                                         "<html><b>ID must be provided for<br>\""
                                                             + idTable.getValueAt(row, NAME).toString()
                                                             + "\" [row "
                                                             + (row + 1)
                                                             + "]",
                                                         "Missing ID",
                                                         JOptionPane.ERROR_MESSAGE,
                                                         OK_OPTION);
                break;
            }
        }

        return idIsMissing;
    }

    /**************************************************************************
     * Determine if any changes will result to the ID colors if the colors are
     * auto-assigned
     *************************************************************************/
    private boolean colorsWillChange()
    {
        // Assume the colors remain unchanged
        boolean colorChange = false;

        // Reset the color array index to the beginning of the array
        perfIDs.resetColorIndex();

        // Step through the rows in the table
        for (int row = 0; row < idTable.getRowCount(); row++)
        {
            // Check if the current color differs from the auto-assigned color
            if (!idTable.getValueAt(row, COLOR).equals(perfIDs.getNextColor()))
            {
                // Set the flag to indicate the color will change and exit the
                // loop
                colorChange = true;
                break;
            }
        }

        return colorChange;
    }

    /**************************************************************************
     * Adjust the table row index to skip rows with no ID and no name. The
     * default ID name is equivalent to having no name
     *************************************************************************/
    private int skipEmptyTableRows(int tableRow)
    {
        // Check if the ID and name fields are empty, and that the end of the
        // table hasn't been reached
        while (tableRow < idTable.getRowCount()
               && idTable.getValueAt(tableRow, ID).toString().isEmpty()
               && idTable.getValueAt(tableRow, NAME).toString().isEmpty())
        {
            // Empty row; increment the index to ignore this row
            tableRow++;
        }

        return tableRow;
    }

    /**************************************************************************
     * Format the floating point ID frequency value as a string, removing any
     * excess trailing zeroes
     * 
     * @param freq
     *            ID frequency
     *************************************************************************/
    private String formatFrequency(double freq)
    {
        return new DecimalFormat("#.###").format(Double.valueOf(freq));
    }
}
