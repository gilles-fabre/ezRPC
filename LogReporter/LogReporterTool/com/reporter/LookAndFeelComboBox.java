package reporter;

import javax.swing.JComboBox;
import javax.swing.LookAndFeel;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;

/** 
 * A combo box that lets the user choose any installed look-and-feel.
 * <p>
 * The {@link #LookAndFeelComboBox()} constructor takes care of finding installed look-and-feels
 * and putting their friendly names into the combo box.
 * <p>
 * The {@link #changeLookAndFeel()} method changes the look-and-feel
 * to the one that is currently selected in the combo box.
 * @author Tom Keel
 */
public class LookAndFeelComboBox extends JComboBox<String> {

	private static final long serialVersionUID = 1L;

	private LookAndFeelInfo[] infoTab = UIManager.getInstalledLookAndFeels();
	private String oldLnfClassName;
	
	/**
	 * Construct a combo box with the friendly names of all installed look-and-feels.
	 * The current look-and-feel will be initially selected in the combo box.
	 */
	public LookAndFeelComboBox() {
		super();
		LookAndFeel oldLnf = UIManager.getLookAndFeel();
		oldLnfClassName = oldLnf == null ? null : oldLnf.getClass().getName();
		for (LookAndFeelInfo info : infoTab) {
			addItem(info.getName());
			if (info.getClassName().equals(oldLnfClassName)) {
				setSelectedItem(info.getName());
			}
		}
	}
	
	/** 
	 * If the look-and-feel currently selected in this combo box
	 * differs from the currently active look-and-feel,
	 * then activate the selected look-and-feel.
	 * <p>
	 * Note that if you change the look-and-feel, then you should also apply
	 * {@link SwingUtilities#updateComponentTreeUI(java.awt.Component)}
	 * to all top-level containers so that they reflect the change.
	 * 
	 * @return the class name of the new look and feel,
	 * or null if the look and feel has not changed.
	 */
	public String changeLookAndFeel() {
		String newClassName = getSelectedLookAndFeelClassName();
		if (newClassName != null) {
			try {
				UIManager.setLookAndFeel(newClassName);
			} catch (Exception e ) {
				e.printStackTrace();
				newClassName = null;
			}
		}
		return newClassName;
	}

	/**
	 * Get the class name of the look-and-feel currently selected in this combo box,
	 * but only if it differs from the currently active look-and-feel.
	 * @return the class name of the currently selected look-and-feel,
	 * or null if the currently selected is the same as the currently active.
	 */
	public String getSelectedLookAndFeelClassName() {
		String selectedName = (String) getSelectedItem();
		for (LookAndFeelInfo info : infoTab) {
			if (info.getName().equals(selectedName) && !info.getClassName().equals(oldLnfClassName)) {
				return info.getClassName();
			}
		}
		return null;
	}

}
