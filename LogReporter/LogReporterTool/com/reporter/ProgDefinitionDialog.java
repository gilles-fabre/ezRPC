package reporter;

import java.awt.Color;
import java.awt.Component;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JOptionPane;

import tracesListener.ProgFilter;

public class ProgDefinitionDialog extends ProgDefinitionDialogVE {
	private static final long serialVersionUID = 1L;
	private boolean canceled = true;
	private boolean hasErrors = false;
	private String errorString = "";
	private ProgFilter progFilter = null;

	private ProgDefinitionDialog(Frame owner, String title, String value, Color color) {
		super(owner, title, value, color);
		okButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				canceled = false;
				ProgDefinitionDialog.this.setVisible(false);
			}
		});
		cancelButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				ProgDefinitionDialog.this.setVisible(false);
			}
		});
		colorPanel.addMouseListener(colorListener);
	}

	private MouseListener colorListener = new MouseAdapter() {

		public void mousePressed(MouseEvent e) {
			doDialog(e);
		}

		private void doDialog(MouseEvent e) {
			Color color = ARGBColorPicker.doIt(App.getMainFrame(), colorPanel.getBackground());
			if (color != null) {
				nameTextField.setForeground(color);
				colorPanel.setBackground(color);
			}
		}
	};

	public static ProgFilter doIt(Frame owner, Component where) {
		return doIt(owner, where, true, "enter a program name here", Color.BLACK);
	}

	public static ProgFilter doIt(Frame owner, Component where, ProgFilter pf) {
		return doIt(owner, where, false, pf.getName(), pf.getColor());
	}

	private static ProgFilter doIt(Frame owner, Component where, boolean isNewOp, String name, Color color) {
		String title = Strings.getString(isNewOp ? "newProg" : "editProg" );
		ProgDefinitionDialog d = new ProgDefinitionDialog(owner, title, name, color);
		d.pack();
		d.setLocationRelativeTo(where);
		for (;;) {
			d.canceled = true;
			d.hasErrors = false;
			d.errorString = "";
			d.progFilter = null;
			doItOnce(d, where, isNewOp, name);
			if (d.canceled) {
				d.dispose();
				return null;
			}
			if (!d.hasErrors) {
				d.dispose();
				return d.progFilter;
			}
		}
	}

	private static void doItOnce(ProgDefinitionDialog d, Component where, boolean isNewOp, String oldName) {
		d.setVisible(true);
		// We are blocked until the dialog is made invisible.
		if (d.canceled) {
			return;
		}

		String newName = d.nameTextField.getText();
		int row = App.getMainFrame().getProgByName(newName);
		if (row >= 0 && (isNewOp || row != App.getMainFrame().getProgByName(oldName))) {
			d.errorString += '"' + newName + "\" " + Strings.getString("nameNotUnique");
			d.hasErrors = true;
		}

		if (!d.hasErrors) {
			d.progFilter = new ProgFilter(newName, d.colorPanel.getBackground());
		} else {
			JOptionPane.showMessageDialog(where, d.errorString, null, JOptionPane.ERROR_MESSAGE);
		}
	}

}
