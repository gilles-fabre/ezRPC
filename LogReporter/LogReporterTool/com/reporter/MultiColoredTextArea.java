package reporter;

import java.awt.Color;
import java.awt.Dimension;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.regex.PatternSyntaxException;

import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;

// An extension of JTextPane that can be used like a simple JTextArea with
// the added ability to append text in any desired color.
// We also force it to be non-editable, and to not do line-wrapping.
// We also keep track of the number of lines; we suppose for doing so that
// the only modifications to the text are via the clear() and append() methods.
public class MultiColoredTextArea extends JTextPane {
	private static final long serialVersionUID = 1L;
	private DefaultStyledDocument document = new DefaultStyledDocument();
	private SimpleAttributeSet attrSet = new SimpleAttributeSet();
	private int lineCount = 0;
	private int currentLine = 0;
	private JTextField regExpField = null;
	private String regExp = "";
	private Pattern pattern = null;
	
	
	public MultiColoredTextArea(JTextField _regExpField) {
		setDocument(document);
		setEditable(false);
		regExpField = _regExpField;
	}

	// This is part of what you have to do to suppress line-wrapping.
    @Override
    public boolean getScrollableTracksViewportWidth() {
        // Only track viewport width when the viewport is wider than the preferred width
        return getUI().getPreferredSize(this).width 
            <= getParent().getSize().width;
    };

    @Override
    public Dimension getPreferredSize() {
        // Avoid substituting the minimum width for the preferred width when the viewport is too narrow
        return getUI().getPreferredSize(this);
    };
    
    // Append the given text in black.
	public void append(String s) {
		append(s, Color.BLACK);
	}

	// Filter out lines that do NOT contain the regExp
	private String filterOutLines(String s) {
		String _regExp = regExpField.getText();
		if (_regExp == null || _regExp.isEmpty())
			return s;
		
		if (!_regExp.equals(regExp) || pattern == null) {
			try {
				regExp = _regExp;
				pattern = Pattern.compile(regExp);
			} catch (PatternSyntaxException e) {
				System.out.println("Misformatted regexp at index " + e.getIndex() + " : " + e.getDescription());
				return s;
			}
		}
		
		String result = "";
		int start = 0, end = 0;
		while ((end = s.indexOf('\n', start)) >= 0) {
			String line = s.substring(start, end + 1);
			start = end + 1;
			Matcher m = pattern.matcher(line);
			if (m.find())
				 result += line;
		}
		
		return result;
	}

	// Append the given text in the given color.
	public void append(String s, Color color) {
		StyleConstants.setForeground(attrSet, color);
		try {
			s = filterOutLines(s);

			int maxLines = App.getPrefs().maxLines;
			lineCount += AppUtil.occurrencesInString(s, '\n');
			document.insertString(document.getLength(), s, attrSet);
			
			// if we have more lines than the specified max, truncate the content by the delta (minimum)
			final String lineBreak = "\n";
		    if (lineCount > maxLines) {
		    	int extra = lineCount - maxLines;
		    	lineCount = maxLines;
				int lines = 0, offset = 0; 
				while (lines < extra) {
					String str = document.getText(offset++, 1);
					if (str.equals(lineBreak))
						++lines;
				}
				document.replace(0, offset, "", attrSet);
			}
		} catch(BadLocationException e) {
			System.err.println("Can't append text: " + e);
		}
	}

	// Erase all text.
	public void clear() {
		lineCount = 0;
		try {
			document.remove(0, document.getLength());
		} catch (BadLocationException e) {
			System.err.println("Can't remove text: " + e);
		}		
	}

	// Return the line count.
	public int getLineCount() {
		return lineCount;
	}
	
	// Return the current line.
	public int getCurrentLine() {
		return currentLine;
	}
	
	// Register a listener for notification of any changes to the text.
	public void addDocumentListener(DocumentListener listener) {
		document.addDocumentListener(listener);
	}
	
