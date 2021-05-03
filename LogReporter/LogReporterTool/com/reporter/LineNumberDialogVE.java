package reporter;

import java.awt.BorderLayout;
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


public class LineNumberDialogVE extends JDialog {
	private static final long serialVersionUID = 1L;
	protected JPanel jContentPane = null;
	protected JPanel buttonPanel = null;
	protected JButton okButton = null;
	protected JButton cancelButton = null;
	protected JPanel contentPanel = null;
	protected JLabel numberLabel = null;
	protected DecimalDigitsTextField numberTextField = null;

	protected LineNumberDialogVE(Frame owner, String title, String number) {
		super(owner);
		initialize();
		setTitle(title);
		numberTextField.setText(number);
		numberTextField.selectAll();
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
			GridBagConstraints gridBagConstraints3 = new GridBagConstraints();
			gridBagConstraints3.gridx = 1;
			gridBagConstraints3.gridy = 0;
			gridBagConstraints3.weightx = 1.0;
			gridBagConstraints3.fill = GridBagConstraints.BOTH;
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridx = 0;
			gridBagConstraints.gridy = 0;
			gridBagConstraints.ipadx = 6;
			gridBagConstraints.ipady = 7;
			numberLabel = new JLabel();
			numberLabel.setText("Line:");
			numberLabel.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
			contentPanel = new JPanel();
			contentPanel.setLayout(new GridBagLayout());
			contentPanel.add(numberLabel, gridBagConstraints);
			contentPanel.add(getNameTextField(), gridBagConstraints3);
		}
		return contentPanel;
	}

	/**
	 * This method initializes numberTextField
	 * 
	 * @return javax.swing.JTextField
	 */
	private JTextField getNameTextField() {
		if (numberTextField == null) {
			numberTextField = new DecimalDigitsTextField();
			numberTextField.setPreferredSize(new Dimension(120, 25));
		}
		return numberTextField;
	}

}
