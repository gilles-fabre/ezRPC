package reporter;

import java.io.File;

import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;

public class FilePicker {
	private JFileChooser fc = null;
	private int result = 0;

	FilePicker(String title, String directory, String file, String filter, boolean isSave) {
		fc = new JFileChooser();
		fc.setDialogTitle(title);
		fc.setDialogType(isSave ? JFileChooser.SAVE_DIALOG : JFileChooser.OPEN_DIALOG);
		fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
		fc.setFileHidingEnabled(false);
		fc.setSelectedFile(new File(directory + file));
		if (!filter.isEmpty()) 
			fc.setFileFilter(new FileNameExtensionFilter("." + filter, filter));
		result = fc.showDialog(App.getMainFrame(), null);
	}

	public String getDirectory() {
		if (result != JFileChooser.APPROVE_OPTION) {
			return null;
		}
		String dirName = fc.getSelectedFile().getParent();
		if (dirName.length() == 0 || dirName.charAt(dirName.length() - 1) != File.separatorChar) {
			dirName += File.separatorChar;
		}
		return dirName;
	}

	public String getFile() {
		return result == JFileChooser.APPROVE_OPTION ? fc.getSelectedFile().getName() : null;
	}
}
