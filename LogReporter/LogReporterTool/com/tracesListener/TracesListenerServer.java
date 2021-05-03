package tracesListener;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;

public class TracesListenerServer extends Observable implements Runnable {
	static final int _DefaultTracesPort = 25998;
	int port;
	ServerSocket serverSocket;
	ArrayList<Object> dataThreads, tracesObservers;
	public boolean autoStartDataThreads = true;

	/**
	 *
	 */
	public TracesListenerServer() throws IOException {
		super();
		this.init(_DefaultTracesPort);
	}

	/**
	 * @param port
	 */
	public TracesListenerServer(int port) throws IOException {
		super();
		this.init(port);
	}

	private void init(int port) throws IOException {
		this.port = port;
		this.dataThreads = new ArrayList<Object>();
		this.tracesObservers = new ArrayList<Object>();
		serverSocket = new ServerSocket(port);
	}

	public void run() {
		TracesDataThread clientDataThread = null;
		Socket clientSocket = null;
		// Create a data thread for each client connexion
		while (!tracesObservers.isEmpty()) {
			// Accept connexions on the server socket
			try {
				clientSocket = serverSocket.accept();
			} catch (IOException e) {
				// Ok if shutdown was called from another thread
				if (!tracesObservers.isEmpty())
					System.err.println("Error accepting socket: " + e.getMessage());
				return;
			}
			// Create a data thread on the new socket
			clientDataThread = new TracesDataThread(clientSocket, this);
			addDataThread(clientDataThread);
			// Start the new data thread
			if (autoStartDataThreads)
				clientDataThread.start();
		}
		shutdown();
	}

	public void shutdown() {
		this.deleteObservers();
		removeAllDataThreads();
		removeAllTracesObservers();
		try {
			serverSocket.close();
		} catch (IOException e) {
			System.err.println("Error closing server socket: " + e.getMessage());
		}
	}

	public boolean dataThreadExited(TracesDataThread aDataThread) {
		return dataThreads.indexOf(aDataThread) == -1;
	}

	public boolean addDataThread(TracesDataThread aDataThread) {
		boolean result;
		// Register all observers for the new data thread
		Iterator<Object> observersIterator = tracesObservers.listIterator();
		while (observersIterator.hasNext())
			aDataThread.addObserver((Observer) observersIterator.next());
		result = dataThreads.add(aDataThread);
		// Notify Server observers that a new data thread was added
		setChanged();
		notifyObservers(aDataThread);
		return result;
	}

	public boolean removeDataThread(TracesDataThread aDataThread) {
		boolean result = dataThreads.remove(aDataThread);
		if (result) {
			// Unregister all data thread observers, close stream.
			aDataThread.shutdown();
			// Notify server observers that a data thread was removed.
			setChanged();
			notifyObservers(aDataThread);
		}
		return result;
	}

	public void removeAllDataThreads() {
		while (!dataThreads.isEmpty()) {
			removeDataThread((TracesDataThread)dataThreads.get(0));
		}
	}

	public boolean addTracesObserver(Observer anObserver) {
		Observable currentObservable;
		// Register this observer for all data threads
		Iterator<Object> dataThreadIterator = dataThreads.listIterator();
		while (dataThreadIterator.hasNext()) {
			currentObservable = (Observable) dataThreadIterator.next();
			currentObservable.addObserver(anObserver);
		}
		return tracesObservers.add(anObserver);
	}

	public boolean removeTracesObserver(Observer anObserver) {
		Observable currentObservable;
		// Unregister this observer from all data threads
		Iterator<Object> dataThreadIterator = dataThreads.listIterator();
		while (dataThreadIterator.hasNext()) {
			currentObservable = (Observable) dataThreadIterator.next();
			currentObservable.deleteObserver(anObserver);
		}
		return tracesObservers.remove(anObserver);
	}

	public void removeAllTracesObservers() {
		while (!tracesObservers.isEmpty()) {
			removeTracesObserver((Observer)tracesObservers.get(0));
		}
	}
}
