package reporter;

import java.awt.Color;
import java.awt.Component;

import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.DefaultTableCellRenderer;

import tracesListener.TracesFilter;

public class ProgTableModel extends AbstractTableModel {

	private static final long serialVersionUID = 1L;

	static final byte _NameColumn = 0;
	static final byte _PidColumn = 1;

	private static final String[] columnNames = { "Name", "PID" };

	TracesFilter filter = null;

	@Override
	public String getColumnName(int column) {
		return columnNames[column];
	}

	@Override
	public Class<?> getColumnClass(int column) {
		return column == _PidColumn ? Integer.class : String.class;
	}

	@Override
	public boolean isCellEditable(int row, int column) {
		return false;
	}

	public int getColumnCount() {
		return columnNames.length;
	}

	public int getRowCount() {
		// this could be called before the filter gets set....
		return filter == null ? 0 : filter.nProgFilters();
	}

	public Object getValueAt(int row, int column) {
		switch (column) {
		case _NameColumn:
			return filter.getProgFilter(row).getName();
		case _PidColumn:
			return filter.getProgFilter(row).getPid();
		}
		return null;
	}

	public TracesFilter getFilter() {
		return filter;
	}

	public void setFilter(TracesFilter filter) {
		this.filter = filter;
		fireTableDataChanged();
	}

	public static final class ColorTextRenderer extends DefaultTableCellRenderer {

		private static final long serialVersionUID = 1L;

		public ColorTextRenderer() {
			setOpaque(true); // must do this for background to show up?
		}

		public Component getTableCellRendererComponent(JTable table, Object object, boolean isSelected, boolean hasFocus, int row, int column) {
			ProgTableModel model = (ProgTableModel) table.getModel();
			String name = (String) object;
			Color color = model.filter.getProgFilter(table.convertRowIndexToModel(row)).getColor();
			setText(name);
			setForeground(color);
			setBackground(isSelected ? table.getSelectionBackground() : table.getBackground());
			return this;
		}
	}

}
