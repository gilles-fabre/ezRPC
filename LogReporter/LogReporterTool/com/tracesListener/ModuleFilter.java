package tracesListener;

import java.awt.Color;
import java.util.prefs.Preferences;

public class ModuleFilter {
	int modId, modMask;
	String name;
	Color color;
	
	/**
	 * @param modId
	 * @param modMask
	 * @param name
	 */
	public ModuleFilter(int modId, int modMask, String name) {
		this(modId, modMask, name, Color.BLACK);
	}
	
	/**
	 * @param modId
	 * @param modMask
	 * @param name
	 * @param color
	 */
	public ModuleFilter(int modId, int modMask, String name, Color color) {
		super();
		this.modId = modId;
		this.modMask = modMask;
		this.name = name;
		this.color = color;
	}

	public ModuleFilter(Preferences aNode) {
		super();
		loadPrefs(aNode);
	}
	
	public int getModId() {
		return modId;
	}

	public String getFormattedModId() {
		return ByteUtil.formatHexInteger(modId);
	}

	public int getModMask() {
		return modMask;
	}

	public String getFormattedModMask() {
		return ByteUtil.formatHexInteger(modMask);
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

	public boolean moduleIdMatches(int aModId) {
		return (modId & modMask) == (aModId & modMask);
	}
	
	public void loadPrefs(Preferences aNode) {
		modId = ByteUtil.parseHexInteger(aNode.get("modId", "0x00000000"));
		modMask = ByteUtil.parseHexInteger(aNode.get("modMask", "0xFFFF0000"));
		name = aNode.get("name", "");
		color = new Color(ByteUtil.parseHexInteger(aNode.get("color", "0x00000000")));
	}
	
	public void savePrefs(Preferences aNode) {
		aNode.put("modId", ByteUtil.formatHexInteger(modId));
		aNode.put("modMask", ByteUtil.formatHexInteger(modMask));
		aNode.put("name", name);
		aNode.put("color", ByteUtil.formatHexInteger(color.getRGB()));
	}

}
