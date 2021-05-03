package reporter;

import java.awt.Toolkit;

import javax.swing.JTextField;
import javax.swing.text.AbstractDocument;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.DocumentFilter;

// A JTextField that allows entering only decimal digits.

public class DecimalDigitsTextField extends JTextField {
	private static final long serialVersionUID = 1L;

	public DecimalDigitsTextField() {
		super();
		addFilter();
	}

	private void addFilter() {
		((AbstractDocument)getDocument()).setDocumentFilter(new NumericDocumentFilter());
	}

	class NumericDocumentFilter extends DocumentFilter {

		public void insertString(FilterBypass fb, int offset, String s, AttributeSet attrs) throws BadLocationException {
			if (s != null) {
				if (isValid(s)) {
					super.insertString(fb, offset, s, attrs);
				} else {
					Toolkit.getDefaultToolkit().beep();
				}
			}
		}

		public void replace(FilterBypass fb, int offset, int length, String text, AttributeSet attrs) throws BadLocationException {
			if (text != null) {
				if (isValid(text)) {
					super.replace(fb, offset, length, text, attrs);
				} else {
					Toolkit.getDefaultToolkit().beep();
				}
			}
		}

		private boolean isValid(String s) {
			for (int i=0; i < s.length(); i++) {
				if (!Character.isDigit(s.charAt(i))) {
					return false;
				}
			}
			return true;
		}
	}
}
