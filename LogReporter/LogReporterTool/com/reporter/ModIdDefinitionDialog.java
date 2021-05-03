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

import tracesListener.ByteUtil;
import tracesListener.ModuleFilter;

public class ModIdDefinitionDialog extends ModIdDefinitionDialogVE {
	private static final long serialVersionUID = 1L;
	private boolean canceled = true;
	private boolean hasErrors = false;
	private String errorString = "";
	private ModuleFilter moduleFilter = null;

	private ModIdDefinitionDialog(Frame owner, String title, String name, String mask, String value, Color color) {
		super(owner, title, name, mask, value, color);
		okButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				canceled = false;
				ModIdDefinitionDialog.this.setVisible(false);
			}
		});
		cancelButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ModIdDefinitionDialog.this.setVisible(false);
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

	public static ModuleFilter doIt(Frame owner, Component where) {
		return doIt(owner, where, true, "enter a name here", "0xFF000000", "0x99000000", Color.BLACK);
	}

	public static ModuleFilter doIt(Frame owner, Component where, ModuleFilter mf) {
		return doIt(owner, where, false, mf.getName(), mf.getFormattedModMask(), mf.getFormattedModId(), mf.getColor());
	}

	private static ModuleFilter doIt(Frame owner, Component where, boolean isNewOp, String name, String mask, String value, Color color) {
		String title = Strings.getString(isNewOp ? "newMod" : "editMod" );
		ModIdDefinitionDialog d = new ModIdDefinitionDialog(owner, title, name, mask, value, color);
		d.pack();
		d.setLocationRelativeTo(where);
		for (;;) {
			d.canceled = true;
			d.hasErrors = false;
			d.errorString = "";
			d.moduleFilter = null;
			doItOnce(d, where, isNewOp, name);
			if (d.canceled) {
				d.dispose();
				return null;
			}
			if (!d.hasErrors) {
				d.dispose();
				return d.moduleFilter;
			}
		}
	}

	private static void doItOnce(ModIdDefinitionDialog d, Component where, boolean isNewOp, String oldName) {
		d.setVisible(true);
		// We are blocked until the dialog is made invisible.
		if (d.canceled) {
			return;
		}

		String newName = d.nameTextField.getText();
		int row = App.getMainFrame().getModIdByName(newName);
		if (row >= 0 && (isNewOp || row != App.getMainFrame().getModIdByName(oldName))) {
			d.errorString += '"' + newName + "\" " + Strings.getString("nameNotUnique");
			d.hasErrors = true;
		}

		int imask = 0;
		try {
			imask = ByteUtil.parseHexInteger(d.maskTextField.getText());
		} catch (RuntimeException e) {
			d.errorString += badHexErrMsg(d.maskTextField.getText(), d.hasErrors);
			d.hasErrors = true;
		}

		int ivalue = 0;
		try {
			ivalue = ByteUtil.parseHexInteger(d.valueTextField.getText());
		} catch (NumberFormatException e) {
			d.errorString += badHexErrMsg(d.valueTextField.getText(), d.hasErrors);
			d.hasErrors = true;
		}

		if (!d.hasErrors) {
			d.moduleFilter = new ModuleFilter(ivalue, imask, newName, d.colorPanel.getBackground());
		} else {
			JOptionPane.showMessageDialog(where, d.errorString, null, JOptionPane.ERROR_MESSAGE);
		}
	}

	private static String badHexErrMsg(String badHexNum, boolean prevErrs) {
		return  (prevErrs ? "\n" : "") + '"' + badHexNum + "\" " + Strings.getString("badHexFormat");
	}

} // @jve:decl-index=0:visual-constraint="10,10"
