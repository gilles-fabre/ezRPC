package reporter;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JDesktopPane;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;
import javax.swing.KeyStroke;
import javax.swing.border.TitledBorder;

public class MainFrameVE extends JFrame {
	protected static final long serialVersionUID = 1L;
	protected JMenuBar mainMenuBar = null;
	protected JMenu fileMenu = null;
	protected JMenuItem exitMI = null;
	protected JMenuItem settingsMI = null;
	protected JMenuItem importMI = null;
	protected JMenuItem exportMI = null;
	protected JMenuItem saveMI = null;
	protected JMenu paletteMenu = null;
	protected JMenuItem newModIdMI = null;
	protected JMenuItem editModIdMI = null;
	protected JMenuItem  copyModIdMI = null;
	protected JMenuItem deleteModIdMI = null;
	protected JMenuItem newProgMI = null;
	protected JMenuItem editProgMI = null;
	protected JMenuItem deleteProgMI = null;
	protected JMenu editMenu = null;
	protected JMenuItem clearWindowMI = null;
	protected JMenuItem appendSeparatorMI = null;
	protected JMenuItem findMI = null;
	protected JMenuItem gotoMI = null;
	protected JMenuItem copyTextMI = null;
	protected JMenuItem selectAllMI = null;
	protected JMenuItem copyProgMI = null;
	protected JMenu viewMenu = null;
	protected JMenu windowMenu = null;
	protected JMenuItem newWindowMI = null;
	protected JMenuItem clearAllMI = null;
	protected JMenuItem closeAllMI = null;
	protected JMenuItem renameWindowMI = null;
	protected JMenu selectWindowMenu = null;
	protected JMenuItem selectNextWindowMI = null;
	protected JMenu helpMenu = null;
	protected JMenuItem memUsageMI = null;
	protected JMenuItem aboutMI = null;
	protected JSplitPane splitPane = null;
	protected JDesktopPane desktopPane = null;
	protected JPopupMenu modIdTablePopupMenu = null;
	protected JMenuItem newModIdPUMI = null;
	protected JMenuItem editModIdPUMI = null;
	protected JMenuItem copyModIdPUMI = null;
	protected JMenuItem deleteModIdPUMI = null;
	protected JPopupMenu progTablePopupMenu = null;
	protected JMenuItem newProgPUMI = null;
	protected JMenuItem editProgPUMI = null;
	protected JMenuItem copyProgPUMI = null;
	protected JMenuItem deleteProgPUMI = null;
	protected JMenuItem cascadeMI = null;
	protected JMenuItem tileMI = null;
	protected JMenuItem hideAllFiltersMI = null;
	protected JMenuItem showAllFiltersMI = null;
	protected JMenuItem traceBgColorMI = null;
	protected JMenuItem lookAndFeelMI = null;
	protected JCheckBoxMenuItem showModuleIdMI = null;
	protected JCheckBoxMenuItem showTimestampMI = null;
	protected JCheckBoxMenuItem showProcessIdMI = null;
	protected JCheckBoxMenuItem showThreadIdMI = null;
	protected JCheckBoxMenuItem showBinaryMI = null;
	protected JCheckBoxMenuItem showFilterNameMI = null;
	protected JCheckBoxMenuItem showWindowPerProcessMI = null;
	protected JSplitPane splitPane1 = null;
	protected JPanel modIdPanel = null;
	protected JScrollPane modIdScrollPane = null;
	protected ModIdTable modIdTable = null;
	protected JPanel progPanel = null;
	protected JScrollPane progScrollPane = null;
	protected ProgTable progTable = null;
	protected JPanel rootPanel = null;
	protected JPanel statusBarPanel = null;
	protected JLabel windowInfoLabel = null;
	protected JLabel pendingTracesLabel = null;
	protected JToolBar toolbar = null;
	protected JToggleButton playButton = null;
	protected JToggleButton pauseButton = null;
	protected JToggleButton stopButton = null;
	protected JToggleButton showTimestampBttn = null;
	protected JToggleButton showProcessIdBttn = null;
	protected JToggleButton showThreadIdBttn = null;
	protected JToggleButton showModuleIdBttn = null;
	protected JToggleButton showFilterNameBttn = null;
	protected JToggleButton showBinaryBttn = null;

	/**
	 * This is the default constructor
	 */
	public MainFrameVE() {
		super();
		initialize();
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		this.setName("mainFrame");
		this.setIconImage(Toolkit.getDefaultToolkit().getImage(getClass().getResource("log-reporter.png")));
		this.setContentPane(getRootPanel());
		this.setSize(new Dimension(373, 274));
		this.setJMenuBar(getMainMenuBar());
		this.getFilterTablePopupMenu();
		this.getProgTablePopupMenu();
	}

