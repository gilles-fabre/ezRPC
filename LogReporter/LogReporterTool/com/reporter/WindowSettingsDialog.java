package reporter;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import tracesListener.TracesFilter;

@SuppressWarnings("rawtypes")
public class WindowSettingsDialog extends JDialog {
	private static final long serialVersionUID = 1L;
	private MainFrame mainFrame;
	private ContentFrame contentFrame;
	private JPanel filterSettingsPanel = null;
	private JTabbedPane jTabbedPane = null;
	private GlobalSettingsPanel globalSettingsPanel = null;
	private JLabel jLabel = null;
	private JLabel jLabel1 = null;
	private JScrollPane globalModSP = null;
	private JScrollPane windowModSP = null;
	private JList globalModList = null;
	private JList windowModList = null;
	private JPanel moduleFilterPanel = null;
	private JPanel buttonPanel = null;
	private JButton okButton = null;
	private JButton cancelButton = null;
	private JPanel labelsPanel = null;
	private JPanel filtersButtonPanel = null;
	private JButton addButton = null;
	private JButton delButton = null;
	// List of filter names.
	private String globalModIds[] = {""};
	private String windowModIds[] = {""};
	private String backupModIds[] = {""};
	private JPanel windowPanel = null;
	private JLabel jLabel2 = null;
	private JTextField windowName = null;

	/**
	 * @param owner
	 */
	public WindowSettingsDialog(MainFrame owner, ContentFrame content) {
		super(owner);
		this.mainFrame = owner;
		this.contentFrame = content;
		initialize();
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		this.setModal(true);
		this.setContentPane(getFilterSettingsPanel());
		this.setPreferredSize(new Dimension(550, 450));
		this.setTitle("Window Settings");
	}

	public static void doIt(MainFrame owner, ContentFrame content) {
		WindowSettingsDialog d = new WindowSettingsDialog(owner,content);
		d.setLocationRelativeTo(owner);
		d.pack();
		d.setVisible(true);
	}

	/**
	 * This method initializes filterSettingsPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getFilterSettingsPanel() {
		if (filterSettingsPanel == null) {
			GridBagConstraints gridBagConstraints6 = new GridBagConstraints();
			gridBagConstraints6.gridx = 0;
			gridBagConstraints6.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints6.gridy = 0;
			GridBagConstraints gridBagConstraints5 = new GridBagConstraints();
			gridBagConstraints5.insets = new Insets(12, 0, 0, 0);
			gridBagConstraints5.gridy = 2;
			gridBagConstraints5.ipadx = -10;
			gridBagConstraints5.anchor = GridBagConstraints.SOUTH;
			gridBagConstraints5.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints5.gridx = 0;
			GridBagConstraints gridBagConstraints4 = new GridBagConstraints();
			gridBagConstraints4.fill = GridBagConstraints.BOTH;
			gridBagConstraints4.gridx = 0;
			gridBagConstraints4.gridy = 1;
			gridBagConstraints4.ipadx = -15;
			gridBagConstraints4.weightx = 1.0;
			gridBagConstraints4.weighty = 1.0;
			gridBagConstraints4.anchor = GridBagConstraints.NORTH;
			gridBagConstraints4.insets = new Insets(0, 0, 12, 0);
			filterSettingsPanel = new JPanel();
			filterSettingsPanel.setLayout(new GridBagLayout());
			filterSettingsPanel.add(getJTabbedPane(), gridBagConstraints4);
			filterSettingsPanel.add(getButtonPanel(), gridBagConstraints5);
			if (contentFrame != null)
				filterSettingsPanel.add(getWindowPanel(), gridBagConstraints6);
		}
		return filterSettingsPanel;
	}

	/**
	 * This method initializes globalModList
	 *
	 * @return javax.swing.JList
	 */
	@SuppressWarnings("unchecked")
	private JList getGlobalModList() {
		if (globalModList == null) {
			TracesFilter filter = mainFrame.getFilter();
			globalModIds = new String[filter.nModuleIdFilters()];
			for (int i=0; i<filter.nModuleIdFilters(); i++)
				globalModIds[i]=filter.getModuleIdFilter(i).getName();
			globalModList = new JList(globalModIds);
		}
		return globalModList;
	}

