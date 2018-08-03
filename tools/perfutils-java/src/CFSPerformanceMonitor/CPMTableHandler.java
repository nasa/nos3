/**
 * CFS Performance Monitor Custom Table Handler
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.ALTERNATE_COLOR;
import static CFSPerformanceMonitor.CPMConstants.CELL_FONT;
import static CFSPerformanceMonitor.CPMConstants.CELL_HORIZONTAL_PADDING;
import static CFSPerformanceMonitor.CPMConstants.CELL_VERTICAL_PADDING;
import static CFSPerformanceMonitor.CPMConstants.COLUMN_NOT_DEFINED;
import static CFSPerformanceMonitor.CPMConstants.FOCUS_COLOR;
import static CFSPerformanceMonitor.CPMConstants.GRID_COLOR;
import static CFSPerformanceMonitor.CPMConstants.HEADER_FONT;
import static CFSPerformanceMonitor.CPMConstants.HEADER_HORIZONTAL_PADDING;
import static CFSPerformanceMonitor.CPMConstants.HEADER_VERTICAL_PADDING;
import static CFSPerformanceMonitor.CPMConstants.INITIAL_VIEWABLE_TABLE_ROWS;
import static CFSPerformanceMonitor.CPMConstants.LAF_OPAQUE;
import static CFSPerformanceMonitor.CPMConstants.LAF_SCROLL_BAR_WIDTH;
import static CFSPerformanceMonitor.CPMConstants.NAME_NOT_DEFINED;
import static CFSPerformanceMonitor.CPMConstants.NOTES_WORD_WRAP;
import static CFSPerformanceMonitor.CPMConstants.OK_CANCEL_OPTION;
import static CFSPerformanceMonitor.CPMConstants.OK_OPTION;
import static CFSPerformanceMonitor.CPMConstants.SELECTED_BACK_COLOR;
import static CFSPerformanceMonitor.CPMConstants.SELECTED_TEXT_COLOR;
import static CFSPerformanceMonitor.CPMConstants.TABLE_TEXT_COLOR;

import java.awt.AWTKeyStroke;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.KeyboardFocusManager;
import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.awt.print.PrinterException;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.text.MessageFormat;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import javax.print.attribute.HashPrintRequestAttributeSet;
import javax.print.attribute.PrintRequestAttributeSet;
import javax.print.attribute.standard.OrientationRequested;
import javax.swing.AbstractCellEditor;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.DefaultCellEditor;
import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.JViewport;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.table.TableRowSorter;

/******************************************************************************
 * CFS Performance Monitor table handler class
 *****************************************************************************/
@SuppressWarnings("serial")
public abstract class CPMTableHandler extends JTable
{
    // Global variables
    private JScrollPane scrollPane;
    private int rowNumberColumn;
    private int idColumn;
    private int nameColumn;
    private int notesColumn;
    private int showColumn;
    private int colorColumn;
    private String[] tooltips;
    private Color background;
    private boolean ignoreFocus;
    private final Border cellBorder;
    private Integer[] minimumWidth;

    // List containing the icon display columns
    private List<IconColumn> iconColumns;

    // Minimum number of columns at which to wrap a multi-line column
    private int minimumWrapColumn;

    // Number of row to display initially
    private int initialViewableRows;

    /**************************************************************************
     * Table handler constructor
     *************************************************************************/
    protected CPMTableHandler()
    {
        // Create the border used to pad the table cells
        cellBorder = BorderFactory.createEmptyBorder(CELL_VERTICAL_PADDING,
                                                     CELL_HORIZONTAL_PADDING,
                                                     CELL_VERTICAL_PADDING,
                                                     CELL_HORIZONTAL_PADDING);

        // Set the number of columns at which to wrap the a multi-line column
        // to the default value
        minimumWrapColumn = NOTES_WORD_WRAP;

        // Set the number of row to initially display to the default value
        initialViewableRows = INITIAL_VIEWABLE_TABLE_ROWS;
    }

    /**************************************************************************
     * Icon display column class
     *************************************************************************/
    protected class IconColumn
    {
        // Column number
        private final int column;

        // Icon to display if the cell value is true
        private final ImageIcon onIcon;

        // Icon to display if the cell value is false
        private final ImageIcon offIcon;

        /**********************************************************************
         * Icon display class constructor
         *********************************************************************/
        public IconColumn(int column, ImageIcon onIcon, ImageIcon offIcon)
        {
            this.column = column;
            this.onIcon = onIcon;
            this.offIcon = offIcon;
        }

        /**********************************************************************
         * Return the icon to display in this cell, based on the column type
         * and cell value
         * 
         * @param value
         *            cell value
         * 
         * @return Icon to display in the cell
         *********************************************************************/
        private ImageIcon getIcon(Object value)
        {
            ImageIcon icon = null;

            // Check if this cell contains a boolean value
            if (value instanceof Boolean)
            {
                // Check if the value is true
                if ((Boolean) value)
                {
                    // Return the 'on' icon
                    icon = onIcon;
                }
                // The value is false
                else
                {
                    // Return the 'off' icon
                    icon = offIcon;
                }
            }
            // Check if this cell contains a color value
            else if (value instanceof Color)
            {
                // Set the icon width and height based on the cell font size
                int size = (int) getFontMetrics(CELL_FONT).getStringBounds("0",
                                                                           null).getHeight() - 3;

                // Create an icon to display the color
                BufferedImage img = new BufferedImage(size,
                                                      size,
                                                      BufferedImage.TYPE_INT_RGB);
                Graphics g = img.getGraphics();
                g.setColor(Color.LIGHT_GRAY);
                g.drawRect(0, 0, size, size);

                // Set the color based on the cell value
                g.setColor((Color) value);
                g.fillRect(1, 1, size - 2, size - 2);

                // Create the color icon
                icon = new ImageIcon(img);
            }

            return icon;
        }
    }

