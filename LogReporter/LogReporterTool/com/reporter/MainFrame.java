package reporter;

import java.awt.Color;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;

import javax.swing.AbstractAction;
import javax.swing.JComponent;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import javax.swing.ListSelectionModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import tracesListener.ModuleFilter;
import tracesListener.ProgFilter;
import tracesListener.TracesFilter;

public class MainFrame extends MainFrameVE implements ChangeListener {
	private static final long serialVersionUID = 1L;

	// The global conglomerate filter, a.k.a. "the palette".
	// There are references to this from the models of both the ModIdTable and the ProgIdTable.
	private TracesFilter filter = null;

	/**
	 * This is the default constructor
	 */
	public MainFrame() {
		super();
		updateViewMenu();
		updateViewToolbar();
		windowMenu.addChangeListener(this);
		modIdPanel.addMouseListener(modIdPopupListener);
		modIdScrollPane.addMouseListener(modIdPopupListener);
		modIdTable.addMouseListener(modIdPopupListener);
		modIdTable.setDragEnabled(true);
		modIdTable.setTransferHandler(new ModIdTableTransferHandler(null));
		progPanel.addMouseListener(progPopupListener);
		progScrollPane.addMouseListener(progPopupListener);
		progTable.addMouseListener(progPopupListener);
		progTable.setDragEnabled(true);
		progTable.setTransferHandler(new ProgTableTransferHandler(null));
		defineKeyBindings();
		setFilter(new TracesFilter());

		pauseButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				App.setLogMode(App.LogMode.PAUSE);
				updateLogModeButtons();
			}
		});
		stopButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				App.setLogMode(App.LogMode.STOP);
				updateLogModeButtons();
			}
		});
		playButton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				App.setLogMode(App.LogMode.PLAY);
				updateLogModeButtons();
			}
		});
		exitMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getInstance().exit();
			}
		});
		settingsMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				openSettings();
				updateViewMenu();
				updateViewToolbar();
			}
		});
		importLogMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				importLog();
			}
		});
		importSettingsMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				importFilters();
			}
		});
		exportSettingsMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				exportFilters();
			}
		});
		saveSettingsMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				saveCurrentContentFrame();
			}
		});
		newModIdMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				newModId(modIdPanel);
			}
		});
		editModIdMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				editSelectedModIds(modIdPanel);
			}
		});
		copyModIdMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				copySelectedModIdsToWindow();
			}
		});
		deleteModIdMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				deleteSelectedModIds();			}
		});
		newModIdPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				newModId(modIdPanel);
			}
		});
		editModIdPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				editSelectedModIds(modIdPanel);
			}
		});
		copyModIdPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				copySelectedModIdsToWindow();
			}
		});
		deleteModIdPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				deleteSelectedModIds();			}
		});
		newProgMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				newProg(progPanel);
			}
		});
		editProgMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				editSelectedProgs(progPanel);
			}
		});
		copyProgMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				copySelectedProgsToWindow();			}
		});
		deleteProgMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				deleteSelectedProgs();
			}
		});
		newProgPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				newProg(progPanel);
			}
		});
		editProgPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				editSelectedProgs(progPanel);
			}
		});
		copyProgPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				copySelectedProgsToWindow();			}
		});
		deleteProgPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				deleteSelectedProgs();
			}
		});
		showModuleIdBttn.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showModuleId = showModuleIdBttn.isSelected();
				updateViewMenu();
			}
		});
		showModuleIdMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showModuleId = showModuleIdMI.isSelected();
				updateViewToolbar();
			}
		});
		showTimestampBttn.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showTimestamp = showTimestampBttn.isSelected();
				updateViewMenu();
			}
		});
		showTimestampMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showTimestamp = showTimestampMI.isSelected();
				updateViewToolbar();
			}
		});
		showProcessIdBttn.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showProcessId = showProcessIdBttn.isSelected();
				updateViewMenu();
			}
		});
		showProcessIdMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showProcessId = showProcessIdMI.isSelected();
				updateViewToolbar();
			}
		});
		showThreadIdBttn.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showThreadId = showThreadIdBttn.isSelected();
				updateViewMenu();
			}
		});
		showThreadIdMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showThreadId = showThreadIdMI.isSelected();
				updateViewToolbar();
			}
		});
		showBinaryBttn.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showBinaryTraces = showBinaryBttn.isSelected();
				updateViewMenu();
			}
		});
		showBinaryMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showBinaryTraces = showBinaryMI.isSelected();
				updateViewToolbar();
			}
		});
		showFilterNameBttn.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showFilterName = showFilterNameBttn.isSelected();
				updateViewMenu();
			}
		});
		showFilterNameMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().showFilterName = showFilterNameMI.isSelected();
				updateViewToolbar();
			}
		});
		showWindowPerProcessMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				App.getPrefs().perThreadWindow = showWindowPerProcessMI.isSelected();
			}
		});
		appendSeparatorMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrame cf = (ContentFrame) desktopPane.getSelectedFrame();
				if (cf != null) {
					cf.insertSeparator();
				}
			}
		});
		clearWindowMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrame cf = (ContentFrame) desktopPane.getSelectedFrame();
				if (cf != null) {
					cf.clear();
				}
			}
		});
		findMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrame cf = (ContentFrame) desktopPane.getSelectedFrame();
				if (cf != null) {
					FindDialog.doIt(MainFrame.this, cf);
				}
			}
		});
		gotoMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrame cf = (ContentFrame) desktopPane.getSelectedFrame();
				if (cf != null) {
					cf.goToLine();
				}
			}
		});
		selectAllMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrame cf = (ContentFrame) desktopPane.getSelectedFrame();
				if (cf != null) {
					cf.textArea.selectAll();
				}
			}
		});
		copyTextMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrame cf = (ContentFrame) desktopPane.getSelectedFrame();
				if (cf != null) {
					cf.textArea.copy();
				}
			}
		});
		traceBgColorMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				askForTraceBgColor();
			}
		});
		lookAndFeelMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				askForLookAndFeel();
			}
		});
		newWindowMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrameEnsemble.getInstance().add();
			}
		});
		renameWindowMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrame cf = (ContentFrame) desktopPane.getSelectedFrame();
				if (cf != null) {
					cf.renameWindow();
				}
			}
		});
		selectNextWindowMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrameEnsemble.getInstance().selectNext();
			}
		});
		clearAllMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrameEnsemble.getInstance().clearAll();
			}
		});
		cascadeMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrameEnsemble.getInstance().cascade();
			}
		});
		tileMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrameEnsemble.getInstance().tile();
			}
		});
		hideAllFiltersMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrameEnsemble.getInstance().hideFilters();
			}
		});
		showAllFiltersMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ContentFrameEnsemble.getInstance().showFilters();
			}
		});
		closeAllMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				int n = ContentFrameEnsemble.getInstance().getNonZombieCount();
				if (n > 0 && JOptionPane.showConfirmDialog(desktopPane, Strings.getString("reallyCloseAll"),
						null, JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
					ContentFrameEnsemble.getInstance().closeAll();
				}
			}
		});
		memUsageMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				showMemUsageDialog();
			}
		});
		aboutMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				showAboutDialog();
			}
		});

		updateLogModeButtons();
	}
	
	private void showMemUsageDialog() {
		Runtime rt = Runtime.getRuntime();
		long heap = rt.totalMemory();
		long used = heap - rt.freeMemory();
		long maxi = rt.maxMemory();
		JOptionPane.showMessageDialog(this,
				toMBStr(used) + " used\n" +
				toMBStr(heap) + " heap\n" +
				toMBStr(maxi) + " limit\n" +
				"Requesting garbage collection...",
				Strings.getString("memUsageMI"), JOptionPane.INFORMATION_MESSAGE);
		System.gc();
	}
	
	private void updateLogModeButtons() {
		App.LogMode mode = App.getLogMode();
		playButton.setSelected(mode == App.LogMode.PLAY);
		playButton.setEnabled(mode != App.LogMode.PLAY);
		pauseButton.setSelected(mode == App.LogMode.PAUSE);
		pauseButton.setEnabled(mode != App.LogMode.PAUSE);
		stopButton.setSelected(mode == App.LogMode.STOP);
		stopButton.setEnabled(mode != App.LogMode.STOP);
	}

	private TablePopupMouseListener modIdPopupListener = new TablePopupMouseListener(modIdTable, modIdTablePopupMenu) {
		@Override
		protected void mouseDoubleClickedOnRow(int row) {
			editIndexedModId(modIdTable.convertRowIndexToModel(row), modIdPanel);
		}
	};

	private TablePopupMouseListener progPopupListener = new TablePopupMouseListener(progTable, progTablePopupMenu) {
		@Override
		protected void mouseDoubleClickedOnRow(int row) {
			editIndexedProg(progTable.convertRowIndexToModel(row), progPanel);
		}
	};

	public TracesFilter getFilter() {
		return filter;
	}

	private void setFilter(TracesFilter filter) {
		this.filter = filter;
		((ModIdTableModel)modIdTable.getModel()).setFilter(filter);
		((ProgTableModel)progTable.getModel()).setFilter(filter);
	}

	public void setFilterAndUpdateContentFrameFilters(TracesFilter filter) {
		setFilter(filter);
		ContentFrameEnsemble.getInstance().updateFilters();
	}

	// Keystrokes and action ids for our key bindings.
	private static final KeyStroke deleteKeyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0);
	private static final String DELETE_ACTION_ID = "delete_action";
	private static final KeyStroke enterKeyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0);
	private static final String ENTER_ACTION_ID = "enter_action";

	private void defineKeyBindings() {
		// The delete key in the modid table deletes selected entries.
		modIdTable.getInputMap(JComponent.WHEN_FOCUSED).put(deleteKeyStroke, DELETE_ACTION_ID);
		modIdTable.getActionMap().put(DELETE_ACTION_ID, new AbstractAction() {
			private static final long serialVersionUID = 1L;
			public void actionPerformed(ActionEvent e) {
				deleteSelectedModIds();
			}
		});
		// The delete key in the prog table deletes selected entries.
		progTable.getInputMap(JComponent.WHEN_FOCUSED).put(deleteKeyStroke, DELETE_ACTION_ID);
		progTable.getActionMap().put(DELETE_ACTION_ID, new AbstractAction() {
			private static final long serialVersionUID = 1L;
			public void actionPerformed(ActionEvent e) {
				deleteSelectedProgs();
			}
		});
		// The enter key in the modid table edits selected entries.
		modIdTable.getInputMap(JComponent.WHEN_FOCUSED).put(enterKeyStroke, ENTER_ACTION_ID);
		modIdTable.getActionMap().put(ENTER_ACTION_ID, new AbstractAction() {
			private static final long serialVersionUID = 1L;
			public void actionPerformed(ActionEvent e) {
				editSelectedModIds(modIdPanel);
			}
		});
		// The enter key in the prog table edits selected entries.
		progTable.getInputMap(JComponent.WHEN_FOCUSED).put(enterKeyStroke, ENTER_ACTION_ID);
		progTable.getActionMap().put(ENTER_ACTION_ID, new AbstractAction() {
			private static final long serialVersionUID = 1L;
			public void actionPerformed(ActionEvent e) {
				editSelectedProgs(progPanel);
			}
		});
	}

	public void stateChanged(ChangeEvent e) {
		if (e.getSource() == windowMenu && windowMenu.isSelected()) {
			constructWindowSelectionSubmenu();
		}
	}

	private void constructWindowSelectionSubmenu() {
		selectWindowMenu.removeAll();
		String[] names = ContentFrameEnsemble.getInstance().getNames();
		for (int i=0; i < names.length; i++) {
			JMenuItem menuItem = new JMenuItem(names[i]);
			selectWindowMenu.add(menuItem);
			menuItem.addActionListener(windowSelectionSubmenuItemListener);
		}
	}

	private ActionListener windowSelectionSubmenuItemListener = new ActionListener() {
		public void actionPerformed(ActionEvent e) {
			JMenuItem menuItem = (JMenuItem) e.getSource();
			ContentFrameEnsemble.getInstance().selectByName(menuItem.getText());
		}
	};

	// update view menu
	private void updateViewMenu() {
		App.Prefs prefs = App.getPrefs();
		getShowTimestampMI().setSelected(prefs.showTimestamp);
		getShowProcessIdMI().setSelected(prefs.showProcessId);
		getShowThreadIdMI().setSelected(prefs.showThreadId);
		getShowModuleIdMI().setSelected(prefs.showModuleId);
		getShowBinaryMI().setSelected(prefs.showBinaryTraces);
		getShowFilterNameMI().setSelected(prefs.showFilterName);
		getShowWindowPerProcessMI().setSelected(prefs.perThreadWindow);
	}

	// update view toolbar
	private void updateViewToolbar() {
		App.Prefs prefs = App.getPrefs();
		getShowTimestampBttn().setSelected(prefs.showTimestamp);
		getShowProcessIdBttn().setSelected(prefs.showProcessId);
		getShowThreadIdBttn().setSelected(prefs.showThreadId);
		getShowModuleIdBttn().setSelected(prefs.showModuleId);
		getShowBinaryBttn().setSelected(prefs.showBinaryTraces);
		getShowFilterNameBttn().setSelected(prefs.showFilterName);
	}

	public void updateStatusBarWindowInfo() {
		ContentFrame frm = (ContentFrame) desktopPane.getSelectedFrame();
		String s = "";
		if (frm != null) {
			int totalLines = frm.textArea.getLineCount();
			int currentLine = frm.textArea.getCurrentLine();
			s = "Line " + currentLine + " / " + totalLines + " in " + frm.getTitle();
		}
		windowInfoLabel.setText(s);
	}

	public void updateStatusBarPendingTraces() {
		pendingTracesLabel.setText(App.getInstance().traceQueueSize() + " Pending Traces");
	}

	private void openSettings() {
		WindowSettingsDialog.doIt(App.getMainFrame(), (ContentFrame)desktopPane.getSelectedFrame());
	}

	private void importLog() {
		FilePicker fp = new FilePicker(Strings.getString("importFile"), ".", "*.log", false);
		String file = fp.getFile();
		if (file == null) {
			return; // user canceled
		}
		
		String dir = fp.getDirectory();
		App app = App.getInstance();
		try {
			app.importLog(dir + file);
		} catch (IOException e) {
			JOptionPane.showMessageDialog(App.getMainFrame(), "Can't read " + dir + file, null, JOptionPane.ERROR_MESSAGE);
			return; // error
		}
	}

	private void importFilters() {
		App.Prefs prefs = App.getPrefs();
		FilePicker fp = new FilePicker(Strings.getString("importSettings"), prefs.importPrefsDir, prefs.importPrefsFile, false);
		String file = fp.getFile();
		if (file == null) {
			return; // user canceled
		}
		String dir = fp.getDirectory();
		prefs.importPrefsDir = dir;
		prefs.importPrefsFile = file;
		App app = App.getInstance();
		try {
			app.importGlobalFilters(dir + file);
		} catch (IOException e) {
			JOptionPane.showMessageDialog(App.getMainFrame(), "Can't read " + dir + file, null, JOptionPane.ERROR_MESSAGE);
			return; // error
		}
		setFilterAndUpdateContentFrameFilters(app.getGlobalFilters());
	}

	private void exportFilters() {
		App.Prefs prefs = App.getPrefs();
		FilePicker fp = new FilePicker(Strings.getString("exportSettings"), prefs.importPrefsDir, prefs.importPrefsFile, true);
		String file = fp.getFile();
		if (file == null) {
			return; // user canceled
		}
		String dir = fp.getDirectory();
		prefs.importPrefsDir = dir;
		prefs.importPrefsFile = file;
		App app = App.getInstance();
		app.saveFilterPrefs();
		try {
			app.exportGlobalFilters(dir + file);
		} catch (IOException e) {
			JOptionPane.showMessageDialog(App.getMainFrame(), "Can't write " + dir + file, null, JOptionPane.ERROR_MESSAGE);
		}
	}

	private void saveCurrentContentFrame() {
		ContentFrame cf = (ContentFrame) desktopPane.getSelectedFrame();
		if (cf != null) {
			cf.save();
		}
	}

	ModuleFilter newModId(Component where) {
		ModuleFilter mf = ModIdDefinitionDialog.doIt(this, where);
		if (mf != null) {
			filter.addModuleIdFilter(mf);
			setFilterAndUpdateContentFrameFilters(filter);
		}
		return mf;
	}

	private void editSelectedModIds(Component where) {
		LinkedList<String> names = new LinkedList<String>();
		ListSelectionModel lsm = modIdTable.getSelectionModel();
		int rowCount = modIdTable.getRowCount();
		for (int iView = 0; iView < rowCount; iView++) {
			if (lsm.isSelectedIndex(iView)) {
				int iModel = modIdTable.convertRowIndexToModel(iView);
				names.addLast((String)modIdTable.getModel().getValueAt(iModel, ModIdTableModel._NameColumn));
			}
		}
		editNamedModIds(names, where);
	}

	void editNamedModIds(LinkedList<String> names, Component where) {
		for (String name : names) {
			editNamedModId(name, where);
		}
	}

	private void editNamedModId(String name, Component where) {
		editIndexedModId(getModIdByName(name), where);
	}

	private void editIndexedModId(int iModel, Component where) {
		if (iModel >= 0) {
			ModuleFilter mf = ModIdDefinitionDialog.doIt(this, where, filter.getModuleIdFilter(iModel));
			if (mf != null) {
				String oldName = (String)modIdTable.getModel().getValueAt(iModel, ModIdTableModel._NameColumn);
				String newName = mf.getName();
				System.out.format("new filter def = %s 0x%08x 0x%08x 0x%08x\n", newName, mf.getModMask(), mf.getModId(), mf.getColor().getRGB());
				if (!oldName.equals(newName)) {
					ContentFrameEnsemble.getInstance().changeModIdName(oldName, newName);
				}
				filter.setModuleIdFilter(iModel, mf);
				setFilterAndUpdateContentFrameFilters(filter);
			}
		}
	}

	// Return the index of the entry in the module table that has the given name,
	// or -1 if not found.
	int getModIdByName(String name) {
		ModIdTableModel model = (ModIdTableModel) modIdTable.getModel();
		int rowCount = model.getRowCount();
		for (int iModel = 0; iModel < rowCount; iModel++) {
			if (name.equals(model.getValueAt(iModel, ModIdTableModel._NameColumn))) {
				return iModel;
			}
		}
		return -1;
	}

	private void copySelectedModIdsToWindow() {
		ListSelectionModel lsm = modIdTable.getSelectionModel();
		ContentFrame contentFrame = (ContentFrame) desktopPane.getSelectedFrame();
		if (contentFrame != null) {
			ArrayList<String> newNames = new ArrayList<String>();
			int rowCount = modIdTable.getRowCount();
			for (int iView = 0; iView < rowCount; iView++) {
				if (lsm.isSelectedIndex(iView)) {
					int iModel = modIdTable.convertRowIndexToModel(iView);
					newNames.add(filter.getModuleIdFilter(iModel).getName());
				}
			}
			contentFrame.addModIdNames(newNames.toArray(new String[0]));
		}
	}

	private void deleteSelectedModIds() {
		unselectInUseModIdsUserWantsToKeep();
		ListSelectionModel lsm = modIdTable.getSelectionModel();
		int nDeleted = 0;
		int nRows = modIdTable.getRowCount();
		for (int iModel = nRows-1; iModel >= 0; iModel--) {
			int iView = modIdTable.convertRowIndexToView(iModel);
			if (lsm.isSelectedIndex(iView)) {
				filter.removeModuleIdFilter(iModel);
				nDeleted++;
			}
		}
		if (nDeleted > 0) {
			setFilterAndUpdateContentFrameFilters(filter);
		}
	}

	// For every selected modid that is used in one or more windows, ask the
	// user if she really wants to delete it, and if she say "no", unselect it.
	private void unselectInUseModIdsUserWantsToKeep() {
		ListSelectionModel lsm = modIdTable.getSelectionModel();
		int nRows = modIdTable.getRowCount();
		for (int iView = 0; iView < nRows; iView++) {
			if (lsm.isSelectedIndex(iView)) {
				int iModel = modIdTable.convertRowIndexToModel(iView);
				String name = (String)modIdTable.getModel().getValueAt(iModel, ModIdTableModel._NameColumn);
				if (ContentFrameEnsemble.getInstance().getModIdUseCount(name) > 0 &&!userReallyWantsToDeleteIt(name, modIdPanel)) {
					lsm.removeSelectionInterval(iView, iView);
				}
			}
		}
	}

	private boolean userReallyWantsToDeleteIt(String name, Component where) {
		return JOptionPane.showConfirmDialog(where,
				"\"" + name + "\"" + " is used in one or more windows!\nDo you really want to delete it?",
				null, JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION;
	}

	ProgFilter newProg(Component where) {
		ProgFilter pf = ProgDefinitionDialog.doIt(this, where);
		if (pf != null) {
			filter.addProgFilter(pf);
			setFilterAndUpdateContentFrameFilters(filter);
		}
		return pf;
	}

	private void editSelectedProgs(Component where) {
		LinkedList<String> names = new LinkedList<String>();
		ListSelectionModel lsm = progTable.getSelectionModel();
		int rowCount = progTable.getRowCount();
		for (int iView = 0; iView < rowCount; iView++) {
			if (lsm.isSelectedIndex(iView)) {
				int iModel = progTable.convertRowIndexToModel(iView);
				names.addLast((String)progTable.getModel().getValueAt(iModel, ProgTableModel._NameColumn));
			}
		}
		editNamedProgs(names, where);
	}

	void editNamedProgs(LinkedList<String> names, Component where) {
		for (String name : names) {
			editNamedProg(name, where);
		}
	}

	private void editNamedProg(String name, Component where) {
		editIndexedProg(getProgByName(name), where);
	}

	private void editIndexedProg(int iModel, Component where) {
		if (iModel >= 0) {
			ProgFilter pf = ProgDefinitionDialog.doIt(this, where, filter.getProgFilter(iModel));
			if (pf != null) {
				String oldName = (String)progTable.getModel().getValueAt(iModel, ProgTableModel._NameColumn);
				String newName = pf.getName();
				System.out.format("new prog def = %s 0x%08x\n", newName, pf.getColor().getRGB());
				if (!oldName.equals(newName)) {
					ContentFrameEnsemble.getInstance().changeProgName(oldName, newName);
				}
				filter.setProgFilter(iModel, pf);
				setFilterAndUpdateContentFrameFilters(filter);
			}
		}
	}

	// Return the index of the entry in the global prog table that has the given name,
	// or -1 if not found.
	int getProgByName(String name) {
		ProgTableModel model = (ProgTableModel) App.getMainFrame().progTable.getModel();
		int rowCount = model.getRowCount();
		for (int row = 0; row < rowCount; row++) {
			if (name.equals(model.getValueAt(row, ProgTableModel._NameColumn))) {
				return row;
			}
		}
		return -1;
	}

	private void copySelectedProgsToWindow() {
		ListSelectionModel lsm = progTable.getSelectionModel();
		ContentFrame contentFrame = (ContentFrame) desktopPane.getSelectedFrame();
		if (contentFrame != null) {
			ArrayList<String> newNames = new ArrayList<String>();
			int rowCount = progTable.getRowCount();
			for (int iView = 0; iView < rowCount; iView++) {
				if (lsm.isSelectedIndex(iView)) {
					int iModel = progTable.convertRowIndexToModel(iView);
					newNames.add(filter.getProgFilter(iModel).getName());
				}
			}
			contentFrame.addProgNames(newNames.toArray(new String[0]));
		}
	}

	private void deleteSelectedProgs() {
		unselectInUseProgsUserWantsToKeep();
		ListSelectionModel lsm = progTable.getSelectionModel();
		int nDeleted = 0;
		int nRows = progTable.getRowCount();
		for (int iModel = nRows-1; iModel >= 0; iModel--) {
			int iView = progTable.convertRowIndexToView(iModel);
			if (lsm.isSelectedIndex(iView)) {
				filter.removeProgFilter(iModel);
				nDeleted++;
			}
		}
		if (nDeleted > 0) {
			setFilterAndUpdateContentFrameFilters(filter);
		}
	}

	// For every selected prog that is used in one or more windows, ask the
	// user if she really wants to delete it, and if she say "no", unselect it.
	private void unselectInUseProgsUserWantsToKeep() {
		ListSelectionModel lsm = progTable.getSelectionModel();
		int nRows = progTable.getRowCount();
		for (int iView = 0; iView < nRows; iView++) {
			if (lsm.isSelectedIndex(iView)) {
				int iModel = progTable.convertRowIndexToModel(iView);
				String name = (String)progTable.getModel().getValueAt(iModel, ProgTableModel._NameColumn);
				if (ContentFrameEnsemble.getInstance().getProgUseCount(name) > 0 &&!userReallyWantsToDeleteIt(name, progPanel)) {
					lsm.removeSelectionInterval(iView, iView);
				}
			}
		}
	}

	// If there is a Prog filter with this name, update the pid
	void updateProgFilterPid(String name, int pid) {
		ProgFilter pf = filter.getProgFilterByName(name);
		if (pf == null)
			return;
		pf.setPid(pid);
		((ProgTableModel)progTable.getModel()).fireTableDataChanged();
	}

	private void askForTraceBgColor() {
		App.Prefs prefs = App.getPrefs();
		Color color = ARGBColorPicker.doIt(this, new Color(prefs.textAreaBgColor));
		if (color != null) {
			prefs.textAreaBgColor = color.getRGB();
			ContentFrameEnsemble.getInstance().setTextAreaBgColor(color);
		}
	}
	
	private void askForLookAndFeel() {
		App.Prefs prefs = App.getPrefs();
		LookAndFeelComboBox lnfcb = new LookAndFeelComboBox();
		int resp = JOptionPane.showConfirmDialog(this, lnfcb, Strings.getString("lookAndFeel"), JOptionPane.OK_CANCEL_OPTION);
		String newLnfClassName = lnfcb.getSelectedLookAndFeelClassName();
		if (resp == JOptionPane.OK_OPTION && newLnfClassName != null) {
			prefs.lookAndFeel = newLnfClassName;
			// Save prefs now since switching to gtk lnf sometimes crashes...
			prefs.save();
			lnfcb.changeLookAndFeel();
			SwingUtilities.updateComponentTreeUI(this);
		}
	}


	private static String toMBStr(long l) {
		int i = (int) ((l + 524288L) >> 20);
		return Integer.toString(i) + " MB";
	}

	private void showAboutDialog() {
		JOptionPane.showMessageDialog(this, Strings.getString("aboutMessage"),
				Strings.getString("aboutMI"), JOptionPane.INFORMATION_MESSAGE);
	}

}
