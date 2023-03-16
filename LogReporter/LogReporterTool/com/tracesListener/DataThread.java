package tracesListener;

import java.util.Observable;

@SuppressWarnings("deprecation")
public class DataThread extends Observable implements Runnable {
	protected volatile boolean 	running = false;
	protected TraceMessage 		firstMessage = null;
	protected Thread 			thread = null;
	
	public DataThread(String name) {
		this.thread = new Thread(this, name);
	}

	public void start() {
		thread.start();
	}

	/*
	 * (non-Javadoc)
	 *
	 * @see java.lang.Runnable#run()
	 */
	public void run() {
		try {
			runInnards();
		} catch (RuntimeException e) {
			setChanged();
			notifyObservers(e);
		}
	}

	protected void runInnards() {
	}

	public void shutdown() {}

	public boolean isRunning() {
		return running;
	}

	public TraceMessage getFirstMessage() {
		return firstMessage;
	}
}
