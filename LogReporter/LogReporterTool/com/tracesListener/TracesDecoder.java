package tracesListener;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;

public class TracesDecoder {
	static final byte _EOM = 0;
	static final byte _Attributes = 1;
	static final byte _IndentationLevel = 2;
	static final byte _ProcessId = 3;
	static final byte _ThreadId = 4;
	static final byte _TraceData = 5;
	static final byte _TimeStamp = 6;
	static final byte _Application = 7;
	static private HashMap<String, Integer> _appCurrentDumpLines = new HashMap<String, Integer>();
	static private HashMap<String, Integer> _appLastDumpLines = new HashMap<String, Integer>();

	static final int msgHeaderSize = 6;

	public static TraceMessage readNextMessage(InputStream in) {
		int toRead, bytesRead, index, paramSize, lastParamIndex;
		byte paramType;
		TraceMessage nextMessage = null;

		try {

			do {
				byte msgHeader[] = new byte[msgHeaderSize];
				// Read message header. Return null if socket closed.
				toRead = msgHeaderSize;
				index = 0;
				while (toRead != 0) {
					if ((bytesRead = in.read(msgHeader, index, toRead)) < 0) {
						throw (new IOException("Message header: got " + bytesRead + " bytes, expected " + toRead));
					}
					index += bytesRead;
					toRead -= bytesRead;
				}

				// Create trace message from header.
				nextMessage = new TraceMessage(msgHeader[0], msgHeader[1],
											   ByteUtil.extractInt(msgHeader, 2) - msgHeaderSize);
				// Throw an exception if message data could not be read
				toRead = nextMessage.msgDataSize;
				index = 0;
				while (toRead != 0) {
					if ((bytesRead = in.read(nextMessage.msgData, index, toRead)) < 0) {
						throw (new IOException("Message data: got " + bytesRead + " bytes, expected " + toRead));
					}
					index += bytesRead;
					toRead -= bytesRead;
				}

				try {
					// Set message parameters
					index = 0;
					lastParamIndex = nextMessage.msgDataSize - 3;
					while (index <= lastParamIndex) {
						paramType = nextMessage.msgData[index++];
						paramSize = ByteUtil.extractShort(nextMessage.msgData, index);
						index += 2;
						switch (paramType) {
						case _EOM:
							// done with this message
							break;
						case _Attributes:
							// get 4 bytes of module id + 1 byte of message type
							nextMessage.traceModule = ByteUtil.extractInt(nextMessage.msgData, index);
							nextMessage.msgType = nextMessage.msgData[index + 4];
							break;
						case _IndentationLevel:
							// get 2 bytes of indentation
							nextMessage.indentationLevel = nextMessage.msgData[index];
							break;
						case _ProcessId:
							// get 4 bytes of process id
							nextMessage.processId = ByteUtil.extractInt(nextMessage.msgData, index);
							break;
						case _ThreadId:
							// get 4 bytes of thread id
							nextMessage.threadId = ByteUtil.extractInt(nextMessage.msgData, index);
							break;
						case _TraceData:
							// get 32 bytes of data size and 32 bytes of data index
							nextMessage.traceDataSize = paramSize;
							nextMessage.traceDataIndex = index;
							break;
						case _TimeStamp:
							// get (long) bytes time stamp
							nextMessage.timeStamp = ByteUtil.extractLong(nextMessage.msgData, index);
							break;
						case _Application:
							// get application name (full path read, keep only basename)
							nextMessage.appName = baseName(new String(nextMessage.msgData, index, paramSize - 1));
							break;
						default:
							// something went wrong
							throw (new IOException("---- ---- PARAMETER TYPE:" + paramType + " SIZE:" + paramSize + " NOT DECODED"));
						}
						index += paramSize;
					}
				} catch (Exception e) {
					System.out.println("caught exception : " + e.getMessage() + " while receiving trace message");
					return null;
				}
				// count the dumped lines (either skipped or not)
				if (_appCurrentDumpLines.containsKey(nextMessage.appName))
					_appCurrentDumpLines.put(nextMessage.appName, _appCurrentDumpLines.get(nextMessage.appName) + 1);

				if (nextMessage.msgType == TraceMessage._StartDump) {
					// has this app already completed a dump during this reporter session?
					if (!_appLastDumpLines.containsKey(nextMessage.appName))
						_appLastDumpLines.put(nextMessage.appName, 0);

					// keep ref of app and count lines
					_appCurrentDumpLines.put(nextMessage.appName, 1);
				}
				else if (nextMessage.msgType == TraceMessage._EndDump) {
					// we've reached the end of the dump
					_appLastDumpLines.put(nextMessage.appName, _appCurrentDumpLines.get(nextMessage.appName));
					_appCurrentDumpLines.remove(nextMessage.appName);
				}

				// log message if skippingLines &&
			} while (_appLastDumpLines.containsKey(nextMessage.appName) &&
					 _appCurrentDumpLines.containsKey(nextMessage.appName) &&
					 _appCurrentDumpLines.get(nextMessage.appName) < _appLastDumpLines.get(nextMessage.appName));

		} catch (IOException ioE) {
			System.err.println(ioE.getMessage());
			nextMessage = null;
		} catch (NullPointerException npE) {
			System.err.println(npE.getMessage());
			nextMessage = null;
		}

		return nextMessage;
	}

	// Returns the pathname with everything up to and including the last '/' removed.
	private static String baseName(String pathName) {
		int i = pathName.lastIndexOf('/');
		return i < 0 ? pathName : pathName.substring(i+1);
	}
}
