package reporter;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Dimension;
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


public class ProgDefinitionDialogVE extends JDialog {
	private static final long serialVersionUID = 1L;
	protected JPanel jContentPane = null;
	protected JPanel buttonPanel = null;
	protected JButton okButton = null;
	protected JButton cancelButton = null;
	protected JPanel contentPanel = null;
	protected JLabel nameLabel = null;
	protected JTextField nameTextField = null;
	protected JLabel colorLabel = null;
	protected JPanel colorPanel = null;

	protected ProgDefinitionDialogVE(Frame owner, String title, String name, Color color) {
		super(owner);
		initialize();
		setTitle(title);
		nameTextField.setText(name);
		nameTextField.setForeground(color);
		nameTextField.selectAll();
		colorPanel.setBackground(color);
	}

	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setModal(true);
		this.setSize(new Dimension(281, 128));
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
			GridBagConstraints gridBagConstraints3 = new GridBagConstraints();
			gridBagConstraints3.fill = GridBagConstraints.BOTH;
			gridBagConstraints3.gridy = 0;
			gridBagConstraints3.weightx = 1.0;
			gridBagConstraints3.gridx = 1;
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
			contentPanel.add(colorLabel, gridBagConstraints11);
			contentPanel.add(getNameTextField(), gridBagConstraints3);
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
			nameTextField.setPreferredSize(new Dimension(240, 25));
		}
		return nameTextField;
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
			colorPanel.setPreferredSize(new Dimension(240, 25));
		}
		return colorPanel;
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
