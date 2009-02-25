import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Hashtable;
import java.util.prefs.Preferences;

import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JMenuItem;
import javax.swing.UIManager;

import org.midibox.apps.miosstudiosid.gui.MIOSStudioSIDGUI;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SplashScreen;

public class MIOSStudioSID extends MIOSStudio {

	protected static String frameTitle = "MIOS Studio + SID V2 Editor";

	protected static String splashTitle = "MIOS Studio beta 9 + SID V2 Editor";

	protected static String splashImage = "splash.jpg";

	protected static String frameComment = "MIOS Studio beta 9 + SID V2 Editor";

	public MIOSStudioSID() {

		this.miosStudio = new org.midibox.apps.miosstudiosid.MIOSStudioSID();

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

		this.miosStudioGUI = new MIOSStudioSIDGUI(
				(org.midibox.apps.miosstudiosid.MIOSStudioSID) miosStudio);
		miosStudioGUI.setCommentLabel(frameComment);

		this.windows = new Hashtable();
		setContentPane(miosStudioGUI);
		setJMenuBar(miosStudioGUI.createMenuBar());
	}

	protected Preferences getPreferences() {

		return Preferences.userRoot().node("org/midibox/miostudiosid/gui");
	}

	protected String getConfigFileName() {

		return ".miosstudiosid";
	}

	protected void createWindowsHashtable() {
		super.createWindowsHashtable();

		windows.put("sidv2librarianWindow", ((MIOSStudioSIDGUI) miosStudioGUI)
				.getSidv2librarianWindow());
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

		miosStudioSID.miosStudioGUI.getFileMenu().add(exitMenuItem);

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
