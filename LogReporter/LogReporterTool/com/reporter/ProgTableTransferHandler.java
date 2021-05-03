package reporter;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.JComponent;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.TransferHandler;

import tracesListener.TracesFilter;

public class ProgTableTransferHandler extends TransferHandler {
	private static final long serialVersionUID = 1L;
	private static final DataFlavor stringArrayFlavor = new DataFlavor(String[].class, "SADF");
	// The content frame to which this is attached, or null if attached to main frame.
	private ContentFrame contentFrame = null;
	private static final DataFlavor[] transferDataFlavors = { stringArrayFlavor };

	public ProgTableTransferHandler(ContentFrame contentFrame) {
		super();
		this.contentFrame = contentFrame;
	}

	//--------
	// IMPORT
	//--------

	public boolean canImport(JComponent comp, DataFlavor[] flavors) {
		// The data flavor must be string array.
		// We can't drop onto the palette, only onto the prog table of a trace window.
		return contentFrame != null && flavors[0] == stringArrayFlavor;
	}

	public boolean importData(JComponent comp, Transferable t) {
		String[] sA;
		try {
			sA = (String[]) t.getTransferData(stringArrayFlavor);
		} catch (Exception e) {
			System.err.println("Can't receive transfer data: " + e);
			return false;
		}
		System.out.println(sA[0]);
		contentFrame.addProgNames(sA);
		return true;
	}

	//--------
	// EXPORT
	//--------

	public int getSourceActions(JComponent comp) {
		return COPY;
	}

	protected Transferable createTransferable(JComponent comp) {
		return new MyTransferable();
	}

//  No need to override this.
//	public Icon getVisualRepresentation(Transferable t) {
//	}

//  No need to override this.
//	public void exportAsDrag(JComponent comp, InputEvent e, int action) {
//	}

//	No need to override this.
//	public void exportToClipboard(JComponent comp, Clipboard clip, int action) throws IllegalStateException {
//	}

//  No need to override this.
//	protected void exportDone(JComponent source, Transferable data, int action) {
//	}


	private static class MyTransferable implements Transferable {

		private JTable progTable = App.getMainFrame().progTable;

		public DataFlavor[] getTransferDataFlavors() {
			return transferDataFlavors;
		}

		public boolean isDataFlavorSupported(DataFlavor flavor) {
			return flavor == stringArrayFlavor;
		}

		public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
			if (flavor != stringArrayFlavor) {
				throw new UnsupportedFlavorException(flavor);
			}
			ListSelectionModel lsm = progTable.getSelectionModel();
			TracesFilter tf = App.getMainFrame().getFilter();
			ArrayList<String> data = new ArrayList<String>();
			for (int iView = 0; iView < progTable.getRowCount(); iView++) {
				if (lsm.isSelectedIndex(iView)) {
					String s = new String();
					int iModel = progTable.convertRowIndexToModel(iView);
					s = tf.getProgFilter(iModel).getName();
					data.add(s);
				}
			}
			return data.toArray(new String[0]);
		}

	}

}