	/**
	 * This method initializes mainMenuBar
	 *
	 * @return javax.swing.JMenuBar
	 */
	private JMenuBar getMainMenuBar() {
		if (mainMenuBar == null) {
			mainMenuBar = new JMenuBar();
			mainMenuBar.add(getFileMenu());
			mainMenuBar.add(getPaletteMenu());
			mainMenuBar.add(getViewMenu());
			mainMenuBar.add(getEditMenu());
			mainMenuBar.add(getWindowMenu());
			mainMenuBar.add(getHelpMenu());
		}
		return mainMenuBar;
	}

	/**
	 * This method initializes fileMenu
	 *
	 * @return javax.swing.JMenu
	 */
	private JMenu getFileMenu() {
		if (fileMenu == null) {
			fileMenu = new JMenu();
			fileMenu.setText(Strings.getString("fileMenu"));
			fileMenu.add(getImportMI());
			fileMenu.add(getExportMI());
			fileMenu.addSeparator();
			fileMenu.add(getSaveMI());
			fileMenu.addSeparator();
			fileMenu.add(getExitMI());
		}
		return fileMenu;
	}

	/**
	 * This method initializes exitMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getExitMI() {
		if (exitMI == null) {
			exitMI = new JMenuItem();
			exitMI.setText(Strings.getString("exitMI"));
		}
		return exitMI;
	}

	/**
	 * This method initializes settingsMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getSettingsMI() {
		if (settingsMI == null) {
			settingsMI = new JMenuItem();
			settingsMI.setText(Strings.getString("settingsMI"));
		}
		return settingsMI;
	}

	/**
	 * This method initializes importMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getImportMI() {
		if (importMI == null) {
			importMI = new JMenuItem();
			importMI.setText(Strings.getString("importMI"));
		}
		return importMI;
	}

	/**
	 * This method initializes exportMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getExportMI() {
		if (exportMI == null) {
			exportMI = new JMenuItem();
			exportMI.setText(Strings.getString("exportMI"));
		}
		return exportMI;
	}

	/**
	 * This method initializes saveMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getSaveMI() {
		if (saveMI == null) {
			saveMI = new JMenuItem();
			saveMI.setText(Strings.getString("saveWindowMI"));
			saveMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, InputEvent.CTRL_DOWN_MASK, false));
		}
		return saveMI;
	}

	/**
	 * This method initializes paletteMenu
	 *
	 * @return javax.swing.JMenu
	 */
	private JMenu getPaletteMenu() {
		if (paletteMenu == null) {
			paletteMenu = new JMenu();
			paletteMenu.setText(Strings.getString("paletteMenu"));
			paletteMenu.add(getNewFilterMI());
			paletteMenu.add(getEditFilterMI());
			paletteMenu.add(getCopyFilterMI());
			paletteMenu.add(getDeleteFilterMI());
			paletteMenu.addSeparator();
			paletteMenu.add(getNewProgMI());
			paletteMenu.add(getEditProgMI());
			paletteMenu.add(getCopyProgMI());
			paletteMenu.add(getDeleteProgMI());
		}
		return paletteMenu;
	}

