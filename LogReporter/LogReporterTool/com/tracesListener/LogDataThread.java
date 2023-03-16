package tracesListener;

import java.io.FileInputStream;
import java.io.IOException;

@SuppressWarnings("deprecation")
public class LogDataThread extends DataThread {
	private FileInputStream logFileIn = null;

	/**
	 * @param log file input stream
	 */
	public LogDataThread(FileInputStream logFileIn) {
		super("logDataThread");
		this.logFileIn = logFileIn;
	}

	protected void runInnards() {
		// Loop reading log messages and notifying observers until EOF or io exception.
		// (Another thread calling our shutdown() method will result in an io exception.)
		running = true;
		TraceMessage currentMessage;
		while ((currentMessage = TracesDecoder.readNextMessage(logFileIn)) != null) {
			if (firstMessage == null) {
				// Keep the first message around for later use.
				firstMessage = currentMessage;
			} else {
				// Copy (by reference) the program name from the first message into this one.
				currentMessage.appName = firstMessage.appName;
			}
			setChanged();
			notifyObservers(currentMessage);
		}
	}

	// Remove all observers, and close the input stream.
	public void shutdown() {
		if (running) {
			running = false;
			deleteObservers();
			try {
				// Note: in could be null because getInputStream() above could have failed.
				if (logFileIn != null) {
					logFileIn.close();
				}
			} catch (IOException e) {
				System.err.println("Closing input stream failed with error: " + e.getMessage());
			}
		}
	}
}
