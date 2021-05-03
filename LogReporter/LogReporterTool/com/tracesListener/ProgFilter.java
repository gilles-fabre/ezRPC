package tracesListener;

import java.awt.Color;
import java.util.prefs.Preferences;

public class ProgFilter {
	String name = null;
	Color color = null;
	int pid = -1;
	
	/**
	 * @param name
	 */
	public ProgFilter(String name, Color color) {
		super();
		this.name = name;
		this.color = color;
		pid = -1;
	}
	
	public ProgFilter(Preferences aNode) {
		super();
		loadPrefs(aNode);
	}
	
	public String getName() {
		return name;
	}
	
	public void setName(String name) {
		this.name = name;
	}
	
	public Color getColor() {
		return color;
	}
	
	public void setColor(Color color) {
		this.color = color;
	}

	public int getPid() {
		return pid;
	}

	public void setPid(int pid) {
		this.pid = pid;
	}

	public boolean nameMatches(String progName) {
		return this.name.equals(progName);
	}
	
	public boolean pidMatches(int progPid) {
		return pid == progPid;
	}
	
	public void loadPrefs(Preferences aNode) {
		name = aNode.get("name", "");
		color = new Color(ByteUtil.parseHexInteger(aNode.get("color", "0x00000000")));
		pid = -1;
	}
	
	public void savePrefs(Preferences aNode) {
		aNode.put("name", name);
		aNode.put("color", ByteUtil.formatHexInteger(color.getRGB()));
	}

}
