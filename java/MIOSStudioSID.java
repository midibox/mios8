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

import javax.swing.JApplet;
import javax.swing.JFrame;
import javax.swing.JMenuItem;

import org.midibox.apps.miosstudiosid.gui.MIOSStudioSIDGUI;
import org.midibox.apps.miosstudiosid.gui.xml.MIOSStudioSIDGUIXML;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SplashScreen;

/**
 * Driver class for launching MIOS Studio application. MIOS Studio can be
 * launched as an application or embedded in a a web page as an applet. When
 * launched as an application, the app shows a splash screen and is launched
 * within its own frame. Preferences are saved and restored from this class.
 * 
 * @author Adam King (adamjking@optusnet.com.au)
 */

public class MIOSStudioSID extends JApplet {

	protected static String oldConfigFileName = ".miosstudiosid";

	protected static String configDirectoryName = ".midibox";

	protected static String configFileName = "miosstudiosid.xml";

	protected static String frameTitle = "MIOS Studio + SID V2 Editor";

	protected static String splashTitle = "MIOS Studio beta 9.3 + SID V2 Editor";

	protected static String splashImage = "splash.jpg";

	protected static String frameComment = "MIOS Studio beta 9.3";

	protected org.midibox.apps.miosstudiosid.MIOSStudioSID miosStudioSID;

	protected MIOSStudioSIDGUI miosStudioSIDGUI;

	public MIOSStudioSID() {

		this.miosStudioSID = new org.midibox.apps.miosstudiosid.MIOSStudioSID();

		File configDirectory = new File(System.getProperty("user.home"),
				configDirectoryName);

		File configFile = new File(configDirectory, configFileName);

		if (configFile.exists()) {

			MIOSStudioSIDGUIXML miosStudioSIDGUIXML = new MIOSStudioSIDGUIXML(
					miosStudioSID, MIOSStudioSIDGUIXML.TAG_ROOT_ELEMENT, true,
					true, true, true, true);

			miosStudioSIDGUIXML.loadXML(configFile);

			this.miosStudioSIDGUI = (MIOSStudioSIDGUI) miosStudioSIDGUIXML
					.getMiosStudioGUI();

		} else {

			// check for old config file

			configFile = new File(System.getProperty("user.home"),
					oldConfigFileName);

			if (configFile.exists()) {

				MIOSStudioSIDGUIXML miosStudioSIDGUIXML = new MIOSStudioSIDGUIXML(
						miosStudioSID, MIOSStudioSIDGUIXML.TAG_ROOT_ELEMENT,
						true, true, true, true, true);

				miosStudioSIDGUIXML.loadXML(configFile);

				this.miosStudioSIDGUI = (MIOSStudioSIDGUI) miosStudioSIDGUIXML
						.getMiosStudioGUI();

				configFile.delete();
			}
		}

		if (miosStudioSIDGUI == null) {

			this.miosStudioSIDGUI = new MIOSStudioSIDGUI(miosStudioSID);
		}

		setContentPane(miosStudioSIDGUI);

		setJMenuBar(miosStudioSIDGUI.createMenuBar());

		miosStudioSIDGUI.setCommentLabel(frameComment);
	}

	public void init() {

	}

	public void destroy() {

		File configDirectory = new File(System.getProperty("user.home"),
				configDirectoryName);

		File configFile = new File(configDirectory, configFileName);

		if (!configDirectory.exists()) {

			try {

				configDirectory.mkdir();

			} catch (Exception e) {

				e.printStackTrace();
			}
		}

		if (!configFile.exists()) {

			try {

				configFile.createNewFile();

			} catch (Exception e) {

				e.printStackTrace();
			}
		}

		if (configFile.exists()) {

			MIOSStudioSIDGUIXML miosStudioSIDGUIXML = new MIOSStudioSIDGUIXML(
					miosStudioSIDGUI, MIOSStudioSIDGUIXML.TAG_ROOT_ELEMENT,
					true, true, true, true, true);

			miosStudioSIDGUIXML.saveXML(configFile);
		}
	}

	public static void main(String[] args) {

		SplashScreen splashScreen = new SplashScreen(splashImage, splashTitle);

		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

		splashScreen.setLocation(
				(screenSize.width - splashScreen.getWidth()) / 2,
				(screenSize.height - splashScreen.getHeight()) / 2);

		splashScreen.setVisible(true);

		final JFrame frame = new JFrame(frameTitle);

		frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

		DialogOwner.setFrame(frame);

		frame.setIconImage(ImageLoader.getImageIcon("ucIcon.png").getImage());

		frame.setBounds(50, 50, (screenSize.width - 100),
				(screenSize.height - 100));

		final MIOSStudioSID miosStudioSID = new MIOSStudioSID();

		miosStudioSID.init();

		frame.setContentPane(miosStudioSID);

		JMenuItem exitMenuItem = new JMenuItem("Exit");
		exitMenuItem.setMnemonic(KeyEvent.VK_X);
		exitMenuItem.setActionCommand("exit");

		exitMenuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {

				miosStudioSID.destroy();

				System.exit(0);
			}
		});

		miosStudioSID.miosStudioSIDGUI.getFileMenu().add(exitMenuItem);

		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent we) {

				miosStudioSID.destroy();

				System.exit(0);
			}
		});

		frame.setVisible(true);
		splashScreen.setVisible(false);
	}
}
