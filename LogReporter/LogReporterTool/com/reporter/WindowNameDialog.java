package reporter;

import java.awt.Component;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JOptionPane;

public class WindowNameDialog extends WindowNameDialogVE {
	private static final long serialVersionUID = 1L;
	private boolean canceled = true;
	private boolean hasErrors = false;
	private String errorString = "";
	private String name = null;

	private WindowNameDialog(Frame owner, String title, String name) {
		super(owner, title, name);
		okButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				canceled = false;
				WindowNameDialog.this.setVisible(false);
			}
		});
		cancelButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				WindowNameDialog.this.setVisible(false);
			}
		});
	}
			
	public static String doIt(Frame owner, Component where, ContentFrame cf, String name) {
		String title = Strings.getString("renameWindow");
		WindowNameDialog d = new WindowNameDialog(owner, title, name);
		d.pack();
		d.setLocationRelativeTo(where);
		for (;;) {
			d.canceled = true;
			d.hasErrors = false;
			d.errorString = "";
			d.name = null;
			doItOnce(d, where, cf, name);
			if (d.canceled) {
				d.dispose();
				return null;
			}
			if (!d.hasErrors) {
				d.dispose();
				return d.name;
			}
		}		
	}

	private static void doItOnce(WindowNameDialog d, Component where, ContentFrame cf, String oldName) {
		d.setVisible(true);
		// We are blocked until the dialog is made invisible.
		if (d.canceled) {
			return;
		}
		
		String newName = d.nameTextField.getText();
		ContentFrame cf2 = ContentFrameEnsemble.getInstance().getContentFrameByName(newName);
		if (cf2 != null && cf2 != cf) {
			d.errorString += '"' + newName + "\" " + Strings.getString("windowNameNotUnique");
			d.hasErrors = true;
		}
		
		if (!d.hasErrors) {
			d.name = newName;
		} else {
			JOptionPane.showMessageDialog(where, d.errorString, null, JOptionPane.ERROR_MESSAGE);
		}
	}
	
}
