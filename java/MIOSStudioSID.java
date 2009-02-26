/*
 * @(#)MIOSStudioSID.java	beta8	2006/04/23
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
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.prefs.Preferences;

import javax.swing.JApplet;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.JMenuItem;
import javax.swing.UIManager;

import org.midibox.apps.miosstudio.gui.MIOSStudioGUI.ExternalCommandButton;
import org.midibox.apps.miosstudio.xml.MIOSStudioXML;
import org.midibox.apps.miosstudiosid.gui.MIOSStudioSIDGUI;
import org.midibox.midi.gui.MidiFilterGUI;
import org.midibox.mios.gui.HexFileUploadGUI;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SplashScreen;

/**
 * Driver class for launching MIOS Studio + SID V2 application. MIOS Studio can be
 * launched as an application or embedded in a a web page as an applet. When
 * launched as an application, the app shows a splash screen and is launched
 * within its own frame. Preferences are saved and restored from this class.
 * 
 * @author Adam King (adamjking@optusnet.com.au)
 */

public class MIOSStudioSID extends JApplet {

	protected static String frameTitle = "MIOS Studio + SID V2 Editor";
	
	protected static String splashTitle = "MIOS Studio beta 9 + SID V2 Editor";

	protected static String splashImage = "splash.jpg";

	protected static String frameComment = "MIOS Studio beta 9 + SID V2 Editor";

	protected org.midibox.apps.miosstudiosid.MIOSStudioSID miosStudioSID;

	protected MIOSStudioSIDGUI miosStudioSIDGUI;

	protected Hashtable windows;

	public MIOSStudioSID() {
		
		this.miosStudioSID = new org.midibox.apps.miosstudiosid.MIOSStudioSID();
		
		loadConfigFile();

		Preferences preferences = getPreferences();

		try {
			UIManager.setLookAndFeel(preferences.get("lookAndFeel", UIManager
					.getCrossPlatformLookAndFeelClassName()));
		} catch (Exception e) {
			System.out.println(e.toString());
		}

		JDialog.setDefaultLookAndFeelDecorated(preferences.getBoolean(
				"defaultDecoratedFrames", false));

		this.miosStudioSIDGUI = new MIOSStudioSIDGUI(miosStudioSID);
		miosStudioSIDGUI.setCommentLabel(frameComment);

		this.windows = new Hashtable();
		setContentPane(miosStudioSIDGUI);
		setJMenuBar(miosStudioSIDGUI.createMenuBar());
	}

	public void init() {

		Preferences preferences = getPreferences();

		HexFileUploadGUI.setCurrentDirectory(preferences.get(
				"uploadCurrentDirectory", HexFileUploadGUI
						.getCurrentDirectory()));

		MIOSStudioSIDGUI.setCurrentDirectory(preferences.get(
				"workspaceCurrentDirectory", MIOSStudioSIDGUI
						.getCurrentDirectory()));

		MidiFilterGUI.setCurrentDirectory(preferences.get(
				"filterCurrentDirectory", MidiFilterGUI.getCurrentDirectory()));

		String[] frames = preferences.get("visibleFrames", "").split(",");
		String[] ec = preferences.get("externalCommands", "").split("\n");

		createWindowsHashtable();

		Enumeration keys = windows.keys();

		int windowNo = 0;

		while (keys.hasMoreElements()) {
			String keyName = (String) keys.nextElement();

			JInternalFrame window = (JInternalFrame) windows.get(keyName);

			int x = preferences.getInt(keyName + "X", 20 * windowNo);
			int y = preferences.getInt(keyName + "Y", 20 * windowNo);
			int width = preferences.getInt(keyName + "Width", window
					.getPreferredSize().width);
			int height = preferences.getInt(keyName + "Height", window
					.getPreferredSize().height);

			Dimension d = window.getPreferredSize();

			if (window.isResizable()) {
				window.setBounds(x, y, Math.max(width, d.width), Math.max(
						height, d.height));
			} else {
				window.setBounds(x, y, d.width, d.height);
			}

			if (preferences.getBoolean(keyName + "Visible", false)) {
				miosStudioSIDGUI.showFrame(window);
				try {
					window.setMaximum(preferences.getBoolean(keyName
							+ "Maximized", false));
					window.setIcon(preferences.getBoolean(
							keyName + "Iconified", false));
				} catch (Exception e) {
					System.out.println(e.toString());
				}
			}

			windowNo++;
		}

		for (int i = frames.length - 1; i >= 0; i--) {
			if (windows.containsKey(frames[i])) {
				((JInternalFrame) windows.get(frames[i])).toFront();
			}
		}

		for (int i = 0; i < ec.length; i++) {
			String[] temp = ec[i].split("\t");
			if (temp.length == 2) {
				miosStudioSIDGUI.addExternalCommandButton(temp[0], temp[1]);
			}
		}
	}

