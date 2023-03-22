package reporter;

import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.FocusTraversalPolicy;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.prefs.Preferences;

import javax.swing.AbstractAction;
import javax.swing.InternalFrameFocusTraversalPolicy;
import javax.swing.JComponent;
import javax.swing.JInternalFrame;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import javax.swing.ListSelectionModel;
import javax.swing.event.CaretEvent;
import javax.swing.event.CaretListener;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.InternalFrameAdapter;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameListener;

import reporter.App.Prefs;
import tracesListener.ByteUtil;
import tracesListener.DataThread;
import tracesListener.ModuleFilter;
import tracesListener.ProgFilter;
import tracesListener.TraceMessage;
import tracesListener.TracesFilter;

public class ContentFrame extends ContentFrameVE {
	private static final long serialVersionUID = 1L;
	private static final int defaultWidth = 640;
	private static final int defaultHeight = 480;
	private static final int defaultDividerLocation = 180;
	private static final int defaultDivider1Location = 200;
	private static final String separatorString =
		"-------------------------------------------------------------------------------\n";
	// This window's conglomerate filter.
	// There are references to this from the models of both the ModIdTable and the ProgIdTable.
	private TracesFilter filter = new TracesFilter();
	// The names of the module id filters that apply to this window.
	private ArrayList<String> modIdNames = new ArrayList<String>();
	// The names of the program filters that apply to this window.
	private ArrayList<String> progNames = new ArrayList<String>();
	// Are we at the start of a new line of trace output?
	private boolean atNewLine = true;
	// The buffer used to build the string representation of one trace message.
	private StringBuilder formattedTraceMessage = new StringBuilder(500);
	// Has this window been destroyed (but not yet removed from our list)?
	boolean isZombie = false;
	private PopupMouseListener textAreaPopupListener = new PopupMouseListener(textAreaPopupMenu);
	private FocusTraversalPolicy normalFTP = null;
	// The buffered messages waiting to be displayed.
	LinkedList<ColorStringList> bufferedMessages = new LinkedList<ColorStringList>();
	// If This content frame is associated to a thread, only take this thread's messages.
	DataThread dataThread = null;
	private boolean isThreadStopped = false;
	// For hiding/showing the the filters in this window.
	private boolean haveGhostDividerLocation = false;
	private int ghostDividerLocation = 0;
	private static final int minVisibleDividerLocation = 20; // anything smaller considered hidden

