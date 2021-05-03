package reporter;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.JDialog;
import javax.swing.WindowConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.text.Caret;

public class FindDialog extends JDialog {
	private static final long serialVersionUID = 1L;
	// The singleton instance.
	private static FindDialog instance = null;
	// The content frame that the find operation applies to.
	// This can change during the lifetime of this (non-modal) dialog.
	private ContentFrame contentFrame = null;

	private FindPanel findPanel = null;

	/**
	 * @param owner
	 */
	private FindDialog(MainFrame owner, ContentFrame contentFrame) {
		super(owner);
		this.contentFrame = contentFrame;
		findPanel = new FindPanel(this);
		findPanel.textField.setText(contentFrame.textArea.getSelectedText());
		findPanel.textField.selectAll();
		findPanel.fromTopButton.addActionListener(findListener);
		findPanel.fromBottomButton.addActionListener(findListener);
		findPanel.nextButton.addActionListener(findListener);
		findPanel.previousButton.addActionListener(findListener);
		findPanel.closeButton.addActionListener(closeListener);
		findPanel.caseCheckBox.setSelected(App.getPrefs().matchCase);
		findPanel.caseCheckBox.addChangeListener(caseListener);
		setContentPane(findPanel);
		setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		addWindowListener(windowListener);
		pack();
		setLocationRelativeTo(contentFrame.textScrollPane);
		setVisible(true);
	}

	private ActionListener findListener = new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			Object source = e.getSource();
			String str = findPanel.textField.getText();
			Caret caret = contentFrame.textArea.getCaret();
			boolean matchCase = App.getPrefs().matchCase;
			if (source == findPanel.fromTopButton) {
				contentFrame.textArea.selectString(str, 0, true, matchCase);
			} else if (source == findPanel.fromBottomButton) {
				contentFrame.textArea.selectString(str, -1, false, matchCase);
			} else if (source == findPanel.nextButton) {
				contentFrame.textArea.selectString(str, caret.getDot(), true, matchCase);
			} else if (source == findPanel.previousButton) {
				int index = caret.getMark() - str.length();
				if (index >= 0) {
					contentFrame.textArea.selectString(str, index, false, matchCase);
				}
			}
		}
	};

	private ChangeListener caseListener = new ChangeListener() {
		 public void stateChanged(ChangeEvent e) {
			 App.getPrefs().matchCase = findPanel.caseCheckBox.isSelected();
		 }
	};

	private ActionListener closeListener = new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			dispose();
		}
	};

	private WindowListener windowListener = new WindowAdapter() {
		public void windowClosed(WindowEvent e) {
			instance = null;
		}
	};

	public static void doIt(MainFrame owner, ContentFrame contentFrame) {
		if (instance == null) {
			instance = new FindDialog(owner, contentFrame);
		}
	}

	public static void onContentFrameActivated(ContentFrame contentFrame) {
		if (instance != null) {
			instance.contentFrame = contentFrame;
		}
	}

}
