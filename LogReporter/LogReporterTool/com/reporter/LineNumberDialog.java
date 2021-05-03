package reporter;

import java.awt.Component;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class LineNumberDialog extends LineNumberDialogVE {
	private static final long serialVersionUID = 1L;
	private static String prevNumber = "";
	private boolean canceled = true;
	private boolean hasErrors = false;
	private String number = null;

	private LineNumberDialog(Frame owner, String title) {
		super(owner, title, prevNumber);
		okButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				canceled = false;
				LineNumberDialog.this.setVisible(false);
			}
		});
		cancelButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				LineNumberDialog.this.setVisible(false);
			}
		});
	}

	public static String doIt(Frame owner, Component where, ContentFrame cf) {
		String title = Strings.getString("goToLine");
		LineNumberDialog d = new LineNumberDialog(owner, title);
		d.pack();
		d.setLocationRelativeTo(where);
		for (;;) {
			d.canceled = true;
			d.hasErrors = false;
			d.number = null;
			doItOnce(d, where, cf);
			if (d.canceled) {
				d.dispose();
				return null;
			}
			if (!d.hasErrors) {
				d.dispose();
				prevNumber = d.number;
				return d.number;
			}
		}
	}

	private static void doItOnce(LineNumberDialog d, Component where, ContentFrame cf) {
		d.setVisible(true);
		// We are blocked until the dialog is made invisible.
		if (d.canceled) {
			return;
		}
		d.number = d.numberTextField.getText();
	}
}
