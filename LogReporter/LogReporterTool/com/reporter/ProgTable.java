package reporter;

import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JTable;
import javax.swing.RowSorter;
import javax.swing.SortOrder;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumnModel;
import javax.swing.table.TableModel;

public class ProgTable extends JTable {
	private static final long serialVersionUID = 1L;
	private static TableCellRenderer colorTextRenderer = new ProgTableModel.ColorTextRenderer();

	public ProgTable(TableModel model) {
		super(model);

		// Set preferred column widths to something reasonable.
		TableColumnModel cm = getColumnModel();
	    cm.getColumn(ProgTableModel._NameColumn).setPreferredWidth(150);

	    // Install a standard row sorter.
		setAutoCreateRowSorter(true);

		// Initially sort by name column, ascending.
		List <RowSorter.SortKey> sortKeys = new ArrayList<RowSorter.SortKey>();
		sortKeys.add(new RowSorter.SortKey(0, SortOrder.ASCENDING));
		getRowSorter().setSortKeys(sortKeys);

		// Make the table fill the viewport even if it's small.
		setFillsViewportHeight(true); // since Java SE 6
	}
	
	// Use the color text renderer for the name column.
	@Override
    public TableCellRenderer getCellRenderer(int row, int column) {
		return
			convertColumnIndexToModel(column) == ProgTableModel._NameColumn ?
			colorTextRenderer : super.getCellRenderer(row, column);
	}
	
    //Implement table header tool tips.
	@Override
	protected JTableHeader createDefaultTableHeader() {
		return new JTableHeader(columnModel) {
			private static final long serialVersionUID = 1L;
			public String getToolTipText(MouseEvent e) {
				String tip = null;
				int iView = columnModel.getColumnIndexAtX(e.getPoint().x);
				int iModel = columnModel.getColumn(iView).getModelIndex();
				switch (iModel) {
				case ProgTableModel._NameColumn:
					tip = Strings.getString("progNameTip");
					break;
				}
				return tip;
			}
		};
	}
}
