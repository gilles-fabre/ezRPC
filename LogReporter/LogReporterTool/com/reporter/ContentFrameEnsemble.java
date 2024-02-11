package reporter;

import java.awt.Color;
import java.awt.Dimension;
import java.beans.PropertyVetoException;
import java.lang.reflect.InvocationTargetException;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.prefs.Preferences;

import javax.swing.JDesktopPane;
import javax.swing.SwingUtilities;

import tracesListener.DataThread;
import tracesListener.TraceMessage;

public class ContentFrameEnsemble {
	private static ContentFrameEnsemble instance = null;
	// The list of content frames.
	private LinkedList<ContentFrame> contentFrameList = new LinkedList<ContentFrame>();
	// The max delta for the position of cascaded content frames.
	private static final int CF_DELTA = 30;
	// The nCFC-th created content frame will be initially positioned at
	// ((nCFC%MODULUS)*DELTA, (nCFC%MODULUS)*DELTA) within the main frame,
	// and will be given a title fabricated from nCFC.
	private static final int CF_MODULUS = 8;
	private int nCFC = 0; // number of created content frames
	// The content frame to be updated by the displayer.
	private ContentFrame contentFrameToUpdate = null;

	static ContentFrameEnsemble getInstance() {
		if (instance == null) {
			instance = new ContentFrameEnsemble();
		}
		return instance;
	}

	// Allocate a new content frame and add it to the list.
	// This gets called on the EDT thread.
	ContentFrame add() {
		ContentFrame contentFrame = new ContentFrame();
		synchronized (contentFrameList) {
			contentFrameList.addLast(contentFrame);
		}
		int offset = (nCFC % CF_MODULUS) * CF_DELTA;
		contentFrame.setLocation(offset, offset);
		if (contentFrame.getTitle().equals("")) {
			contentFrame.setTitle(inventUniqueName());
		}
		JDesktopPane dtp = App.getMainFrame().getDesktopPane();
		dtp.add(contentFrame);
		contentFrame.setVisible(true);
		nCFC++;
		return contentFrame;
	}

