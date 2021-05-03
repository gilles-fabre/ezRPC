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

public class ModIdTableTransferHandler extends TransferHandler {
	private static final long serialVersionUID = 1L;
	private static final DataFlavor stringArrayArrayFlavor = new DataFlavor(String[][].class, "SAADF");

	// The content frame to which this is attached, or null if attached to main frame.
	private ContentFrame contentFrame = null;

	private static final DataFlavor[] transferDataFlavors = { stringArrayArrayFlavor };

	public ModIdTableTransferHandler(ContentFrame contentFrame) {
		super();
		this.contentFrame = contentFrame;
	}

	//--------
	// IMPORT
	//--------

	public boolean canImport(JComponent comp, DataFlavor[] flavors) {
		// The data flavor must be string array array.
		// We can't drop onto the palette, only onto the filter table of a trace window.
		return contentFrame != null && flavors[0] == stringArrayArrayFlavor;
	}

	public boolean importData(JComponent comp, Transferable t) {
		String[][] sAA;
		try {
			sAA = (String[][]) t.getTransferData(stringArrayArrayFlavor);
		} catch (Exception e) {
			System.err.println("Can't receive transfer data: " + e);
			return false;
		}
		// We want the first column of the sAA
		String sA[] = new String[sAA.length];
		for (int i=0; i < sAA.length; i++) {
			sA[i] = sAA[i][0];
		}
		contentFrame.addModIdNames(sA);
		return true;
	}

	//--------
	// EXPORT
	//--------

	public int getSourceActions(JComponent comp) {
		System.out.println("getSourceActions()");
		return COPY;
	}

	protected Transferable createTransferable(JComponent comp) {
		System.out.println("createTransferable()");
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

		private JTable modIdTable = App.getMainFrame().modIdTable;

		public DataFlavor[] getTransferDataFlavors() {
			return transferDataFlavors;
		}

		public boolean isDataFlavorSupported(DataFlavor flavor) {
			return flavor == stringArrayArrayFlavor;
		}

		public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
			if (flavor != stringArrayArrayFlavor) {
				throw new UnsupportedFlavorException(flavor);
			}
			ListSelectionModel lsm = modIdTable.getSelectionModel();
			TracesFilter tf = App.getMainFrame().getFilter();
			ArrayList<String[]> data = new ArrayList<String[]>();
			for (int iView = 0; iView < modIdTable.getRowCount(); iView++) {
				if (lsm.isSelectedIndex(iView)) {
					String[] sa = new String[3];
					int iModel = modIdTable.convertRowIndexToModel(iView);
					sa[0] = tf.getModuleIdFilter(iModel).getName();
					sa[1] = tf.getModuleIdFilter(iModel).getFormattedModMask();
					sa[2] = tf.getModuleIdFilter(iModel).getFormattedModId();
					data.add(sa);
				}
			}
			return data.toArray(new String[0][3]);
		}

	}

}
