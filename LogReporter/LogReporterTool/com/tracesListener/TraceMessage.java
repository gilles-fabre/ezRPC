package tracesListener;


public class TraceMessage {
	public static final byte _Text = 0;
	public static final byte _TextLine = 1;
	public static final byte _Binary = 2;
	public static final byte _StartDump = 3;
	public static final byte _EndDump = 4;

	public byte tracesVersionMajor, tracesVersionMinor, indentationLevel, msgType, msgData[];
	public int msgDataSize, processId, threadId, traceModule, traceDataSize, traceDataIndex;
	public long timeStamp; 
	public String appName;

	/**
	 * @param tracesVersionMajor
	 * @param tracesVersionMinor
	 * @param dataSize
	 */
	public TraceMessage(byte tracesVersionMajor, byte tracesVersionMinor, int msgDataSize) {
		super();
		this.tracesVersionMajor = tracesVersionMajor;
		this.tracesVersionMinor = tracesVersionMinor;
		this.msgDataSize = msgDataSize <= 0 ? 0 : msgDataSize;
		this.msgData = new byte[this.msgDataSize];
	}

	private String getHexString() {
		return ByteUtil.formatHexByteBuffer(msgData,traceDataIndex,traceDataSize);
	}

	public String getDataString() {
		if (msgType == _Binary)
			return getHexString();

		String log = new String();
		for (int i = 0; i < indentationLevel; i++)
			log += "  ";

		log += new String(msgData,traceDataIndex,traceDataSize);
		return log;
	}
}
