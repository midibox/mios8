package org.midibox.apps.miosstudiosid.gui;

import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.util.Observable;
import java.util.Observer;

import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.JMenuItem;

import org.midibox.apps.miosstudio.gui.MIOSStudioGUI;
import org.midibox.apps.miosstudiosid.MIOSStudioSID;
import org.midibox.apps.sidv2editor.gui.SIDV2EditorGUI;
import org.midibox.utils.gui.ImageLoader;

public class MIOSStudioSIDGUI extends MIOSStudioGUI implements Observer {

	protected SIDV2EditorGUI sidv2librariangui;

	protected MIOSStudioInternalFrame sidv2librarianWindow;

	public MIOSStudioSIDGUI(MIOSStudioSID miosStudioSID) {
		super(miosStudioSID);

		sidv2librariangui.getSidv2librarian().getSIDLibController()
				.addObserver(this);
	}

	public MIOSStudioInternalFrame getSidv2librarianWindow() {
		return sidv2librarianWindow;
	}

	protected void createInternalFrames() {
		super.createInternalFrames();

		// SIDV2 Librarian

		sidv2librariangui = new SIDV2EditorGUI(
				((org.midibox.apps.miosstudiosid.MIOSStudioSID) miosStudio)
						.getSidv2librarian());

		Icon icon = ImageLoader.getImageIcon("sid.png");

		sidv2librarianWindow = new MIOSStudioInternalFrame("SID V2 Editor",
				false, true, false, true, icon, sidv2librariangui);

		// Don't display menu bar when integrated in MIOS Studio
		sidv2librariangui.createMenu();

		sidv2librarianWindow.pack();

		internalFrames.add(sidv2librarianWindow);

		midiDeviceRoutingGUI.addMidiDeviceIcon(
				((org.midibox.apps.miosstudiosid.MIOSStudioSID) miosStudio)
						.getSidv2librarian().getSysExController(), icon);
	}

	protected void createMIOSMenu() {

		super.createMIOSMenu();

		JMenuItem menuItem = new JMenuItem("SID V2 Editor");
		menuItem.setActionCommand("sidv2_librarian");
		menuItem.addActionListener(this);
		miosMenu.add(menuItem);
	}

	protected void createMIOSButtons() {

		super.createMIOSButtons();

		Insets insets = new Insets(2, 2, 2, 2);

		JButton button = new JButton(ImageLoader.getImageIcon("sid.png"));
		button.setToolTipText("SID V2 Editor");
		button.setActionCommand("sidv2_librarian");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);
	}

	public void actionPerformed(ActionEvent ae) {

		super.actionPerformed(ae);

		if (ae.getActionCommand().equals("sidv2_librarian")) {
			showFrame(sidv2librarianWindow);

		}
	}

	public void update(Observable observable, Object object) {

		super.update(observable, object);

		if (object == "Edit") {

			boolean portsReleased = miosStudio.getMidiDeviceRouting()
					.isPortsReleased();

			if (!portsReleased) {

				System.out.println("Now releasing ports!");

				miosStudio.getMidiDeviceRouting().setPortsReleased(true);

				System.out.println("Ports released successfully");

				miosStudio.getMidiDeviceRouting().setPortsReleased(false);

				System.out.println("Ports recaimed successfully");
			}
		}
	}
}
