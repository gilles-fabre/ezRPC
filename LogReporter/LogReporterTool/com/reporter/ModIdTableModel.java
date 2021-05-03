package reporter;

import java.awt.Color;
import java.awt.Component;

import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.DefaultTableCellRenderer;

import tracesListener.TracesFilter;

public class ModIdTableModel extends AbstractTableModel {
	private static final long serialVersionUID = 1L;
	static final byte _NameColumn = 0;
	static final byte _MaskColumn = 1;
	static final byte _ValueColumn = 2;
	private static final String[] columnNames = { "Name", "Mask", "Value" };

	TracesFilter filter = null;

	@Override
	public String getColumnName(int column) {
		return columnNames[column];
	}

	@Override
	public Class<?> getColumnClass(int column) {
		return String.class;
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
		return filter == null ? 0 : filter.nModuleIdFilters();
	}

	public Object getValueAt(int row, int column) {
		switch (column) {
		case _NameColumn:
			return filter.getModuleIdFilter(row).getName();
		case _MaskColumn:
			return filter.getModuleIdFilter(row).getFormattedModMask();
		case _ValueColumn:
			return filter.getModuleIdFilter(row).getFormattedModId();
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
			ModIdTableModel model = (ModIdTableModel) table.getModel();
			String name = (String) object;
			Color color = model.filter.getModuleIdFilter(table.convertRowIndexToModel(row)).getColor();
			setText(name);
			setForeground(color);
			setBackground(isSelected ? table.getSelectionBackground() : table.getBackground());
			return this;
		}
	}
}