    /**************************************************************************
     * Set the minimum number of columns at which to wrap the text in a
     * multi-line column
     *************************************************************************/
    protected void setMinimumWrapColumn(int numColumns)
    {
        minimumWrapColumn = numColumns;
    }

    /**************************************************************************
     * Set the initial number of rows to display when the table first appears
     *************************************************************************/
    protected void setInitialViewableRows(int numRows)
    {
        initialViewableRows = numRows;
    }

    /**************************************************************************
     * Return the array of minimum column widths
     * 
     * @return Minimum width, in pixels, required to display the longest value
     *         for a column
     *************************************************************************/
    protected Integer[] getMinWidth()
    {
        return minimumWidth;
    }

    /**************************************************************************
     * Return the viewport of the scroll pane in which the table resides
     * 
     * @return The table's scroll pane viewport (null if the table isn't in a
     *         scroll pane)
     *************************************************************************/
    protected JViewport getViewport()
    {
        return scrollPane.getViewport();
    }

    /**************************************************************************
     * Placeholder for method to load and format the table data
     *************************************************************************/
    protected abstract void loadAndFormatData();

    /**************************************************************************
     * Return false so that none of the table cells can be changed
     *************************************************************************/
    @Override
    public boolean isCellEditable(int row, int column)
    {
        return false;
    }

    /**************************************************************************
     * Return the default table header object
     *************************************************************************/
    @Override
    protected JTableHeader createDefaultTableHeader()
    {
        return new JTableHeader(columnModel)
        {
            /******************************************************************
             * Provide the tooltip text for the column headers
             *****************************************************************/
            @Override
            public String getToolTipText(MouseEvent me)
            {
                // Get the tool tip for the column header underneath the mouse
                // cursor
                return tooltips[columnModel.getColumn(columnModel.getColumnIndexAtX(me.getPoint().x)).getModelIndex()];
            }
        };
    }

    /**************************************************************************
     * Set common table parameters and characteristics (for tables without
     * special columns)
     * 
     * @param intervalSelection
     *            selection mode used by the row and column selection models
     * 
     * @param background
     *            table background color (when row not selected)
     * 
     * @param ignoreFocus
     *            true to ignore if the table has focus when determining the
     *            row colors
     * 
     * @param multilineColumn
     *            column number to display multi-line text
     * 
     * @param padAllColumns
     *            true if all columns should have a space prepended to the cell
     *            contents (doesn't affect index and color cells)
     * 
     * @param tooltips
     *            tool tip text to display when hovering over the column
     *            headers
     *************************************************************************/
    protected void setFixedCharacteristics(int intervalSelection,
                                           Color background,
                                           boolean ignoreFocus,
                                           int multilineColumn,
                                           String[] tooltips)
    {
        setFixedCharacteristics(null,
                                intervalSelection,
                                background,
                                ignoreFocus,
                                COLUMN_NOT_DEFINED,
                                COLUMN_NOT_DEFINED,
                                multilineColumn,
                                COLUMN_NOT_DEFINED,
                                COLUMN_NOT_DEFINED,
                                COLUMN_NOT_DEFINED,
                                null,
                                tooltips);
    }

