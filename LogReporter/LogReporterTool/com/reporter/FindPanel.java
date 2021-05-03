package reporter;

import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Rectangle;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.JCheckBox;

public class FindPanel extends JPanel {
	private static final long serialVersionUID = 1L;
	JButton fromTopButton = null;
	JButton fromBottomButton = null;
	JButton nextButton = null;
	JButton previousButton = null;
	JButton closeButton = null;
	private JPanel textPanel = null;
	private JPanel casePanel = null;
	private JLabel textLabel = null;
	JTextField textField = null;
	JCheckBox caseCheckBox = null;
	private JLabel caseLabel = null;

	/**
	 * This is the default constructor
	 */
	public FindPanel(JDialog dlg) {
		super();
		initialize();
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		GridBagConstraints gridBagConstraints11 = new GridBagConstraints();
		gridBagConstraints11.gridx = 0;
		gridBagConstraints11.gridwidth = 2;
		gridBagConstraints11.insets = new Insets(3, 0, 3, 0);
		gridBagConstraints11.anchor = GridBagConstraints.CENTER;
		gridBagConstraints11.gridy = 1;
		GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
		gridBagConstraints1.gridx = 0;
		gridBagConstraints1.fill = GridBagConstraints.BOTH;
		gridBagConstraints1.weightx = 1.0D;
		gridBagConstraints1.weighty = 1.0D;
		gridBagConstraints1.gridy = 3;
		GridBagConstraints gridBagConstraints = new GridBagConstraints();
		gridBagConstraints.gridx = 0;
		gridBagConstraints.gridwidth = 2;
		gridBagConstraints.fill = GridBagConstraints.BOTH;
		gridBagConstraints.weightx = 1.0D;
		gridBagConstraints.weighty = 2.0D;
		gridBagConstraints.insets = new Insets(5, 1, 0, 0);
		gridBagConstraints.gridy = 0;
		GridBagConstraints fromTopButtonConstraints = new GridBagConstraints();
		fromTopButtonConstraints.gridx = 0;
		fromTopButtonConstraints.gridy = 2;
		fromTopButtonConstraints.weightx = 1.0D;
		fromTopButtonConstraints.weighty = 1.0D;
		fromTopButtonConstraints.fill = GridBagConstraints.BOTH;
		GridBagConstraints nextButtonConstraints = new GridBagConstraints();
		nextButtonConstraints.gridx = 1;
		nextButtonConstraints.gridy = 2;
		nextButtonConstraints.weightx = 1.0D;
		nextButtonConstraints.weighty = 1.0D;
		nextButtonConstraints.fill = GridBagConstraints.BOTH;
		GridBagConstraints previousButtonConstraints = new GridBagConstraints();
		previousButtonConstraints.gridx = 1;
		previousButtonConstraints.gridy = 3;
		previousButtonConstraints.weightx = 1.0D;
		previousButtonConstraints.weighty = 1.0D;
		previousButtonConstraints.fill = GridBagConstraints.BOTH;
		this.setLayout(new GridBagLayout());
		this.setBounds(new Rectangle(0, 0, 318, 138));
		GridBagConstraints closeConstraints = new GridBagConstraints();
		closeConstraints.gridx = 0;
		closeConstraints.gridy = 4;
		closeConstraints.weightx = 1.0D;
		closeConstraints.weighty = 1.0D;
		closeConstraints.gridwidth = 3;
		closeConstraints.gridheight = 2;
		closeConstraints.insets = new Insets(11, 0, 0, 0);
		closeConstraints.fill = GridBagConstraints.BOTH;
		this.add(getFromTopButton(), fromTopButtonConstraints);
		this.add(getNextButton(), nextButtonConstraints);
		this.add(getFromBottomButton(), gridBagConstraints1);
		this.add(getPreviousButton(), previousButtonConstraints);
		this.add(getCloseButton(), closeConstraints);
		this.add(getTextPanel(), gridBagConstraints);
		this.add(getCasePanel(), gridBagConstraints11);
	}

	/**
	 * This method initializes fromTopButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getFromTopButton() {
		if (fromTopButton == null) {
			fromTopButton = new JButton();
			fromTopButton.setText("First");
			fromTopButton.setPreferredSize(new Dimension(100, 25));
		}
		return fromTopButton;
	}

	/**
	 * This method initializes nextButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getNextButton() {
		if (nextButton == null) {
			nextButton = new JButton();
			nextButton.setText("Next");
			nextButton.setPreferredSize(new Dimension(100, 25));
		}
		return nextButton;
	}

	/**
	 * This method initializes previousButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getPreviousButton() {
		if (previousButton == null) {
			previousButton = new JButton();
			previousButton.setText("Previous");
			previousButton.setPreferredSize(new Dimension(100, 25));
		}
		return previousButton;
	}

	/**
	 * This method initializes closeButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getCloseButton() {
		if (closeButton == null) {
			closeButton = new JButton();
			closeButton.setText("Close");
			closeButton.setPreferredSize(new Dimension(100, 25));
		}
		return closeButton;
	}

	/**
	 * This method initializes textPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getTextPanel() {
		if (textPanel == null) {
			textPanel = new JPanel();
			textPanel.setLayout(new BoxLayout(getTextPanel(), BoxLayout.X_AXIS));
			textLabel = new JLabel();
			textLabel.setHorizontalAlignment(SwingConstants.CENTER);
			textLabel.setText("Find: ");
			textLabel.setHorizontalTextPosition(SwingConstants.CENTER);
			textPanel.add(textLabel, null);
			textPanel.add(getTextField(), null);
		}
		return textPanel;
	}

	/**
	 * This method initializes fromBottomButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getFromBottomButton() {
		if (fromBottomButton == null) {
			fromBottomButton = new JButton();
			fromBottomButton.setPreferredSize(new Dimension(100, 25));
			fromBottomButton.setText("Last");
		}
		return fromBottomButton;
	}

	/**
	 * This method initializes casePanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getCasePanel() {
		if (casePanel == null) {
			casePanel = new JPanel();
			casePanel.setLayout(new BoxLayout(getCasePanel(), BoxLayout.X_AXIS));
			casePanel.add(getCaseCheckBox(), null);
			caseLabel = new JLabel();
			caseLabel.setText(" Match Case");
			casePanel.add(caseLabel, null);
		}
		return casePanel;
	}

	/**
	 * This method initializes textField
	 *
	 * @return javax.swing.JTextField
	 */
	private JTextField getTextField() {
		if (textField == null) {
			textField = new JTextField();
			textField.setPreferredSize(new Dimension(250, 25));
		}
		return textField;
	}

	/**
	 * This method initializes caseCheckBox
	 *
	 * @return javax.swing.JCheckBox
	 */
	private JCheckBox getCaseCheckBox() {
		if (caseCheckBox == null) {
			caseCheckBox = new JCheckBox();
		}
		return caseCheckBox;
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
