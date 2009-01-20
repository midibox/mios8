import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.prefs.Preferences;

import javax.swing.JApplet;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;

import org.midibox.apps.SIDV2librarian.gui.SIDV2librarianGUI;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.ImageLoader;

public class SIDV2librarian extends JApplet implements ActionListener {

	private org.midibox.apps.SIDV2librarian.SIDV2librarian sidv2librarian;

	private SIDV2librarianGUI sidv2librarianGUI;

	private Preferences prefs = Preferences.userRoot().node(
			"org/midibox/apps/SIDV2librarian");

	public SIDV2librarian() {

		sidv2librarian = new org.midibox.apps.SIDV2librarian.SIDV2librarian();

		sidv2librarianGUI = new SIDV2librarianGUI(sidv2librarian);

		setContentPane(sidv2librarianGUI);

		setJMenuBar(sidv2librarianGUI.createMenu());

		JMenu fileMenu = sidv2librarianGUI.getFileMenu();

		JMenu editMenu = sidv2librarianGUI.getEditMenu();

		fileMenu.addSeparator();

		JMenuItem menuItem = new JMenuItem("Exit", KeyEvent.VK_X);
		menuItem.setActionCommand("Exit");
		menuItem.addActionListener(this);
		fileMenu.add(menuItem);

		editMenu.addSeparator();

		menuItem = new JMenuItem("Preferences", KeyEvent.VK_R);
		menuItem.setActionCommand("Preferences");
		menuItem.addActionListener(this);
		editMenu.add(menuItem);
	}

	public void destroy() {
		super.destroy();

		tryExit();
	}

	public void init() {
		super.init();

		if (sidv2librarian.getMidiDeviceRouting().getMidiReadDevices().size() < 1) {
			JOptionPane
					.showMessageDialog(
							null,
							"Your system does not appear to have any MIDI-devices connected (please close program)!.",
							"Error", JOptionPane.OK_OPTION);
		} else {
			sidv2librarian.setInputDeviceIndex(0);
			sidv2librarian.setOutputDeviceIndex(0);
			sidv2librarian.getMidiDeviceRouting().connectDevices(
					sidv2librarian.getInputMidiDevice(),
					sidv2librarian.getLocalMidiDevice());
			sidv2librarian.getMidiDeviceRouting().connectDevices(
					sidv2librarian.getLocalMidiDevice(),
					sidv2librarian.getOutputMidiDevice());
		}

		recallConnections();
	}

	public static void main(String[] args) {

		final JFrame frame = new JFrame("MidiBox SID V2 Editor");

		DialogOwner.setFrame(frame);

		final SIDV2librarian sidv2librarian = new SIDV2librarian();

		frame.setContentPane(sidv2librarian);

		frame.setIconImage(ImageLoader.getImageIcon("sid.png").getImage());
		frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent ev) {
				sidv2librarian.tryExit();
			}
		});

		frame.setResizable(false);

		frame.pack();

		frame.setVisible(true);

		sidv2librarian.init();
	}

	private void tryExit() {
		int n = JOptionPane.showConfirmDialog(null,
				"Are you sure you want to exit?", "Exit?",
				JOptionPane.YES_NO_OPTION);
		if (n == JOptionPane.YES_OPTION) {

			storeConnections();
			sidv2librarian.closeMidi();

			System.exit(0);
		}
	}

	private void recallConnections() {
		int input = prefs.getInt("INPUT_DEVICE", 0);
		int output = prefs.getInt("OUTPUT_DEVICE", 0);
		int in = sidv2librarian.findInputDeviceHash(input);
		int out = sidv2librarian.findOuputDeviceHash(output);

		if (in != -1 && out != -1) {
			sidv2librarian.setInputDeviceIndex(in);
			sidv2librarian.setOutputDeviceIndex(out);
		} else {
			JOptionPane
					.showMessageDialog(
							null,
							"Your MIDI configuration has changed, please check connections!",
							"MIDI configuration changed!",
							JOptionPane.OK_OPTION);

			sidv2librarianGUI.showMidiDeviceRoutingDialog();
		}
	}

	private void storeConnections() {
		int input = sidv2librarian.getInputDeviceHash();
		int output = sidv2librarian.getOutputDeviceHash();
		prefs.putInt("INPUT_DEVICE", input);
		prefs.putInt("OUTPUT_DEVICE", output);
	}

	public void actionPerformed(ActionEvent ae) {
		if (ae.getActionCommand().equals("Preferences")) {
			sidv2librarianGUI.showMidiDeviceRoutingDialog();
		} else if (ae.getActionCommand().equals("Exit")) {
			tryExit();
		}
	}
}
