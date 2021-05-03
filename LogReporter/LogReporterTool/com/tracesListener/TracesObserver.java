package tracesListener;

import java.io.PrintStream;
import java.util.Observable;
import java.util.Observer;

public class TracesObserver implements Observer {
	PrintStream out;
	TracesFilter filter;

	/**
	 * @param out
	 * @param filter
	 */
	public TracesObserver(PrintStream out, TracesFilter filter) {
		super();
		this.out = out;
		this.filter = filter;
	}


	/* (non-Javadoc)
	 * @see java.util.Observer#update(java.util.Observable, java.lang.Object)
	 */
	public void update(Observable observable, Object arg) {
		TraceMessage msg = (TraceMessage) arg;
		TracesDataThread dataThread = (TracesDataThread) observable;
		if (dataThread.getFirstMessage() == msg)
			out.println("---- ---- " + msg.appName + " - " + ByteUtil.formatHexInteger(msg.traceModule) + " (" + msg.processId + ") ");
		if (filter.messageMatches(msg)) {
			if (filter.matchModFilter != null)
				out.print(filter.matchModFilter.name + " : ");
			out.println(ByteUtil.formatHexInteger(msg.traceModule) + " (" + msg.processId + ") "
					+ msg.getDataString());
		}
	}

}