	/**
	 * This method initializes newModIdMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getNewFilterMI() {
		if (newModIdMI == null) {
			newModIdMI = new JMenuItem();
			newModIdMI.setText(Strings.getString("newModIdMI"));
		}
		return newModIdMI;
	}

	/**
	 * This method initializes editModIdMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getEditFilterMI() {
		if (editModIdMI == null) {
			editModIdMI = new JMenuItem();
			editModIdMI.setText(Strings.getString("editModIdMI"));
		}
		return editModIdMI;
	}

	/**
	 * This method initializes copyModIdMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getCopyFilterMI() {
		if (copyModIdMI == null) {
			copyModIdMI = new JMenuItem();
			copyModIdMI.setText(Strings.getString("copyModIdMI"));
		}
		return copyModIdMI;
	}

	/**
	 * This method initializes deleteModIdMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getDeleteFilterMI() {
		if (deleteModIdMI == null) {
			deleteModIdMI = new JMenuItem();
			deleteModIdMI.setText(Strings.getString("deleteModIdMI"));
		}
		return deleteModIdMI;
	}

	/**
	 * This method initializes newProgMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getNewProgMI() {
		if (newProgMI == null) {
			newProgMI = new JMenuItem();
			newProgMI.setText(Strings.getString("newProgMI"));
		}
		return newProgMI;
	}

	/**
	 * This method initializes editProgMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getEditProgMI() {
		if (editProgMI == null) {
			editProgMI = new JMenuItem();
			editProgMI.setText(Strings.getString("editProgMI"));
		}
		return editProgMI;
	}

	/**
	 * This method initializes copyProgMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getCopyProgMI() {
		if (copyProgMI == null) {
			copyProgMI = new JMenuItem();
			copyProgMI.setText(Strings.getString("copyProgMI"));
		}
		return copyProgMI;
	}

	/**
	 * This method initializes deleteProgMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getDeleteProgMI() {
		if (deleteProgMI == null) {
			deleteProgMI = new JMenuItem();
			deleteProgMI.setText(Strings.getString("deleteProgMI"));
		}
		return deleteProgMI;
	}

	/**
	 * This method initializes newModIdPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getNewFilterPUMI() {
		if (newModIdPUMI == null) {
			newModIdPUMI = new JMenuItem();
			newModIdPUMI.setText(Strings.getString("newModIdMI"));
		}
		return newModIdPUMI;
	}

	/**
	 * This method initializes editModIdPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getEditFilterPUMI() {
		if (editModIdPUMI == null) {
			editModIdPUMI = new JMenuItem();
			editModIdPUMI.setText(Strings.getString("editModIdMI"));
		}
		return editModIdPUMI;
	}

	/**
	 * This method initializes copyModIdPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getCopyFilterPUMI() {
		if (copyModIdPUMI == null) {
			copyModIdPUMI = new JMenuItem();
			copyModIdPUMI.setText(Strings.getString("copyModIdMI"));
		}
		return copyModIdPUMI;
	}

	/**
	 * This method initializes deleteModIdPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getDeleteFilterPUMI() {
		if (deleteModIdPUMI == null) {
			deleteModIdPUMI = new JMenuItem();
			deleteModIdPUMI.setText(Strings.getString("deleteModIdMI"));
		}
		return deleteModIdPUMI;
	}

	/**
	 * This method initializes newProgPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getNewProgPUMI() {
		if (newProgPUMI == null) {
			newProgPUMI = new JMenuItem();
			newProgPUMI.setText(Strings.getString("newProgMI"));
		}
		return newProgPUMI;
	}

	/**
	 * This method initializes editProgPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getEditProgPUMI() {
		if (editProgPUMI == null) {
			editProgPUMI = new JMenuItem();
			editProgPUMI.setText(Strings.getString("editProgMI"));
		}
		return editProgPUMI;
	}

	/**
	 * This method initializes copyProgPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getCopyProgPUMI() {
		if (copyProgPUMI == null) {
			copyProgPUMI = new JMenuItem();
			copyProgPUMI.setText(Strings.getString("copyProgMI"));
		}
		return copyProgPUMI;
	}

	/**
	 * This method initializes deleteProgPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getDeleteProgPUMI() {
		if (deleteProgPUMI == null) {
			deleteProgPUMI = new JMenuItem();
			deleteProgPUMI.setText(Strings.getString("deleteProgMI"));
		}
		return deleteProgPUMI;
	}

	/**
	 * This method initializes editMenu
	 *
	 * @return javax.swing.JMenu
	 */
	private JMenu getEditMenu() {
		if (editMenu == null) {
			editMenu = new JMenu();
			editMenu.setText(Strings.getString("editMenu"));
			editMenu.add(getClearWindowMI());
			editMenu.add(getAppendSeparatorMI());
			editMenu.add(getFindMI());
			editMenu.add(getGotoMI());
			editMenu.add(getSelectAllMI());
			editMenu.add(getCopyTextMI());
		}
		return editMenu;
	}

	/**
	 * This method initializes appendSeparatorMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getAppendSeparatorMI() {
		if (appendSeparatorMI == null) {
			appendSeparatorMI = new JMenuItem();
			appendSeparatorMI.setText(Strings.getString("appendSeparatorMI"));
			appendSeparatorMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_L, InputEvent.CTRL_DOWN_MASK, false));
		}
		return appendSeparatorMI;
	}

	/**
	 * This method initializes findMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getFindMI() {
		if (findMI == null) {
			findMI = new JMenuItem();
			findMI.setText(Strings.getString("findMI"));
			findMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F, InputEvent.CTRL_DOWN_MASK, false));
		}
		return findMI;
	}

	/**
	 * This method initializes gotoMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getGotoMI() {
		if (gotoMI == null) {
			gotoMI = new JMenuItem();
			gotoMI.setText(Strings.getString("gotoMI"));
			gotoMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, InputEvent.CTRL_DOWN_MASK, false));
		}
		return gotoMI;
	}

	/**
	 * This method initializes selectAllPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getSelectAllMI() {
		if (selectAllMI == null) {
			selectAllMI = new JMenuItem();
			selectAllMI.setText(Strings.getString("selectAllMI"));
			selectAllMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, InputEvent.CTRL_DOWN_MASK, false));
		}
		return selectAllMI;
	}

	/**
	 * This method initializes copyTextMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getCopyTextMI() {
		if (copyTextMI == null) {
			copyTextMI = new JMenuItem();
			copyTextMI.setText(Strings.getString("copyTextMI"));
			copyTextMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, InputEvent.CTRL_DOWN_MASK, false));
		}
		return copyTextMI;
	}

	/**
	 * This method initializes viewMenu
	 *
	 * @return javax.swing.JMenu
	 */
	private JMenu getViewMenu() {
		if (viewMenu == null) {
			viewMenu = new JMenu();
			viewMenu.setText(Strings.getString("viewMenu"));
			viewMenu.add(getShowTimestampMI());
			viewMenu.add(getShowProcessIdMI());
			viewMenu.add(getShowThreadIdMI());
			viewMenu.add(getShowModuleIdMI());
			viewMenu.add(getShowBinaryMI());
			viewMenu.add(getShowFilterNameMI());
			viewMenu.addSeparator();
			viewMenu.add(getShowWindowPerProcessMI());
			viewMenu.addSeparator();
			viewMenu.add(getTraceBgColorMI());
			viewMenu.add(getLookAndFeelMI());
		}
		return viewMenu;
	}

