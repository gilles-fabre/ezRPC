/**
 * A simple ARGB Color picker. The Swing JColorChooser is over complicated...
 */

package reporter;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JSlider;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.WindowConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class ARGBColorPicker {
    Color                   color;
    JDialog                 dialog;
    ARGBColorPickerPanel    colorPanel;

    class OkAction implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            color = colorPanel.colorCircle.getBackground();
            dialog.setVisible(false);
            dialog.dispose();
        }
    }

    class CancelAction implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            color = null;
            dialog.setVisible(false);
            dialog.dispose();
        }
    }

    public ARGBColorPicker(JFrame parent, Color color, Boolean withAlpha) {
    	dialog = new JDialog(parent, Strings.getString("ARGBColorPicker.Title"), true); //$NON-NLS-1$
        colorPanel = new ARGBColorPickerPanel(color);
        colorPanel.alphaSlider.setEnabled(withAlpha);
        colorPanel.alphaSpinner.setEnabled(withAlpha);
        colorPanel.validate();
        dialog.add(colorPanel);
        dialog.pack();
        dialog.setLocationRelativeTo(parent);
        dialog.setResizable(false);
        dialog.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
    }

    class ARGBColorPickerPanel extends ARGBColorPanel {
        /**
		 *
		 */
		private static final long serialVersionUID = 1L;

		class ChangeAlphaAction implements ChangeListener {
            public void stateChanged(ChangeEvent e) {
                Color oldColor = colorPanel.colorCircle.getBackground();
                int value = oldColor.getAlpha();

                try {
                    value = JSlider.class.cast(e.getSource()).getValue();
                    colorPanel.alphaSpinner.setValue(value);
                } catch (ClassCastException ex) {
                    value = Integer.class.cast(JSpinner.class.cast(e.getSource()).getValue());
                    colorPanel.alphaSlider.setValue(value);
                }

                Color newColor = new Color(oldColor.getRed(), oldColor.getGreen(), oldColor.getBlue(), value);
                colorPanel.colorCircle.setBackground(newColor);
                colorPanel.colorArea.setBackground(newColor);
            }
        }

        class ChangeRedAction implements ChangeListener {
            public void stateChanged(ChangeEvent e) {
                Color oldColor = colorPanel.colorCircle.getBackground();
                int value = oldColor.getRed();

                try {
                    value = JSlider.class.cast(e.getSource()).getValue();
                    colorPanel.redSpinner.setValue(value);
                } catch (ClassCastException ex) {
                    value = Integer.class.cast(JSpinner.class.cast(e.getSource()).getValue());
                    colorPanel.redSlider.setValue(value);
                }

                Color newColor = new Color(value, oldColor.getGreen(), oldColor.getBlue(), oldColor.getAlpha());
                colorPanel.colorCircle.setBackground(newColor);
                colorPanel.colorArea.setBackground(newColor);
            }
        }

        class ChangeGreenAction implements ChangeListener {
            public void stateChanged(ChangeEvent e) {
                Color oldColor = colorPanel.colorCircle.getBackground();
                int value = oldColor.getGreen();

                try {
                    value = JSlider.class.cast(e.getSource()).getValue();
                    colorPanel.greenSpinner.setValue(value);
                } catch (ClassCastException ex) {
                    value = Integer.class.cast(JSpinner.class.cast(e.getSource()).getValue());
                    colorPanel.greenSlider.setValue(value);
                }

                Color newColor = new Color(oldColor.getRed(), value, oldColor.getBlue(), oldColor.getAlpha());
                colorPanel.colorCircle.setBackground(newColor);
                colorPanel.colorArea.setBackground(newColor);
            }
        }

        class ChangeBlueAction implements ChangeListener {
            public void stateChanged(ChangeEvent e) {
                Color oldColor = colorPanel.colorCircle.getBackground();
                int value = oldColor.getBlue();

                try {
                    value = JSlider.class.cast(e.getSource()).getValue();
                    colorPanel.blueSpinner.setValue(value);
                } catch (ClassCastException ex) {
                    value = Integer.class.cast(JSpinner.class.cast(e.getSource()).getValue());
                    colorPanel.blueSlider.setValue(value);
                }

                Color newColor = new Color(oldColor.getRed(), oldColor.getGreen(), value, oldColor.getAlpha());
                colorPanel.colorCircle.setBackground(newColor);
                colorPanel.colorArea.setBackground(newColor);
            }
        }

        public void update() {
            Color color = colorPanel.colorCircle.getBackground();
            colorPanel.redSpinner.setValue(color.getRed());
            colorPanel.greenSpinner.setValue(color.getGreen());
            colorPanel.blueSpinner.setValue(color.getBlue());
        }

        ARGBColorPickerPanel(Color color) {
            super();
            colorCircle.setBackground(color);
            colorArea.setBackground(color);

            okButton.addActionListener(new OkAction());
            cancelButton.addActionListener(new CancelAction());

            alphaSlider.setValue(color.getAlpha());
            alphaSlider.addChangeListener(new ChangeAlphaAction());

            redSlider.setValue(color.getRed());
            redSlider.addChangeListener(new ChangeRedAction());

            greenSlider.setValue(color.getGreen());
            greenSlider.addChangeListener(new ChangeGreenAction());

            blueSlider.setValue(color.getBlue());
            blueSlider.addChangeListener(new ChangeBlueAction());

            SpinnerNumberModel 	alphaModel = new SpinnerNumberModel(color.getAlpha(), 0, 255, 1);
            alphaSpinner.setModel(alphaModel);
            alphaSpinner.addChangeListener(new ChangeAlphaAction());

            SpinnerNumberModel 	redModel = new SpinnerNumberModel(color.getRed(), 0, 255, 1);
            redSpinner.setModel(redModel);
            redSpinner.addChangeListener(new ChangeRedAction());

            SpinnerNumberModel 	greenModel = new SpinnerNumberModel(color.getGreen(), 0, 255, 1);
            greenSpinner.setModel(greenModel);
            greenSpinner.addChangeListener(new ChangeGreenAction());

            SpinnerNumberModel 	blueModel = new SpinnerNumberModel(color.getBlue(), 0, 255, 1);
            blueSpinner.setModel(blueModel);
            blueSpinner.addChangeListener(new ChangeBlueAction());
        }
    }


    public Color getColor() {
        dialog.setVisible(true);
        return color;
    }

    static Color doIt(JFrame parent, Color color) {
    	return new ARGBColorPicker(parent, color, false).getColor();
    }
}
