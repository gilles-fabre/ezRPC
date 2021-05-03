package reporter;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JInternalFrame;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.border.TitledBorder;

public class ContentFrameVE extends JInternalFrame {
	private static final long serialVersionUID = 1L;
	private static final int defaultWidth = 640;
	private static final int defaultHeight = 600;
	private static final int defaultDividerLocation = 175;
	protected JPopupMenu modIdTablePopupMenu = null;
	protected JMenuItem newModIdPUMI = null;
	protected JMenuItem editModIdPUMI = null;
	protected JMenuItem deleteModIdPUMI = null;
	protected JPopupMenu progTablePopupMenu = null;
	protected JMenuItem newProgPUMI = null;
	protected JMenuItem editProgPUMI = null;
	protected JMenuItem deleteProgPUMI = null;
	protected JPopupMenu textAreaPopupMenu = null;
	protected JMenuItem renameWindowPUMI = null;
	protected JMenuItem hideFiltersPUMI = null;
	protected JMenuItem showFiltersPUMI = null;
	protected JMenuItem clearWindowPUMI = null;
	protected JMenuItem insertSeparatorPUMI = null;
	protected JMenuItem findPUMI = null;
	protected JMenuItem gotoPUMI = null;
	protected JMenuItem selectAllPUMI = null;
	protected JMenuItem copyTextPUMI = null;
	protected JMenuItem savePUMI = null;
	protected JSplitPane jSplitPane = null;
	protected JPanel    filteredTextPane = null;
	protected JTextField regExpField = null;
	protected JScrollPane textScrollPane = null;
	protected MultiColoredTextArea textArea = null;
	protected JSplitPane jSplitPane1 = null;
	protected JPanel modIdPanel = null;
	protected JScrollPane modIdScrollPane = null;
	protected ModIdTable modIdTable = null;
	protected JPanel progPanel = null;
	protected JScrollPane progScrollPane = null;
	protected ProgTable progTable = null;

	/**
	 * This is the default constructor
	 */
	public ContentFrameVE() {
		super();
		initialize();
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		this.setName("contentFrame");
		this.setSize(defaultWidth, defaultHeight);
		this.setFrameIcon(new ImageIcon(getClass().getResource("log-reporter-step.png")));
		this.setResizable(true);
		this.setMaximizable(true);
		this.setIconifiable(true);
		this.setClosable(true);
		this.setContentPane(getJSplitPane());
		this.getModIdTablePopupMenu();
		this.getProgTablePopupMenu();
		this.getTextAreaPopupMenu();
	}

	/**
	 * This method initializes modIdTablePopupMenu
	 *
	 * @return javax.swing.JPopupMenu
	 */
	private JPopupMenu getModIdTablePopupMenu() {
		if (modIdTablePopupMenu == null) {
			modIdTablePopupMenu = new JPopupMenu();
			modIdTablePopupMenu.add(getNewModIdPUMI());
			modIdTablePopupMenu.add(getEditModIdPUMI());
			modIdTablePopupMenu.add(getDeleteModIdPUMI());
		}
		return modIdTablePopupMenu;
	}

	/**
	 * This method initializes newModIdPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getNewModIdPUMI() {
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
	private JMenuItem getEditModIdPUMI() {
		if (editModIdPUMI == null) {
			editModIdPUMI = new JMenuItem();
			editModIdPUMI.setText(Strings.getString("editModIdMI"));
		}
		return editModIdPUMI;
	}

	/**
	 * This method initializes deleteModIdPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getDeleteModIdPUMI() {
		if (deleteModIdPUMI == null) {
			deleteModIdPUMI = new JMenuItem();
			deleteModIdPUMI.setText(Strings.getString("deleteModIdMI"));
		}
		return deleteModIdPUMI;
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
			progTablePopupMenu.add(getDeleteProgPUMI());
		}
		return modIdTablePopupMenu;
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
	 * This method initializes textAreaPopupMenu
	 *
	 * @return javax.swing.JPopupMenu
	 */
	private JPopupMenu getTextAreaPopupMenu() {
		if (textAreaPopupMenu == null) {
			textAreaPopupMenu = new JPopupMenu();
			textAreaPopupMenu.add(getRenameWindowPUMI());
			textAreaPopupMenu.addSeparator();
			textAreaPopupMenu.add(getHideFiltersPUMI());
			textAreaPopupMenu.add(getShowFiltersPUMI());
			textAreaPopupMenu.addSeparator();
			textAreaPopupMenu.add(getClearWindowPUMI());
			textAreaPopupMenu.add(getInsertSeparatorPUMI());
			textAreaPopupMenu.add(getFindPUMI());
			textAreaPopupMenu.add(getGotoPUMI());
			textAreaPopupMenu.add(getSelectAllPUMI());
			textAreaPopupMenu.add(getCopyTextPUMI());
			textAreaPopupMenu.addSeparator();
			textAreaPopupMenu.add(getSavePUMI());
		}
		return textAreaPopupMenu;
	}

