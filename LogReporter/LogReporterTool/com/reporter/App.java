package reporter;

import java.awt.Color;
import java.awt.Rectangle;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.util.LinkedList;
import java.util.Observable;
import java.util.Observer;
import java.util.concurrent.Semaphore;
import java.util.prefs.BackingStoreException;
import java.util.prefs.InvalidPreferencesFormatException;
import java.util.prefs.Preferences;

import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;
import javax.swing.WindowConstants;

import tracesListener.DataThread;
import tracesListener.LogDataThread;
import tracesListener.ProgFilter;
import tracesListener.TraceMessage;
import tracesListener.TracesDataThread;
import tracesListener.TracesFilter;
import tracesListener.TracesListenerServer;

@SuppressWarnings("deprecation")
public final class App {
	// Let the user pause/stop/(re)start the log
	static public enum LogMode {PLAY, PAUSE, STOP};
	private static LogMode logMode;

	// The singleton instance.
	private static App instance = null;
	
	// Preferences.
	private Prefs prefs = new Prefs();
	
	// The main JFrame.
	private MainFrame mainFrame = null;
	
	// This object notifies us of TracesDataThread creation/destruction,
	// and of the arrival of each TraceMessage. We tell it when to shut down.
	private TracesListenerServer tracesListenerServer = null;
	
	// The queue of trace messages waiting to be displayed.
	private LinkedList<TraceQueueItem> traceMessageQueue = new LinkedList<TraceQueueItem>();
	
	// The max number of trace messages we will consume in any one cycle.
	private final static int traceMessageQueueMaxBuffer = 500;
	
	// The consumer thread waits on this semaphore
	private Semaphore traceMessageAvailable = new Semaphore(0);
	
	// True between startup and shutdown
	private boolean running = false;
	
	// Used to pass the "traces data thread" from which the first message is coming
	// from the consumer thread to the EDT thread.
	private DataThread dataThreadWithAFirstMessage;


	private static String	PREFERRED_LOOK_AND_FEEL = "Nimbus";
	// Preferences file passed as argument, if any.
	private String prefsFile = null;

	static App getInstance() {
		return instance;
	}

	static Prefs getPrefs() {
		return instance.prefs;
	}

	static MainFrame getMainFrame() {
		return instance.mainFrame;
	}

	static void setLogMode(LogMode mode) {
		logMode = mode;
	}

	static LogMode getLogMode() {
		return logMode;
	}

	// App Phase 1 -- initialization prior to constructing the UI.
	// This gets called on the main (non-EDT) thread.
	private void initialize(String[] args) {
		System.out.println("initialize()");
		prefs.load();
		if (args.length == 1) {
			prefsFile = args[0];
		}
		AppUtil.printMemStats();
		AppUtil.printSysProps();
		logMode = LogMode.PLAY;
	}

	// App Phase 2 -- construction of the initial UI, execution of main loop.
	// This gets called on the EDT.
	private void startup() {
		System.out.println("startup()");
		running = true;
		setLookAndFeelWithFallback();
		createMainFrame();
		AppUtil.printMemStats();
		startListening();
	}

	// App Phase 3 -- clean up and exit.
	// This is normally called on the EDT.
	// But in a weird error condition it may be called on another thread.
	private void shutdown() {
		System.out.println("shutdown() - " + traceQueueSize() + " messages queued");
		running = false;
		// Save prefs.
		prefs.save();
		// Shut down the io stuff.
		if (tracesListenerServer != null) {
			tracesListenerServer.shutdown();
		}
		AppUtil.printMemStats();
		System.exit(0);
	}

	// Clean up and exit if it makes sense, user agrees, etc.
	void exit() {
		shutdown();
	}

	int traceQueueSize() {
		return traceMessageQueue.size();
	}

	TracesFilter getGlobalFilters() {
		return prefs.getGlobalFilters();
	}

	// Save all filter prefs -- main frame and all content frames.
	void saveFilterPrefs() {
		prefs.saveFilterPrefs();
		prefs.saveFramesPrefs();
	}

