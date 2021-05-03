package reporter;

import java.awt.event.MouseEvent;

import javax.swing.JPopupMenu;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.SwingUtilities;

class TablePopupMouseListener extends PopupMouseListener {
	private JTable table;
	
	public TablePopupMouseListener(JTable table, JPopupMenu popupMenu) {
		super(popupMenu);
		this.table = table;
	}
	
	@Override
	public void mousePressed(MouseEvent e) {
		selectIfRightMousePressOnUnselectedRow(e, table);
		super.mousePressed(e);
	}
	
	public void mouseClicked(MouseEvent e) {
		if (e.getClickCount() == 2) {
			mouseDoubleClickedOnRow(table.rowAtPoint(e.getPoint()));
		} else {
			super.mouseClicked(e);
		}
	}
	
	// Override this method to handle a double-click on a row in the table.
	protected void mouseDoubleClickedOnRow(int row) {
	}

	// If the given event is a press of the right mouse button,
	// and the pointer is on a non-selected row of the given table,
	// make it become the only selected row in that table.
	private static void selectIfRightMousePressOnUnselectedRow(MouseEvent e, JTable table) {
    	if (SwingUtilities.isRightMouseButton(e)) {
    		int row = table.rowAtPoint(e.getPoint());
    		if (row >= 0) {
    			// The cursor is on some table entry.
    			ListSelectionModel model = table.getSelectionModel();
				if (!model.isSelectedIndex(row)) {
					// The table entry is not selected.
					model.setSelectionInterval(row, row);
				}	    			
    		}
    	}
    }
}