	// Return the first non-zombie content frame, or null if there is none.
	private ContentFrame getFirst() {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				return contentFrame;
			}
		}
		return null;
	}

	// Return the first non-zombie content frame following (circularly) the given one.
	private ContentFrame getNext(ContentFrame current) {
		if (current == null) {
			return null;
		}
		boolean found = false;
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				if (found) {
					return contentFrame;
				}
				if (contentFrame == current) {
					found = true;
				}
			}
		}
		if (found) {
			for (ContentFrame contentFrame : contentFrameList) {
				if (!contentFrame.isZombie) {
					return contentFrame;
				}
			}
		}
		return null;
	}

	// Select the first non-zombie content frame if there is one.
	void selectFirst() {
		ContentFrame contentFrame = getFirst();
		if (contentFrame != null) {
			contentFrame.setSelected(true);
		}
	}

	// Select the non-zombie content frame following the currently selected one.
	void selectNext() {
		ContentFrame contentFrame = getSelected();
		if (contentFrame != null) {
			contentFrame = getNext(contentFrame);
			contentFrame.setSelected(true);
		}
	}

	boolean selectByName(String name) {
		ContentFrame contentFrame = getContentFrameByName(name);
		if (contentFrame != null) {
			contentFrame.setSelected(true);
			return true;
		}
		return false;
	}

	String getNameOfSelected() {
		ContentFrame contentFrame = getSelected();
		return contentFrame != null ? contentFrame.getTitle() : "";
	}

	// Return the currently selected content frame, or null if there is none.
	private ContentFrame getSelected() {
		for (ContentFrame contentFrame : contentFrameList) {
			if (contentFrame.isSelected()) {
				assert !contentFrame.isZombie;
				return contentFrame;
			}
		}
		return null;
	}

	private String inventUniqueName() {
		for (int n=0; ; n++) {
			String name = "Window " + n;
			if (getContentFrameByName(name) == null) {
				return name;
			}
		}
	}

	ContentFrame getContentFrameByName(String name) {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				if (contentFrame.getTitle().equals(name)) {
					return contentFrame;
				}
			}
		}
		return null;
	}

	String[] getNames() {
		int i = 0;
		int n = getNonZombieCount();
		String[] names = new String[n];
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				names[i++] = contentFrame.getTitle();
			}
		}
		return names;
	}

	void updateFilters() {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.updateFilter();
			}
		}
	}

	void updateProgFilterPid(String name, int pid) {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.updateProgFilterPid(name, pid);
			}
		}
	}

	void changeModIdName(String oldName, String newName) {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.changeModIdName(oldName, newName);
			}
		}
	}

	void changeProgName(String oldName, String newName) {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.changeProgName(oldName, newName);
			}
		}
	}

	int getModIdUseCount(String name) {
		int n = 0;
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie && contentFrame.isModIdUsed(name)) {
					n++;
			}
		}
		return n;
	}

	int getProgUseCount(String name) {
		int n = 0;
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie && contentFrame.isProgUsed(name)) {
					n++;
			}
		}
		return n;
	}

	void clearAll() {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.clear();
			}
		}
	}

	void separateAll() {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.insertSeparator();
			}
		}
	}

	void closeAll() {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.dispose();
			}
		}
	}

	void cascade() {
		int n = getNonZombieCount();
		if (n < 2) {
			tile(n);
			return;
		}
		Dimension dtpSize = App.getMainFrame().getDesktopPane().getSize();
		int wSizeX = dtpSize.width / 2;
		int wSizeY = dtpSize.height / 2;
		int deltaX = wSizeX / (n-1);
		int deltaY = wSizeY / (n-1);
		if (deltaY > CF_DELTA) {
			double aspectRatio = ((double) dtpSize.width) / ((double) dtpSize.height);
			deltaX = (int) ((((double) CF_DELTA) * aspectRatio) + 0.5);
			deltaY = CF_DELTA;
			wSizeX = dtpSize.width - ((n-1) * deltaX);
			wSizeY = dtpSize.height - ((n-1) * deltaY);
		}
		ContentFrame topCF = null;
		int i = 0;
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.setBounds(i*deltaX, i*deltaY, wSizeX, wSizeY);
				try {
					contentFrame.setMaximum(false);
				} catch (PropertyVetoException e) {
					// tough luck
				}
				contentFrame.moveToFront();
				topCF = contentFrame;
				i++;
			}
		}
		if (topCF != null) {
			topCF.setSelected(true);
		}
	}

	void tile() {
		tile(getNonZombieCount());
	}

	private void tile(int n) {
		if (n == 0) {
			return;
		}
		int nX = (int) Math.sqrt((double)n);
		int nY = nX;
		if ((nX * nY) < n) nY++;
		if ((nX * nY) < n) nX++;
		assert (nX * nY) >= n;
		Dimension dtpSize = App.getMainFrame().getDesktopPane().getSize();
		int w = dtpSize.width / nX;
		int h = dtpSize.height / nY;
		int i = 0;
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.setBounds(w * (i % nX), h * (i / nX), w, h);
				try {
					contentFrame.setMaximum(false);
				} catch (PropertyVetoException e) {
					// tough luck
				}
				i++;
			}
		}
		assert i == n;
	}

	void hideFilters() {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.hideFilters();
			}
		}
	}

	void showFilters() {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.showFilters();
			}
		}
	}

	int getNonZombieCount() {
		int n = 0;
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				n++;
			}
		}
		return n;
	}

	void setTextAreaBgColor(Color color) {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.textArea.setBackground(color);
			}
		}
	}

	// For each non-zombie content frame, if the given message coming from the given thread
	// matches that content frame, buffer it to that content frame.
	// This gets called on the consumer thread.
	void bufferIfMatches(TraceMessage msg, DataThread dt) {
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie) {
				contentFrame.bufferIfMatches(msg, dt);
			}
		}
	}

	// For each content frame that has buffered messages waiting to display,
	// invoke the displayer to display them on the EDT thread.
	// If any zombies, invoke the the zombieRemover to delete them on the EDT thread.
	// This gets called on the consumer thread.
	void displayBufferedMessages() throws InterruptedException {
		int nZombies = 0;
		ContentFrame[] cfa;
		// Make a copy of the content frame list.
		// This is the only code run on a non-EDT thread that consults the content frame list.
		synchronized (contentFrameList) {
			cfa = contentFrameList.toArray(new ContentFrame[contentFrameList.size()]);
		}
		for (ContentFrame contentFrame : cfa) {
			if (contentFrame.isZombie) {
				nZombies++;
			} else if (!contentFrame.bufferedMessages.isEmpty()) {
				try {
					contentFrameToUpdate = contentFrame;
					SwingUtilities.invokeAndWait(displayer);
				} catch (InvocationTargetException e) {
					// OutOfMemoryErrors often caught here.
					App.getInstance().displayErrorAndDie(e);
				}
			}
		}
		if (nZombies != 0) {
			try {
				SwingUtilities.invokeAndWait(zombieRemover);
			} catch (InvocationTargetException e) {
				App.getInstance().displayErrorAndDie(e);
			}
		}
	}

	private Runnable displayer = new Runnable() {
		// Display the messages that have been buffered in the contentFrameToUpdate.
		// This gets called on the EDT.
		// The consumer thread is blocked until we return.
		public void run() {
			contentFrameToUpdate.displayBufferedMessages();
			App.getMainFrame().updateStatusBarPendingTraces();
		}
	};

	private Runnable zombieRemover = new Runnable() {
		// Remove all zombies from our list of content frames.
		// This gets called on the EDT.
		// The consumer thread is blocked until we return.
		public void run() {
			for (Iterator<ContentFrame> it = contentFrameList.iterator(); it.hasNext();) {
				ContentFrame contentFrame = it.next();
				if (contentFrame.isZombie) {
					it.remove();
					System.out.println("content frame removed: " + contentFrame);
				}
			}
		}
	};

	void loadPrefs(Preferences aNode) {
		int nWindows = aNode.getInt("nWindows", 0);
		String aPath = aNode.absolutePath();
		closeAll();
		for (int i=0; i < nWindows; i++ ) {
			ContentFrame aContentFrame = add();
			aContentFrame.loadPrefs(aNode.node(aPath + "/window" + i));
		}
		if (nWindows == 0 && !App.getPrefs().perThreadWindow) {
			ContentFrame aContentFrame = add();
			try {
				aContentFrame.setMaximum(true);
			} catch (PropertyVetoException e) {
				System.err.println("Can't maximize window " + e);
			}
		}
	}

	void savePrefs(Preferences aNode) {
		String aPath = aNode.absolutePath();
		int n = 0;
		for (ContentFrame contentFrame : contentFrameList) {
			if (!contentFrame.isZombie && contentFrame.dataThread == null) {
				contentFrame.savePrefs(aNode.node(aPath + "/window" + n));
				n++;
			}
			aNode.putInt("nWindows", n);
		}
	}

}