	void importLog(String fileLocation) throws IOException {
		try {
			FileInputStream logFileIn = new FileInputStream(fileLocation);
			LogDataThread logImporter = new LogDataThread(logFileIn);
			tracesListenerServer.addDataThread(logImporter);
			logImporter.start();
			
		} catch (IOException e) {
			System.err.println("Can't read log file: " + e);
			throw e;
		}
	}

	// Sets the new global filters in the preferences from the prefs file
	// Reloads the content frames preferences
	// Updates the content frames filters with App's getGlobalFilters
	void importGlobalFilters(String fileLocation) throws IOException {
		try {
			FileInputStream prefsFileIn = new FileInputStream(fileLocation);
			prefs.importGlobalFilters(prefsFileIn);
			prefsFileIn.close();
		} catch (IOException e) {
			System.err.println("Can't read preferences file: " + e);
			throw e;
		}
		prefs.loadFramesPrefs();
	}

	// Update the windows filters from the global filters
	void updateContentFrameFilters() {
		ContentFrameEnsemble contentFrameEnsemble = ContentFrameEnsemble.getInstance();
		contentFrameEnsemble.updateFilters();
	}
	
	// The content frames preferences are updated in fNode when setGlobalFilters is called
	void exportGlobalFilters(String fileLocation) throws IOException {
		try {
			FileOutputStream prefsFileOut = new FileOutputStream(fileLocation);
			prefs.exportGlobalFilters(prefsFileOut);
			prefsFileOut.close();
		} catch (IOException e) {
			System.err.println("Can't save preferences file: " + e);
			throw e;
		}
	}
	
	// Set the look-and-feel according to user preference.
	// If that fails, try reverting to the cross-platform look-and-feel (aka "metal").
	private void setLookAndFeelWithFallback() {
		if (!setLookAndFeel()) {
			prefs.lookAndFeel = UIManager.getCrossPlatformLookAndFeelClassName();
			setLookAndFeel();
		}
	}

	// Set the look-and-feel according to user preference.
	private boolean setLookAndFeel() {
		try {
			UIManager.setLookAndFeel(prefs.lookAndFeel);
			return true;
		} catch (Exception e) {
			System.err.println("Can't set look and feel " + prefs.lookAndFeel + ": " + e);
			return false;
		}
	}
	
