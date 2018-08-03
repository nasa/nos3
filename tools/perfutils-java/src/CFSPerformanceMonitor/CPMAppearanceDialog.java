/**
 * CFS Performance Monitor Appearance Dialog
 */
package CFSPerformanceMonitor;

import static CFSPerformanceMonitor.CPMConstants.CLOSE_OPTION;
import static CFSPerformanceMonitor.CPMConstants.LABEL_FONT_BOLD;
import static CFSPerformanceMonitor.CPMConstants.LABEL_HORIZONTAL_SPACING;
import static CFSPerformanceMonitor.CPMConstants.LABEL_VERTICAL_SPACING;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ButtonGroup;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSeparator;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;

/******************************************************************************
 * CFS Performance Monitor application appearance dialog class
 *****************************************************************************/
@SuppressWarnings("serial")
public class CPMAppearanceDialog extends CPMDialogHandler
{
    private final CPMMain perfMain;

    /**************************************************************************
     * Application appearance dialog class constructor
     * 
     * @param perfMain
     *            main class
     *************************************************************************/
    protected CPMAppearanceDialog(CPMMain perfMain)
    {
        this.perfMain = perfMain;

        // Create the application appearance dialog
        initialize();
    }

    /**************************************************************************
     * Create the application appearance dialog
     *************************************************************************/
    private void initialize()
    {
        // Set the initial layout manager characteristics
        GridBagConstraints gbc = new GridBagConstraints(0,
                                                        0,
                                                        GridBagConstraints.REMAINDER,
                                                        1,
                                                        0.0,
                                                        0.0,
                                                        GridBagConstraints.LINE_START,
                                                        GridBagConstraints.NONE,
                                                        new Insets(0,
                                                                   LABEL_HORIZONTAL_SPACING,
                                                                   LABEL_VERTICAL_SPACING * 2,
                                                                   LABEL_HORIZONTAL_SPACING),
                                                        LABEL_HORIZONTAL_SPACING,
                                                        0);

        // Create the look & feel components ----------------------------------
        JPanel lafPanel = new JPanel(new GridBagLayout());

        // Set up the label over all radio buttons
        JLabel lafLabel = new JLabel("Select the application's 'look & feel'");
        lafLabel.setFont(LABEL_FONT_BOLD);
        lafPanel.add(lafLabel, gbc);

        // Listen for radio button selection changes
        ActionListener listener = new ActionListener()
        {
            /******************************************************************
             * Select the look & feel to use based on the radio button
             * selection
             *****************************************************************/
            @Override
            public void actionPerformed(ActionEvent ae)
            {
                // Check if the selected look & feel differs from the one
                // currently in use
                if (!perfMain.getLookAndFeel().equals(((JRadioButton) ae.getSource()).getText()))
                {
                    // Save the selected look & feel name for storage in the
                    // program preferences backing store and implement it
                    perfMain.setLookAndFeel(((JRadioButton) ae.getSource()).getText());

                    // Update this dialog so that it uses the selected look &
                    // feel. The button sizes must be recalculated and the
                    // dialog packed since each look & feel changes the sizes
                    // of components
                    SwingUtilities.updateComponentTreeUI(CPMAppearanceDialog.this);
                    CPMAppearanceDialog.this.setButtonWidth();
                    CPMAppearanceDialog.this.pack();
                }
            }
        };

        // Obtain the list of available look & feels to use in creating the
        // radio buttons
        LookAndFeelInfo[] lafInfo = UIManager.getInstalledLookAndFeels();

        // Set up storage for the look & feel radio buttons
        JRadioButton[] lafButton = new JRadioButton[lafInfo.length];
        ButtonGroup lafGroup = new ButtonGroup();
        gbc.insets.set(0,
                       LABEL_HORIZONTAL_SPACING * 2,
                       LABEL_VERTICAL_SPACING * 2,
                       LABEL_HORIZONTAL_SPACING);

        // Create radio buttons for each available look & feel
        for (int lafIndex = 0; lafIndex < lafInfo.length; lafIndex++)
        {
            lafButton[lafIndex] = new JRadioButton(lafInfo[lafIndex].getName(),
                                                   (lafInfo[lafIndex].getName().equals(perfMain.getLookAndFeel())));

            lafButton[lafIndex].setFont(LABEL_FONT_BOLD);
            lafButton[lafIndex].addActionListener(listener);

            // Add the radio buttons to a group so that only one is active at a
            // time
            lafGroup.add(lafButton[lafIndex]);

            // Position the radio button in the dialog panel
            gbc.gridy++;
            lafPanel.add(lafButton[lafIndex], gbc);
        }

        // Add a separator
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridy++;
        gbc.insets.set(0,
                       LABEL_HORIZONTAL_SPACING,
                       0,
                       LABEL_HORIZONTAL_SPACING);
        lafPanel.add(new JSeparator(JSeparator.HORIZONTAL), gbc);

        // Display the Application Appearance dialog and wait for the user to
        // select the Okay button
        showOptionsDialog(perfMain.getMainFrame(),
                          lafPanel,
                          "Application Appearance",
                          CLOSE_OPTION,
                          false);
    }
}
