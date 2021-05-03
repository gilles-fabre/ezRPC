package reporter;

import java.awt.GridLayout;

import javax.swing.JCheckBox;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

public class GlobalSettingsPanel extends JPanel {
	private static final long serialVersionUID = 1L;
	private JCheckBox showModuleId = null;
	private JCheckBox showTimestamp = null;
	private JCheckBox showProcessId = null;
	private JCheckBox showThreadId = null;
	private JCheckBox showBinaryTraces = null;
	private JCheckBox showFilterName = null;
	private JCheckBox perThreadWindow = null;
	private JSpinner maxLinesSpinnerField = null;
	
	/**
	 * This is the default constructor
	 */
	public GlobalSettingsPanel() {
		super();
		initialize();
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		GridLayout gridLayout = new GridLayout();
		gridLayout.setRows(8);
		gridLayout.setColumns(1);
		this.setLayout(gridLayout);
		//this.setPreferredSize(new Dimension(300, 100));
		this.setName("Global Settings");
		//this.setSize(new Dimension(300, 100));
		this.add(getShowModuleId(), null);
		this.add(getShowTimestamp(), null);
		this.add(getShowProcessId(), null);
		this.add(getShowThreadId(), null);
		this.add(getShowBinaryTraces(), null);
		this.add(getShowFilterName(), null);
		this.add(getPerThreadWindow(), null);
		this.add(getMaxLinesSpinnerField(), null);
		getShowModuleId().setSelected(App.getPrefs().showModuleId);
		getShowTimestamp().setSelected(App.getPrefs().showTimestamp);
		getShowProcessId().setSelected(App.getPrefs().showProcessId);
		getShowThreadId().setSelected(App.getPrefs().showThreadId);
		getShowBinaryTraces().setSelected(App.getPrefs().showBinaryTraces);
		getShowFilterName().setSelected(App.getPrefs().showFilterName);
		getPerThreadWindow().setSelected(App.getPrefs().perThreadWindow);
	}

	/**
	 * This method initializes showModuleId
	 *
	 * @return javax.swing.JCheckBox
	 */
	public JCheckBox getShowModuleId() {
		if (showModuleId == null) {
			showModuleId = new JCheckBox();
			showModuleId.setText("Show Module Id");
		}
		return showModuleId;
	}

	/**
	 * This method initializes showTimestamp
	 *
	 * @return javax.swing.JCheckBox
	 */
	public JCheckBox getShowTimestamp() {
		if (showTimestamp == null) {
			showTimestamp = new JCheckBox();
			showTimestamp.setText("Show Timestamp");
		}
		return showTimestamp;
	}

	/**
	 * This method initializes showProcessId
	 *
	 * @return javax.swing.JCheckBox
	 */
	public JCheckBox getShowProcessId() {
		if (showProcessId == null) {
			showProcessId = new JCheckBox();
			showProcessId.setText("Show Process Id");
		}
		return showProcessId;
	}

	/**
	 * This method initializes showThreadId
	 *
	 * @return javax.swing.JCheckBox
	 */
	public JCheckBox getShowThreadId() {
		if (showThreadId == null) {
			showThreadId = new JCheckBox();
			showThreadId.setText("Show Thread Id");
		}
		return showThreadId;
	}

	/**
	 * This method initializes showBinaryTraces
	 *
	 * @return javax.swing.JCheckBox
	 */
	public JCheckBox getShowBinaryTraces() {
		if (showBinaryTraces == null) {
			showBinaryTraces = new JCheckBox();
			showBinaryTraces.setText("Show binary traces");
		}
		return showBinaryTraces;
	}

	/**
	 * This method initializes showFilterName
	 *
	 * @return javax.swing.JCheckBox
	 */
	public JCheckBox getShowFilterName() {
		if (showFilterName == null) {
			showFilterName = new JCheckBox();
			showFilterName.setText("Show filter name");
		}
		return showFilterName;
	}

	/**
	 * This method initializes perThreadWindow
	 *
	 * @return javax.swing.JCheckBox
	 */
	public JCheckBox getPerThreadWindow() {
		if (perThreadWindow == null) {
			perThreadWindow = new JCheckBox();
			perThreadWindow.setText("Open one Window per Thread");
		}
		return perThreadWindow;
	}


	/**
	 * This method initializes maxLinesSpinnerField
	 * 
	 * @return javax.swing.JSpinner
	 */
	public JSpinner getMaxLinesSpinnerField() {
		if (maxLinesSpinnerField == null) {
			maxLinesSpinnerField = new JSpinner(new SpinnerNumberModel(App.getPrefs().maxLines, App.getPrefs().DEFAULT_MIN_LINES, App.getPrefs().DEFAULT_MAX_LINES, App.getPrefs().LINES_INCREMENT));
			maxLinesSpinnerField.setToolTipText("Maximum number of lines");
	    }
		return maxLinesSpinnerField;
	}
}  //  @jve:decl-index=0:visual-constraint="10,10"