	// Unegister a listener for notification of any changes to the text.
	public void removeDocumentListener(DocumentListener listener) {
		document.removeDocumentListener(listener);
	}
	
	// The position of the "caret" within the text has changed.
	// We update the current line number appropriately.
	public void caretUpdate() {
		int dotPos = getCaret().getDot();
		int endPos = document.getEndPosition().getOffset();
		if (dotPos >= endPos - 1) {
			// At the end of the document (most frequent case).
			// Just use the line count (fast).
			currentLine = lineCount;
		} else {
			// Not at the end of the document (user changed position).
			// Have to count lines.
			String text = getText();
			int n = 1;
			for (int i=0; i < dotPos; i++) {
				if (text.charAt(i) == '\n') {
					n++;
				}
			}
			currentLine = n;
		}
	}

	// This is part of what you have to do to suppress line-wrapping.
	@Override
	public void setSize(Dimension d) {
		if (d.width < getParent().getSize().width) {
			d.width = getParent().getSize().width;
		}
		super.setSize(d);
	}

	// Select the first occurence of the given string starting at the given index,
	// and going in the given direction (forward or backward).
	// A negative index means "starting at end of string".
	public void selectString(String str, int index, boolean forward, boolean matchCase) {
		String text = getText();
		if (!matchCase) {
			str = str.toLowerCase();
			text = text.toLowerCase();
		}
		if (index < 0) {
			index = text.length();
		} else if (index > 0) {
			index = EOLHack.translateIndexFromCaretView(text, index);
		}
		int newIndex = forward ? text.indexOf(str, index) : text.lastIndexOf(str, index);
		if (newIndex != -1) {
			newIndex = EOLHack.translateIndexToCaretView(text, newIndex);
			setCaretPosition(newIndex);
			moveCaretPosition(newIndex + str.length());
			getCaret().setSelectionVisible(true);
		}
	}
	
	// Select the entire contents of line number l.
	public void goToLine(int l) {
		if (!(1 <= l && l <= lineCount)) {
			l = lineCount;
		}
		String text = getText();
		int i = charIndexFromLineNumber(text, l);
		int j = i;
		while (j < text.length() && text.charAt(j) != '\n') {
			j++;
		}
		if (i < j && text.charAt(j-1) == '\r') {
			j--; // windows eol...
		}
		int lineLen = j - i;
		i = EOLHack.translateIndexToCaretView(text, i);
		setCaretPosition(i + lineLen);
		moveCaretPosition(i);
		getCaret().setSelectionVisible(true);
	}
	
	private int charIndexFromLineNumber(String text, int l) {
		if (l <= 1) {
			return 0;
		}
		int linesToSkip = l - 1;
		int i = 0;
		while (linesToSkip > 0) {
			if (text.charAt(i) == '\n') {
				linesToSkip--;
			}
			i++;
		}
		return i;
	}

	// This class is a hack to work around an eol bug on Windows.
	// The problem is that getText() sees CR-LF, but the caret sees only LF.
	private static final class EOLHack {

		private static boolean initialized = false;
		private static boolean translationRequired;

		private static boolean translationRequired() {
			if (!initialized) {
				translationRequired = System.getProperty("line.separator").equals("\r\n");
				initialized = true;
			}
			return translationRequired;
		}

		public static int translateIndexFromCaretView(String text, int index) {
			if (!translationRequired()) {
				return index;
			}
			int simuIndex = 0;
			int realIndex = 0;
			while (simuIndex < index) {
				if (text.charAt(realIndex) != '\r') {
					simuIndex++;
				}
				realIndex++;
			}
			return realIndex;
		}

		public static int translateIndexToCaretView(String text, int index) {
			if (!translationRequired()) {
				return index;
			}
			int simuIndex = 0;
			int realIndex = 0;
			while (realIndex < index) {
				if (text.charAt(realIndex) != '\r') {
					simuIndex++;
				}
				realIndex++;
			}
			return simuIndex;
		}
	}
}