	public void destroy() {

		saveConfigFile();

		Preferences preferences = getPreferences();

		preferences.put("lookAndFeel", miosStudioSIDGUI.getLookAndFeel());
		preferences.putBoolean("defaultDecoratedFrames", miosStudioSIDGUI
				.isDefaultDecoratedFrames());

		preferences.put("uploadCurrentDirectory", HexFileUploadGUI
				.getCurrentDirectory());

		preferences.put("workspaceCurrentDirectory", MIOSStudioSIDGUI
				.getCurrentDirectory());

		preferences.put("filterCurrentDirectory", MidiFilterGUI
				.getCurrentDirectory());

		JInternalFrame[] frames = miosStudioSIDGUI.getDesktop().getAllFrames();
		String visibleFrames = "";

		for (int i = 0; i < frames.length; i++) {
			Enumeration keys = windows.keys();
			while (keys.hasMoreElements()) {
				String key = (String) keys.nextElement();
				if (windows.get(key) == frames[i]) {
					visibleFrames += (key + ",");
				}
			}
		}

		preferences.put("visibleFrames", visibleFrames);

		Enumeration keys = windows.keys();
		while (keys.hasMoreElements()) {
			String keyName = (String) keys.nextElement();
			JInternalFrame window = (JInternalFrame) windows.get(keyName);

			boolean visible = window.isVisible();
			try {
				preferences.putBoolean(keyName + "Iconified", window.isIcon());
				preferences.putBoolean(keyName + "Maximized", window
						.isMaximum());
				window.setIcon(false);
				window.setMaximum(false);
			} catch (Exception e) {
				System.out.println(e);
			}
			preferences.putInt(keyName + "X", window.getX());
			preferences.putInt(keyName + "Y", window.getY());
			preferences.putInt(keyName + "Width", window.getWidth());
			preferences.putInt(keyName + "Height", window.getHeight());
			preferences.putBoolean(keyName + "Visible", visible);
		}

		String externalCommandsString = "";

		for (int i = 0; i < miosStudioSIDGUI.getExternalCommands().size(); i++) {
			ExternalCommandButton ecb = (ExternalCommandButton) miosStudioSIDGUI
					.getExternalCommands().elementAt(i);
			externalCommandsString += ecb.commandName + "\t"
					+ ecb.externalCommand + "\n";
		}
		preferences.put("externalCommands", externalCommandsString);
	}

	public void exit(JFrame frame) {

		Preferences preferences = getPreferences();

		preferences.putInt("mainWindowX", frame.getX());
		preferences.putInt("mainWindowY", frame.getY());
		preferences.putInt("mainWindowWidth", frame.getWidth());
		preferences.putInt("mainWindowHeight", frame.getHeight());

		System.exit(0);
	}

	protected Preferences getPreferences() {

		return Preferences.userRoot().node("org/midibox/miostudiosid/gui");
	}

	protected String getConfigFileName() {

		return ".miosstudiosid";
	}

