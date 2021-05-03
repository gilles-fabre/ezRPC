package tracesListener;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Observable;

public class TracesDataThread extends Observable implements Runnable {
	private volatile boolean running = false;
	private BufferedInputStream in = null;
	private TraceMessage firstMessage = null;
	private Socket clientSocket;
	private TracesListenerServer listenerServer;
	private Thread thread;

	/**
	 * @param clientSocket
	 * @param listenerServer
	 */
	TracesDataThread(Socket clientSocket, TracesListenerServer listenerServer) {
		super();
		this.clientSocket = clientSocket;
		this.listenerServer = listenerServer;
		this.thread = new Thread(this, "TraceData");
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

	private void runInnards() {
		// Loop reading trace messages and notifying observers until EOF or io exception.
		// (Another thread calling our shutdown() method will result in an io exception.)
		running = true;
		try {
			in = new BufferedInputStream(clientSocket.getInputStream());
			TraceMessage currentMessage;
			while ((currentMessage = TracesDecoder.readNextMessage(in)) != null) {
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
		} catch (IOException e) {
			System.err.println("Reading input stream failed with error: " + e.getMessage());
		}
		// Unregister from the listener server.
		// If we are not already unregistered, this will call our shutdown method.
		listenerServer.removeDataThread(this);
	}

	// Remove all observers, and close the input stream.
	void shutdown() {
		if (running) {
			running = false;
			deleteObservers();
			try {
				// Note: in could be null because getInputStream() above could have failed.
				// Note: closing in closes the underlying socket, but closing a closed socket is ok.
				if (in != null) {
					in.close();
				}
				clientSocket.close();
			} catch (IOException e) {
				System.err.println("Closing input stream failed with error: " + e.getMessage());
			}
		}
	}

	public boolean isRunning() {
		return running;
	}

	public TraceMessage getFirstMessage() {
		return firstMessage;
	}

}