	/**
	 * This is the ContentFrame default constructor
	 */
	public ContentFrame() {
		super();
		textArea.setBackground(new Color(App.getPrefs().textAreaBgColor));
		textArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
		newModIdPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				newModId();
			};
		});
		editModIdPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				editSelectedModIds();
			};
		});
		deleteModIdPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				deleteSelectedModIds();
			};
		});
		newProgPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				newProg();
			};
		});
		editProgPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				editSelectedProgs();
			};
		});
		deleteProgPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				deleteSelectedProgs();
			};
		});
		clearWindowPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				clear();
			};
		});
		renameWindowPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				renameWindow();
			};
		});
		hideFiltersPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				hideFilters();
			};
		});
		showFiltersPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				showFilters();
			};
		});
		insertSeparatorPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				insertSeparator();
			};
		});
		findPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				assert ((ContentFrame) e.getSource()) == ContentFrame.this;
				FindDialog.doIt(App.getMainFrame(), ContentFrame.this);
			};
		});
		gotoPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				goToLine();
			};
		});
		selectAllPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				textArea.selectAll();
			};
		});
		copyTextPUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				textArea.copy();
			};
		});
		savePUMI.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				save();
			};
		});;
		modIdPanel.addMouseListener(modIdPopupListener);
		modIdScrollPane.addMouseListener(modIdPopupListener);
		modIdTable.addMouseListener(modIdPopupListener);
		progPanel.addMouseListener(progPopupListener);
		progScrollPane.addMouseListener(progPopupListener);
		progTable.addMouseListener(progPopupListener);
		textArea.addMouseListener(textAreaPopupListener);
		addInternalFrameListener(internalFrameListener);
		setTransferHandler(new ModIdTableTransferHandler(this));
		progPanel.setTransferHandler(new ProgTableTransferHandler(this));
		setCustomFocusTraversalPolicy(this);
		defineKeyBindings();
		setFilter(new TracesFilter());
	}

	// We want the text area to initially have the focus.
	// So we have to install a custom "focus traversal policy" for internal frames.
	private void setCustomFocusTraversalPolicy(ContentFrame contentFrame) {
		normalFTP = contentFrame.getFocusTraversalPolicy();
		contentFrame.setFocusTraversalPolicy(new InternalFrameFocusTraversalPolicy() {
			public Component getInitialComponent(JInternalFrame frame) {
				return textArea;
			}
			public Component getDefaultComponent(Container container) {
				return textArea;
			}
			public Component getFirstComponent(Container container) {
				return normalFTP.getFirstComponent(container);
			}
			public Component getLastComponent(Container container) {
				return normalFTP.getLastComponent(container);
			}
			public Component getComponentAfter(Container container, Component component) {
				return normalFTP.getComponentAfter(container, component);
			}
			public Component getComponentBefore(Container container, Component component) {
				return normalFTP.getComponentBefore(container, component);
			}
		});
	}

	private InternalFrameListener internalFrameListener = new InternalFrameAdapter() {
		public void internalFrameClosed(InternalFrameEvent e) {
			// If were to immediately remove this content frame from the list within
			// ContentFrameEnsemble, we could get ConcurrentModificationExceptions because
			// we might be inside a loop that is modifying the list for some other reason.
			// So we just mark this content frame as a zombie here, and let
			// ContentFrameEnsemble.zombieRemover.run() remove it later.
			assert ((ContentFrame) e.getSource()) == ContentFrame.this;
			isZombie = true;
		}

		public void internalFrameActivated(InternalFrameEvent e) {
			assert ((ContentFrame) e.getSource()) == ContentFrame.this;
			// The caret info for this content frame may be stale.
			// The window info in the status bar must be updated.
			// Further updates of the caret info and of the status bar window info
			// Will be done upon notification of document events and caret events.
			textArea.caretUpdate();
			App.getMainFrame().updateStatusBarWindowInfo();
			textArea.addDocumentListener(documentListener);
			textArea.addCaretListener(caretListener);
			// If a find dialog is running, it needs to know that a different c.f. is now active.
			FindDialog.onContentFrameActivated(ContentFrame.this);
		}

		public void internalFrameDeactivated(InternalFrameEvent e) {
			assert ((ContentFrame) e.getSource()) == ContentFrame.this;
			// We can get by without document and caret events from inactive content frames.
			textArea.removeDocumentListener(documentListener);
			textArea.removeCaretListener(caretListener);
		}
	};

	// Note: we are only registered for these events while this content frame is active.
	private DocumentListener documentListener = new DocumentListener() {
		public void changedUpdate(DocumentEvent e) {
		}

		public void insertUpdate(DocumentEvent e) {
			assert isSelected();
			App.getMainFrame().updateStatusBarWindowInfo();
		}

		public void removeUpdate(DocumentEvent e) {
			assert isSelected();
			App.getMainFrame().updateStatusBarWindowInfo();
		}
	};

	// Note: we are only registered for these events while this content frame is active.
	private CaretListener caretListener = new CaretListener() {
		public void caretUpdate(CaretEvent e) {
			assert isSelected();
			textArea.caretUpdate();
			App.getMainFrame().updateStatusBarWindowInfo();
		}
	};

	private TablePopupMouseListener modIdPopupListener = new TablePopupMouseListener(modIdTable, modIdTablePopupMenu) {
		@Override
		protected void mouseDoubleClickedOnRow(int row) {
			// assertion: the given row is the only one selected
			editSelectedModIds();
		}
	};

	private TablePopupMouseListener progPopupListener = new TablePopupMouseListener(progTable, progTablePopupMenu) {
		@Override
		protected void mouseDoubleClickedOnRow(int row) {
			// assertion: the given row is the only one selected
			editSelectedProgs();
		}
	};

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
				editSelectedModIds();
			}
		});
		// The enter key in the prog table edits selected entries.
		progTable.getInputMap(JComponent.WHEN_FOCUSED).put(enterKeyStroke, ENTER_ACTION_ID);
		progTable.getActionMap().put(ENTER_ACTION_ID, new AbstractAction() {
			private static final long serialVersionUID = 1L;
			public void actionPerformed(ActionEvent e) {
				editSelectedProgs();
			}
		});
	}

	// Prompt the user to create a new module id filter in the palette,
	// then add the new module id filter to this window,
	// and re-create the conglomerate filter.
	private void newModId() {
		ModuleFilter mf = App.getMainFrame().newModId(modIdPanel);
		if (mf != null) {
			addModIdName(mf.getName());
		}
	}

	// Using the module id filters that are selected in this window,
	// prompt the user to edit those module id filters in the palette.
	// (That will propagate the changes to the module id filters in this window
	// and any other windows that have those module id filters.)
	private void editSelectedModIds() {
		LinkedList<String> names = new LinkedList<String>();
		ListSelectionModel lsm = modIdTable.getSelectionModel();
		int rowCount = modIdTable.getRowCount();
		for (int iView = 0; iView < rowCount; iView++) {
			if (lsm.isSelectedIndex(iView)) {
				int iModel = modIdTable.convertRowIndexToModel(iView);
				names.addLast(filter.getModuleIdFilter(iModel).getName());
			}
		}
		App.getMainFrame().editNamedModIds(names, modIdPanel);
	}

	// Delete all selected module id filters in this window,
	// and re-create the conglomerate filter.
	private void deleteSelectedModIds() {
		ListSelectionModel lsm = modIdTable.getSelectionModel();
		int rowCount = modIdTable.getRowCount();
		for (int iModel = rowCount-1; iModel >= 0; iModel--) {
			int iView = modIdTable.convertRowIndexToView(iModel);
			if (lsm.isSelectedIndex(iView)) {
				modIdNames.remove(filter.getModuleIdFilter(iModel).getName());
			}
		}
		updateFilter();
	}

	// Prompt the user to create a new program filter in the palette,
	// then add the new program filter to this window,
	// and re-create the conglomerate filter.
	private void newProg() {
		ProgFilter pf = App.getMainFrame().newProg(progPanel);
		if (pf != null) {
			addProgName(pf.getName());
		}
	}

	// Using the program filters that are selected in this window,
	// prompt the user to edit those program filters in the palette.
	// (That will propagate the changes to the program filters in this window
	// and any other windows that have those program filters.)
	private void editSelectedProgs() {
		LinkedList<String> names = new LinkedList<String>();
		ListSelectionModel lsm = progTable.getSelectionModel();
		int rowCount = progTable.getRowCount();
		for (int iView = 0; iView < rowCount; iView++) {
			if (lsm.isSelectedIndex(iView)) {
				int iModel = progTable.convertRowIndexToModel(iView);
				names.addLast(filter.getProgFilter(iModel).getName());
			}
		}
		App.getMainFrame().editNamedProgs(names, progPanel);
	}

	// Delete all selected program filters in this window,
	// and re-create the conglomerate filter.
	private void deleteSelectedProgs() {
		ListSelectionModel lsm = progTable.getSelectionModel();
		int rowCount = progTable.getRowCount();
		for (int iModel = rowCount-1; iModel >= 0; iModel--) {
			int iView = progTable.convertRowIndexToView(iModel);
			if (lsm.isSelectedIndex(iView)) {
				progNames.remove(filter.getProgFilter(iModel).getName());
			}
		}
		updateFilter();
	}

	void renameWindow() {
		String name = WindowNameDialog.doIt(App.getMainFrame(), this, this, getTitle());
		if (name != null) {
			setTitle(name);
		}
	}

	void clear() {
		textArea.clear();
		atNewLine = true;
	}

	void insertSeparator() {
		if (!atNewLine) {
			textArea.append("\n");
		}
		textArea.append(separatorString);
		atNewLine = true;
	}

	void goToLine() {
		String s = LineNumberDialog.doIt(App.getMainFrame(), this.textScrollPane, this);
		int n = s.length() == 0 ? 0 : Integer.parseInt(s);
		textArea.goToLine(n);
	}

	void save() {
		App.Prefs prefs = App.getPrefs();
		FilePicker fp = new FilePicker(Strings.getString("saveWindow"), prefs.saveWindowDir, prefs.saveWindowFile, "txt", true);
		String file = fp.getFile();
		if (file == null) {
			return; // user canceled
		}
		String dir = fp.getDirectory();
		prefs.saveWindowDir = dir;
		prefs.saveWindowFile = file;
		String fullPath = dir + file;
		try {
			textArea.write(new OutputStreamWriter(new FileOutputStream(fullPath)));
		} catch (IOException e) {
			System.err.println("Can't save '" + fullPath + "' : " + e);
			JOptionPane.showMessageDialog(App.getMainFrame(), "Can't write " + fullPath, null, JOptionPane.ERROR_MESSAGE);
		}
	}

	void hideFilters() {
		int dividerLocation = jSplitPane.getDividerLocation();
		if (dividerLocation >= minVisibleDividerLocation) {
			haveGhostDividerLocation = true;
			ghostDividerLocation = dividerLocation;
		}
		jSplitPane.setDividerLocation(0);
	}

	void showFilters() {
		int dividerLocation = jSplitPane.getDividerLocation();
		if (dividerLocation < minVisibleDividerLocation) {
			if (haveGhostDividerLocation) {
				jSplitPane.setDividerLocation(ghostDividerLocation);
				haveGhostDividerLocation = false;
			} else {
				jSplitPane.setDividerLocation(0.25);
			}
		}
	}

	// If there is a Prog filter with this name, update the pid
	void updateProgFilterPid(String name, int pid) {
		ProgFilter pf = filter.getProgFilterByName(name);
		if (pf == null || pf.getPid() == pid)
			return;
		pf.setPid(pid);
		((ProgTableModel)progTable.getModel()).fireTableDataChanged();
	}

	// Replace the old name with the new name in the list of names of module id filters.
	void changeModIdName(String oldName, String newName) {
		int i = modIdNames.indexOf(oldName);
		if (i >= 0) {
			System.out.format("Changing %s to %s\n", oldName, newName);
			modIdNames.set(i, newName);
		}
	}

	// Replace the old name with the new name in the list of program names
	// within the conglomerate filter.
	void changeProgName(String oldName, String newName) {
		int i = progNames.indexOf(oldName);
		if (i >= 0) {
			System.out.format("Changing %s to %s\n", oldName, newName);
			progNames.set(i, newName);
		}
	}

	boolean isModIdUsed(String name) {
		return modIdNames.indexOf(name) >= 0;
	}

	boolean isProgUsed(String name) {
		return progNames.indexOf(name) >= 0;
	}

	// Set the list of names of module id filters that apply to this window,
	// and re-create the conglomerate filter.
	void setModIdNames(String names[]) {
		modIdNames = new ArrayList<String>(names.length);
		for (String name : names) {
			modIdNames.add(name);
		}
		updateFilter();
	}

	// Set the list of program names that apply to this window,
	// and re-create the conglomerate filter.
	void setProgNames(String names[]) {
		progNames = new ArrayList<String>(names.length);
		for (String name : names) {
			progNames.add(name);
		}
		updateFilter();
	}

	// Add module id filters from the list except those already there,
	// and re-create the conglomerate filter.
	void addModIdNames(String names[]) {
		for (String name : names) {
			if (!modIdNames.contains(name)) {
				modIdNames.add(name);
			}
		}
		updateFilter();
	}

	// Add the given module id filter unless it's already there,
	// and re-create the conglomerate filter.
	private void addModIdName(String name) {
		if (!modIdNames.contains(name)) {
			modIdNames.add(name);
			updateFilter();
		}
	}

	// Add program filters from the list except those already there,
	// and re-create the conglomerate filter.
	void addProgNames(String names[]) {
		for (String name : names) {
			if (!progNames.contains(name))
				progNames.add(name);
		}
		updateFilter();
	}

	void addProgName(String name) {
		if (!progNames.contains(name)) {
			progNames.add(name);
			updateFilter();
		}
	}

	// This sets the conglomerate filter that applies to this window,
	// and updates the underlying table models appropriately.
	private void setFilter(TracesFilter filter) {
		this.filter = filter;
		((ModIdTableModel)modIdTable.getModel()).setFilter(filter);
		((ProgTableModel)progTable.getModel()).setFilter(filter);
	}

	// Re-create the conglomerate filter from list of module id filter names
	// and the global conglomerate filter.
	void updateFilter() {
		TracesFilter tFilter = new TracesFilter();
		TracesFilter gFilter = App.getMainFrame().getFilter();
		// delete modIdNames that are no longer in the global filters
		for (int i = modIdNames.size()-1; i >= 0; i--) {
			if (gFilter.getModuleIdFilterByName(modIdNames.get(i)) == null) {
				modIdNames.remove(i);
			}
		}
		// delete progNames that are no longer in the global filters
		for (int i = progNames.size()-1; i >= 0; i--) {
			if (gFilter.getProgFilterByName(progNames.get(i)) == null) {
				progNames.remove(i);
			}
		}
		// Construct the new conglomerate filter for this window.
		for (int i=0; i<modIdNames.size(); i++) {
			tFilter.addModuleIdFilter(gFilter.getModuleIdFilterByName(modIdNames.get(i)));
			tFilter.sortModuleFiltersBySpecificity();
		}
		for (int i=0; i<progNames.size(); i++) {
			tFilter.addProgFilter(gFilter.getProgFilterByName(progNames.get(i)));
		}
		if (filter != null) {
			for (int i=0; i<filter.nProcessIdFilters(); i++) {
				tFilter.addProcessIdFilter(filter.getProcessIdFilter(i));
			}
		}
		setFilter(tFilter);
	}

	void loadPrefs(Preferences aNode) {
		setSize(aNode.getInt("width", defaultWidth), aNode.getInt("height", defaultHeight));
		setLocation(aNode.getInt("x", 0),aNode.getInt("y", 0));
		try {
			setMaximum(aNode.getBoolean("isMaximum", false));
		} catch (Exception e) {
			// tough luck...
		}
		jSplitPane.setDividerLocation(aNode.getInt("divider", defaultDividerLocation));
		jSplitPane1.setDividerLocation(aNode.getInt("divider1", defaultDivider1Location));
		haveGhostDividerLocation = aNode.getBoolean("haveGhostDivider", false);
		ghostDividerLocation = aNode.getInt("ghostDivider", 0);
		setTitle(aNode.get("title", getTitle()));

		int nMod = aNode.getInt("nModFilters", 0);
		modIdNames = new ArrayList<String>();
		while (modIdNames.size() < nMod) {
			modIdNames.add(aNode.get("mod" + modIdNames.size(), ""));
		}

		int nProg = aNode.getInt("nProgFilters", 0);
		progNames = new ArrayList<String>();
		while (progNames.size() < nProg) {
			progNames.add(aNode.get("prog" + progNames.size(), ""));
		}

		int defaultColumnWidth = jSplitPane.getDividerLocation() / modIdTable.getColumnCount();
		for (int i=0; i<modIdTable.getColumnCount(); i++) {
			String colName = modIdTable.getColumnName(i);
			modIdTable.getColumn(colName).setPreferredWidth(aNode.getInt(colName, defaultColumnWidth));
		}

		defaultColumnWidth = jSplitPane.getDividerLocation() / progTable.getColumnCount();
		for (int i=0; i<progTable.getColumnCount(); i++) {
			String colName = progTable.getColumnName(i);
			progTable.getColumn(colName).setPreferredWidth(aNode.getInt(colName, defaultColumnWidth));
		}

		getRegExpField().setText(aNode.get("regexp", ""));
	}

	void savePrefs(Preferences aNode) {
		Dimension mySize = getSize();
		aNode.putInt("x", getX());
		aNode.putInt("y", getY());
		aNode.putInt("height", mySize.height);
		aNode.putInt("width", mySize.width);
		aNode.putBoolean("isMaximum", isMaximum());
		aNode.put("title", this.getTitle());
		aNode.putInt("divider", jSplitPane.getDividerLocation());
		aNode.putInt("divider1", jSplitPane1.getDividerLocation());
		aNode.putBoolean("haveGhostDivider", haveGhostDividerLocation);
		aNode.putInt("ghostDivider", ghostDividerLocation);
		aNode.put("regexp", getRegExpField().getText());

		aNode.putInt("nModFilters", modIdNames.size());
		for (int i=0; i<modIdNames.size(); i++) {
			aNode.put("mod" + i, modIdNames.get(i));
		}

		for (int i=0; i<modIdTable.getColumnCount(); i++) {
			String colName = modIdTable.getColumnName(i);
			aNode.putInt(colName, modIdTable.getColumn(colName).getWidth());
		}

		aNode.putInt("nProgFilters", progNames.size());
		for (int i=0; i<progNames.size(); i++) {
			aNode.put("prog" + i, progNames.get(i));
		}

		for (int i=0; i<progTable.getColumnCount(); i++) {
			String colName = progTable.getColumnName(i);
			aNode.putInt(colName, progTable.getColumn(colName).getWidth());
		}
	}

	// If the data thread already stopped, mark its window as closed
	private void checkDataThreadStopped() {
		if (dataThread == null || dataThread.isRunning() || isThreadStopped)
			return;
		setTitle(getTitle() + " --- Closed ---");
		isThreadStopped = true;
	}

	// If the given message, which came from the given thread, matches this content frame, buffer it.
	// This gets called on the consumer thread.
	void bufferIfMatches(TraceMessage msg, DataThread dt) {
    	if (dataThread == null || dataThread == dt) {
    		if (filter.messageMatches(msg)) {
    			formatTraceMessage(msg);
    			String tms = formattedTraceMessage.toString();
    			if (bufferedMessages.isEmpty() || bufferedMessages.getLast().color != filter.matchColor) {
    				bufferedMessages.addLast(new ColorStringList(filter.matchColor));
    			}
    			bufferedMessages.getLast().addLast(tms);
				updateProgFilterPid(msg.appName, msg.processId);
    		}
    	}
	}

	// Display all buffered messages.
	// This gets called on the EDT thread.
	void displayBufferedMessages() {
		switch (App.getLogMode()) {
		case STOP:
			bufferedMessages.clear();
			break;

		case PAUSE:
			break;

		case PLAY:
			while (!bufferedMessages.isEmpty()) {
				ColorStringList csl = bufferedMessages.removeFirst();
				textArea.append(csl.toString(), csl.color);
			}
			break;
		}
		checkDataThreadStopped();
	}

	// Format the given trace message, leaving the result in formattedTraceMessage.
	// This gets called on the consumer thread.
	private void formatTraceMessage(TraceMessage msg) {
		formattedTraceMessage.setLength(0); // reset to empty string
		switch (msg.msgType) {
		case TraceMessage._Binary:
			if (App.getPrefs().showBinaryTraces) {
				appendPrefixString(msg);
				formattedTraceMessage.append(msg.getDataString());
				atNewLine = true;
			}
			break;
		case TraceMessage._Text:
			if (atNewLine) {
				appendPrefixString(msg);
			}
			formattedTraceMessage.append(getDataString(msg));
			atNewLine = (formattedTraceMessage.charAt(formattedTraceMessage.length() - 1) == '\n');
			break;
		case TraceMessage._TextLine:
			if (atNewLine) {
				appendPrefixString(msg);
			}
			formattedTraceMessage.append(getDataString(msg));
			formattedTraceMessage.append('\n');
			atNewLine = true;
			break;
		case TraceMessage._StartDump:
		case TraceMessage._EndDump:
			// don't log these markers
			break;
		default:
			System.err.println("Unknown trace message type: " + msg.msgType);
			break;
		}
	}

	// Append the string of optional stuff that precedes the actual trace data to formattedTraceMessage.
	// This gets called on the consumer thread.
	private void appendPrefixString(TraceMessage msg) {
		Prefs prefs = App.getPrefs();
		if (prefs.showTimestamp) {
			Timestamp ts = new Timestamp(msg.timeStamp);
			
			formattedTraceMessage.append(ts.toString());
			formattedTraceMessage.append(' ');
		}
		if (prefs.showProcessId) {
			formattedTraceMessage.append('p');
			formattedTraceMessage.append(ByteUtil.formatHexInteger(msg.processId));
			formattedTraceMessage.append(' ');
		}
		if (prefs.showThreadId) {
			formattedTraceMessage.append('t');
			formattedTraceMessage.append(ByteUtil.formatHexInteger(msg.threadId));
			formattedTraceMessage.append(' ');
		}
		if (prefs.showModuleId) {
			formattedTraceMessage.append('m');
			formattedTraceMessage.append(ByteUtil.formatHexInteger(msg.traceModule));
			formattedTraceMessage.append(' ');
		}
		if (prefs.showFilterName && filter.matchModFilter != null) {
			formattedTraceMessage.append('f');
			formattedTraceMessage.append(filter.matchModFilter.getName());
			formattedTraceMessage.append(' ');
		}
	}

	// Return msg.getDataString(), with all non-printing chars replaced by '?',
	// except for tabs (replaced by 4 blanks) and linefeeds (untouched).
	private String getDataString(TraceMessage msg) {
		String ds = msg.getDataString();
		int nTabs = 0;
		int nBads = 0;
		// First pass: count tabs and bad chars.
		for (int i=0; i < ds.length(); i++) {
			char c = ds.charAt(i);
			if (c < 0x20) {
				if (c == '\t') {
					nTabs++;
				} else if (c != '\n') {
					nBads++;
				}
			}  else if (c > 0x7E) {
				nBads++;
			}
		}
		if (nTabs + nBads == 0) {
			return ds;
		}
		// Second pass: replace tabs with blanks, bad chars with '?'.
		StringBuilder sb = new StringBuilder(ds.length() + 3*nTabs);
		for (int i=0; i < ds.length(); i++) {
			char c = ds.charAt(i);
			if (c < 0x20) {
				if (c == '\t') {
					sb.append("    ");
				} else if (c == '\n') {
					sb.append('\n');
				} else {
					sb.append('?');
				}
			} else if (c > 0x7E) {
				sb.append('?');
			} else {
				sb.append(c);
			}
		}
		return sb.toString();
	}

	// Convenience: caller doesn't have to try/catch.
	@Override
	public void setSelected(boolean b) {
		try {
			super.setSelected(b);
		} catch (java.beans.PropertyVetoException e) {
			System.err.println("Can't set selected: " + e);
		}
	}

	// A list of strings, all of the same color.
	private final class ColorStringList {
		public Color color;
		private LinkedList<String> strings = new LinkedList<String>();

		public ColorStringList(Color color) {
			this.color = color;
		}

		public void addLast(String s) {
			strings.addLast(s);
		}

		public String toString() {
			if (strings.size() == 1) {
				return strings.getFirst();
			}
			int len = 0;
			for (String s : strings) {
				len += s.length();
			}
			if (len == 0) {
				return "";
			}
			StringBuilder sb = new StringBuilder(len);
			for (String s : strings) {
				sb.append(s);
			}
			return sb.toString();
		}
	}

	public ArrayList<String> getModIdNames() {
		return modIdNames;
	}
}