	/**
	 * This method initializes renameWindowPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getRenameWindowPUMI() {
		if (renameWindowPUMI == null) {
			renameWindowPUMI = new JMenuItem();
			renameWindowPUMI.setText(Strings.getString("renameWindowMI"));
		}
		return renameWindowPUMI;
	}

	/**
	 * This method initializes hideFiltersPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getHideFiltersPUMI() {
		if (hideFiltersPUMI == null) {
			hideFiltersPUMI = new JMenuItem();
			hideFiltersPUMI.setText(Strings.getString("hideFiltersMI"));
		}
		return hideFiltersPUMI;
	}

	/**
	 * This method initializes showFiltersPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getShowFiltersPUMI() {
		if (showFiltersPUMI == null) {
			showFiltersPUMI = new JMenuItem();
			showFiltersPUMI.setText(Strings.getString("showFiltersMI"));
		}
		return showFiltersPUMI;
	}

	/**
	 * This method initializes clearWindowPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getClearWindowPUMI() {
		if (clearWindowPUMI == null) {
			clearWindowPUMI = new JMenuItem();
			clearWindowPUMI.setText(Strings.getString("clearWindowMI"));
			clearWindowPUMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, InputEvent.ALT_DOWN_MASK, false));
		}
		return clearWindowPUMI;
	}

	/**
	 * This method initializes insertSeparatorPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getInsertSeparatorPUMI() {
		if (insertSeparatorPUMI == null) {
			insertSeparatorPUMI = new JMenuItem();
			insertSeparatorPUMI.setText(Strings.getString("appendSeparatorMI"));
			insertSeparatorPUMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_L, InputEvent.CTRL_DOWN_MASK, false));
		}
		return insertSeparatorPUMI;
	}

	/**
	 * This method initializes findPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getFindPUMI() {
		if (findPUMI == null) {
			findPUMI = new JMenuItem();
			findPUMI.setText(Strings.getString("findMI"));
			findPUMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F, InputEvent.CTRL_DOWN_MASK, false));
		}
		return findPUMI;
	}

	/**
	 * This method initializes gotoPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getGotoPUMI() {
		if (gotoPUMI == null) {
			gotoPUMI = new JMenuItem();
			gotoPUMI.setText(Strings.getString("gotoMI"));
			gotoPUMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, InputEvent.CTRL_DOWN_MASK, false));
		}
		return gotoPUMI;
	}

	/**
	 * This method initializes selectAllPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getSelectAllPUMI() {
		if (selectAllPUMI == null) {
			selectAllPUMI = new JMenuItem();
			selectAllPUMI.setText(Strings.getString("selectAllMI"));
			selectAllPUMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, InputEvent.CTRL_DOWN_MASK, false));
		}
		return selectAllPUMI;
	}

	/**
	 * This method initializes copyTextPUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getCopyTextPUMI() {
		if (copyTextPUMI == null) {
			copyTextPUMI = new JMenuItem();
			copyTextPUMI.setText(Strings.getString("copyTextMI"));
			copyTextPUMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, InputEvent.CTRL_DOWN_MASK, false));
		}
		return copyTextPUMI;
	}

	/**
	 * This method initializes savePUMI
	 *
	 * @return javax.swing.JMenuItem
	 */
	private JMenuItem getSavePUMI() {
		if (savePUMI == null) {
			savePUMI = new JMenuItem();
			savePUMI.setText(Strings.getString("saveWindowMI"));
			savePUMI.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, InputEvent.CTRL_DOWN_MASK, false));
		}
		return savePUMI;
	}

	/**
	 * This method initializes jSplitPane
	 *
	 * @return javax.swing.JSplitPane
	 */
	private JSplitPane getJSplitPane() {
		if (jSplitPane == null) {
			jSplitPane = new JSplitPane();
			jSplitPane.setContinuousLayout(true);
			jSplitPane.setDividerLocation(defaultDividerLocation);
			jSplitPane.setOneTouchExpandable(true);
			jSplitPane.setDividerSize(10);
			jSplitPane.setLeftComponent(getJSplitPane1());
			jSplitPane.setRightComponent(getFilteredTextPane());
		}
		return jSplitPane;
	}

	/**
	 * This method initialized filteredTextPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getFilteredTextPane() {
		if (filteredTextPane == null) {
			filteredTextPane = new JPanel();
			filteredTextPane.setLayout(new BorderLayout());
			JPanel innerPane = new JPanel();
			innerPane.setLayout(new BoxLayout(innerPane, BoxLayout.X_AXIS));
			innerPane.add(new JLabel("filter on regular expression : "), BorderLayout.LINE_START);
			innerPane.add(getRegExpField(), BorderLayout.LINE_END);
			filteredTextPane.add(innerPane, BorderLayout.NORTH);
			filteredTextPane.add(getTextScrollPane(), BorderLayout.CENTER);
		}
		return filteredTextPane;
	}
	
	/**
	 * This method initialized regExpField
	 * 
	 * @return javax.swing.JPanel
	 */
	protected JTextField getRegExpField() {
		if (regExpField == null) {
			regExpField = new JTextField();
			regExpField.setMinimumSize(new Dimension(100, regExpField.getHeight() * 2));
		}
		return regExpField;
	}
	
	/**
	 * This method initializes textScrollPane
	 *
	 * @return javax.swing.JScrollPane
	 */
	private JScrollPane getTextScrollPane() {
		if (textScrollPane == null) {
			textScrollPane = new JScrollPane();
			textScrollPane.setViewportView(getTextArea());
		}
		return textScrollPane;
	}

	/**
	 * This method initializes textArea
	 *
	 * @return MultiColoredTextArea
	 */
	private MultiColoredTextArea getTextArea() {
		if (textArea == null) {
			textArea = new MultiColoredTextArea(getRegExpField());
		}
		return textArea;
	}

	/**
	 * This method initializes jSplitPane1
	 *
	 * @return javax.swing.JSplitPane
	 */
	private JSplitPane getJSplitPane1() {
		if (jSplitPane1 == null) {
			jSplitPane1 = new JSplitPane();
			jSplitPane1.setOrientation(JSplitPane.VERTICAL_SPLIT);
			jSplitPane1.setDividerLocation(300);
			jSplitPane1.setDividerSize(10);
			jSplitPane1.setContinuousLayout(true);
			jSplitPane1.setOneTouchExpandable(true);
			jSplitPane1.setTopComponent(getModIdPanel());
			jSplitPane1.setBottomComponent(getProgPanel());
		}
		return jSplitPane1;
	}

	/**
	 * This method initializes modIdPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getModIdPanel() {
		if (modIdPanel == null) {
			modIdPanel = new JPanel();
			modIdPanel.setLayout(new BorderLayout());
			modIdPanel.setToolTipText(Strings.getString("modIdFiltersTip"));
			modIdPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEmptyBorder(1, 0, 0, 0), Strings.getString("modIdFilters"), TitledBorder.CENTER, TitledBorder.BELOW_TOP, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			modIdPanel.setMinimumSize(new Dimension(0, 25));
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
	 * @return javax.swing.JTable
	 */
	private JTable getModIdTable() {
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
			progPanel.setToolTipText(Strings.getString("progFiltersTip"));
			progPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createEmptyBorder(1, 0, 0, 0), Strings.getString("progFilters"), TitledBorder.CENTER, TitledBorder.BELOW_TOP, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
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
	 * @return com.gfprod.reporter.ProgTable
	 */
	private ProgTable getProgTable() {
		if (progTable == null) {
			progTable = new ProgTable(new ProgTableModel());
		}
		return progTable;
	}

}