	protected void saveConfigFile() {

		File configFile = new File(System.getProperty("user.home"),
				getConfigFileName());

		if (!configFile.exists()) {
			try {
				configFile.createNewFile();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		if (configFile.exists()) {

			MIOSStudioXML miosStudioXML = new MIOSStudioXML(miosStudioSID,
					MIOSStudioXML.TAG_ROOT_ELEMENT);

			miosStudioXML.saveXML(configFile);
		}
	}

	protected void loadConfigFile() {
		
		File configFile = new File(System.getProperty("user.home"),
				getConfigFileName());

		if (configFile.exists()) {

			MIOSStudioXML miosStudioXML = new MIOSStudioXML(miosStudioSID,
					MIOSStudioXML.TAG_ROOT_ELEMENT);

			miosStudioXML.loadXML(configFile);

		} else {

			miosStudioSID.getHexFileUploadDeviceManager().newHexFileUploadDevice();
		}
	}

	protected void createWindowsHashtable() {

		windows.put("midiDevicesWindow", miosStudioSIDGUI
				.getMidiDeviceRoutingWindow());
		windows.put("midiOutPortMonitorWindow", miosStudioSIDGUI
				.getMidiOutPortMonitorWindow());
		windows.put("midiInPortMonitorWindow", miosStudioSIDGUI
				.getMidiInPortMonitorWindow());
		windows.put("virtualKeyboardWindow", miosStudioSIDGUI
				.getMidiKeyboardControllerWindow());
		/*
		 * windows.put("sysexWindow", miosStudioGUI
		 * .getSysexSendReceiveDeviceManagerWindow());
		 */

		windows.put("uploadWindow", miosStudioSIDGUI
				.getHexFileUploadDeviceManagerWindow());
		windows.put("lcdWindow", miosStudioSIDGUI.getLcdMessageWindow());
		windows.put("debugWindow", miosStudioSIDGUI.getDebugFunctionWindow());
		windows
				.put("miosTerminalWindow", miosStudioSIDGUI
						.getMIOSTerminalWindow());
		windows.put("sidv2librarianWindow", ((MIOSStudioSIDGUI) miosStudioSIDGUI)
				.getSidv2librarianWindow());
		
		windows.put("helpWindow", miosStudioSIDGUI.getHelpWindow());

	}

	public static void main(String[] args) {
		
		SplashScreen splashScreen = new SplashScreen(splashImage, splashTitle);

		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

		splashScreen.setLocation(
				(screenSize.width - splashScreen.getWidth()) / 2,
				(screenSize.height - splashScreen.getHeight()) / 2);

		splashScreen.setVisible(true);

		Preferences preferences = Preferences.userRoot().node(
				"org/midibox/miostudiosid/gui");

		try {
			UIManager.setLookAndFeel(preferences.get("lookAndFeel", UIManager
					.getCrossPlatformLookAndFeelClassName()));
		} catch (Exception e) {
			System.out.println(e.toString());
		}

		JFrame.setDefaultLookAndFeelDecorated(preferences.getBoolean(
				"defaultDecoratedFrames", false));

		final JFrame frame = new JFrame(frameTitle);
		
		frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

		DialogOwner.setFrame(frame);
		
		final MIOSStudioSID miosStudioSID = new MIOSStudioSID();
		miosStudioSID.init();

		frame.setContentPane(miosStudioSID);
		frame.setIconImage(ImageLoader.getImageIcon("ucIcon.png").getImage());

		frame.setBounds(preferences.getInt("mainWindowX", 50), preferences
				.getInt("mainWindowY", 50), preferences.getInt(
				"mainWindowWidth", (screenSize.width - 100)), preferences
				.getInt("mainWindowHeight", (screenSize.height - 100)));

		JMenuItem exitMenuItem = new JMenuItem("Exit");
		exitMenuItem.setMnemonic(KeyEvent.VK_X);
		exitMenuItem.setActionCommand("exit");

		exitMenuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				miosStudioSID.destroy();
				miosStudioSID.exit(frame);
			}
		});

		miosStudioSID.miosStudioSIDGUI.getFileMenu().add(exitMenuItem);

		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent we) {
				miosStudioSID.destroy();
				miosStudioSID.exit(frame);
			}
		});

		frame.setVisible(true);
		splashScreen.setVisible(false);
	}
}