    /**************************************************************************
     * Set common table parameters and characteristics. These only need to be
     * set when the table is initially created and do not require to be updated
     * if the table is changed while it is visible
     * 
     * @param scrollPane
     *            the scroll pane in which the table resides. Set to null if
     *            the table isn't in a scroll pane
     * 
     * @param intervalSelection
     *            selection mode used by the row and column selection models
     * 
     * @param background
     *            table background color (when row not selected)
     * 
     * @param ignoreFocus
     *            true to ignore if the table has focus when determining the
     *            row colors
     * 
     * @param idColumn
     *            ID column number. Set to COLUMN_NOT_DEFINED if none of the
     *            table's columns contain IDs
     * 
     * @param nameColumn
     *            ID name column number. Set to COLUMN_NOT_DEFINED if none of
     *            the table's columns contain ID names
     * 
     * @param notesColumn
     *            column number to display multi-line text, such as
     *            user-supplied annotations
     * 
     * @param showColumn
     *            show ID in plots column number. Set to COLUMN_NOT_DEFINED if
     *            none of the table's columns contain Show IDs check boxes
     * 
     * @param colorColumn
     *            ID color column number. Set to COLUMN_NOT_DEFINED if none of
     *            the table's columns contain color icons
     * 
     * @param rowNumberColumn
     *            ID index column number. Set to COLUMN_NOT_DEFINED if none of
     *            the table's columns contain index values
     * 
     * @param padAllColumns
     *            true if all columns should have a space prepended to the cell
     *            contents (doesn't affect index and color cells)
     * 
     * @param iconColumns
     *            list of IconColumn objects containing information for the
     *            columns that should be displayed as icons instead of as text
     * 
     * @param tooltips
     *            tool tip text to display when hovering over the column
     *            headers
     *************************************************************************/
    protected void setFixedCharacteristics(JScrollPane scrollPane,
                                           int intervalSelection,
                                           Color background,
                                           boolean ignoreFocus,
                                           int idColumn,
                                           int nameColumn,
                                           int notesColumn,
                                           int showColumn,
                                           int colorColumn,
                                           int rowNumberColumn,
                                           List<IconColumn> iconColumns,
                                           String[] tooltips)
    {
        // Set the table's scroll pane
        this.scrollPane = scrollPane;

        // Set the table's non-selected background color
        this.background = background;

        // Set to true to keep rows highlighted when the table loses focus
        this.ignoreFocus = ignoreFocus;

        // Set to the column number that contains IDs
        this.idColumn = idColumn;

        // Set to the column number that contains ID names
        this.nameColumn = nameColumn;

        // Set to the column number that contains annotations
        this.notesColumn = notesColumn;

        // Set to the column number that contains Show IDs check boxes
        this.showColumn = showColumn;

        // Set to the column number that contains color icons
        this.colorColumn = colorColumn;

        // Set to the column number that contains non-sortable row number
        // values
        this.rowNumberColumn = rowNumberColumn;

        // Set to the icon display column list
        this.iconColumns = iconColumns;

        // Store the tool tip text
        this.tooltips = tooltips;

        // Enable to allow sorting table rows
        setAutoCreateRowSorter(true);

        // Enable to allow a row, or contiguous rows, to be selected
        setRowSelectionAllowed(true);
        setSelectionMode(intervalSelection);

        // Remove the table border
        setBorder(BorderFactory.createEmptyBorder());

        // Set the focus to a cell if the keyboard is used to select it. Needed
        // under some circumstances to make text cursor appear in table cell
        // when editing
        setSurrendersFocusOnKeystroke(true);

        // Make the table fill the dialog
        setFillsViewportHeight(true);

        // Disable the capability to reorder the table's columns
        getTableHeader().setReorderingAllowed(false);

        // Add padding to the row height to improve readability
        setRowHeight(getRowHeight() + CELL_VERTICAL_PADDING * 2);

        // Set the table header font and calculate the header height. The
        // column widths are calculated elsewhere
        getTableHeader().setFont(HEADER_FONT);
        getTableHeader().setPreferredSize(new Dimension(1,
                                                        ((int) getFontMetrics(HEADER_FONT).getStringBounds("X",
                                                                                                           null).getHeight()
                                                            * 2)
                                                            + HEADER_VERTICAL_PADDING));

        // Set the font for the table cells
        setFont(CELL_FONT);

        // Listen for changes made by the user to the table's cells
        new TableCellListener();

        // Change TAB/SHIFT-TAB behavior so that focus jumps between the tables
        // and the buttons
        setTableKeyboardTraversal();

        // Create the table model. The data and column headers are added later
        // in case these need to be adjusted (e.g., loading a new log)
        setModel(new DefaultTableModel()
        {
            /******************************************************************
             * Override getColumnClass to allow sorting of the Statistics
             * "Entry" and "Exit" columns in numerical (versus string) order
             * and to allow check boxes to be displayed in the ID Editor "Show"
             * column
             *****************************************************************/
            @Override
            public Class<?> getColumnClass(int column)
            {
                Class<?> returnClass;

                // Check if the table is empty (i.e., has no rows)
                if (getRowCount() == 0)
                {
                    // Return a dummy class to prevent an error
                    returnClass = null;
                }
                // The table has at least one row
                else
                {
                    // Return the class of the object in the target column
                    returnClass = getValueAt(0, column).getClass();
                }

                return returnClass;
            }
        });

        // Exit the cell's editor, if active, when the cell loses focus
        putClientProperty("terminateEditOnFocusLost", Boolean.TRUE);

        // Add a listener for table focus changes
        addFocusListener(new FocusAdapter()
        {
            /******************************************************************
             * Handle loss of keyboard focus for the table
             *****************************************************************/
            @Override
            public void focusLost(FocusEvent fe)
            {
                // Force a repaint so that any highlighted rows are
                // unhighlighted when the table loses focus
                repaint();
            }

            /******************************************************************
             * Handle gain of keyboard focus for the table
             *****************************************************************/
            @Override
            public void focusGained(FocusEvent fe)
            {
                // Force a repaint so that all cells are highlighted in a row
                // when the table row gains focus
                repaint();
            }
        });

        // Load the table data and format the table cells
        loadAndFormatData();

        // Set the initial number of rows to display in the table
        setPreferredScrollableViewportSize(new Dimension(getPreferredSize().width,
                                                         initialViewableRows
                                                             * getRowHeight()));

        // Set the keys that initiate table cell editing and the keys to be
        // ignored when initiating editing
        setEditorKeys();
    }

    /**************************************************************************
     * Set common table characteristics. This must be called again if the table
     * changes while it is visible; e.g., the look & feel is changed or new
     * table data is loaded
     * 
     * @param tableData
     *            two-dimensional array of data to be placed into the table
     * 
     * @param columnNames
     *            array of HTML-formatted names for the table columns
     * 
     * @param centerText
     *            true to center the text within the cells
     * 
     * @param showScrollBar
     *            true if the table width must account for a vertical scroll
     *            bar
     * 
     * @param calcTotalWidth
     *            true to calculate the total width of the table
     * 
     * @return Combined width of all columns in pixels
     *************************************************************************/
    protected int setUpdatableCharacteristics(Object[][] tableData,
                                              String[] columnNames,
                                              Boolean centerText,
                                              Boolean showScrollBar,
                                              Boolean calcTotalWidth)
    {
        int totalWidth = 0;

        // Initialize the row sorter to null in order to prevent an error when
        // the table is empty
        setRowSorter(null);

        // Place the data into the table model along with the column names
        ((DefaultTableModel) getModel()).setDataVector(tableData, columnNames);

        // Enable/disable table sort capability based on if any rows exist
        setTableSortable();

        // Set up the renderers for the table cells
        setCellRenderers(centerText);

        // Set up the table grid lines
        setTableGrid();

        // Set the editor characteristics for the editable cells
        setCellEditors();

        // Check if the width should be calculated
        if (calcTotalWidth)
        {
            // Calculate and set the widths of the tab columns and get
            // the minimum width needed to display the tab's table
            totalWidth = calcColumnWidths(showScrollBar);
        }

        return totalWidth;
    }