	/**
	 * This method initializes windowMenu
	 *
	 * @return javax.swing.JMenu
	 */
	private JMenu getWindowMenu() {
		if (windowMenu == null) {
			windowMenu = new JMenu();
			windowMenu.setText(Strings.getString("windowMenu"));
			windowMenu.add(getNewWindowMI());
			windowMenu.add(getRenameWindowMI());
			windowMenu.add(getSelectWindowMenu());
			windowMenu.add(getSelectNextWindowMI());
			windowMenu.addSeparator();
			windowMenu.add(getCascadeMI());
			windowMenu.add(getTileMI());
			windowMenu.add(getHideAllFiltersMI());
			windowMenu.add(getShowAllFiltersMI());
			windowMenu.addSeparator();
			windowMenu.add(getClearAllMI());
			windowMenu.add(getCloseAllMI());
			windowMenu.addSeparator();
			windowMenu.add(getSettingsMI());
		}
		return windowMenu;
	}

	/**
	 * This method initializes newWindowMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getNewWindowMI() {
		if (newWindowMI == null) {
			newWindowMI = new JMenuItem();
			newWindowMI.setText(Strings.getString("newWindowMI"));
			newWindowMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, InputEvent.CTRL_DOWN_MASK, false));
		}
		return newWindowMI;
	}

	/**
	 * This method initializes helpMenu
	 *
	 * @return javax.swing.JMenu
	 */
	private JMenu getHelpMenu() {
		if (helpMenu == null) {
			helpMenu = new JMenu();
			helpMenu.setText(Strings.getString("helpMenu"));
			helpMenu.add(getMemUsageMI());
			helpMenu.add(getAboutMI());
		}
		return helpMenu;
	}

	/**
	 * This method initializes memUsageMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getMemUsageMI() {
		if (memUsageMI == null) {
			memUsageMI = new JMenuItem();
			memUsageMI.setText(Strings.getString("memUsageMI"));
		}
		return memUsageMI;
	}

	/**
	 * This method initializes aboutMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getAboutMI() {
		if (aboutMI == null) {
			aboutMI = new JMenuItem();
			aboutMI.setText(Strings.getString("aboutMI"));
		}
		return aboutMI;
	}

	/**
	 * This method initializes clearWindowMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getClearWindowMI() {
		if (clearWindowMI == null) {
			clearWindowMI = new JMenuItem();
			clearWindowMI.setText(Strings.getString("clearWindowMI"));
			clearWindowMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, InputEvent.ALT_DOWN_MASK, false));
		}
		return clearWindowMI;
	}

	/**
	 * This method initializes clearAllMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getClearAllMI() {
		if (clearAllMI == null) {
			clearAllMI = new JMenuItem();
			clearAllMI.setText(Strings.getString("clearAllWindowsMI"));
			clearAllMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, InputEvent.ALT_DOWN_MASK | InputEvent.CTRL_DOWN_MASK, false));
		}
		return clearAllMI;
	}

	/**
	 * This method initializes closeAllMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getCloseAllMI() {
		if (closeAllMI == null) {
			closeAllMI = new JMenuItem();
			closeAllMI.setText(Strings.getString("closeAllWindowsMI"));
		}
		return closeAllMI;
	}

	/**
	 * This method initializes selectWindowMenu
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenu getSelectWindowMenu() {
		if (selectWindowMenu == null) {
			selectWindowMenu = new JMenu();
			selectWindowMenu.setText(Strings.getString("selectWindowMenu"));
		}
		return selectWindowMenu;
	}

	/**
	 * This method initializes selectNextWindowMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getSelectNextWindowMI() {
		if (selectNextWindowMI == null) {
			selectNextWindowMI = new JMenuItem();
			selectNextWindowMI.setText(Strings.getString("selectNextWindowMI"));
			selectNextWindowMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, InputEvent.ALT_DOWN_MASK | InputEvent.CTRL_DOWN_MASK, false));
		}
		return selectNextWindowMI;
	}

	/**
	 * This method initializes renameWindowMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getRenameWindowMI() {
		if (renameWindowMI == null) {
			renameWindowMI = new JMenuItem();
			renameWindowMI.setText(Strings.getString("renameWindowMI"));
		}
		return renameWindowMI;
	}

	/**
	 * This method initializes modIdTablePopupMenu
	 *
	 * @return javax.swing.JPopupMenu
	 */
	private JPopupMenu getFilterTablePopupMenu() {
		if (modIdTablePopupMenu == null) {
			modIdTablePopupMenu = new JPopupMenu();
			modIdTablePopupMenu.add(getNewFilterPUMI());
			modIdTablePopupMenu.add(getEditFilterPUMI());
			modIdTablePopupMenu.add(getCopyFilterPUMI());
			modIdTablePopupMenu.add(getDeleteFilterPUMI());
		}
		return modIdTablePopupMenu;
	}

