package tracesListener;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.prefs.Preferences;

@SuppressWarnings("deprecation")
public class TracesFilter {
	public static final String globalFiltersKey = "globalFilters";

	/**
	 * TracesFilter message matching rules for PIDs and sysclass modules
	 * - ProcessId or ModuleId matches if one of the list item matches, or if the the list is empty
	 * - Message matches if ProcessId and ModuleId matches.
	 *
	 * modFilters is a list of ModuleFilters
	 * pidFilters is a list of Integers
	 */

	// Our mega-filter is composed of three lists of elementary filters.
	ArrayList<Integer>      pidFilters  = new ArrayList<Integer>();      // process id filters
	ArrayList<ModuleFilter> modFilters  = new ArrayList<ModuleFilter>(); // module id filters
	ArrayList<ProgFilter>   progFilters = new ArrayList<ProgFilter>();   // program name filters

	// The elementary filters matched by the most recent match.
	public ModuleFilter matchModFilter = null;
	public Integer matchPidFilter = null;
	public ProgFilter matchProgFilter = null;

	// The color associated with the the most recent match.
	public Color matchColor = Color.BLACK;

	public TracesFilter() {
		super();
	}

	//----- adders

	public void addProcessIdFilter(int aFilter) {
		pidFilters.add(new Integer(aFilter));
	}

	public void addModuleIdFilter(ModuleFilter aFilter) {
		modFilters.add(aFilter);
	}

	public void addProgFilter(ProgFilter aFilter) {
		progFilters.add(aFilter);
	}

	//----- getters

	public int getProcessIdFilter(int index) {
		return ((Integer)pidFilters.get(index)).intValue();
	}

	public ModuleFilter getModuleIdFilter(int index) {
		return (ModuleFilter)modFilters.get(index);
	}

	public ProgFilter getProgFilter(int index) {
		return (ProgFilter)progFilters.get(index);
	}

	//----- setters

	public void setProcessIdFilter(int index, int aFilter) {
		pidFilters.set(index, new Integer(aFilter));
	}

	public void setModuleIdFilter(int index, ModuleFilter aFilter) {
		modFilters.set(index, aFilter);
	}

	public void setProgFilter(int index, ProgFilter aFilter) {
		progFilters.set(index, aFilter);
	}

	//----- removers

	public void removeProcessIdFilter(int index) {
		pidFilters.remove(index);
	}

	public void removeModuleIdFilter(int index) {
		modFilters.remove(index);
	}

	public void removeProgFilter(int index) {
		progFilters.remove(index);
	}

	//----- getters by name

	public int indexOfModuleIdFilterByName(String moduleId) {
		for (int i=0; i<modFilters.size(); i++)
			if (((ModuleFilter)modFilters.get(i)).getName().equals(moduleId))
				return i;
		return -1;
	}

	public ModuleFilter getModuleIdFilterByName(String moduleId) {
		int index = indexOfModuleIdFilterByName(moduleId);
		if (index == -1)
			return null;
		return getModuleIdFilter(index);
	}

	public int indexOfProgFilterByName(String progName) {
		for (int i=0; i<progFilters.size(); i++)
			if (((ProgFilter)progFilters.get(i)).getName().equals(progName))
				return i;
		return -1;
	}

	public ProgFilter getProgFilterByName(String progName) {
		int index = indexOfProgFilterByName(progName);
		return index == -1 ? null : getProgFilter(index);
	}

	//----- counters

	public int nProcessIdFilters() {
		return pidFilters.size();
	}

	public int nModuleIdFilters() {
		return modFilters.size();
	}

	public int nProgFilters() {
		return progFilters.size();
	}

	//----- matchers

	private boolean processIdMatches(int processId) {
		matchPidFilter = null;
		if (pidFilters.isEmpty())
			return true;
		Integer pId;
		Iterator<Integer> iterator = pidFilters.iterator();
		while (iterator.hasNext()) {
			pId = (Integer) iterator.next();
			if (pId.intValue() == processId) {
				matchPidFilter = pId;
				return true;
			}
		}
		return false;
	}

