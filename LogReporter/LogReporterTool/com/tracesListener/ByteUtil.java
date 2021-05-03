package tracesListener;

public class ByteUtil {
	private static final byte[] hexDigits = {
		'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
	};
	private static final int BYTES_PER_LINE = 8; // bytes of binary input per formatted line
	private static final int OFFSET_TO_ASCII = (3 * BYTES_PER_LINE) + 2;
	private static final int FORMATTED_CHARS_PER_LINE = OFFSET_TO_ASCII + BYTES_PER_LINE + 1;

	// This produces a "hex dump" string representation of the given bytes.
	// The output string is one or more lines, with 16 bytes printed on each line
	// (except perhaps the last line). Each line shows the bytes in both hex format
	// and ascii, e.g.
	// 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50   ABCDEFGHIJKLMNOP
	public static String formatHexByteBuffer(byte buffer[], int offset, int size) {
		if (size <= 0) {
			return "\n";
		}
		int nLines = (size + (BYTES_PER_LINE-1)) / BYTES_PER_LINE;
		byte[] buf = new byte[nLines * FORMATTED_CHARS_PER_LINE];
		int i = 0; // index into input buffer
		int j = 0; // index into output buf
		for (int row = 0; row < nLines; row++) {
			for (int col = 0; col < BYTES_PER_LINE; col++) {
				byte h1, h2, ascii;
				if (i < size) {
					byte thisByte = buffer[offset + i];
					h1 = hexDigits[(thisByte >> 4) & 0xF];
					h2 = hexDigits[thisByte & 0xF];
					ascii = makePrintable(thisByte);
				} else {
					h1 = ' ';
					h2 = ' ';
					ascii = ' ';
				}
				buf[j + (3*col) + 0] = h1;
				buf[j + (3*col) + 1] = h2;
				buf[j + (3*col) + 2] = ' ';
				buf[j + OFFSET_TO_ASCII + col] = ascii;
				i++;
			}
			buf[j + (OFFSET_TO_ASCII-2)] = ' ';
			buf[j + (OFFSET_TO_ASCII-1)] = ' ';
			buf[j + (FORMATTED_CHARS_PER_LINE-1)] = '\n';
			j += FORMATTED_CHARS_PER_LINE;
		}
		return new String(buf);
	}

	private static byte makePrintable(byte x) {
		int xx = ((int)x) & 0xFF;
		return 0x20 <= xx && xx <= 0x7E ? x : (byte)'.';
	}

	// count number of occurrences of c in buffer in the interval [from..to[
	public static int countOccurrences(byte buffer[], char c, int from, int to) {
		if (buffer == null)
			return 0;
		int count=0;
		while (from < to) {
			if (buffer[from++] == c)
				count++;
		}
		return count;
	}

	public static String formatHexByteBuffer(byte buffer[]) {
		return formatHexByteBuffer(buffer,0,buffer.length);
	}

	public static String formatHexInteger(int anInt) {
		byte[] buf = new byte[10];
		buf[0] = '0';
		buf[1] = 'x';
		for (int i=9; i>=2; i--) {
			buf[i] = hexDigits[anInt & 0xF];
			anInt >>>= 4;
		}
		return new String(buf);
	}

	public static int parseHexInteger(String aString) {
		if (aString.startsWith("0x"))
			return (int)Long.parseLong(aString.substring(2),16);
		return (int)Long.parseLong(aString,16);
	}

	public static int extractInt(byte[] buffer, int offset) {
		return ((0x000000ff & buffer[offset]) << 24)
				| ((0x000000ff & buffer[offset + 1]) << 16)
				| ((0x000000ff & buffer[offset + 2]) << 8)
				| (0x000000ff & buffer[offset + 3]);
	}

	public static long extractLong(byte[] buffer, int offset) {
		return ((0x00000000000000ffL & buffer[offset]) << 56)
				| ((0x00000000000000ffL & buffer[offset + 1]) << 48)
				| ((0x00000000000000ffL & buffer[offset + 2]) << 40)
				| ((0x00000000000000ffL & buffer[offset + 3]) << 32)
				| ((0x00000000000000ffL & buffer[offset + 4]) << 24)
				| ((0x00000000000000ffL & buffer[offset + 5]) << 16)
				| ((0x00000000000000ffL & buffer[offset + 6]) << 8)
				| (0x00000000000000ffL & buffer[offset + 7]);
	}

	public static short extractShort(byte[] buffer, int offset) {
		return (short) (((0x00ff & buffer[offset]) << 8) | ((0x00ff) & buffer[offset + 1]));
	}
}