	/**
	 * This method initializes progTablePopupMenu
	 *
	 * @return javax.swing.JPopupMenu
	 */
	private JPopupMenu getProgTablePopupMenu() {
		if (progTablePopupMenu == null) {
			progTablePopupMenu = new JPopupMenu();
			progTablePopupMenu.add(getNewProgPUMI());
			progTablePopupMenu.add(getEditProgPUMI());
			progTablePopupMenu.add(getCopyProgPUMI());
			progTablePopupMenu.add(getDeleteProgPUMI());
		}
		return progTablePopupMenu;
	}

	/**
	 * This method initializes splitPane
	 *
	 * @return javax.swing.JSplitPane
	 */
	private JSplitPane getSplitPane() {
		if (splitPane == null) {
			splitPane = new JSplitPane();
			splitPane.setDividerLocation(175);
			splitPane.setDividerSize(10);
			splitPane.setContinuousLayout(true);
			splitPane.setOneTouchExpandable(true);
			splitPane.setLeftComponent(getSplitPane1());
			splitPane.setRightComponent(getDesktopPane());
		}
		return splitPane;
	}

	/**
	 * This method initializes desktopPane
	 *
	 * @return javax.swing.JDesktopPane
	 */
	JDesktopPane getDesktopPane() {
		if (desktopPane == null) {
			desktopPane = new JDesktopPane();
			desktopPane.setName("desktopPane");
		}
		return desktopPane;
	}

	/**
	 * This method initializes cascadeMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getCascadeMI() {
		if (cascadeMI == null) {
			cascadeMI = new JMenuItem();
			cascadeMI.setText(Strings.getString("cascadeWindowsMI"));
		}
		return cascadeMI;
	}

	/**
	 * This method initializes tileMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getTileMI() {
		if (tileMI == null) {
			tileMI = new JMenuItem();
			tileMI.setText(Strings.getString("tileWindowsMI"));
		}
		return tileMI;
	}

	/**
	 * This method initializes hideAllFiltersMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getHideAllFiltersMI() {
		if (hideAllFiltersMI == null) {
			hideAllFiltersMI = new JMenuItem();
			hideAllFiltersMI.setText(Strings.getString("hideAllFiltersMI"));
		}
		return hideAllFiltersMI;
	}

	/**
	 * This method initializes showAllFiltersMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getShowAllFiltersMI() {
		if (showAllFiltersMI == null) {
			showAllFiltersMI = new JMenuItem();
			showAllFiltersMI.setText(Strings.getString("showAllFiltersMI"));
		}
		return showAllFiltersMI;
	}

	/**
	 * This method initializes traceBgColorMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getTraceBgColorMI() {
		if (traceBgColorMI == null) {
			traceBgColorMI = new JMenuItem();
			traceBgColorMI.setText(Strings.getString("traceBgColorMI"));
		}
		return traceBgColorMI;
	}

	/**
	 * This method initializes lookAndFeelMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getLookAndFeelMI() {
		if (lookAndFeelMI == null) {
			lookAndFeelMI = new JMenuItem();
			lookAndFeelMI.setText(Strings.getString("lookAndFeelMI"));
		}
		return lookAndFeelMI;
	}

	/**
	 * This method initializes showModuleIdMI
	 *
	 * @return javax.swing.JCheckBoxMenuItem
	 */
	protected JCheckBoxMenuItem getShowModuleIdMI() {
		if (showModuleIdMI == null) {
			showModuleIdMI = new JCheckBoxMenuItem();
			showModuleIdMI.setText(Strings.getString("showModuleIdMI"));
		}
		return showModuleIdMI;
	}

