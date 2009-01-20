package org.midibox.utils.gui;

import javax.swing.JDialog;
import javax.swing.JFrame;

public class DialogOwner {

	private static JFrame frame;

	private static JDialog dialog;

	public static JFrame getFrame() {
		return frame;
	}

	public static void setFrame(JFrame frame) {
		DialogOwner.frame = frame;
	}

	public static JDialog getDialog() {
		return dialog;
	}

	public static void setDialog(JDialog dialog) {
		DialogOwner.dialog = dialog;
	}
}
