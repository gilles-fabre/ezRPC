package reporter;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;


public class ModIdDefinitionDialogVE extends JDialog {
	private static final long serialVersionUID = 1L;
	protected JPanel jContentPane = null;
	protected JPanel buttonPanel = null;
	protected JButton okButton = null;
	protected JButton cancelButton = null;
	protected JPanel contentPanel = null;
	protected JLabel nameLabel = null;
	protected JLabel maskLabel = null;
	protected JLabel valueLabel = null;
	protected JTextField nameTextField = null;
	protected JTextField maskTextField = null;
	protected JTextField valueTextField = null;
	protected JLabel colorLabel = null;
	protected JPanel colorPanel = null;

	protected ModIdDefinitionDialogVE(Frame owner, String title, String name, String mask, String value, Color color) {
		super(owner);
		initialize();
		setTitle(title);
		nameTextField.setText(name);
		nameTextField.setForeground(color);
		nameTextField.selectAll();
		maskTextField.setText(mask);
		valueTextField.setText(value);
		colorPanel.setBackground(color);
	}

	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setModal(true);
		this.setContentPane(getJContentPane());
	}


	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			BorderLayout borderLayout = new BorderLayout();
			borderLayout.setHgap(8);
			borderLayout.setVgap(8);
			jContentPane = new JPanel();
			jContentPane.setLayout(borderLayout);
			jContentPane.setBorder(BorderFactory.createEmptyBorder(12, 12, 12, 12));
			jContentPane.add(getButtonPanel(), BorderLayout.SOUTH);
			jContentPane.add(getContentPanel(), BorderLayout.CENTER);
		}
		return jContentPane;
	}

	/**
	 * This method initializes buttonPanel
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getButtonPanel() {
		if (buttonPanel == null) {
			GridLayout gridLayout = new GridLayout();
			gridLayout.setRows(1);
			gridLayout.setHgap(5);
			buttonPanel = new JPanel();
			buttonPanel.setLayout(gridLayout);
			buttonPanel.add(getOkButton(), null);
			buttonPanel.add(getCancelButton(), null);
		}
		return buttonPanel;
	}

	/**
	 * This method initializes okButton
	 * 
	 * @return javax.swing.JButton
	 */
	private JButton getOkButton() {
		if (okButton == null) {
			okButton = new JButton();
			okButton.setText("OK");
		}
		return okButton;
	}

	/**
	 * This method initializes cancelButton
	 * 
	 * @return javax.swing.JButton
	 */
	private JButton getCancelButton() {
		if (cancelButton == null) {
			cancelButton = new JButton();
			cancelButton.setText("Cancel");
		}
		return cancelButton;
	}

	/**
	 * This method initializes contentPanel
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getContentPanel() {
		if (contentPanel == null) {
			GridBagConstraints gridBagConstraints21 = new GridBagConstraints();
			gridBagConstraints21.gridx = 1;
			gridBagConstraints21.fill = GridBagConstraints.BOTH;
			gridBagConstraints21.gridy = 3;
			GridBagConstraints gridBagConstraints11 = new GridBagConstraints();
			gridBagConstraints11.gridx = 0;
			gridBagConstraints11.ipadx = 6;
			gridBagConstraints11.ipady = 7;
			gridBagConstraints11.gridy = 3;
			colorLabel = new JLabel();
			colorLabel.setText("Color:");
			GridBagConstraints gridBagConstraints5 = new GridBagConstraints();
			gridBagConstraints5.fill = GridBagConstraints.BOTH;
			gridBagConstraints5.gridy = 2;
			gridBagConstraints5.weightx = 1.0;
			gridBagConstraints5.gridx = 1;
			GridBagConstraints gridBagConstraints4 = new GridBagConstraints();
			gridBagConstraints4.fill = GridBagConstraints.BOTH;
			gridBagConstraints4.gridy = 1;
			gridBagConstraints4.weightx = 1.0;
			gridBagConstraints4.gridx = 1;
			GridBagConstraints gridBagConstraints3 = new GridBagConstraints();
			gridBagConstraints3.fill = GridBagConstraints.BOTH;
			gridBagConstraints3.gridy = 0;
			gridBagConstraints3.weightx = 1.0;
			gridBagConstraints3.gridx = 1;
			GridBagConstraints gridBagConstraints2 = new GridBagConstraints();
			gridBagConstraints2.gridx = 0;
			gridBagConstraints2.ipadx = 6;
			gridBagConstraints2.ipady = 7;
			gridBagConstraints2.gridy = 2;
			valueLabel = new JLabel();
			valueLabel.setText("Value:");
			valueLabel.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.gridx = 0;
			gridBagConstraints1.ipadx = 6;
			gridBagConstraints1.ipady = 7;
			gridBagConstraints1.gridy = 1;
			maskLabel = new JLabel();
			maskLabel.setText("Mask:");
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridx = 0;
			gridBagConstraints.ipadx = 6;
			gridBagConstraints.ipady = 7;
			gridBagConstraints.gridy = 0;
			nameLabel = new JLabel();
			nameLabel.setText("Name:");
			nameLabel.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
			contentPanel = new JPanel();
			contentPanel.setLayout(new GridBagLayout());
			contentPanel.add(nameLabel, gridBagConstraints);
			contentPanel.add(maskLabel, gridBagConstraints1);
			contentPanel.add(valueLabel, gridBagConstraints2);
			contentPanel.add(colorLabel, gridBagConstraints11);
			contentPanel.add(getNameTextField(), gridBagConstraints3);
			contentPanel.add(getMaskTextField(), gridBagConstraints4);
			contentPanel.add(getValueTextField(), gridBagConstraints5);
			contentPanel.add(getColorPanel(), gridBagConstraints21);
		}
		return contentPanel;
	}

	/**
	 * This method initializes nameTextField
	 * 
	 * @return javax.swing.JTextField
	 */
	private JTextField getNameTextField() {
		if (nameTextField == null) {
			nameTextField = new JTextField();
			nameTextField.setPreferredSize(new Dimension(180, 25));
		}
		return nameTextField;
	}

	/**
	 * This method initializes maskTextField
	 * 
	 * @return javax.swing.JTextField
	 */
	private JTextField getMaskTextField() {
		if (maskTextField == null) {
			maskTextField = new JTextField();
			maskTextField.setFont(new Font("Monospaced", Font.PLAIN, 12));
			maskTextField.setPreferredSize(new Dimension(180, 25));
		}
		return maskTextField;
	}

	/**
	 * This method initializes valueTextField
	 * 
	 * @return javax.swing.JTextField
	 */
	private JTextField getValueTextField() {
		if (valueTextField == null) {
			valueTextField = new JTextField();
			valueTextField.setFont(new Font("Monospaced", Font.PLAIN, 12));
			valueTextField.setPreferredSize(new Dimension(180, 25));
		}
		return valueTextField;
	}

	/**
	 * This method initializes colorPanel
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getColorPanel() {
		if (colorPanel == null) {
			colorPanel = new JPanel();
			colorPanel.setLayout(new BorderLayout());
			colorPanel.setBackground(new Color(0, 72, 168));
			colorPanel.setPreferredSize(new Dimension(180, 25));
		}
		return colorPanel;
	}

}