	/**
	 * This method initializes windowModList
	 *
	 * @return javax.swing.JList
	 */
	@SuppressWarnings("unchecked")
	private JList getWindowModList() {
		if (windowModList == null) {
			windowModIds = new String[contentFrame.getModIdNames().size()];
			for (int i=0; i<contentFrame.getModIdNames().size(); i++)
				windowModIds[i]=contentFrame.getModIdNames().get(i);
			windowModList = new JList(windowModIds);
			backupModIds = windowModIds;
		}
		return windowModList;
	}

	/**
	 * This method initializes moduleFilterPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getModuleFilterPanel() {
		if (moduleFilterPanel == null) {
			GridBagConstraints gridBagConstraints3 = new GridBagConstraints();
			gridBagConstraints3.fill = GridBagConstraints.BOTH;
			gridBagConstraints3.gridy = 1;
			gridBagConstraints3.ipady = -14;
			gridBagConstraints3.weightx = 1.0;
			gridBagConstraints3.weighty = 1.0;
			gridBagConstraints3.anchor = GridBagConstraints.EAST;
			gridBagConstraints3.gridx = 2;
			GridBagConstraints gridBagConstraints2 = new GridBagConstraints();
			gridBagConstraints2.fill = GridBagConstraints.BOTH;
			gridBagConstraints2.gridy = 1;
			gridBagConstraints2.ipady = -14;
			gridBagConstraints2.weightx = 1.0;
			gridBagConstraints2.weighty = 1.0;
			gridBagConstraints2.anchor = GridBagConstraints.WEST;
			gridBagConstraints2.gridx = 0;
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.gridx = 1;
			gridBagConstraints1.ipadx = -10;
			gridBagConstraints1.ipady = -14;
			gridBagConstraints1.fill = GridBagConstraints.BOTH;
			gridBagConstraints1.weightx = 0.2D;
			gridBagConstraints1.weighty = 1.0D;
			gridBagConstraints1.gridy = 1;
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridwidth = 3;
			gridBagConstraints.gridy = 0;
			gridBagConstraints.ipadx = 318;
			gridBagConstraints.anchor = GridBagConstraints.NORTH;
			gridBagConstraints.gridx = 0;
			moduleFilterPanel = new JPanel();
			moduleFilterPanel.setLayout(new GridBagLayout());
			moduleFilterPanel.setPreferredSize(new Dimension(500, 340));
			moduleFilterPanel.add(getLabelsPanel(), gridBagConstraints);
			moduleFilterPanel.add(getFiltersButtonPanel(), gridBagConstraints1);
			moduleFilterPanel.add(getGlobalModSP(), gridBagConstraints2);
			moduleFilterPanel.add(getWindowModSP(), gridBagConstraints3);
		}
		return moduleFilterPanel;
	}

	/**
	 * This method initializes buttonPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getButtonPanel() {
		if (buttonPanel == null) {
			FlowLayout flowLayout = new FlowLayout();
			flowLayout.setAlignment(FlowLayout.RIGHT);
			flowLayout.setAlignment(FlowLayout.RIGHT);
			buttonPanel = new JPanel();
			buttonPanel.setLayout(flowLayout);
			buttonPanel.setPreferredSize(new Dimension(500, 30));
			buttonPanel.add(getOkButton(), null);
			buttonPanel.add(getCancelButton(), null);
		}
		return buttonPanel;
	}

	/**
	 * This method initializes okButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getOkButton() {
		if (okButton == null) {
			okButton = new JButton();
			okButton.setText("OK");
			okButton.setPreferredSize(new Dimension(80, 25));
			okButton.addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					if (contentFrame != null) {
						contentFrame.setModIdNames(windowModIds);
						contentFrame.setTitle(getWindowName().getText());
					}
					App.getPrefs().showModuleId = getGlobalSettingsPanel().getShowModuleId().getSelectedObjects() != null;
					App.getPrefs().showTimestamp = getGlobalSettingsPanel().getShowTimestamp().getSelectedObjects() != null;
					App.getPrefs().showProcessId = getGlobalSettingsPanel().getShowProcessId().getSelectedObjects() != null;
					App.getPrefs().showThreadId = getGlobalSettingsPanel().getShowThreadId().getSelectedObjects() != null;
					App.getPrefs().showBinaryTraces = getGlobalSettingsPanel().getShowBinaryTraces().getSelectedObjects() != null;
					App.getPrefs().showFilterName = getGlobalSettingsPanel().getShowFilterName().getSelectedObjects() != null;
					App.getPrefs().perThreadWindow = getGlobalSettingsPanel().getPerThreadWindow().getSelectedObjects() != null;
					App.getPrefs().maxLines = (int)getGlobalSettingsPanel().getMaxLinesSpinnerField().getValue();
					WindowSettingsDialog.this.dispose();
				}
			});
		}
		return okButton;
	}

	/**
	 * This method initializes cancelButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getCancelButton() {
		if (cancelButton == null) {
			cancelButton = new JButton();
			cancelButton.setText("Cancel");
			cancelButton.setPreferredSize(new Dimension(80, 25));
			cancelButton.addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					if (contentFrame != null)
						contentFrame.setModIdNames(backupModIds);
					WindowSettingsDialog.this.dispose();
				}
			});
		}
		return cancelButton;
	}

	/**
	 * This method initializes labelsPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getLabelsPanel() {
		if (labelsPanel == null) {
			jLabel1 = new JLabel();
			jLabel1.setText("Window filters");
			jLabel = new JLabel();
			jLabel.setText("Global filters");
			jLabel.setDisplayedMnemonic(KeyEvent.VK_UNDEFINED);
			labelsPanel = new JPanel();
			labelsPanel.setLayout(new BorderLayout());
			labelsPanel.add(jLabel, BorderLayout.WEST);
			labelsPanel.add(jLabel1, BorderLayout.EAST);
		}
		return labelsPanel;
	}

	/**
	 * This method initializes filtersButtonPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getFiltersButtonPanel() {
		if (filtersButtonPanel == null) {
			filtersButtonPanel = new JPanel();
			filtersButtonPanel.setLayout(new GridLayout(2, 1));
			filtersButtonPanel.setPreferredSize(new Dimension(70, 340));
			filtersButtonPanel.add(getAddButton());
			filtersButtonPanel.add(getDelButton());
		}
		return filtersButtonPanel;
	}

	/**
	 * This method initializes addButton
	 *
	 * @return javax.swing.JButton
	 */
	@SuppressWarnings("unchecked")
	private JButton getAddButton() {
		if (addButton == null) {
			addButton = new JButton();
			addButton.setText("Add");
			addButton.setPreferredSize(new Dimension(70, 25));
			addButton.addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					int indices[] = null;
					indices = globalModList.getSelectedIndices();
					if (indices != null) {
						addModules(indices);
						windowModList.setListData(windowModIds);
					}
				}
			});
		}
		return addButton;
	}

	/**
	 * This method initializes delButton
	 *
	 * @return javax.swing.JButton
	 */
	@SuppressWarnings("unchecked")
	private JButton getDelButton() {
		if (delButton == null) {
			delButton = new JButton();
			delButton.setText("Del");
			delButton.setPreferredSize(new Dimension(70, 25));
			delButton.addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					int indices[] = null;
					indices = windowModList.getSelectedIndices();
					if (indices != null) {
						delModules(indices);
						windowModList.setListData(windowModIds);
					}
				}
			});
		}
		return delButton;
	}

	private void addModules(int indices[]) {
		int i;
		String addNames[] = new String[indices.length];
		for (i=0; i<indices.length; i++)
			addNames[i] = globalModIds[indices[i]];
		contentFrame.addModIdNames(addNames);
		windowModIds = new String[contentFrame.getModIdNames().size()];
		i=0;
		for (String filterName : contentFrame.getModIdNames())
			windowModIds[i++] = filterName;
	}

	private void delModules(int indices[]) {
		int i,j,k;
		String tmpModIds[] = new String[windowModIds.length - indices.length];
		i=j=k=0;
		while (k<indices.length) {
			while (j<indices[k])
				tmpModIds[i++] = windowModIds[j++];
			j++;
			k++;
		}
		while (i<tmpModIds.length)
			tmpModIds[i++] = windowModIds[j++];
		windowModIds = tmpModIds;
		contentFrame.setModIdNames(windowModIds);
	}

	/**
	 * This method initializes globalModSP
	 *
	 * @return javax.swing.JScrollPane
	 */
	private JScrollPane getGlobalModSP() {
		if (globalModSP == null) {
			globalModSP = new JScrollPane();
			globalModSP.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
			globalModSP.setPreferredSize(new Dimension(210, 340));
			globalModSP.setViewportView(getGlobalModList());
			globalModSP.setBorder(null);
		}
		return globalModSP;
	}

	/**
	 * This method initializes windowModSP
	 *
	 * @return javax.swing.JScrollPane
	 */
	private JScrollPane getWindowModSP() {
		if (windowModSP == null) {
			windowModSP = new JScrollPane();
			windowModSP.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
			windowModSP.setPreferredSize(new Dimension(210, 340));
			windowModSP.setViewportView(getWindowModList());
		}
		return windowModSP;
	}

	/**
	 * This method initializes jTabbedPane
	 *
	 * @return javax.swing.JTabbedPane
	 */
	private JTabbedPane getJTabbedPane() {
		if (jTabbedPane == null) {
			jTabbedPane = new JTabbedPane();
			jTabbedPane.addTab("Global settings", null, getGlobalSettingsPanel()
					, "Informations to display on every trace message");
			if (contentFrame != null)
				jTabbedPane.addTab("Filter settings", null, getModuleFilterPanel()
						, "Module filters available on the current window");
		}
		return jTabbedPane;
	}

	/**
	 * This method initializes globalSettingsPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private GlobalSettingsPanel getGlobalSettingsPanel() {
		if (globalSettingsPanel == null) {
			globalSettingsPanel = new GlobalSettingsPanel();
		}
		return globalSettingsPanel;
	}

	/**
	 * This method initializes windowPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getWindowPanel() {
		if (windowPanel == null) {
			GridBagConstraints gridBagConstraints8 = new GridBagConstraints();
			gridBagConstraints8.fill = GridBagConstraints.BOTH;
			gridBagConstraints8.gridx = 1;
			gridBagConstraints8.gridy = 0;
			gridBagConstraints8.weightx = 0.8D;
			gridBagConstraints8.weighty = 1.0D;
			gridBagConstraints8.anchor = GridBagConstraints.EAST;
			gridBagConstraints8.insets = new Insets(5, 3, 5, 5);
			GridBagConstraints gridBagConstraints7 = new GridBagConstraints();
			gridBagConstraints7.insets = new Insets(7, 5, 7, 2);
			gridBagConstraints7.gridy = 0;
			gridBagConstraints7.weightx = 0.2D;
			gridBagConstraints7.weighty = 1.0D;
			gridBagConstraints7.anchor = GridBagConstraints.WEST;
			gridBagConstraints7.fill = GridBagConstraints.BOTH;
			gridBagConstraints7.gridx = 0;
			jLabel2 = new JLabel();
			jLabel2.setText("Window name:");
			jLabel2.setHorizontalAlignment(SwingConstants.RIGHT);
			jLabel2.setDisplayedMnemonic(KeyEvent.VK_UNDEFINED);
			windowPanel = new JPanel();
			windowPanel.setLayout(new GridBagLayout());
			windowPanel.add(jLabel2, gridBagConstraints7);
			windowPanel.add(getWindowName(), gridBagConstraints8);
		}
		return windowPanel;
	}

	/**
	 * This method initializes windowName
	 *
	 * @return javax.swing.JTextField
	 */
	private JTextField getWindowName() {
		if (windowName == null) {
			windowName = new JTextField();
			windowName.setText(contentFrame.getTitle());
		}
		return windowName;
	}
}
