package reporter;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.JPopupMenu;

class PopupMouseListener extends MouseAdapter {
	private JPopupMenu popupMenu;
	
	public PopupMouseListener(JPopupMenu popupMenu) {
		super();
		this.popupMenu = popupMenu;
	}
	
	@Override
	public void mousePressed(MouseEvent e) {
		maybeShowPopup(e);
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		maybeShowPopup(e);
	}

	private void maybeShowPopup(MouseEvent e) {
		if (e.isPopupTrigger()) {
			popupMenu.show(e.getComponent(), e.getX(), e.getY());
		}
	}
}
