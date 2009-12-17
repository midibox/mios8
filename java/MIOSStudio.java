/*
 * @(#)MIOSStudio.java	beta8	2006/04/23
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
import java.awt.event.ContainerEvent;
import java.awt.event.ContainerListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;

import javax.swing.JApplet;
import javax.swing.JFrame;
import javax.swing.JMenuItem;
import javax.swing.UIManager;

import org.midibox.apps.miosstudio.gui.MIOSStudioGUI;
import org.midibox.apps.miosstudio.gui.xml.MIOSStudioGUIXML;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SplashScreen;

import sun.awt.WindowClosingListener;

/**
 * Driver class for launching MIOS Studio application. MIOS Studio can be
 * launched as an application or embedded in a a web page as an applet. When
 * launched as an application, the app shows a splash screen and is launched
 * within its own frame. Preferences are saved and restored from this class.
 * 
 * @author Adam King (adamjking@optusnet.com.au)
 */

public class MIOSStudio extends JApplet {

	protected static String oldConfigFileName = ".miosstudio";

	protected static String configDirectoryName = ".midibox";

	protected static String configFileName = "miosstudio.xml";

	protected static String frameTitle = "MIOS Studio";

	protected static String splashTitle = "MIOS Studio beta 9.4";

	protected static String splashImage = "splash.jpg";

	protected static String frameComment = "MIOS Studio beta 9.4";

	protected org.midibox.apps.miosstudio.MIOSStudio miosStudio;

	protected MIOSStudioGUI miosStudioGUI;

	public MIOSStudio() {

		this.miosStudio = new org.midibox.apps.miosstudio.MIOSStudio();

		File configDirectory = new File(System.getProperty("user.home"),
				configDirectoryName);

		File configFile = new File(configDirectory, configFileName);

		if (configFile.exists()) {

			MIOSStudioGUIXML miosStudioGUIXML = new MIOSStudioGUIXML(
					miosStudio, MIOSStudioGUIXML.TAG_ROOT_ELEMENT, true, true,
					true, true, true);

			miosStudioGUIXML.loadXML(configFile);

			this.miosStudioGUI = miosStudioGUIXML.getMiosStudioGUI();

		} else {

			// check for old config file

			configFile = new File(System.getProperty("user.home"),
					oldConfigFileName);

			if (configFile.exists()) {

				MIOSStudioGUIXML miosStudioGUIXML = new MIOSStudioGUIXML(
						miosStudio, MIOSStudioGUIXML.TAG_ROOT_ELEMENT, true,
						true, true, true, true);

				miosStudioGUIXML.loadXML(configFile);

				this.miosStudioGUI = miosStudioGUIXML.getMiosStudioGUI();

				configFile.delete();
			}
		}

		if (miosStudioGUI == null) {

			this.miosStudioGUI = new MIOSStudioGUI(miosStudio);
		}

		setContentPane(miosStudioGUI);

		if (miosStudioGUI.isMDI()) {

			miosStudioGUI.setCommentLabel(frameComment);
		}
	}

	public void init() {
		setJMenuBar(miosStudioGUI.createMenuBar());
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

			MIOSStudioGUIXML miosStudioGUIXML = new MIOSStudioGUIXML(
					miosStudioGUI, MIOSStudioGUIXML.TAG_ROOT_ELEMENT, true,
					true, true, true, true);

			miosStudioGUIXML.saveXML(configFile);
		}
	}

	public static void main(String[] args) {

		System.setProperty("apple.laf.useScreenMenuBar", "true");

		SplashScreen splashScreen = new SplashScreen(splashImage, splashTitle);

		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

		splashScreen.setLocation(
				(screenSize.width - splashScreen.getWidth()) / 2,
				(screenSize.height - splashScreen.getHeight()) / 2);

		splashScreen.setVisible(true);

		final JFrame frame = new JFrame(frameTitle);

		DialogOwner.setFrame(frame);

		frame.setIconImage(ImageLoader.getImageIcon("ucIcon.png").getImage());

		frame.setBounds(50, 50, (screenSize.width - 100),
				(screenSize.height - 100));

		final MIOSStudio miosStudio = new MIOSStudio();

		if (System.getProperty("mrj.version") == null) {

			JMenuItem exitMenuItem = new JMenuItem("Exit");
			exitMenuItem.setMnemonic(KeyEvent.VK_X);
			exitMenuItem.setActionCommand("exit");

			exitMenuItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ae) {

					miosStudio.destroy();
					
					System.exit(0);
				}
			});
			
			miosStudio.setJMenuBar(miosStudio.miosStudioGUI.createMenuBar());
			
			miosStudio.miosStudioGUI.getFileMenu().add(exitMenuItem);
			
			frame.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {

					miosStudio.destroy();

					System.exit(0);
				}
			});

		} else {

			frame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
			
			Runtime.getRuntime().addShutdownHook(new Thread() {

				public void run() {
					miosStudio.destroy();
				}

			});
			
			if (UIManager.getLookAndFeel().getClass().getName().toString() == UIManager
					.getSystemLookAndFeelClassName()) {

				frame.setJMenuBar(miosStudio.miosStudioGUI.createMenuBar());
			}

			else {

				miosStudio
						.setJMenuBar(miosStudio.miosStudioGUI.createMenuBar());
			}
		}
		
		frame.setContentPane(miosStudio);

		if (!miosStudio.miosStudioGUI.isMDI()) {

			frame.setResizable(false);

			frame.pack();

			miosStudio.miosStudioGUI.getToolBar().addContainerListener(
					new ContainerListener() {

						@Override
						public void componentRemoved(ContainerEvent e) {

							frame.pack();

						}

						@Override
						public void componentAdded(ContainerEvent e) {

							frame.pack();

						}
					});

		}

		frame.setVisible(true);

		splashScreen.setVisible(false);

	}

}