	/**
	 * This method initializes showTimestampMI
	 *
	 * @return javax.swing.JCheckBoxMenuItem
	 */
	protected JCheckBoxMenuItem getShowTimestampMI() {
		if (showTimestampMI == null) {
			showTimestampMI = new JCheckBoxMenuItem();
			showTimestampMI.setText(Strings.getString("showTimestampMI"));
		}
		return showTimestampMI;
	}

	/**
	 * This method initializes showProcessIdMI
	 *
	 * @return javax.swing.JCheckBoxMenuItem
	 */
	protected JCheckBoxMenuItem getShowProcessIdMI() {
		if (showProcessIdMI == null) {
			showProcessIdMI = new JCheckBoxMenuItem();
			showProcessIdMI.setText(Strings.getString("showProcessIdMI"));
		}
		return showProcessIdMI;
	}

	/**
	 * This method initializes showThreadIdMI
	 *
	 * @return javax.swing.JCheckBoxMenuItem
	 */
	protected JCheckBoxMenuItem getShowThreadIdMI() {
		if (showThreadIdMI == null) {
			showThreadIdMI = new JCheckBoxMenuItem();
			showThreadIdMI.setText(Strings.getString("showThreadIdMI"));
		}
		return showThreadIdMI;
	}

	/**
	 * This method initializes splitPane1
	 *
	 * @return javax.swing.JSplitPane
	 */
	private JSplitPane getSplitPane1() {
		if (splitPane1 == null) {
			splitPane1 = new JSplitPane();
			splitPane1.setOrientation(JSplitPane.VERTICAL_SPLIT);
			splitPane1.setDividerLocation(300);
			splitPane1.setDividerSize(10);
			splitPane1.setContinuousLayout(true);
			splitPane1.setOneTouchExpandable(true);
			splitPane1.setTopComponent(getFilterPanel());
			splitPane1.setBottomComponent(getProgPanel());
		}
		return splitPane1;
	}

	/**
	 * This method initializes modIdPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getFilterPanel() {
		if (modIdPanel == null) {
			modIdPanel = new JPanel();
			modIdPanel.setLayout(new BorderLayout());
			modIdPanel.setMinimumSize(new Dimension(0, 25));
			modIdPanel.setToolTipText(Strings.getString("modIdFilterPaletteTip"));
			modIdPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEmptyBorder(1, 0, 0, 0), Strings.getString("modIdFilterPalette"), TitledBorder.CENTER, TitledBorder.BELOW_TOP, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			modIdPanel.add(getModIdScrollPane(), getModIdScrollPane().getName());
		}
		return modIdPanel;
	}

	/**
	 * This method initializes modIdScrollPane
	 *
	 * @return javax.swing.JScrollPane
	 */
	private JScrollPane getModIdScrollPane() {
		if (modIdScrollPane == null) {
			modIdScrollPane = new JScrollPane();
			modIdScrollPane.setMinimumSize(new Dimension(0, 0));
			modIdScrollPane.setViewportView(getModIdTable());
		}
		return modIdScrollPane;
	}

	/**
	 * This method initializes modIdTable
	 *
	 * @return com.reporter.ModIdTable
	 */
	private ModIdTable getModIdTable() {
		if (modIdTable == null) {
			modIdTable = new ModIdTable(new ModIdTableModel());
		}
		return modIdTable;
	}

	/**
	 * This method initializes progPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getProgPanel() {
		if (progPanel == null) {
			progPanel = new JPanel();
			progPanel.setLayout(new BorderLayout());
			progPanel.setMinimumSize(new Dimension(0, 25));
			progPanel.setToolTipText(Strings.getString("progFilterPaletteTip"));
			progPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEmptyBorder(1, 0, 0, 0), Strings.getString("progFilterPalette"), TitledBorder.CENTER, TitledBorder.BELOW_TOP, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			progPanel.add(getProgScrollPanel(), getProgScrollPanel().getName());
		}
		return progPanel;
	}

	/**
	 * This method initializes progScrollPanel
	 *
	 * @return javax.swing.JScrollPane
	 */
	private JScrollPane getProgScrollPanel() {
		if (progScrollPane == null) {
			progScrollPane = new JScrollPane();
			progScrollPane.setMinimumSize(new Dimension(0, 0));
			progScrollPane.setViewportView(getProgTable());
		}
		return progScrollPane;
	}

