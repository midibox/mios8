/*
 * @(#)VirtualKeyboardDriver.java	beta8	2006/04/23
 *
 * Copyright (C) 2008    Adam King (adamjking@optusnet.com.au)
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this application; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.util.prefs.Preferences;

import javax.swing.JApplet;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JMenuItem;
import javax.swing.UIManager;

import org.midibox.apps.virtualkeyboard.gui.VirtualKeyboardGUI;
import org.midibox.apps.virtualkeyboard.xml.VirtualKeyboardXML;
import org.midibox.utils.gui.ImageLoader;

public class VirtualKeyboard extends JApplet {

	final static Preferences preferences = Preferences.userRoot().node(
			"org/midibox/virtualkeyboard/gui");

	final static String configFileName = ".virtualkeyboard";

	private org.midibox.apps.virtualkeyboard.VirtualKeyboard virtualKeyboard;

	private VirtualKeyboardGUI virtualKeyboardGUI;

	public VirtualKeyboard() {

		virtualKeyboard = new org.midibox.apps.virtualkeyboard.VirtualKeyboard();

		loadConfigFile();

		try {
			UIManager.setLookAndFeel(preferences.get("lookAndFeel", UIManager
					.getCrossPlatformLookAndFeelClassName()));
		} catch (Exception e) {
			System.out.println(e.toString());
		}

		JDialog.setDefaultLookAndFeelDecorated(preferences.getBoolean(
				"defaultDecoratedFrames", false));

		JFrame.setDefaultLookAndFeelDecorated(preferences.getBoolean(
				"defaultDecoratedFrames", false));

		virtualKeyboardGUI = new VirtualKeyboardGUI(virtualKeyboard);

		getContentPane().add(virtualKeyboardGUI);

		setJMenuBar(virtualKeyboardGUI.createMenuBar());
	}

	public void init() {

		virtualKeyboardGUI.setShowConnections(preferences.getBoolean(
				"showConnections", true));
	}

	protected void loadConfigFile() {

		File configFile = new File(System.getProperty("user.home"),
				configFileName);

		if (configFile.exists()) {

			VirtualKeyboardXML virtualKeyboardXML = new VirtualKeyboardXML(
					virtualKeyboard, VirtualKeyboardXML.TAG_ROOT_ELEMENT);

			virtualKeyboardXML.loadXML(configFile);
		}
	}

	public void destroy() {

		saveConfigFile();

		preferences.put("lookAndFeel", virtualKeyboardGUI.getLookAndFeel());

		preferences.putBoolean("defaultDecoratedFrames", virtualKeyboardGUI
				.isDefaultDecoratedFrames());

		preferences.putBoolean("showConnections", virtualKeyboardGUI
				.isShowConnections());
	}

	protected void saveConfigFile() {

		File configFile = new File(System.getProperty("user.home"),
				configFileName);

		if (!configFile.exists()) {
			try {
				configFile.createNewFile();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		if (configFile.exists()) {

			VirtualKeyboardXML virtualKeyboardXML = new VirtualKeyboardXML(
					virtualKeyboard, VirtualKeyboardXML.TAG_ROOT_ELEMENT);

			virtualKeyboardXML.saveXML(configFile);
		}
	}

	public static void exit(JFrame frame) {

		preferences.putInt("mainWindowX", frame.getX());
		preferences.putInt("mainWindowY", frame.getY());

		System.exit(0);
	}

	public static void main(String[] args) {

		final VirtualKeyboard virtualKeyboardGUIDriver = new VirtualKeyboard();

		virtualKeyboardGUIDriver.init();

		final JFrame frame = new JFrame("Virtual Keyboard");

		frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

		frame.setContentPane(virtualKeyboardGUIDriver);

		frame.setIconImage(ImageLoader.getImageIcon("piano.png").getImage());
		frame.pack();
		frame.setResizable(false);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

		frame.setLocation(preferences.getInt("mainWindowX", 50), preferences
				.getInt("mainWindowY", 50));

		frame.setLocation(preferences.getInt("windowX", 50), preferences
				.getInt("windowY", 50));

		JMenuItem exitMenuItem = new JMenuItem("Exit");
		exitMenuItem.setMnemonic(KeyEvent.VK_X);
		exitMenuItem.setActionCommand("exit");

		exitMenuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				virtualKeyboardGUIDriver.destroy();
				exit(frame);
			}
		});

		virtualKeyboardGUIDriver.virtualKeyboardGUI.getFileMenu().add(
				exitMenuItem);

		virtualKeyboardGUIDriver.virtualKeyboardGUI.getMidiDeviceRoutingGUI()
				.addComponentListener(new ComponentAdapter() {
					public void componentShown(ComponentEvent arg0) {
						frame.pack();
					}

					public void componentHidden(ComponentEvent arg0) {
						frame.pack();
					}
				});

		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent we) {
				virtualKeyboardGUIDriver.destroy();
				exit(frame);
			}
		});

		frame.setVisible(true);
	}
}