	private boolean moduleIdMatches(int moduleId) {
		matchModFilter = null;
		if (modFilters.isEmpty())
			return true;
		ModuleFilter mId;
		Iterator<ModuleFilter> iterator = modFilters.iterator();
		while (iterator.hasNext()) {
			mId = (ModuleFilter) iterator.next();
			if (mId.moduleIdMatches(moduleId)) {
				matchModFilter = mId;
				matchColor = mId.color;
				return true;
			}
		}
		return false;
	}

	private boolean progMatches(String progName) {
		matchProgFilter = null;
		if (progFilters.isEmpty())
			return true;
		ProgFilter filter;
		Iterator<ProgFilter> iterator = progFilters.iterator();
		while (iterator.hasNext()) {
			filter = (ProgFilter) iterator.next();
			if (filter.nameMatches(progName)) {
				matchProgFilter = filter;
				matchColor = filter.color;
				return true;
			}
		}
		return false;
	}

	//----- message matcher

	public boolean messageMatches(TraceMessage aMessage) {
		matchColor = Color.BLACK;
		return
			progMatches(aMessage.appName) &&
			moduleIdMatches(aMessage.traceModule) &&
			processIdMatches(aMessage.processId);
	}

	public void loadPrefs(Preferences aNode) {
		pidFilters = new ArrayList<Integer>();
		modFilters = new ArrayList<ModuleFilter>();
		progFilters = new ArrayList<ProgFilter>();
		String aPath = aNode.absolutePath();
		int nPid = aNode.getInt("nPidFilters", 0);
		while (pidFilters.size() < nPid) {
			int aPidFilter = aNode.getInt("pidFilter" + pidFilters.size(), 0);
			addProcessIdFilter(aPidFilter);
		}
		int nMod = aNode.getInt("nModFilters", 0);
		while (modFilters.size() < nMod) {
			ModuleFilter aModFilter = new ModuleFilter(aNode.node(aPath + "/modFilter" + modFilters.size()));
			addModuleIdFilter(aModFilter);
		}
		int nProg = aNode.getInt("nProgFilters", 0);
		while (progFilters.size() < nProg) {
			ProgFilter aProgFilter = new ProgFilter(aNode.node(aPath + "/progFilter" + progFilters.size()));
			addProgFilter(aProgFilter);
		}
	}

	public void savePrefs(Preferences aNode) {
		String aPath = aNode.absolutePath();
		aNode.putInt("nPidFilters", pidFilters.size());
		for (int i=0; i<pidFilters.size(); i++)
			aNode.putInt("pidFilter" + i, getProcessIdFilter(i));
		aNode.putInt("nModFilters", modFilters.size());
		for (int i=0; i<modFilters.size(); i++)
			getModuleIdFilter(i).savePrefs(aNode.node(aPath + "/modFilter" + i));
		aNode.putInt("nProgFilters", progFilters.size());
		for (int i=0; i<progFilters.size(); i++)
			getProgFilter(i).savePrefs(aNode.node(aPath + "/progFilter" + i));
	}

	// Sort the module ids filters by their masks, so that more "specific" filters
	// appear before less "specific" filters. We want to guarantee that if mask1
	// "covers" mask2, i.e.
	//    (mask1 != mask2) && ((mask1 & mask2) == mask2)
	// then mask1 precedes mask2 in the resulting list. It is sufficient to sort
	// by the number of one bits in the mask, reversed.
	public void sortModuleFiltersBySpecificity() {
		Collections.sort(modFilters, new Comparator<ModuleFilter>() {
		    public int compare(ModuleFilter mf1, ModuleFilter mf2) {
		        return Integer.bitCount(mf2.modMask) - Integer.bitCount(mf1.modMask);
		    }});
	}
}