    /**************************************************************************
     * Enable (if rows exist) or disable (if no rows exist) the capability to
     * sort the table
     *************************************************************************/
    protected void setTableSortable()
    {
        // Check if any rows exist in the table
        if (getRowCount() != 0)
        {
            // Set up sorter to allow sorting the table rows by clicking the
            // column header
            TableRowSorter<DefaultTableModel> sorter = new TableRowSorter<DefaultTableModel>((DefaultTableModel) getModel());
            setRowSorter(sorter);

            // Check if the row number column is defined
            if (rowNumberColumn != COLUMN_NOT_DEFINED)
            {
                // Set the row number column so that it can't be sorted
                sorter.setSortable(rowNumberColumn, false);
            }
        }
        // The table is empty; disable sorting to prevent an error that would
        // occur if sorting was attempted
        else
        {
            setRowSorter(null);
        }
    }

    /**************************************************************************
     * Change TAB/SHIFT-TAB behavior so that focus jumps between the table and
     * any other components (i.e., the buttons in the dialog containing the
     * table)
     *************************************************************************/
    private void setTableKeyboardTraversal()
    {
        Set<AWTKeyStroke> forward = new HashSet<AWTKeyStroke>(getFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS));
        forward.add(KeyStroke.getKeyStroke("TAB"));
        setFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS, forward);
        Set<AWTKeyStroke> backward = new HashSet<AWTKeyStroke>(getFocusTraversalKeys(KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS));
        backward.add(KeyStroke.getKeyStroke("shift TAB"));
        setFocusTraversalKeys(KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS, backward);
    }

    /**************************************************************************
     * Set the table header and cell renderers
     * 
     * @param centerText
     *            true to center the text within the cells
     *************************************************************************/
    private void setCellRenderers(boolean centerText)
    {
        // Create a cell renderer to position the text in the specified columns
        // and remove the focus border
        DefaultTableCellRenderer cellRenderer = setTableCellRenderer();
        cellRenderer.setHorizontalAlignment(centerText
                                                      ? JLabel.CENTER
                                                      : JLabel.LEFT);

        // Center the table column header text
        ((DefaultTableCellRenderer) getTableHeader().getDefaultRenderer()).setHorizontalAlignment(JLabel.CENTER);

        // Set the logs table's column size; all but the Name and Notes columns
        // have fixed widths
        for (int column = 0; column < getColumnModel().getColumnCount(); column++)
        {
            // Get the table column to shorten the calls below
            TableColumn tc = getColumnModel().getColumn(column);

            // Set the header renderer
            tc.setHeaderRenderer(getTableHeader().getDefaultRenderer());

            // Check if this is the Name column
            if (column == nameColumn)
            {
                // Use a cell renderer to remove the focus border and make the
                // column resizable
                tc.setCellRenderer(setTableCellRenderer());
                tc.setResizable(true);
            }
            // Check if this is the Notes column
            else if (column == notesColumn)
            {
                // Use a cell renderer to remove the focus border and make the
                // column resizable
                tc.setCellRenderer(new MultilineCellRenderer());
                tc.setResizable(true);
            }
            // Not the Name or Notes columns
            else
            {
                // Set the maximum size of the column and prevent resizing it
                tc.setResizable(false);

                // Only apply the cell renderer to columns that are not
                // displayed as check boxes
                if (column != showColumn)
                {
                    // Use the cell renderer to center the text in the column
                    // and remove the focus border
                    tc.setCellRenderer(cellRenderer);
                }
            }
        }
    }

    /**************************************************************************
     * Calculate and then set the minimum width for each column and return the
     * combined width of all of the columns
     * 
     * @param showScrollBar
     *            true if the table width must account for a vertical scroll
     *            bar
     * 
     * @return Combined width of all columns in pixels
     *************************************************************************/
    private int calcColumnWidths(boolean showScrollBar)
    {
        minimumWidth = new Integer[getColumnModel().getColumnCount()];

        // Set the logs table's column size; all but the Name and Notes columns
        // have fixed widths
        for (int column = 0; column < getColumnModel().getColumnCount(); column++)
        {
            // Get the table column to shorten the calls below
            TableColumn tc = getColumnModel().getColumn(column);

            // Get the table header renderer
            TableCellRenderer renderer = tc.getHeaderRenderer();

            // Get the component used to draw the table header cell
            Component comp = renderer.getTableCellRendererComponent(this,
                                                                    tc.getHeaderValue(),
                                                                    false,
                                                                    false,
                                                                    -1,
                                                                    column);

            // Save the width required to hold the header text
            minimumWidth[column] = comp.getPreferredSize().width
                                   + (HEADER_HORIZONTAL_PADDING * 2);

            // Check that this column isn't displaying Color values. The colors
            // column uses buttons to show the color. The renderer, however,
            // looks at the text representation of the cell object which would
            // cause the width to be sized too large. The Color column's header
            // text width is sufficient to size the column
            if (column != colorColumn)
            {
                // Step through each row in the table
                for (int row = 0; row < getRowCount(); row++)
                {
                    // Get the component used to draw the table cell from the
                    // cell's renderer. The Notes column uses a multi-line
                    // renderer which doesn't return the correct information;
                    // therefore a default renderer is used for this case
                    comp = prepareRenderer((column == notesColumn)
                                                                  ? getDefaultRenderer(String.class)
                                                                  : getCellRenderer(row, column),
                                           row,
                                           column);

                    // Compare the width of the cell to the largest width found
                    // so far and store it if it's larger. The width returned
                    // by getPreferredSize() includes the cell padding, so it
                    // doesn't have to be added here
                    minimumWidth[column] = Math.max(comp.getPreferredSize().width + 2,
                                                    minimumWidth[column]);
                }
            }

            // Check if this is the ID or ID name column. If no row exists for
            // this table then the column width is determined by a default
            // value
            if (column == idColumn || column == nameColumn)
            {
                // Get the component used to draw the table cell from the
                // cell's renderer and use it to determine the width of the
                // cell in pixels. Store this value if it's wider than the
                // largest width found so far
                comp = getCellRenderer(0, column).getTableCellRendererComponent(this,
                                                                                (column == idColumn
                                                                                                   ? "0x00000000"
                                                                                                   : NAME_NOT_DEFINED),
                                                                                false,
                                                                                false,
                                                                                0,
                                                                                column);
                minimumWidth[column] = Math.max(comp.getPreferredSize().width + 2,
                                                minimumWidth[column]);
            }
            // Check if this is the Notes column
            else if (column == notesColumn)
            {
                // Calculate the starting width of the Notes column. If the
                // length of the note exceeds a specified value then wrap the
                // text at the value
                int notesWrap = minimumWrapColumn
                                * (int) getFontMetrics(CELL_FONT).getStringBounds("X",
                                                                                  null).getWidth();
                minimumWidth[column] = Math.min(notesWrap, minimumWidth[column]);
            }
        }

        // Set the table column widths
        setColumnWidths(0, minimumWidth);

        // Return the total width of the table. Include space for the table's
        // vertical scroll bar, if present
        return getPreferredSize().width
               + (showScrollBar
                               ? LAF_SCROLL_BAR_WIDTH
                               : 0);
    }

    /**************************************************************************
     * Set the table columns widths
     * 
     * @param minWidth
     *            array containing the column widths in pixels
     * 
     * @param startColumn
     *            index of the initial column to adjust
     *************************************************************************/
    protected void setColumnWidths(int startColumn, Integer[] minWidth)
    {
        // Step through each column in the table
        for (int column = 0; column < getColumnModel().getColumnCount(); column++)
        {
            // Calculate the index into the minimum width array
            int offsetColumn = column + startColumn;

            // Get the table column to shorten the calls below
            TableColumn tc = getColumnModel().getColumn(column);

            // Check that this isn't the Name or Notes column
            if (offsetColumn != nameColumn && offsetColumn != notesColumn)
            {
                // Set the maximum size of the column to prevent it from being
                // resized
                tc.setMaxWidth(minWidth[offsetColumn]);
            }

            // Set the minimum and preferred width of the table column
            tc.setPreferredWidth(minWidth[offsetColumn]);
            tc.setMinWidth(minWidth[offsetColumn]);
        }
    }

    /**************************************************************************
     * Set the keys that initiate editing the cell with the focus and which
     * keys do not
     *************************************************************************/
    private void setEditorKeys()
    {
        // Add a key press listener to filter out invalid editor initiation
        // keys
        addKeyListener(new KeyAdapter()
        {
            /******************************************************************
             * Handle key press events
             *****************************************************************/
            @Override
            public void keyPressed(KeyEvent ke)
            {
                // Check that the Alt key isn't pressed. If the Alt key is
                // pressed this allows the keyboard mnemonic keys to activate
                // their respective controls
                if (!ke.isAltDown())
                {
                    switch (ke.getKeyCode())
                    {
                        case KeyEvent.VK_SPACE:
                        case KeyEvent.VK_HOME:
                        case KeyEvent.VK_END:
                        case KeyEvent.VK_PAGE_UP:
                        case KeyEvent.VK_PAGE_DOWN:
                        case KeyEvent.VK_LEFT:
                        case KeyEvent.VK_RIGHT:
                        case KeyEvent.VK_KP_LEFT:
                        case KeyEvent.VK_KP_RIGHT:
                            // Allow the space key to initiate editing as
                            // usual. Also keep the traversal keys active;
                            // these won't initiate editing in the cell
                            break;

                        case KeyEvent.VK_UP:
                        case KeyEvent.VK_DOWN:
                        case KeyEvent.VK_KP_UP:
                        case KeyEvent.VK_KP_DOWN:
                            // Check if the Ctrl key is pressed
                            if (ke.isControlDown())
                            {
                                // Ignore Ctrl+up/Down arrow key sequences. If
                                // the Ctrl key is not pressed the Up and Down
                                // arrow keys are handled as traversal keys
                                ke.consume();
                            }

                            break;

                        case KeyEvent.VK_A:
                        case KeyEvent.VK_C:
                            // Check if the Ctrl key is pressed
                            if (ke.isControlDown())
                            {
                                // Allow Ctrl+A and Ctrl+C key sequences to
                                // remain active for selecting all table text
                                // and copying table text to the clipboard,
                                // respectively
                                break;
                            }

                        default:
                            // Ignore all other keys (alpha-numeric, function,
                            // punctuation, etc.)
                            ke.consume();
                            break;
                    }
                }
            }
        });
    }

    /**************************************************************************
     * Set the characteristics of a cell's text area for displaying
     * word-wrapped text
     *************************************************************************/
    private void setMultilineEditorCharacteristics(JTextArea textArea)
    {
        // Set the text to automatically wrap at whitespace boundaries
        textArea.setLineWrap(true);
        textArea.setWrapStyleWord(true);

        // Set the font
        textArea.setFont(CELL_FONT);

        // Add some space around the text's perimeter to separate it from the
        // text area's border
        textArea.setBorder(cellBorder);
    }

    /**************************************************************************
     * Set the editors for the editable cells based on the column type
     *************************************************************************/
    private void setCellEditors()
    {
        // Create a text field so that its properties can be set and then used
        // to create a default editor for cells containing text
        JTextField textField = new JTextField();

        // Set the font used while editing the cell's text
        textField.setFont(CELL_FONT);

        // Set a border to pad the cell contents while editing
        textField.setBorder(cellBorder);

        // Create the cell editor
        DefaultCellEditor dce = new DefaultCellEditor(textField);

        // Step through each column in the table
        for (int column = 0; column < getColumnCount(); column++)
        {
            // Check if the column's contents is editable
            if (isCellEditable(0, column))
            {
                // Check if this is the Notes column
                if (column == notesColumn)
                {
                    // Set the editor so that a text entry pop-up dialog
                    // appears when a Notes column cell is selected
                    getColumnModel().getColumn(column).setCellEditor(new TablePopupEditor());
                }
                // Check if this column displays Color values
                else if (column == colorColumn)
                {
                    // Set the editor so that a color choice dialog appears
                    // when a Color column cell is selected
                    getColumnModel().getColumn(column).setCellEditor(new ColorEditor());
                }
                // Check that this column doesn't contain the Show ID check
                // boxes (e.g., Name, ID, or Freq columns)
                else if (column != showColumn)
                {
                    // Set the editor so that the contents can be modified
                    // within the table cell
                    getColumnModel().getColumn(column).setCellEditor(dce);
                }
            }
        }
    }

    /**************************************************************************
     * Class for displaying a text input dialog when a cell in the Notes column
     * is selected
     *************************************************************************/
    private class TablePopupEditor extends DefaultCellEditor implements TableCellEditor
    {
        private String currentText;
        private final JTextArea cellOverlay;

        /**********************************************************************
         * TablePopupEditor constructor
         *********************************************************************/
        private TablePopupEditor()
        {
            super(new JTextField());

            // Create a text area to overlay the cell being edited. The overlay
            // characteristics are set to mimic the cell's contents so that the
            // cell appears unchanged while the dialog is open (otherwise the
            // cell's text is reduced to a single line). The overlay also
            // allows the editor dialog is centered over the cell
            cellOverlay = new JTextArea();
            cellOverlay.setForeground(SELECTED_TEXT_COLOR);
            cellOverlay.setBackground(FOCUS_COLOR);
            setMultilineEditorCharacteristics(cellOverlay);
        }

        /**********************************************************************
         * Display a text input dialog to edit the contents of a table's cell.
         * Update the cell with the text if the user selects the input dialog's
         * Okay button
         *********************************************************************/
        @Override
        public Component getTableCellEditorComponent(final JTable table,
                                                     Object value,
                                                     boolean isSelected,
                                                     int row,
                                                     int column)
        {
            // This is required so that the cell overlay component will be on
            // the screen when its location is used to determine the dialog's
            // position
            SwingUtilities.invokeLater(new Runnable()
            {
                @Override
                public void run()
                {
                    // Set the number of rows in the text input area based on
                    // the length of the text. It's wrapped in a ScrollPane, so
                    // the text will "fit" regardless
                    int numRows = currentText.length() / minimumWrapColumn;

                    // Keep the number of rows within set bounds to prevent the
                    // text input dialog from becoming too small or too large
                    if (numRows < 5)
                    {
                        numRows = 5;
                    }
                    else if (numRows > 15)
                    {
                        numRows = 15;
                    }

                    // Create a text area for displaying/updating the text
                    JTextArea textEditor = new JTextArea(numRows,
                                                         minimumWrapColumn);
                    setMultilineEditorCharacteristics(textEditor);

                    // Initialize the dialog with the cell's text
                    textEditor.setText(currentText);

                    // Reset the text area focus traversal keys to use the
                    // default response to the Tab and Shift+Tab key presses
                    // (i.e., focus change) instead of inserting a tab into the
                    // text
                    textEditor.setFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS, null);
                    textEditor.setFocusTraversalKeys(KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS, null);

                    // Put the text area into a scroll pane to accommodate
                    // lengthy text. Then put the scroll pane into a panel so
                    // that a border can be applied. The layout allows the text
                    // area to resize to fit the dialog if it's resized
                    JScrollPane scrollPane = new JScrollPane(textEditor);
                    JPanel inputPanel = new JPanel();
                    inputPanel.setLayout(new BoxLayout(inputPanel,
                                                       BoxLayout.X_AXIS));
                    inputPanel.setBorder(BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
                    inputPanel.add(scrollPane);

                    // Display the text input dialog
                    if (new CPMDialogHandler().showOptionsDialog(cellOverlay,
                                                                 inputPanel,
                                                                 "Enter text",
                                                                 OK_CANCEL_OPTION,
                                                                 true) == OK_OPTION)
                    {
                        // User selected the Okay button; get the text that was
                        // entered
                        currentText = textEditor.getText();
                    }

                    // Send an event to indicate that editing has completed
                    fireEditingStopped();
                }
            });

            // Store the initial text contained in the cell
            currentText = value.toString();

            // Set the overlay text so that it's the same as the cell's
            // starting text
            cellOverlay.setText(currentText);

            return cellOverlay;
        }

        /**********************************************************************
         * Return the contents of the cell being edited
         *********************************************************************/
        @Override
        public Object getCellEditorValue()
        {
            return currentText;
        }
    }

    /**************************************************************************
     * Class for displaying a color choice dialog when a cell in the Color
     * column is selected
     *************************************************************************/
    private class ColorEditor extends AbstractCellEditor implements TableCellEditor
    {
        /**********************************************************************
         * Display a color choice dialog and update the ID color if a valid
         * color is selected
         *********************************************************************/
        @Override
        public Component getTableCellEditorComponent(final JTable table,
                                                     Object value,
                                                     boolean isSelected,
                                                     final int row,
                                                     final int column)
        {
            // Set the selected row to the one containing the chosen Color cell
            // and the focus to the Color cell. This is in case a cell is
            // chosen in a row other than the currently selected row
            changeSelection(row, column, false, false);

            // Display the color choice dialog
            Color newColor = new CPMDialogHandler().showColorDialog(table.getParent(),
                                                                    "ID \'"
                                                                        + getValueAt(row, nameColumn)
                                                                        + "\' Color",
                                                                    OK_CANCEL_OPTION,
                                                                    (Color) table.getValueAt(row, column));

            // Check if a valid color was selected
            if (newColor != null)
            {
                // Update the table to show the selected color
                setValueAt(newColor, row, column);
            }

            return null;
        }

        /**********************************************************************
         * This is unused for the color editor, but this method must be
         * provided since AbstractCellEditor doesn't
         *********************************************************************/
        @Override
        public Object getCellEditorValue()
        {
            return null;
        }
    }

    /**************************************************************************
     * Create a cell renderer in order to remove the focus border, if present
     *************************************************************************/
    private DefaultTableCellRenderer setTableCellRenderer()
    {
        // Create a cell renderer
        return new DefaultTableCellRenderer()
        {
            /******************************************************************
             * Override this method so that the focus border is removed
             *****************************************************************/
            @Override
            public Border getBorder()
            {
                return null;
            }

            /******************************************************************
             * Override this method so that the row number is always displayed
             * in the Row column, a color box is displayed in the Color column,
             * and to pad the name in the Name column
             *****************************************************************/
            @Override
            public Component getTableCellRendererComponent(JTable table,
                                                           Object value,
                                                           boolean isSelected,
                                                           boolean hasFocus,
                                                           int row,
                                                           int column)
            {
                super.getTableCellRendererComponent(table,
                                                    value,
                                                    isSelected,
                                                    hasFocus,
                                                    row,
                                                    column);

                // Pad the sides of the cell
                setBorder(cellBorder);

                // Set the icon to null. This prevents an icon from appearing
                // in every cell of the table
                setIcon(null);

                // Check if this is the row number column
                if (column == rowNumberColumn)
                {
                    // Set the cell's text to the indicated row number (add 1
                    // so that numbering begins at 1)
                    setText(Integer.toString(row + 1));
                }
                // Check if any of the columns is displaying an icon
                else if (iconColumns != null)
                {
                    // Step through the icon display columns (if any)
                    for (IconColumn ic : iconColumns)
                    {
                        // Check if the column matches one of the icon display
                        // columns
                        if (column == ic.column)
                        {
                            // Set the display icon based on the column type
                            // and value
                            setIcon(ic.getIcon(value));

                            // Hide the text for this cell
                            setText(null);

                            // No need to keep looking so exit the loop
                            break;
                        }
                    }
                }

                return this;
            }
        };
    }

    /**************************************************************************
     * Create a cell renderer in order to adjust the row height to fit the text
     * and to remove the focus border
     *************************************************************************/
    private class MultilineCellRenderer extends JTextArea implements TableCellRenderer
    {
        /**********************************************************************
         * Table cell renderer constructor
         *********************************************************************/
        public MultilineCellRenderer()
        {
            // Set the line wrap, border, and font characteristics of the cell
            setMultilineEditorCharacteristics(this);

            // Set to paint every pixel within the cell. This is needed to
            // prevent a border appearing around the cell for some look & feels
            setOpaque(true);
        }

        /**********************************************************************
         * Override this method so that the focus border is removed
         *********************************************************************/
        @Override
        public Border getBorder()
        {
            return null;
        }

        /**********************************************************************
         * Override this method so that the row height can be adjusted to fit
         * the text
         *********************************************************************/
        @Override
        public Component getTableCellRendererComponent(JTable table,
                                                       Object value,
                                                       boolean isSelected,
                                                       boolean hasFocus,
                                                       int row,
                                                       int column)
        {
            // Set the cell contents to the current value
            setText(value.toString());

            // Set the text area's width to the table column width. This is
            // needed so that the JTextArea calculates the height required to
            // show all of the cell's text
            setSize(table.getColumnModel().getColumn(column).getWidth(), 0);

            // Get the height required to show all of the cell's text
            int rowHeight = getPreferredSize().height;

            // Check if the cell height needs to change
            if (table.getRowHeight(row) != rowHeight)
            {
                // Adjust the row height
                setRowHeight(row, rowHeight);
            }

            return this;
        }
    }

    /**************************************************************************
     * Override prepareRenderer to allow adjusting the foreground and
     * background colors of the table's cells
     *************************************************************************/
    @Override
    public Component prepareRenderer(TableCellRenderer renderer,
                                     int row,
                                     int column)
    {
        Component comp = super.prepareRenderer(renderer, row, column);

        // Check if this is one of the selected rows and, if the focus isn't
        // being ignored, that the table has the focus
        if ((isFocusOwner() || ignoreFocus) && isRowSelected(row))
        {
            // Set the fore- and background colors for the selected row
            ((JComponent) comp).setForeground(SELECTED_TEXT_COLOR);
            ((JComponent) comp).setBackground(SELECTED_BACK_COLOR);

            // Check if this is the currently selected single cell
            if (column == getSelectedColumn() && getSelectedRowCount() == 1)
            {
                // Color the cell background to indicate it has the focus
                ((JComponent) comp).setBackground(FOCUS_COLOR);
            }
        }
        // Row is not selected and/or the table does not have the focus
        else
        {
            // True if this is an even numbered row
            boolean evenRow = row % 2 == 0;

            // Check if this is a column of check boxes
            if (column == showColumn)
            {
                // Set the cell opaque flag, which is based on the selected
                // look & feel and if the row number is odd (in order to
                // alternate the row colors)
                ((JComponent) comp).setOpaque(!evenRow
                                                      ? true
                                                      : LAF_OPAQUE);
            }

            // Set the fore- and background colors for the non- selected row.
            // Alternate the row colors every other row
            ((JComponent) comp).setBackground(evenRow
                                                     ? background
                                                     : ALTERNATE_COLOR);
            ((JComponent) comp).setForeground(TABLE_TEXT_COLOR);
        }

        return comp;
    }

    /**************************************************************************
     * Enable the table grid and set the spacing and color
     *************************************************************************/
    private void setTableGrid()
    {
        // Set up the table grid lines
        setShowGrid(true);
        setIntercellSpacing(new Dimension(1, 1));
        setGridColor(GRID_COLOR);
    }

    /**********************************************************************
     * Placeholder for handling changes to the editable table cells
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
     *********************************************************************/
    protected void handleCellContentChange(int row,
                                           int column,
                                           Object oldValue,
                                           Object newValue)
    {
    }

    /**************************************************************************
     * Listen for changes made to individual table cells via the
     * TableCellEditor. When editing begins, the cell's value is saved, and
     * when editing ends the cell contents is checked against the saved value
     * to see if it changed. If so, the new contents is validated and if found
     * to be invalid then the original value is restored. Also, if the user
     * exits editing via the ESC key the cell contents is replaced by the
     * original value
     *************************************************************************/
    private class TableCellListener implements PropertyChangeListener, Runnable
    {
        private int editRow;
        private int editColumn;
        private Object oldValue;
        private Object newValue;

        /**********************************************************************
         * Create a TableCellListener
         *********************************************************************/
        protected TableCellListener()
        {
            addPropertyChangeListener(this);
        }

        /**********************************************************************
         * Create a TableCellListener with a copy of all the data relevant to
         * the change of data for a given cell
         *********************************************************************/
        private TableCellListener(int row,
                                  int column,
                                  Object oldValue,
                                  Object newValue)
        {
            this.editRow = row;
            this.editColumn = column;
            this.oldValue = oldValue;
            this.newValue = newValue;
        }

        /**********************************************************************
         * Listen for property changes
         *********************************************************************/
        @Override
        public void propertyChange(PropertyChangeEvent pce)
        {
            // Check if the property being changed is from the cell editor
            if (pce.getPropertyName().equals("tableCellEditor"))
            {
                // Check if editing has initiated on the cell
                if (isEditing())
                {
                    // The invokeLater call is necessary since the row and
                    // column being edited are not available when the
                    // "tableCellEditor" PropertyChangeEvent is fired. This
                    // call results in the "run" method being invoked
                    // asynchronously, which allows the row and column to be
                    // determined
                    SwingUtilities.invokeLater(this);
                }
                // Editing has completed on this cell. Ignore changes if this
                // cell is a Show ID check box
                else if (editColumn != showColumn)
                {
                    // Get the cell value from the table; delete leading and
                    // trailing whitespace
                    newValue = getModel().getValueAt(editRow,
                                                     editColumn).toString().trim();

                    // Store the 'cleaned' value back in the table
                    setValueAt(newValue,
                               convertRowIndexToView(editRow),
                               editColumn);

                    // The data has changed, validate the contents
                    if (!newValue.equals(oldValue))
                    {
                        // Make a copy of the data in case another cell starts
                        // editing while processing this cell's change
                        new TableCellListener(editRow,
                                              editColumn,
                                              oldValue,
                                              newValue);

                        // Handle changes to the cell contents
                        handleCellContentChange(editRow,
                                                editColumn,
                                                oldValue,
                                                newValue);
                    }
                }
            }
        }

        /**********************************************************************
         * Get the selected row and column, initiate editing (if needed),
         * convert the row index to the table model, and store the cell
         * original cell value
         *********************************************************************/
        @Override
        public void run()
        {
            // Get the cell row and column that has the focus
            editRow = getSelectedRow();
            editColumn = getSelectedColumn();

            // Check if the cell is not actively being edited
            if (!isEditing())
            {
                // Initiate editing on the selected cell. This cover a specific
                // case where the editor doesn't engage following resizing
                // (smaller) a single table column
                editCellAt(editRow, editColumn);
            }

            // Convert the row index to the table model index, in case the rows
            // have been sorted. Since column swapping isn't allowed by the
            // application there is no need to convert the column index
            editRow = convertRowIndexToModel(editRow);

            // Store the cell's original contents so that he cell can be
            // restored if the new value is invalid or abandoned (by the user
            // pressing the ESC key)
            oldValue = getModel().getValueAt(editRow, editColumn);
        }
    }

    /**************************************************************************
     * Output the table to the user-selected printer (or file)
     * 
     * @param tableName
     *            table name; displayed at the top of each printed page
     * 
     * @param dialog
     *            parent dialog for this table
     * 
     * @param orientation
     *            page orientation; e.g., OrientationRequested.PORTRAIT or
     *            OrientationRequested.LANDSCAPE
     *************************************************************************/
    protected void printTable(String tableName,
                              JDialog dialog,
                              OrientationRequested orientation)
    {
        try
        {
            // Create an attribute set for modifying the print attributes
            PrintRequestAttributeSet attr = new HashPrintRequestAttributeSet();

            // Set the page orientation
            attr.add(orientation);

            // Display a printer dialog to obtain the desired destination and
            // output to the selected printer
            print(JTable.PrintMode.FIT_WIDTH,
                  new MessageFormat(tableName),
                  new MessageFormat("page {0}"),
                  true,
                  attr,
                  true);
        }
        catch (PrinterException pe)
        {
            // Inform the user that printing failed
            new CPMDialogHandler().showMessageDialog(dialog,
                                                     "<html><b>Table " +
                                                         tableName
                                                         + " printing failed",
                                                     "Print Fail",
                                                     JOptionPane.WARNING_MESSAGE,
                                                     OK_OPTION);
        }
    }
}
