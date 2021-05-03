package tracesListener;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.prefs.Preferences;

public class CmdLineServer {
	public static final String version = "0.1";

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		TracesListenerServer serverListener = null;
		Thread serverThread = null;
		TracesFilter tracesFilter = new TracesFilter();

		// Get application user preferences
		Preferences prefsNode, filtersNode;
		prefsNode = Preferences.userRoot().node("com/reporter");
		try {
			// Open traces filters preferences from input file
			if (args.length == 1) {
				// Clear trace filters
				filtersNode = prefsNode.node(prefsNode.absolutePath() + "/" + TracesFilter.globalFiltersKey);
				filtersNode.removeNode();
				filtersNode.flush();
				// Read global filters from file
				FileInputStream prefsFileIn = new FileInputStream(args[0]);
				Preferences.importPreferences(prefsFileIn);
				prefsFileIn.close();
				filtersNode = prefsNode.node(prefsNode.absolutePath() + "/" + TracesFilter.globalFiltersKey);
				tracesFilter.loadPrefs(filtersNode);
			}
		} catch (Exception e) {
			System.err.println(e.getMessage());
			System.exit(1);
		}

		TracesObserver stdoutObserver = new TracesObserver(System.out,
				tracesFilter);
		System.out.println("Reporter " + version + " console logging with "
				+ tracesFilter.nModuleIdFilters() + " module filters.");
		// Create the server socket.
		// This could fail if, e.g., the socket address is already in use.
		try {
			serverListener = new TracesListenerServer();
		} catch (IOException e) {
			System.err.println("Can't create server: " + e);
			System.exit(1);
		}
		// Add one observer that will print all client traces to stdout
		serverListener.addTracesObserver(stdoutObserver);
		// Print out traces port on the command line
		System.out.println("Reporter " + version + " listening on port:"
				+ serverListener.port);
		System.out.println("Strike any key to exit");
		// Start server thread
		serverThread = new Thread(serverListener);
		serverThread.start();
		try {
			// When user strikes a key, remove the stdout observer and let
			// server exit
			System.in.read();
			serverListener.shutdown();
		} catch (IOException e) {
			// Otherwise, kill the process to exit
			System.err.println("Cannot read line on standard output: "
					+ e.getMessage());
			System.out.println("Strike CTRL-C to exit");
		}
		// Wait for server thread to exit
		try {
			serverThread.join(0);
		} catch (InterruptedException e) {
			System.err.println("Traces server thread exited with error: "
					+ e.getMessage());
		}
	}

}