	private void createMainFrame() {
		mainFrame = new MainFrame();
		mainFrame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		mainFrame.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
				mainFrame.setVisible(false);
				exit();
			}
		});
		mainFrame.setBounds(prefs.mainFramePosX, prefs.mainFramePosY, prefs.mainFrameSizeX, prefs.mainFrameSizeY);
		mainFrame.getDesktopPane().setBackground(new Color(prefs.mainFrameBgColor));
		mainFrame.setTitle(Strings.getString("Application.title"));
		if (prefsFile != null) {
			try {
				importGlobalFilters(prefsFile);
			} catch (IOException e) {
			}
		} else {
			prefs.loadFramesPrefs();
		}
		mainFrame.setFilterAndUpdateContentFrameFilters(getGlobalFilters());
		mainFrame.updateStatusBarPendingTraces();
		// We set the main frame visible AFTER constructing all the content frames,
		// since otherwise the main frame appears blank for a good while.
		mainFrame.setVisible(true);
		// We select a content frame AFTER setting the main frame visible, since
		// otherwise the selection operation has no effect.
		if (!ContentFrameEnsemble.getInstance().selectByName(prefs.selectedWindow)) {
			ContentFrameEnsemble.getInstance().selectFirst();
		}
	}

	private void startListening() {
		Thread consumerThread = new Thread(consumer, "Consumer");
		consumerThread.start();
		try {
			tracesListenerServer = new TracesListenerServer();
		} catch (IOException e) {
			System.err.println("Can't create server socket: " + e);
			displayErrorAndDie(Strings.getString("listenFailure"));
		}
		// The data threads are started by the server.
		tracesListenerServer.autoStartDataThreads = false;
		tracesListenerServer.addObserver(new Observer() {
			public void update(Observable source, Object o) {
				if (o instanceof TracesDataThread) {
					onDataThreadCreatedOrDestroyed((TracesDataThread) o);
				} else if (o instanceof Exception) {
					displayErrorAndDie((Exception) o);
				}
			}
		});
		tracesListenerServer.addTracesObserver(new Observer() {
			public void update(Observable source, Object o) {
				if (o instanceof TraceMessage) {
					onTraceMessageReceivedFrom((TraceMessage) o, (DataThread) source);
				} else if (o instanceof Exception) {
					displayErrorAndDie((Exception) o);
				}
			}
		});
		Thread serverThread = new Thread(tracesListenerServer, "Listener");
		serverThread.setDaemon(true);
		serverThread.start();
	}

	// A data thread has been created, or is about to be destroyed.
	// This gets called on some non-EDT thread.
	private void onDataThreadCreatedOrDestroyed(TracesDataThread dataThread) {
		if (!tracesListenerServer.dataThreadExited(dataThread)) {
			// Created.
			System.out.format("---> data thread %s\n", dataThread);
			dataThread.start();
		} else {
			// Destroyed.
			System.out.format("<--- data thread %s\n", dataThread);
		}
	}

	// Arrival of a new trace message; queue it.
	// This gets called by multiple non-EDT threads.
	private void onTraceMessageReceivedFrom(TraceMessage aTraceMessage, DataThread aDataThread) {
		TraceQueueItem item = new TraceQueueItem(aTraceMessage, aDataThread);
		synchronized (traceMessageQueue) {
			traceMessageQueue.addLast(item);
		}
		traceMessageAvailable.release();
	}

	private Runnable consumer = new Runnable() {
		public void run() {
			try {
				runInnards();
			} catch (RuntimeException e) {
				displayErrorAndDie(e);
			}
		}

		// Consume the queue of trace messages.
		private void runInnards() {
			while (running) {
				// Block until there is at least one queued message.
				// Acquire the rights to as many as possible up to a max of traceMessageQueueMaxBuffer.
				traceMessageAvailable.acquireUninterruptibly();
				int nMsg = Math.min(traceMessageQueueMaxBuffer, traceQueueSize());
				if (nMsg > 1) {
					traceMessageAvailable.acquireUninterruptibly(nMsg - 1);
				}
				// Remove the messages from the queue and buffer them to the content frames.
				bufferQueuedMessages(nMsg);
				// Have the EDT thread display the messages in the content frames.
				try {
					ContentFrameEnsemble.getInstance().displayBufferedMessages();
				} catch (InterruptedException e) {
					System.err.println("Consumer thread interrupted: " + e);
				}
			}
		}
	};

	// Remove nMsg messages from the queue, and dispatch them to the content frames.
	// This gets called on the consumer thread.
	private void bufferQueuedMessages(int nMsg) {
		// First remove the messages from the queue onto a separate list,
		// so as to minimize the time that we hold the lock on the queue.
		LinkedList<TraceQueueItem> bufferedMessageQueue = new LinkedList<TraceQueueItem>();
		synchronized (traceMessageQueue) {
			for (int i=0; i < nMsg; i++) {
				bufferedMessageQueue.addLast(traceMessageQueue.removeFirst());
			}
		}
		// Now process the messages we removed from the queue.
		for (int i=0; i < nMsg; i++) {
			TraceQueueItem item = bufferedMessageQueue.removeFirst();
			if (item.isFirstMessage()) {
				try {
					dataThreadWithAFirstMessage = item.dataThread;
					SwingUtilities.invokeAndWait(dataThreadFirstMessageHandler);
				} catch (InvocationTargetException e) {
					displayErrorAndDie(e);
				} catch (InterruptedException e) {
					System.err.println("Interrupted invoking dataThreadFirstMessageHandler: " + e);
				}
			}
			ContentFrameEnsemble.getInstance().bufferIfMatches(item.message, item.dataThread);
		}
	}

	private Runnable dataThreadFirstMessageHandler = new Runnable() {
		// A data thread got its first message.
		// - If we have a perThreadWindow policy then create a frame
		// filtered with the pid and put the with "app name (pid)" as title.
		// - Add a global program filter with the app name if it doesn't already exist.
		// Reset the global time stamp base if this is the first message at all.
		// - This method is called only once per data thread, on the EDT thread.
		public void run() {
			TraceMessage firstMessage = dataThreadWithAFirstMessage.getFirstMessage();
			firstMessage.appName = transformProgName(firstMessage.appName);
			// Note what's going on here. We have transformed the name within the first
			// message within the data thread. But underneath us, in TracesDataThread.run,
			// the transformed name will be copied from the first message into each new message.
			System.out.println("dataThreadFirstMessageHandler: " + firstMessage.appName + " (" + firstMessage.processId + ")");
			if (prefs.perThreadWindow) {
				ContentFrame frm = ContentFrameEnsemble.getInstance().add();
				frm.jSplitPane.setDividerLocation(0);
				frm.setTitle(firstMessage.appName + " (" + firstMessage.processId + ")");
				frm.dataThread = dataThreadWithAFirstMessage;
			}
			if (mainFrame != null) {
				TracesFilter tf = mainFrame.getFilter();
				ProgFilter pf = tf.getProgFilterByName(firstMessage.appName);
				if (pf == null) {
					// This program is not in the program filter palette, so add it there.
					pf = new ProgFilter(firstMessage.appName, Color.BLACK);
					pf.setPid(firstMessage.processId);
					tf.addProgFilter(pf);
					mainFrame.setFilterAndUpdateContentFrameFilters(tf);
				} else {
					// This program is already in the program filter palette, so update it with the pid.
					mainFrame.updateProgFilterPid(pf.getName(), firstMessage.processId);
					ContentFrameEnsemble.getInstance().updateProgFilterPid(pf.getName(), firstMessage.processId);
				}
			}
		}

		private static final String progPrefix = "";

		// Remove the prefix "bar:com.gfprod." from the program name if it's there.
		// TODO: This should really be more general and configurable by the user...
		private String transformProgName(String name) {
			if (name == null)
				return "?";
			
			return name.startsWith(progPrefix) ? name.substring(progPrefix.length()) : name;
		}
	};

	void displayErrorAndDie(Throwable e) {
		e.printStackTrace();
		StringBuilder s = new StringBuilder(100);
		do {
			s.append('\n');
			s.append(e.toString());
			e = e.getCause();
		} while (e != null);
		displayErrorAndDie("INTERNAL ERROR detected on thread " + Thread.currentThread().getName() + s);
	}

	private void displayErrorAndDie(String message) {
		errorMessage = message;
		if (javax.swing.SwingUtilities.isEventDispatchThread()) {
			errorDisplayer.run();
		} else {
			try {
				javax.swing.SwingUtilities.invokeAndWait(errorDisplayer);
			} catch (InvocationTargetException e) {
				System.err.println("Error invoking errorDisplayer: " + e);
				e.printStackTrace();
			} catch (InterruptedException e) {
				System.err.println("Interrupted invoking errorDisplayer: " + e);
			}
		}
		shutdown();
	}

	private String errorMessage = null;

	private Runnable errorDisplayer = new Runnable() {
		public void run() {
			JOptionPane.showMessageDialog(mainFrame, errorMessage, null, JOptionPane.ERROR_MESSAGE);
		}
	};

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		System.out.format("%s %s\n", Strings.getString("Application.name"),
				Strings.getString("Application.version"));
		instance = new App();
		instance.initialize(args);
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				instance.startup();
			}
		});
	}

	// Get the preferred look and feel if possible, otherwise default to system
	public static String getPreferredLookAndFeel() {
		for (LookAndFeelInfo lfi : UIManager.getInstalledLookAndFeels()) {
		    if (PREFERRED_LOOK_AND_FEEL.equals(lfi.getName()))
		        return lfi.getClassName();
		}
		return UIManager.getSystemLookAndFeelClassName();
	}
	
	// Inner class encapsulating a trace message and the "data thread" it came from.
	private static final class TraceQueueItem {
		TraceMessage message;
		DataThread dataThread;

		public TraceQueueItem(TraceMessage message, DataThread dataThread) {
			this.message = message;
			this.dataThread = dataThread;
		}

		public boolean isFirstMessage() {
			return message == dataThread.getFirstMessage();
		}
	}

	// Inner class encapsulating this app's preferences.
	final class Prefs {
		public int DEFAULT_MIN_LINES = 50;
		public int DEFAULT_MAX_LINES = 5000;
		public int LINES_INCREMENT = 50;
			
		// User preferences node
		private Preferences uNode = Preferences.userNodeForPackage(App.class);
		// Global filters node, under user preferences
		private Preferences fNode = null;

		private int mainFramePosX;
		private int mainFramePosY;
		private int mainFrameSizeX;
		private int mainFrameSizeY;
		private int mainFrameBgColor;
		int textAreaBgColor;

		public boolean perThreadWindow = false;
		public boolean showModuleId = false;
		public boolean showTimestamp = false;
		public boolean showProcessId = false;
		public boolean showThreadId = false;
		public boolean showBinaryTraces = false;
		public boolean showFilterName = false;
		public boolean matchCase = false;
		public String selectedWindow = null;
		public String saveWindowDir = null;
		public String saveWindowFile = null;
		public String importPrefsDir = null;
		public String importPrefsFile = null;
		public int	  maxLines = 500;
		public String lookAndFeel = null;

		private void load() {
			mainFramePosX = uNode.getInt("mainFramePosX", 0);
			mainFramePosY = uNode.getInt("mainFramePosY", 0);
			mainFrameSizeX = uNode.getInt("mainFrameSizeX", 940);
			mainFrameSizeY = uNode.getInt("mainFrameSizeY", 780);
			mainFrameBgColor = uNode.getInt("mainFrameBgColor", 0x809090);
			textAreaBgColor = uNode.getInt("textAreaBgColor", 0xFFEFCF);
			perThreadWindow = uNode.getBoolean("perThreadWindow", false);
			showModuleId = uNode.getBoolean("showModuleId", false);
			showTimestamp = uNode.getBoolean("showTimestamp", false);
			showProcessId = uNode.getBoolean("showProcessId", false);
			showThreadId = uNode.getBoolean("showThreadId", false);
			showBinaryTraces = uNode.getBoolean("showBinaryTraces", true);
			showFilterName = uNode.getBoolean("showFilterName", false);
			matchCase = uNode.getBoolean("matchCase", false);
			selectedWindow = uNode.get("selectedWindow", "");
			saveWindowDir = uNode.get("saveWindowDir", "");
			saveWindowFile = uNode.get("saveWindowFile", "traces.txt");
			importPrefsDir = uNode.get("importPrefsDir", "");
			importPrefsFile = uNode.get("importPrefsFile", "reporterPrefs.xml");
			maxLines = uNode.getInt("maxLines", DEFAULT_MAX_LINES);
			lookAndFeel = uNode.get("lookAndFeel", App.getPreferredLookAndFeel());
			fNode = uNode.node(uNode.absolutePath() + "/" + TracesFilter.globalFiltersKey);
		}

		// Note: this must be called before the main frame is destroyed.
		void save() {
			Rectangle r = mainFrame.getBounds();
			uNode.putInt("mainFramePosX", r.x);
			uNode.putInt("mainFramePosY", r.y);
			uNode.putInt("mainFrameSizeX", r.width);
			uNode.putInt("mainFrameSizeY", r.height);
			uNode.putInt("mainFrameBgColor", mainFrameBgColor);
			uNode.putInt("textAreaBgColor", textAreaBgColor);
			uNode.putBoolean("perThreadWindow", perThreadWindow);
			uNode.putBoolean("showModuleId", showModuleId);
			uNode.putBoolean("showTimestamp", showTimestamp);
			uNode.putBoolean("showProcessId", showProcessId);
			uNode.putBoolean("showThreadId", showThreadId);
			uNode.putBoolean("showBinaryTraces", showBinaryTraces);
			uNode.putBoolean("showFilterName", showFilterName);
			uNode.putBoolean("matchCase", matchCase);
			uNode.put("selectedWindow", ContentFrameEnsemble.getInstance().getNameOfSelected());
			uNode.put("saveWindowDir", saveWindowDir);
			uNode.put("saveWindowFile", saveWindowFile);
			uNode.put("importPrefsDir", importPrefsDir);
			uNode.put("importPrefsFile", importPrefsFile);
			uNode.putInt("maxLines", maxLines);
			uNode.put("lookAndFeel", lookAndFeel);
			saveFilterPrefs();
			saveFramesPrefs();
			try {
				uNode.flush();
			} catch (BackingStoreException e) {
				System.err.println("Can't flush prefs: " + e);
			}
		}

		// Creates a new TracesFilter from global filters preferences node
		private TracesFilter getGlobalFilters() {
			TracesFilter globalFilters = new TracesFilter();
			globalFilters.loadPrefs(fNode);
			return globalFilters;
		}

		// Updates global filters preferences node from TracesFilter parameter
		// Adds content frames preferences to the global filters preferences node
		private void saveFilterPrefs() {
			fNode = uNode.node(uNode.absolutePath() + "/" + TracesFilter.globalFiltersKey);
			try {
				fNode.removeNode();
			} catch (BackingStoreException e) {
				System.err.println("Can't clear global filters: " + e);
			}
			fNode = uNode.node(uNode.absolutePath() + "/" + TracesFilter.globalFiltersKey);
			if (mainFrame != null && mainFrame.getFilter() != null) {
				mainFrame.getFilter().savePrefs(fNode);
			}
		}

		private void saveFramesPrefs() {
			ContentFrameEnsemble contentFrameEnsemble = ContentFrameEnsemble.getInstance();
			contentFrameEnsemble.savePrefs(fNode);
			MainFrame mainFrame = App.getMainFrame();
			fNode.putInt("divider",mainFrame.splitPane.getDividerLocation());
			fNode.putInt("divider1",mainFrame.splitPane1.getDividerLocation());

			for (int i=0; i<mainFrame.modIdTable.getColumnCount(); i++) {
				String colName = mainFrame.modIdTable.getColumnName(i);
				fNode.putInt(colName, mainFrame.modIdTable.getColumn(colName).getWidth());
			}

			for (int i=0; i<mainFrame.progTable.getColumnCount(); i++) {
				String colName = mainFrame.progTable.getColumnName(i);
				fNode.putInt(colName, mainFrame.progTable.getColumn(colName).getWidth());
			}
		}

		private void loadFramesPrefs() {
			ContentFrameEnsemble contentFrameEnsemble = ContentFrameEnsemble.getInstance();
			contentFrameEnsemble.loadPrefs(fNode);
			MainFrame mainFrame = App.getMainFrame();
			mainFrame.splitPane.setDividerLocation(fNode.getInt("divider", (int)(mainFrameSizeX * 0.2)));
			mainFrame.splitPane1.setDividerLocation(fNode.getInt("divider1", (int)(mainFrameSizeY * 0.66)));

			int defaultColumnWidth = mainFrame.splitPane.getDividerLocation() / mainFrame.modIdTable.getColumnCount();
			for (int i=0; i<mainFrame.modIdTable.getColumnCount(); i++) {
				String colName = mainFrame.modIdTable.getColumnName(i);
				mainFrame.modIdTable.getColumn(colName).setPreferredWidth(fNode.getInt(colName, defaultColumnWidth));
			}

			defaultColumnWidth = mainFrame.splitPane.getDividerLocation() / mainFrame.progTable.getColumnCount();
			for (int i=0; i<mainFrame.progTable.getColumnCount(); i++) {
				String colName = mainFrame.progTable.getColumnName(i);
				mainFrame.progTable.getColumn(colName).setPreferredWidth(fNode.getInt(colName, defaultColumnWidth));
			}
		}

		// Updates global filters preferences node from preferences InputStream
		private void importGlobalFilters(InputStream aStream) throws IOException {
			fNode = uNode.node(uNode.absolutePath() + "/" + TracesFilter.globalFiltersKey);
			try {
				fNode.removeNode();
			} catch (BackingStoreException e) {
				System.err.println("Can't clear global filters: " + e);
			}
			try {
				Preferences.importPreferences(aStream);
			} catch (InvalidPreferencesFormatException e) {
				System.err.println("Can't import global filters: " + e);
			}
			fNode = uNode.node(uNode.absolutePath() + "/" + TracesFilter.globalFiltersKey);
		}

		private void exportGlobalFilters(OutputStream aStream) throws IOException {
			if (fNode != null) {
				try {
					fNode.exportSubtree(aStream);
				} catch (BackingStoreException e) {
					System.err.println("Can't export global filters: " + e);
				}
			}
		}

	}
}