	/**
	 * This method initializes progTable
	 *
	 * @return com.reporter.ProgTable
	 */
	private ProgTable getProgTable() {
		if (progTable == null) {
			progTable = new ProgTable(new ProgTableModel());
		}
		return progTable;
	}

	/**
	 * This method initializes rootPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getRootPanel() {
		if (rootPanel == null) {
			rootPanel = new JPanel(new BorderLayout());
			rootPanel.add(getToolbar(), BorderLayout.PAGE_START);
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.gridx = 0;
			gridBagConstraints1.weightx = 1.0D;
			gridBagConstraints1.weighty = 0.0D;
			gridBagConstraints1.anchor = GridBagConstraints.SOUTH;
			gridBagConstraints1.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints1.insets = new Insets(0, 5, 0, 5);
			gridBagConstraints1.gridy = 1;
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.fill = GridBagConstraints.BOTH;
			gridBagConstraints.weighty = 1.0D;
			gridBagConstraints.anchor = GridBagConstraints.NORTH;
			gridBagConstraints.gridx = 0;
			gridBagConstraints.gridy = 0;
			gridBagConstraints.weightx = 1.0;
			JPanel innerPanel = new JPanel(new GridBagLayout());
			innerPanel.setSize(new Dimension(300, 260));
			innerPanel.add(getSplitPane(), gridBagConstraints);
			innerPanel.add(getStatusBarPanel(), gridBagConstraints1);
			rootPanel.add(innerPanel, BorderLayout.CENTER);
		}
		return rootPanel;
	}

	/**
	 * This method initializes playButton
	 *
	 * @return javax.swing.JButton
	 */
	private JToggleButton getPlayButton() {
		if (playButton == null) {
			playButton = new JToggleButton();
			playButton.setFocusPainted(false);
			playButton.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("play.png"))));
			playButton.setToolTipText(Strings.getString("playButtonTip"));
		}

		return playButton;
	}

	/**
	 * This method initializes pauseButton
	 *
	 * @return javax.swing.JButton
	 */
	private JToggleButton getPauseButton() {
		if (pauseButton == null) {
			pauseButton = new JToggleButton();
			pauseButton.setFocusPainted(false);
			pauseButton.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("pause.png"))));
			pauseButton.setToolTipText(Strings.getString("pauseButtonTip"));
		}

		return pauseButton;
	}

	/**
	 * This method initializes stopButton
	 *
	 * @return javax.swing.JButton
	 */
	private JToggleButton getStopButton() {
		if (stopButton == null) {
			stopButton = new JToggleButton();
			stopButton.setFocusPainted(false);
			stopButton.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("stop.png"))));
			stopButton.setToolTipText(Strings.getString("stopButtonTip"));
		}

		return stopButton;
	}

	/**
	 * This method initializes binaryButton
	 *
	 * @return javax.swing.JButton
	 */
	protected JToggleButton getShowBinaryBttn() {
		if (showBinaryBttn == null) {
			showBinaryBttn = new JToggleButton();
			showBinaryBttn.setFocusPainted(false);
			showBinaryBttn.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("binary.png"))));
			showBinaryBttn.setToolTipText(Strings.getString("binaryButtonTip"));
		}

		return showBinaryBttn;
	}

	/**
	 * This method initializes timestampButton
	 *
	 * @return javax.swing.JButton
	 */
	protected JToggleButton getShowTimestampBttn() {
		if (showTimestampBttn == null) {
			showTimestampBttn = new JToggleButton();
			showTimestampBttn.setFocusPainted(false);
			showTimestampBttn.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("timestamp.png"))));
			showTimestampBttn.setToolTipText(Strings.getString("timestampButtonTip"));
		}

		return showTimestampBttn;
	}

	/**
	 * This method initializes filterButton
	 *
	 * @return javax.swing.JButton
	 */
	protected JToggleButton getShowFilterNameBttn() {
		if (showFilterNameBttn == null) {
			showFilterNameBttn = new JToggleButton();
			showFilterNameBttn.setFocusPainted(false);
			showFilterNameBttn.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("filter.png"))));
			showFilterNameBttn.setToolTipText(Strings.getString("filterButtonTip"));
		}

		return showFilterNameBttn;
	}

	/**
	 * This method initializes pidButton
	 *
	 * @return javax.swing.JButton
	 */
	protected JToggleButton getShowProcessIdBttn() {
		if (showProcessIdBttn == null) {
			showProcessIdBttn = new JToggleButton();
			showProcessIdBttn.setFocusPainted(false);
			showProcessIdBttn.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("pid.png"))));
			showProcessIdBttn.setToolTipText(Strings.getString("pidButtonTip"));
		}

		return showProcessIdBttn;
	}

	/**
	 * This method initializes tidButton
	 *
	 * @return javax.swing.JButton
	 */
	protected JToggleButton getShowThreadIdBttn() {
		if (showThreadIdBttn == null) {
			showThreadIdBttn = new JToggleButton();
			showThreadIdBttn.setFocusPainted(false);
			showThreadIdBttn.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("tid.png"))));
			showThreadIdBttn.setToolTipText(Strings.getString("tidButtonTip"));
		}

		return showThreadIdBttn;
	}

	/**
	 * This method initializes midButton
	 *
	 * @return javax.swing.JButton
	 */
	protected JToggleButton getShowModuleIdBttn() {
		if (showModuleIdBttn == null) {
			showModuleIdBttn = new JToggleButton();
			showModuleIdBttn.setFocusPainted(false);
			showModuleIdBttn.setIcon(new ImageIcon(Toolkit.getDefaultToolkit().getImage(getClass().getResource("mid.png"))));
			showModuleIdBttn.setToolTipText(Strings.getString("midButtonTip"));
		}

		return showModuleIdBttn;
	}

	/**
	 * This method initializes toolbar
	 *
	 * @return javax.swing.JToolBar
	 */
	private class ToolBarSeparator extends JPanel {
		private static final long serialVersionUID = 1L;

		ToolBarSeparator(int width, int height) {
			super();
			setSize(width, height);
		}
	};
	private JToolBar getToolbar() {
		if (toolbar == null) {
			toolbar = new JToolBar();
			toolbar.add(getPlayButton());
			toolbar.add(getPauseButton());
			toolbar.add(getStopButton());
			toolbar.add(new ToolBarSeparator(getStopButton().getWidth() * 4, getStopButton().getHeight())); // JSeparator seems to be ignored in JToolbar...
			toolbar.add(getShowTimestampBttn());
			toolbar.add(getShowProcessIdBttn());
			toolbar.add(getShowThreadIdBttn());
			toolbar.add(getShowModuleIdBttn());
			toolbar.add(getShowBinaryBttn());
			toolbar.add(getShowFilterNameBttn());
		}

		return toolbar;
	}

	/**
	 * This method initializes showBinaryMI
	 *
	 * @return javax.swing.JCheckBoxMenuItem
	 */
	protected JCheckBoxMenuItem getShowBinaryMI() {
		if (showBinaryMI == null) {
			showBinaryMI = new JCheckBoxMenuItem();
			showBinaryMI.setText(Strings.getString("showBinaryMI"));
		}
		return showBinaryMI;
	}

	/**
	 * This method initializes showFilterNameMI
	 *
	 * @return javax.swing.JCheckBoxMenuItem
	 */
	protected JCheckBoxMenuItem getShowFilterNameMI() {
		if (showFilterNameMI == null) {
			showFilterNameMI = new JCheckBoxMenuItem();
			showFilterNameMI.setText(Strings.getString("showFilterNameMI"));
		}
		return showFilterNameMI;
	}

	/**
	 * This method initializes showWindowPerProcessMI
	 *
	 * @return javax.swing.JCheckBoxMenuItem
	 */
	protected JCheckBoxMenuItem getShowWindowPerProcessMI() {
		if (showWindowPerProcessMI == null) {
			showWindowPerProcessMI = new JCheckBoxMenuItem();
			showWindowPerProcessMI.setText(Strings.getString("showWindowPerProcessMI"));
		}
		return showWindowPerProcessMI;
	}

	/**
	 * This method initializes statusBarPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getStatusBarPanel() {
		if (statusBarPanel == null) {
			GridBagConstraints gridBagConstraints3 = new GridBagConstraints();
			gridBagConstraints3.gridx = 1;
			gridBagConstraints3.weightx = 0.0D;
			gridBagConstraints3.anchor = GridBagConstraints.EAST;
			gridBagConstraints3.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints3.gridy = 0;
			GridBagConstraints gridBagConstraints2 = new GridBagConstraints();
			gridBagConstraints2.gridx = 0;
			gridBagConstraints2.anchor = GridBagConstraints.WEST;
			gridBagConstraints2.weightx = 0.5D;
			gridBagConstraints2.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints2.gridy = 0;
			pendingTracesLabel = new JLabel();
			windowInfoLabel = new JLabel();
			statusBarPanel = new JPanel();
			statusBarPanel.setLayout(new GridBagLayout());
			statusBarPanel.setPreferredSize(new Dimension(300, 21));
			statusBarPanel.add(windowInfoLabel, gridBagConstraints2);
			statusBarPanel.add(pendingTracesLabel, gridBagConstraints3);
		}
		return statusBarPanel;
	}

}  //  @jve:decl-index=0:visual-constraint="11,11"
