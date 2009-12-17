/*
 * @(#)MIOSStudioGUI.java	beta8	2006/04/23
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

package org.midibox.apps.miosstudio.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.FontMetrics;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.File;
import java.net.URL;
import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.swing.Box;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComponent;
import javax.swing.JDesktopPane;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JSeparator;
import javax.swing.JTextField;
import javax.swing.JToolBar;
import javax.swing.UIManager;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

import org.midibox.apps.miosstudio.MIOSStudio;
import org.midibox.apps.miosstudio.gui.xml.MIOSStudioGUIXML;
import org.midibox.apps.miosstudio.xml.MIOSStudioXML;
import org.midibox.midi.MidiFilter;
import org.midibox.midi.MidiKeyboardController;
import org.midibox.midi.SysexSendReceive;
import org.midibox.midi.VirtualMidiDevice;
import org.midibox.midi.gui.MidiDeviceManagerGUI;
import org.midibox.midi.gui.MidiDeviceRoutingGUI;
import org.midibox.midi.gui.MidiFilterGUI;
import org.midibox.midi.gui.MidiFilterManagerGUI;
import org.midibox.midi.gui.MidiKeyboardControllerGUI;
import org.midibox.midi.gui.MidiMonitorFilteredGUI;
import org.midibox.midi.gui.SysexSendReceiveManagerGUI;
import org.midibox.mios.HexFileUpload;
import org.midibox.mios.gui.DebugFunctionGUI;
import org.midibox.mios.gui.HexFileUploadManagerGUI;
import org.midibox.mios.gui.LCDMessageGUI;
import org.midibox.utils.ResourceLoader;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.FontLoader;
import org.midibox.utils.gui.HelpPane;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SimpleFileChooserFilter;
import org.midibox.utils.gui.SplitButton;

public class MIOSStudioGUI extends JPanel implements ActionListener,
		MouseListener, PropertyChangeListener, MenuListener, Observer {

	protected MIOSStudio miosStudio;

	private JDesktopPane desktop;

	private HelpPane helpPane;

	protected Vector internalFrames;

	private Container helpWindow;

	private JDialog midiDeviceManagerDialog;

	private JDialog midiFilterManagerDialog;

	protected MidiDeviceRoutingGUI midiDeviceRoutingGUI;

	private Container midiDeviceRoutingWindow;

	private MidiMonitorFilteredGUI midiOutPortMonitorGUI;

	private Container midiOutPortMonitorWindow;

	private MidiMonitorFilteredGUI midiInPortMonitorGUI;

	private Container midiInPortMonitorWindow;

	private MidiKeyboardControllerGUI midiKeyboardControllerGUI;

	private Container midiKeyboardControllerWindow;

	private SysexSendReceiveManagerGUI sysexSendReceiveDeviceManagerGUI;

	private Container sysexSendReceiveDeviceManagerWindow;

	private HexFileUploadManagerGUI hexFileUploadDeviceManagerGUI;

	private Container hexFileUploadDeviceManagerWindow;

	/*
	 * private MemoryReadWriteGUI memoryReadWriteGUI;
	 */

	private LCDMessageGUI lcdMessageGUI;

	private Container lcdMessageWindow;

	private DebugFunctionGUI debugFunctionGUI;

	private Container debugFunctionWindow;

	private MidiMonitorFilteredGUI miosTerminalGUI;

	private Container miosTerminalWindow;

	private JDialog thruFilterProperties;

	protected JToolBar toolBar;

	protected JPopupMenu toolBarMenu;

	private ExternalButtonProperties externalButtonProperties;

	private Vector externalCommands = new Vector();

	protected JMenu fileMenu;

	protected JMenu midiMenu;

	protected JMenu miosMenu;

	protected JMenu optionsMenu;

	protected WindowMenu windowMenu;

	protected JMenu helpMenu;

	private JMenuItem midiThruMenuItem;

	private JCheckBoxMenuItem showInternalMenuItem;

	private JMenu lookAndFeelMenu;

	private String lookAndFeel;

	private JCheckBoxMenuItem MDIMenuItem;

	private boolean MDI;

	private boolean MDIflag;

	private JLabel commentLabel;

	private JMenu MRUMenu;

	private JPopupMenu MRUPopupMenu;

	private SplitButton openMRUButton;

	private static int maxMRU = 10;

	private static String currentDirectory = "";

	private static Vector MRU = new Vector();

	private static JFileChooser fc = null;

	public MIOSStudioGUI(MIOSStudio miosStudio) {

		this(miosStudio, true);
	}

	public MIOSStudioGUI(MIOSStudio miosStudio, boolean MDI) {

		super(new BorderLayout());

		this.miosStudio = miosStudio;

		this.MDI = MDI;

		this.MDIflag = MDI;

		miosStudio.addObserver(this);

		miosStudio.getSysexSendReceiveManager().addObserver(this);

		miosStudio.getHexFileUploadManager().addObserver(this);

		lookAndFeel = UIManager.getLookAndFeel().getClass().getName();

		createFrames();

		midiDeviceRoutingGUI.addMidiDeviceIcon(VirtualMidiDevice.class,
				ImageLoader.getImageIcon("virtualMidiDevice.png"));

		midiDeviceRoutingGUI.addMidiDeviceIcon(MidiFilter.class, ImageLoader
				.getImageIcon("filter.png"));

		// start In/Out/Terminal thread
		miosStudio.getMidiInPortMonitor().getMidiMonitor().deleteObserver(
				midiInPortMonitorGUI);

		miosStudio.getMidiOutPortMonitor().getMidiMonitor().deleteObserver(
				midiOutPortMonitorGUI);

		miosStudio.getMIOSTerminal().getMidiMonitor().deleteObserver(
				miosTerminalGUI);

		Thread t = new Thread() {

			public void run() {

				while (true) {

					midiInPortMonitorGUI.check();
					midiOutPortMonitorGUI.check();
					miosTerminalGUI.check();

					try {
						Thread.sleep(50);
					} catch (Exception e) {

					}
				}
			}

		};

		//t.setDaemon(true);
		t.start();

		add(createToolBar(), BorderLayout.NORTH);

		if (MDI) {
			add(createMainPanel(), BorderLayout.CENTER);
			add(createCommentBar(), BorderLayout.SOUTH);
		}
	}

	private JDesktopPane createMainPanel() {

		if (MDI) {

			desktop = new JDesktopPane() {
				ImageIcon backgroundImage = ImageLoader
						.getImageIcon("logo_watermark.png");

				public void paintComponent(Graphics g) {
					super.paintComponent(g);
					backgroundImage
							.paintIcon(this, g,
									(this.getWidth() - backgroundImage
											.getIconWidth()) - 20, (this
											.getHeight() - backgroundImage
											.getIconHeight()) - 20);
				}
			}; // JDesktop pane with paint method overridden to display
			// watermark

			desktop.setBackground(Color.WHITE);

		}

		return desktop;
	}

	protected void createFrames() {

		internalFrames = new Vector();

		// Help Window

		helpPane = HelpPane.createSingleton(ResourceLoader
				.getResource("help/index.html"));
		helpPane.addPropertyChangeListener(this);

		helpWindow = createFrame("MIOS Studio Help", true, true, ImageLoader
				.getImageIcon("help.png"), helpPane);

		packFrame(helpWindow);

		internalFrames.add(helpWindow);

		// MIDI Device Routing

		ImageIcon icon = ImageLoader.getImageIcon("midiRouting.png");
		midiDeviceRoutingGUI = new MidiDeviceRoutingGUI(miosStudio
				.getMidiDeviceRouting());

		midiDeviceRoutingWindow = createFrame("MIDI Device Routing", true, // resizable

				true, icon, midiDeviceRoutingGUI);

		packFrame(midiDeviceRoutingWindow);

		internalFrames.add(midiDeviceRoutingWindow);

		icon = ImageLoader.getImageIcon("ucIcon.png");

		/*
		 * midiDeviceRoutingGUI.addMidiDeviceIcon(miosStudio
		 * .getMiosStudioInPort(), icon);
		 * 
		 * midiDeviceRoutingGUI.addMidiDeviceIcon(miosStudio
		 * .getMiosStudioOutPort(), icon);
		 */

		// MIDI OUT Port
		midiOutPortMonitorGUI = new MidiMonitorFilteredGUI(miosStudio
				.getMidiOutPortMonitor());

		icon = ImageLoader.getImageIcon("midiOut.png");

		midiOutPortMonitorWindow = createFrame("MIDI Monitor: OUT", true, true,
				icon, midiOutPortMonitorGUI);

		packFrame(midiOutPortMonitorWindow);

		internalFrames.add(midiOutPortMonitorWindow);

		midiDeviceRoutingGUI.addMidiDeviceIcon(miosStudio
				.getMidiOutPortMonitor(), icon);

		// MIDI IN Port
		midiInPortMonitorGUI = new MidiMonitorFilteredGUI(miosStudio
				.getMidiInPortMonitor());

		icon = ImageLoader.getImageIcon("midiIn.png");

		midiInPortMonitorWindow = createFrame("MIDI Monitor: IN", true, true,
				icon, midiInPortMonitorGUI);

		packFrame(midiInPortMonitorWindow);

		internalFrames.add(midiInPortMonitorWindow);

		midiDeviceRoutingGUI.addMidiDeviceIcon(miosStudio
				.getMidiInPortMonitor(), icon);

		// MIDI Keyboard Controller

		icon = ImageLoader.getImageIcon("piano.png");

		midiKeyboardControllerGUI = new MidiKeyboardControllerGUI(miosStudio
				.getMidiKeyboardController());

		midiKeyboardControllerWindow = createFrame("MIDI Keyboard Controller",
				false, // resizable

				false, // maximizable
				icon, midiKeyboardControllerGUI);

		packFrame(midiKeyboardControllerWindow);

		internalFrames.add(midiKeyboardControllerWindow);

		midiDeviceRoutingGUI.addMidiDeviceIcon(MidiKeyboardController.class,
				icon);

		// Sysex Send/Receive

		sysexSendReceiveDeviceManagerGUI = new SysexSendReceiveManagerGUI(
				miosStudio.getSysexSendReceiveManager());

		icon = ImageLoader.getImageIcon("sysex.png");

		sysexSendReceiveDeviceManagerWindow = createFrame("Sysex Send/Receive",
				true, true, icon, sysexSendReceiveDeviceManagerGUI);

		packFrame(sysexSendReceiveDeviceManagerWindow);

		internalFrames.add(sysexSendReceiveDeviceManagerWindow);

		midiDeviceRoutingGUI.addMidiDeviceIcon(SysexSendReceive.class, icon);

		// Hex Upload

		hexFileUploadDeviceManagerGUI = new HexFileUploadManagerGUI(miosStudio
				.getHexFileUploadManager());

		icon = ImageLoader.getImageIcon("hex.png");

		hexFileUploadDeviceManagerWindow = createFrame("MIOS Hex File Upload",
				true, true, icon, hexFileUploadDeviceManagerGUI);

		packFrame(hexFileUploadDeviceManagerWindow);

		internalFrames.add(hexFileUploadDeviceManagerWindow);

		midiDeviceRoutingGUI.addMidiDeviceIcon(HexFileUpload.class, icon);

		// Read/Write
		/*
		 * icon = ImageLoader.getImageIcon("readWrite.png");
		 * 
		 * memoryReadWriteGUI = new MemoryReadWriteGUI(miosStudio
		 * .getMemoryReadWriteDevice().getMemoryReadWrite());
		 * 
		 * tabbedPane.addTab("MIOS Memory Read/Write", icon,
		 * memoryReadWriteGUI);
		 * midiDeviceRoutingGUI.addMidiDeviceIcon(MemoryReadWriteDevice.class,
		 * icon);
		 */

		// LCD Message
		icon = ImageLoader.getImageIcon("lcd.png");

		lcdMessageGUI = new LCDMessageGUI(miosStudio.getLcdMessage());

		lcdMessageWindow = createFrame("MIOS LCD Message", true, true, icon,
				lcdMessageGUI);

		packFrame(lcdMessageWindow);

		internalFrames.add(lcdMessageWindow);

		midiDeviceRoutingGUI
				.addMidiDeviceIcon(miosStudio.getLcdMessage(), icon);

		// MIOS Debug

		icon = ImageLoader.getImageIcon("debug.png");

		debugFunctionGUI = new DebugFunctionGUI(miosStudio.getDebugFunction());

		debugFunctionWindow = createFrame("MIOS Debug Functions", true, true,
				icon, debugFunctionGUI);

		packFrame(debugFunctionWindow);

		internalFrames.add(debugFunctionWindow);

		midiDeviceRoutingGUI.addMidiDeviceIcon(miosStudio.getDebugFunction(),
				icon);

		// MIOS Terminal
		miosTerminalGUI = new MidiMonitorFilteredGUI(miosStudio
				.getMIOSTerminal());

		icon = ImageLoader.getImageIcon("miosTerminal.png");

		miosTerminalWindow = createFrame("MIOS Terminal", true, true, icon,
				miosTerminalGUI);

		packFrame(miosTerminalWindow);

		internalFrames.add(miosTerminalWindow);

		midiDeviceRoutingGUI.addMidiDeviceIcon(miosStudio.getMIOSTerminal(),
				icon);
	}

	public JMenuBar createMenuBar() {
		JMenuBar menuBar = new JMenuBar();

		createFileMenu();
		createMIDIMenu();
		createMIOSMenu();
		createOptionsMenu();
		createWindowMenu();
		createHelpMenu();

		menuBar.add(fileMenu);
		menuBar.add(midiMenu);
		menuBar.add(miosMenu);
		menuBar.add(optionsMenu);
		menuBar.add(windowMenu);
		menuBar.add(helpMenu);

		return menuBar;
	}

	protected void createFileMenu() {

		fileMenu = new JMenu("File");

		JMenuItem menuItem = new JMenuItem("Open Workspace");
		menuItem.setActionCommand("open_workspace");
		menuItem.addActionListener(this);
		fileMenu.add(menuItem);

		MRUMenu = new JMenu("Open Recent Workspace");
		MRUMenu.addMenuListener(this);
		fileMenu.add(MRUMenu);

		menuItem = new JMenuItem("Save Workspace");
		menuItem.setActionCommand("save_workspace");
		menuItem.addActionListener(this);
		fileMenu.add(menuItem);
	}

	protected void createMIDIMenu() {
		midiMenu = new JMenu("MIDI");

		JMenuItem menuItem = new JMenuItem("MIDI Devices");
		menuItem.setActionCommand("midi_devices");
		menuItem.addActionListener(this);
		midiMenu.add(menuItem);

		menuItem = new JMenuItem("MIDI Filters");
		menuItem.setActionCommand("midi_filters");
		menuItem.addActionListener(this);
		midiMenu.add(menuItem);

		menuItem = new JMenuItem("MIDI Monitor: OUT");
		menuItem.setActionCommand("midi_out_port_monitor");
		menuItem.addActionListener(this);
		midiMenu.add(menuItem);

		menuItem = new JMenuItem("MIDI Monitor: IN");
		menuItem.setActionCommand("midi_in_port_monitor");
		menuItem.addActionListener(this);
		midiMenu.add(menuItem);

		menuItem = new JMenuItem("MIDI Keyboard Controller");
		menuItem.setActionCommand("piano_controller");
		menuItem.addActionListener(this);
		midiMenu.add(menuItem);

		menuItem = new JMenuItem("Sysex Send/Recieve");
		menuItem.setActionCommand("sysex");
		menuItem.addActionListener(this);
		midiMenu.add(menuItem);
	}

	protected void createMIOSMenu() {

		miosMenu = new JMenu("MIOS");

		JMenuItem menuItem = new JMenuItem("MIOS Hex File Upload");
		menuItem.setActionCommand("open_hex_file");
		menuItem.addActionListener(this);
		miosMenu.add(menuItem);

		menuItem = new JMenuItem("MIOS LCD Message");
		menuItem.setActionCommand("lcd_message");
		menuItem.addActionListener(this);
		miosMenu.add(menuItem);

		menuItem = new JMenuItem("MIOS Debug Functions");
		menuItem.setActionCommand("debug_functions");
		menuItem.addActionListener(this);
		miosMenu.add(menuItem);

		menuItem = new JMenuItem("MIOS Terminal");
		menuItem.setActionCommand("mios_terminal");
		menuItem.addActionListener(this);
		miosMenu.add(menuItem);
	}

	protected void createOptionsMenu() {

		optionsMenu = new JMenu("Options");

		JMenuItem menuItem = new JMenuItem("MIDI Device Routing");
		menuItem.setActionCommand("midi_routing");
		menuItem.addActionListener(this);
		optionsMenu.add(menuItem);

		showInternalMenuItem = new JCheckBoxMenuItem("Show Internal Routing",
				miosStudio.isRouteIndividualDevices());
		showInternalMenuItem.setActionCommand("route_internal");
		showInternalMenuItem.addActionListener(this);
		optionsMenu.add(showInternalMenuItem);

		menuItem = new JMenuItem("Restore Default Internal Routing");
		menuItem.setActionCommand("restore_default");
		menuItem.addActionListener(this);
		optionsMenu.add(menuItem);

		optionsMenu.addSeparator();
		JMenu thruMenu = new JMenu("MIDI Thru");

		midiThruMenuItem = new JCheckBoxMenuItem("Send MIDI Thru via Out Port",
				miosStudio.isMidiThruOutPort());
		midiThruMenuItem.setActionCommand("midi_thru");
		midiThruMenuItem.addActionListener(this);
		thruMenu.add(midiThruMenuItem);

		menuItem = new JMenuItem("MIDI Thru Filter");
		menuItem.setActionCommand("midi_thru_filter");
		menuItem.addActionListener(this);
		thruMenu.add(menuItem);

		/*
		 * menuItem = new JMenuItem("Show MIDI Thru Map");
		 * menuItem.setActionCommand("midi_thru_map");
		 * menuItem.addActionListener(this); thruMenu.add(menuItem);
		 */

		optionsMenu.add(thruMenu);
		optionsMenu.addSeparator();
		lookAndFeelMenu = new JMenu("Look & Feel");
		lookAndFeelMenu.addMenuListener(this);
		optionsMenu.add(lookAndFeelMenu);

		MDIMenuItem = new JCheckBoxMenuItem("MDI", MDIflag);
		MDIMenuItem.addActionListener(this);
		MDIMenuItem.setActionCommand("MDI");
		optionsMenu.add(MDIMenuItem);
	}

	protected void createWindowMenu() {

		windowMenu = new WindowMenu(this);
		windowMenu.setMnemonic(KeyEvent.VK_W);
	}

	protected void createHelpMenu() {

		helpMenu = new JMenu("Help");
		helpMenu.setMnemonic(KeyEvent.VK_H);

		JMenuItem menuItem = new JMenuItem("MIOS Studio Help");
		menuItem.setMnemonic(KeyEvent.VK_I);

		menuItem.setActionCommand("help_index");
		menuItem.addActionListener(this);
		helpMenu.add(menuItem);

		menuItem = new JMenuItem("MIOS Function Reference");
		menuItem.setMnemonic(KeyEvent.VK_F);

		menuItem.setActionCommand("mios_fun");
		menuItem.addActionListener(this);
		helpMenu.add(menuItem);
	}

	public void addExternalCommandButton(String commandName, String command) {
		ExternalCommandButton button = new ExternalCommandButton();
		button.setCommandName(commandName);
		button.setExternalCommand(command);
		externalCommands.add(button);
		toolBar.add(button);
		assignExternalButtonMnemonics();
		toolBar.setVisible(false);
		toolBar.setVisible(true);
	}

	public SysexSendReceiveManagerGUI getSysexSendReceiveDeviceManagerGUI() {
		return sysexSendReceiveDeviceManagerGUI;
	}

	public HexFileUploadManagerGUI getHexFileUploadDeviceManagerGUI() {
		return hexFileUploadDeviceManagerGUI;
	}

	public Vector getInternalFrames() {
		return internalFrames;
	}

	public static String getCurrentDirectory() {
		return currentDirectory;
	}

	public static void setCurrentDirectory(String currentDirectory) {

		MIOSStudioGUI.currentDirectory = currentDirectory;
	}

	public void setLookAndFeel(String lookAndFeel) {

		this.lookAndFeel = lookAndFeel;

		if (!lookAndFeel
				.equals(UIManager.getLookAndFeel().getClass().getName())) {

			JOptionPane
					.showMessageDialog(
							MIOSStudioGUI.this,
							"The selected Look & Feel will be applied the next time you restart MIOS Studio",
							"ALERT", JOptionPane.ERROR_MESSAGE);
		}
	}

	public static Vector getMRU() {
		return MRU;
	}

	public void removeExternalCommandButton(ExternalCommandButton button) {
		externalCommands.remove(button);
		toolBar.remove(button);
		assignExternalButtonMnemonics();
		toolBar.setVisible(false);
		toolBar.setVisible(true);
	}

	protected void assignExternalButtonMnemonics() {
		for (int i = 0; i < externalCommands.size() && i < 9; i++) {
			((ExternalCommandButton) externalCommands.elementAt(i))
					.setMnemonic(0x31 + i);
		}
	}

	protected JToolBar createToolBar() {

		toolBar = new JToolBar("Tool Bar", JToolBar.HORIZONTAL);
		toolBar.setFloatable(false);
		toolBar.setRollover(true);

		createWorkspaceButtons();

		toolBar.addSeparator();

		createMIDIButtons();

		toolBar.addSeparator();

		createMIOSButtons();

		toolBar.addSeparator();

		createOptionsButtons();

		toolBar.addSeparator();

		createHelpButtons();

		toolBar.addSeparator();

		toolBarMenu = new JPopupMenu();
		JMenuItem addButton = new JMenuItem("Add External Command Button");
		addButton.setActionCommand("add_command_button");
		addButton.addActionListener(this);
		toolBarMenu.add(addButton);

		toolBar.addMouseListener(this);
		toolBar.setVisible(true);
		return toolBar;
	}

	protected void createWorkspaceButtons() {

		Insets insets = new Insets(2, 2, 2, 2);

		JButton button = new JButton(ImageLoader.getImageIcon("open.png"));
		button.setToolTipText("Open Workspace");
		button.setActionCommand("open_workspace");
		button.addActionListener(this);
		button.setMargin(insets);

		MRUPopupMenu = new JPopupMenu();
		MRUPopupMenu.addPopupMenuListener(new PopupMenuListener() {

			public void popupMenuCanceled(PopupMenuEvent e) {

			}

			public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {

			}

			public void popupMenuWillBecomeVisible(PopupMenuEvent e) {

				MIOSStudioGUI.this.buildMRUMenu(MRUPopupMenu);

			}
		});

		openMRUButton = new SplitButton(button, MRUPopupMenu);
		openMRUButton.setRollover(true);

		toolBar.add(openMRUButton);

		button = new JButton(ImageLoader.getImageIcon("save.png"));
		button.setToolTipText("Save Workspace");
		button.setActionCommand("save_workspace");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

	}

	protected void createMIDIButtons() {

		Insets insets = new Insets(2, 2, 2, 2);

		JButton button = new JButton(ImageLoader
				.getImageIcon("midiDevices.png"));
		button.setToolTipText("MIDI Devices");
		button.setActionCommand("midi_devices");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("filter.png"));
		button.setToolTipText("MIDI Filters");
		button.setActionCommand("midi_filters");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

		toolBar.addSeparator();

		button = new JButton(ImageLoader.getImageIcon("midiOut.png"));
		button.setToolTipText("MIDI Monitor: OUT");
		button.setActionCommand("midi_out_port_monitor");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("midiIn.png"));
		button.setToolTipText("MIDI Monitor: IN");
		button.setActionCommand("midi_in_port_monitor");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("piano.png"));
		button.setToolTipText("MIDI Keyboard Controller");
		button.setActionCommand("piano_controller");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("sysex.png"));
		button.setToolTipText("Sysex Send/Receive");
		button.setActionCommand("sysex");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);
	}

	protected void createMIOSButtons() {

		Insets insets = new Insets(2, 2, 2, 2);

		JButton button = new JButton(ImageLoader.getImageIcon("hex.png"));
		button.setToolTipText("MIOS Hex File Upload");
		button.setActionCommand("open_hex_file");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("lcd.png"));
		button.setToolTipText("MIOS LCD Message");
		button.setActionCommand("lcd_message");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("debug.png"));
		button.setToolTipText("MIOS Debug Functions");
		button.setActionCommand("debug_functions");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("miosTerminal.png"));
		button.setToolTipText("MIOS Terminal");
		button.setActionCommand("mios_terminal");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);
	}

	protected void createOptionsButtons() {

		Insets insets = new Insets(2, 2, 2, 2);

		JButton button = new JButton(ImageLoader
				.getImageIcon("midiRouting.png"));
		button.setToolTipText("MIDI Device Routing");
		button.setActionCommand("midi_routing");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);
	}

	protected void createHelpButtons() {

		Insets insets = new Insets(2, 2, 2, 2);

		JButton button = new JButton(ImageLoader.getImageIcon("help.png"));
		button.setToolTipText("MIOS Studio Help");
		button.setActionCommand("help_window");
		button.addActionListener(this);
		button.setMargin(insets);
		toolBar.add(button);

	}

	private void runExternalCommand(String command) {
		try {
			Runtime rt = Runtime.getRuntime();
			rt.exec(command);
		} catch (Exception e) {
			JOptionPane.showMessageDialog(this, "Cannot run command: "
					+ command + "\nReceived error: " + e.getLocalizedMessage(),
					"Cannot run command", JOptionPane.ERROR_MESSAGE);
		}
	}

	private JPanel createCommentBar() {
		JPanel commentBar = new JPanel(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridheight = 1;
		gbc.gridwidth = 1;

		gbc.fill = GridBagConstraints.BOTH;
		gbc.weightx = 1.0;
		commentBar.add(Box.createGlue(), gbc);
		gbc.gridx++;

		gbc.weightx = 0.0;
		gbc.insets = new Insets(2, 2, 2, 2);

		commentBar.add(new JSeparator(JSeparator.VERTICAL), gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 10, 2, 20);

		commentLabel = new JLabel();
		commentBar.add(commentLabel, gbc);
		return commentBar;
	}

	public void setCommentLabel(String text) {

		commentLabel.setText(text);
	}

	private void showMidiThruFilter() {
		if (thruFilterProperties == null) {
			final MidiFilterGUI midiFilterGUI = new MidiFilterGUI(miosStudio
					.getMidiThruFilter());

			thruFilterProperties = new JDialog(DialogOwner.getFrame(),
					"MIDI Thru Filter", false);
			thruFilterProperties.setContentPane(midiFilterGUI);
			thruFilterProperties.pack();
			thruFilterProperties.setLocationRelativeTo(this);
			thruFilterProperties.setVisible(true);

			thruFilterProperties.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					miosStudio.getMidiThruFilter()
							.deleteObserver(midiFilterGUI);
					thruFilterProperties = null;
				}
			});
		}
		thruFilterProperties.requestFocus();
	}

	/*
	 * private void showMidiThruMap() { if (thruMapProperties == null) { final
	 * MidiMapGUI midiMapGUI = new MidiMapGUI(miosStudio
	 * .getMidiThruMapDevice().getMidiMap());
	 * 
	 * thruMapProperties = new JDialog();
	 * thruMapProperties.setContentPane(midiMapGUI);
	 * thruMapProperties.setModal(false); thruMapProperties.setTitle("MIDI Thru
	 * Map"); thruMapProperties.pack();
	 * thruMapProperties.setLocationRelativeTo(this);
	 * thruMapProperties.setVisible(true);
	 * 
	 * thruMapProperties.addWindowListener(new WindowAdapter() { public void
	 * windowClosing(WindowEvent we) { miosStudio.getMidiThruMapDevice()
	 * .deleteObserver(midiMapGUI); thruMapProperties = null; } }); }
	 * thruMapProperties.requestFocus(); }
	 */

	protected void onHelpIndex() {
		URL currentURL = helpPane.getCurrentURL();
		if (currentURL != null) {
			if (!currentURL.equals(ResourceLoader
					.getResource("help/index.html"))) {
				helpPane.goToURL(ResourceLoader.getResource("help/index.html"));
			} else {
				showFrame(helpWindow);
			}
		} else {
			helpPane.goToURL(ResourceLoader.getResource("help/index.html"));
		}
	}

	protected void onMiosFun() {
		helpPane.goToURL(ResourceLoader.getResource("help/mios_fun.html"));
	}

	public void showFrame(Container container) {

		if (MDI) {

			JInternalFrame frame = (JInternalFrame) container;
			try {
				if (!frame.isVisible()) {
					frame.setVisible(true);
					desktop.add(frame);
					frame.moveToFront();
					frame.setSelected(true);
				} else if (frame.isIcon()) {
					frame.setIcon(false);
					frame.moveToFront();
					frame.setSelected(true);
				} else {
					frame.moveToFront();
					frame.setSelected(true);
				}
			} catch (java.beans.PropertyVetoException e) {
				System.out.println(e.toString());
			}
		} else {
			JFrame frame = (JFrame) container;

			if (!frame.isVisible()) {
				frame.setVisible(true);
				frame.toFront();
			} else if (frame.getExtendedState() == Frame.ICONIFIED) {
				frame.setExtendedState(Frame.NORMAL);
				frame.toFront();
			} else {
				frame.toFront();
			}
		}
	}

	private void showMidiDeviceManagerDialog() {
		if (midiDeviceManagerDialog == null) {
			final MidiDeviceManagerGUI midiDeviceManagerGUI = new MidiDeviceManagerGUI(
					miosStudio.getMidiDeviceManager());

			midiDeviceManagerDialog = new JDialog(DialogOwner.getFrame(),
					"MIDI Devices", false);
			midiDeviceManagerDialog.setContentPane(midiDeviceManagerGUI);
			midiDeviceManagerDialog.pack();
			midiDeviceManagerDialog.setLocationRelativeTo(this);
			midiDeviceManagerDialog.setVisible(true);

			midiDeviceManagerDialog.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					miosStudio.getMidiDeviceManager().deleteObserver(
							midiDeviceManagerGUI);
					midiDeviceManagerDialog = null;
				}
			});
		}
		midiDeviceManagerDialog.requestFocus();
	}

	private void showMidiFilterManagerDialog() {
		if (midiFilterManagerDialog == null) {
			final MidiFilterManagerGUI midiFilterManagerGUI = new MidiFilterManagerGUI(
					miosStudio.getMidiFilterManager());

			midiFilterManagerDialog = new JDialog(DialogOwner.getFrame(),
					"Filters", false);
			midiFilterManagerDialog.setContentPane(midiFilterManagerGUI);
			midiFilterManagerDialog.pack();
			midiFilterManagerDialog.setLocationRelativeTo(this);
			midiFilterManagerDialog.setVisible(true);

			midiFilterManagerDialog.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					miosStudio.getMidiFilterManager().deleteObserver(
							midiFilterManagerGUI);
					midiFilterManagerDialog = null;
				}
			});
		}
		midiFilterManagerDialog.requestFocus();
	}

	/*
	 * private void showMapManagerDialog() { if (midiMapManagerDialog == null) {
	 * final MidiMapDeviceManagerGUI midiMapManagerGUI = new
	 * MidiMapDeviceManagerGUI( midiDeviceRouting.getMidiMapManager());
	 * 
	 * midiMapManagerDialog = new JDialog();
	 * midiMapManagerDialog.setModal(false);
	 * midiMapManagerDialog.setTitle("Maps");
	 * midiMapManagerDialog.setContentPane(midiMapManagerGUI);
	 * midiMapManagerDialog.pack();
	 * midiMapManagerDialog.setLocationRelativeTo(this);
	 * midiMapManagerDialog.setVisible(true);
	 * 
	 * midiMapManagerDialog.addWindowListener(new WindowAdapter() { public void
	 * windowClosing(WindowEvent we) {
	 * midiDeviceRouting.getMidiMapManager().deleteObserver( midiMapManagerGUI);
	 * midiMapManagerDialog = null; } }); } midiMapManagerDialog.requestFocus();
	 * }
	 */

	private void buildMRUMenu(JComponent menu) {

		menu.removeAll();

		Iterator it = MRU.iterator();

		while (it.hasNext()) {

			final JMenuItem menuItem = new JMenuItem((String) it.next());

			menuItem.addActionListener(new ActionListener() {

				public void actionPerformed(ActionEvent ae) {

					File file = new File(menuItem.getText());

					openWorkspace(file);
				}
			});

			menu.add(menuItem, 0);
		}
	}

	private void buildLookAndFeel() {

		JCheckBoxMenuItem item;

		for (int i = 0; i < UIManager.getInstalledLookAndFeels().length; i++) {
			final int l = i;
			item = new JCheckBoxMenuItem(
					UIManager.getInstalledLookAndFeels()[i].getName());
			lookAndFeelMenu.add(item);
			if (UIManager.getInstalledLookAndFeels()[i].getClassName().equals(
					lookAndFeel)) {
				item.setSelected(true);
			}
			item.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ae) {
					try {

						setLookAndFeel(UIManager.getInstalledLookAndFeels()[l]
								.getClassName());

					} catch (Exception e) {
						System.out.println(e.toString());
					}
				}
			});
		}
	}

	public JMenu getFileMenu() {
		return fileMenu;
	}

	public MIOSStudio getMiosStudio() {
		return miosStudio;
	}

	public Vector getExternalCommands() {
		return externalCommands;
	}

	public String getLookAndFeel() {
		return lookAndFeel;
	}

	public boolean isMDI() {
		return MDI;
	}

	public boolean isMDIflag() {
		return MDIflag;
	}

	public void setMDIflag(boolean MDIflag) {

		if (this.MDI != MDIflag) {

			JOptionPane
					.showMessageDialog(
							MIOSStudioGUI.this,
							"The selected layout will be applied the next time you restart MIOS Studio",
							"ALERT", JOptionPane.ERROR_MESSAGE);
		}

		this.MDIflag = MDIflag;
	}

	public JDesktopPane getDesktop() {
		return desktop;
	}

	protected void openWorkspace() {

		if (fc == null) {
			fc = new JFileChooser(currentDirectory);
			SimpleFileChooserFilter fileFilter = new SimpleFileChooserFilter(
					"XML files", "xml", "XML Workspace Definition");
			fc.addChoosableFileFilter(fileFilter);
			fc.setAcceptAllFileFilterUsed(false);
		}

		File noFile = new File("");
		File noFiles[] = { noFile };
		fc.setSelectedFile(noFile);
		fc.setSelectedFiles(noFiles);

		int nRetVal = fc.showOpenDialog(this);

		if (nRetVal == JFileChooser.APPROVE_OPTION) {

			final File file = fc.getSelectedFile();

			openWorkspace(file);

			currentDirectory = fc.getCurrentDirectory().toString();
		}
	}

	protected void openWorkspace(File workspaceFile) {

		final File file = workspaceFile;

		if (file.exists()) {

			final WorkspaceOptionDialog workspaceOptionDialog = new WorkspaceOptionDialog();

			Boolean bool = MIOSStudioGUIXML.containsTag(file,
					MIOSStudioXML.TAG_ROOT_ELEMENT);
			workspaceOptionDialog.modelCheckBox.setEnabled(bool);
			workspaceOptionDialog.modelCheckBox.setSelected(bool);

			bool = MIOSStudioGUIXML.containsTag(file,
					MIOSStudioGUIXML.TAG_INTERNAL_FRAMES);
			workspaceOptionDialog.guiCheckBox.setEnabled(bool);
			workspaceOptionDialog.guiCheckBox.setSelected(bool);

			bool = MIOSStudioGUIXML.containsTag(file,
					MIOSStudioGUIXML.TAG_EXTERNAL_COMMANDS);
			workspaceOptionDialog.externalCommandsCheckBox.setEnabled(bool);
			workspaceOptionDialog.externalCommandsCheckBox.setSelected(bool);

			bool = MIOSStudioGUIXML.containsTag(file,
					MIOSStudioGUIXML.TAG_LOOK_AND_FEEL);
			workspaceOptionDialog.lookAndFeelCheckBox.setEnabled(bool);
			workspaceOptionDialog.lookAndFeelCheckBox.setSelected(bool);

			workspaceOptionDialog.pack();

			workspaceOptionDialog.okButton
					.addActionListener(new ActionListener() {

						public void actionPerformed(ActionEvent ae) {

							workspaceOptionDialog.setVisible(false);

							MIOSStudioGUIXML miosStudioGUIXML = new MIOSStudioGUIXML(
									MIOSStudioGUI.this,
									MIOSStudioGUIXML.TAG_ROOT_ELEMENT,
									workspaceOptionDialog.modelCheckBox
											.isSelected(),
									workspaceOptionDialog.guiCheckBox
											.isSelected(),
									workspaceOptionDialog.externalCommandsCheckBox
											.isSelected(),
									workspaceOptionDialog.lookAndFeelCheckBox
											.isSelected(), false);

							miosStudioGUIXML.loadXML(file);

						}
					});

			workspaceOptionDialog.setLocationRelativeTo(this);

			workspaceOptionDialog.setVisible(true);

			saveMRU(file.getPath());

		} else {

			JOptionPane.showMessageDialog(this,
					"Workspace configuration file no longer exists",
					"File does not exist", JOptionPane.ERROR_MESSAGE);
		}
	}

	public void saveMRU(String file) {

		MRU.remove(file);

		MRU.add(file);

		for (int i = MRU.size() - maxMRU; i > 0; i--) {

			MRU.removeElementAt(i - 1);
		}
	}

	protected void saveWorkspace() {

		if (fc == null) {
			fc = new JFileChooser(currentDirectory);
			SimpleFileChooserFilter fileFilter = new SimpleFileChooserFilter(
					"XML files", "xml", "XML Workspace Definition");
			fc.addChoosableFileFilter(fileFilter);
			fc.setAcceptAllFileFilterUsed(false);
		}

		File noFile = new File("");
		File noFiles[] = { noFile };
		fc.setSelectedFile(noFile);
		fc.setSelectedFiles(noFiles);

		int nRetVal = fc.showSaveDialog(this);

		if (nRetVal == JFileChooser.APPROVE_OPTION) {

			final File file = fc.getSelectedFile();

			final WorkspaceOptionDialog workspaceOptionDialog = new WorkspaceOptionDialog();

			workspaceOptionDialog.pack();

			workspaceOptionDialog.okButton
					.addActionListener(new ActionListener() {

						public void actionPerformed(ActionEvent ae) {

							workspaceOptionDialog.setVisible(false);

							MIOSStudioGUIXML miosStudioGUIXML = new MIOSStudioGUIXML(
									MIOSStudioGUI.this,
									MIOSStudioGUIXML.TAG_ROOT_ELEMENT,
									workspaceOptionDialog.modelCheckBox
											.isSelected(),
									workspaceOptionDialog.guiCheckBox
											.isSelected(),
									workspaceOptionDialog.externalCommandsCheckBox
											.isSelected(),
									workspaceOptionDialog.lookAndFeelCheckBox
											.isSelected(), false);

							miosStudioGUIXML.saveXML(file);

							currentDirectory = fc.getCurrentDirectory()
									.toString();
						}
					});

			workspaceOptionDialog.setLocationRelativeTo(this);

			workspaceOptionDialog.setVisible(true);

			saveMRU(file.getPath());
		}
	}

	/*
	 * public MIOSStudioInternalFrame getSysexSendReceiveDeviceManagerWindow() {
	 * return sysexSendReceiveDeviceManagerWindow; }
	 */
	public void mousePressed(MouseEvent me) {
		Object source = me.getSource();

		if (source == toolBar) {
			if (me.getButton() == MouseEvent.BUTTON3) {
				toolBarMenu.show(toolBar, me.getX(), me.getY());
			}
		} else if (source instanceof ExternalCommandButton) {
			if (me.getButton() == MouseEvent.BUTTON3) {
				((ExternalCommandButton) source).popMenu.show(
						((ExternalCommandButton) source), me.getX(), me.getY());
			}
		}
	}

	public void mouseReleased(MouseEvent me) {

	}

	public void mouseClicked(MouseEvent me) {

	}

	public void mouseEntered(MouseEvent me) {

	}

	public void mouseExited(MouseEvent me) {

	}

	public void propertyChange(PropertyChangeEvent pce) {
		showFrame(helpWindow);
	}

	public void menuCanceled(MenuEvent e) {
	}

	public void menuDeselected(MenuEvent e) {
		Object source = e.getSource();

		if (source == windowMenu) {
			windowMenu.removeAll();
		} else if (source == lookAndFeelMenu) {
			lookAndFeelMenu.removeAll();
		}
	}

	public void menuSelected(MenuEvent e) {
		Object source = e.getSource();

		if (source == windowMenu) {
			windowMenu.buildChildMenus();
		} else if (source == lookAndFeelMenu) {
			buildLookAndFeel();
		} else if (source == MRUMenu) {
			buildMRUMenu(MRUMenu);
		}
	}

	public void update(Observable observable, Object object) {

		if (object == MIOSStudio.ROUTE_INDIVIDUAL_DEVICES) {

			showInternalMenuItem.setSelected(miosStudio
					.isRouteIndividualDevices());

		} else if (object == MIOSStudio.MIDI_THRU_OUT_PORT) {

			midiThruMenuItem.setSelected(miosStudio.isMidiThruOutPort());

		} else if (observable == miosStudio.getSysexSendReceiveManager()) {

			Dimension size = sysexSendReceiveDeviceManagerWindow.getSize();

			Dimension preferred = sysexSendReceiveDeviceManagerWindow
					.getPreferredSize();

			if ((size.height < preferred.height)
					|| size.width < preferred.width) {

				packFrame(sysexSendReceiveDeviceManagerWindow);
			}

		} else if (observable == miosStudio.getHexFileUploadManager()) {

			Dimension size = hexFileUploadDeviceManagerWindow.getSize();

			Dimension preferred = hexFileUploadDeviceManagerWindow
					.getPreferredSize();

			if ((size.height < preferred.height)
					|| size.width < preferred.width) {

				packFrame(hexFileUploadDeviceManagerWindow);
			}
		}
	}

	public void actionPerformed(ActionEvent ae) {

		if (ae.getActionCommand().equals("midi_devices")) {
			showMidiDeviceManagerDialog();

		} else if (ae.getActionCommand().equals("midi_filters")) {
			showMidiFilterManagerDialog();

		} else if (ae.getActionCommand().equals("midi_routing")) {
			showFrame(midiDeviceRoutingWindow);

		} else if (ae.getActionCommand().equals("midi_out_port_monitor")) {
			showFrame(midiOutPortMonitorWindow);

		} else if (ae.getActionCommand().equals("midi_in_port_monitor")) {
			showFrame(midiInPortMonitorWindow);

		} else if (ae.getActionCommand().equals("piano_controller")) {
			showFrame(midiKeyboardControllerWindow);

		} else if (ae.getActionCommand().equals("sysex")) {
			showFrame(sysexSendReceiveDeviceManagerWindow);

		} else if (ae.getActionCommand().equals("open_hex_file")) {
			showFrame(hexFileUploadDeviceManagerWindow);

		} else if (ae.getActionCommand().equals("lcd_message")) {
			showFrame(lcdMessageWindow);

		} else if (ae.getActionCommand().equals("debug_functions")) {
			showFrame(debugFunctionWindow);

		} else if (ae.getActionCommand().equals("mios_terminal")) {
			showFrame(miosTerminalWindow);

		} else if (ae.getActionCommand().equals("route_internal")) {
			showFrame(midiDeviceRoutingWindow);
			miosStudio.setRouteIndividualDevices(showInternalMenuItem
					.isSelected());

		} else if (ae.getActionCommand().equals("restore_default")) {
			miosStudio.connectDevices();

		} else if (ae.getActionCommand().equals("midi_thru_filter")) {
			showMidiThruFilter();

		} else if (ae.getActionCommand().equals("midi_thru")) {
			miosStudio.setMidiThruOutPort(midiThruMenuItem.isSelected());

		} else if (ae.getActionCommand().equals("help_window")) {
			showFrame(helpWindow);

		} else if (ae.getActionCommand().equals("help_index")) {
			onHelpIndex();

		} else if (ae.getActionCommand().equals("mios_fun")) {
			onMiosFun();

		} else if (ae.getActionCommand().equals("externalCommand")) {
			runExternalCommand(((ExternalCommandButton) ae.getSource()).externalCommand);

		} else if (ae.getActionCommand().equals("add_command_button")) {
			addExternalCommandButton("[empty]", "");

		} else if (ae.getActionCommand().equals("open_workspace")) {
			openWorkspace();

		} else if (ae.getActionCommand().equals("save_workspace")) {
			saveWorkspace();

		} else if (ae.getActionCommand().equals("MDI")) {

			setMDIflag(MDIMenuItem.isSelected());
		}
	}

	private Container createFrame(String name, boolean resizeable,
			boolean maximizeable, ImageIcon icon, JComponent contentPane) {

		Container container;

		if (MDI) {

			container = new MIOSStudioInternalFrame(name, resizeable,
					maximizeable, icon, contentPane);

		} else {

			JFrame frame = new JFrame(name);
			frame.setResizable(resizeable);
			frame.setContentPane(contentPane);

			frame.setIconImage(icon.getImage());
			
			frame.setJMenuBar(createMenuBar());

			container = frame;
		}

		return container;
	}

	private void packFrame(Container frame) {

		if (frame instanceof JFrame) {

			((JFrame) frame).pack();
		}

		if (frame instanceof JInternalFrame) {

			((JInternalFrame) frame).pack();
		}
	}

	public JToolBar getToolBar() {

		return toolBar;
	}

	public class MIOSStudioInternalFrame extends JInternalFrame {
		public MIOSStudioInternalFrame(String name, boolean resizeable,
				boolean maximizeable, Icon icon, JComponent contentPane) {
			super(name, resizeable, true, maximizeable, true);
			setFrameIcon(icon);
			setContentPane(contentPane);
		}

		public Dimension getMinimumSize() {
			return getPreferredSize();
		}
	}

	public class ExternalCommandButton extends JButton implements
			ActionListener {

		public String externalCommand = "";

		public String commandName = "";

		public JPopupMenu popMenu;

		public ExternalCommandButton() {
			super(ImageLoader.getImageIcon("external.png"));

			Insets insets = new Insets(2, 2, 2, 2);

			setMargin(insets);
			setActionCommand("externalCommand");
			addActionListener(MIOSStudioGUI.this);
			setToolTipText(commandName);

			popMenu = new JPopupMenu();

			JMenuItem item = new JMenuItem("Properties");
			item.setActionCommand("show_properties");
			item.addActionListener(this);
			popMenu.add(item);

			item = new JMenuItem("Remove Button");
			item.setActionCommand("remove_command_button");
			item.addActionListener(this);
			popMenu.add(item);

			addMouseListener(MIOSStudioGUI.this);
		}

		public void actionPerformed(ActionEvent ae) {
			if (ae.getActionCommand().equals("remove_command_button")) {
				removeExternalCommandButton(this);
			} else {
				externalButtonProperties = new ExternalButtonProperties(this);
				externalButtonProperties.setSize(500, 150);
				externalButtonProperties
						.setLocationRelativeTo(MIOSStudioGUI.this);
				externalButtonProperties.setVisible(true);
			}
		}

		public void setExternalCommand(String externalCommand) {
			this.externalCommand = externalCommand;
		}

		public void setCommandName(String commandName) {
			this.commandName = commandName;
			setToolTipText(commandName);
		}

		public void paintComponent(Graphics g) {
			super.paintComponent(g);
			Graphics2D g2d = (Graphics2D) g;
			g.setFont(FontLoader.getFont("uni05_53.ttf", 8f));
			String number = "" + (externalCommands.indexOf(this) + 1);
			FontMetrics fm = g2d.getFontMetrics();
			int offsetX = fm.stringWidth(number);
			g.drawString(number, getWidth() - offsetX - 4, getHeight() - 4);
		}
	}

	public class ExternalButtonProperties extends JDialog implements
			ActionListener {
		public ExternalCommandButton externalCommandButton;

		public JTextField commandName;

		public JTextField commandString;

		public JButton okButton;

		public ExternalButtonProperties(
				ExternalCommandButton externalCommandButton) {

			super(DialogOwner.getFrame(), "External Command Button "
					+ (externalCommands.indexOf(externalCommandButton) + 1),
					false);

			this.externalCommandButton = externalCommandButton;

			setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);

			setLayout(new GridBagLayout());
			GridBagConstraints gbc = new GridBagConstraints();
			gbc.gridx = 0;
			gbc.gridy = 0;
			gbc.gridwidth = 1;
			gbc.gridheight = 1;

			gbc.insets = new Insets(4, 4, 4, 4);

			gbc.anchor = GridBagConstraints.WEST;
			JLabel label = new JLabel("Command Name: ");
			add(label, gbc);
			gbc.gridx++;

			gbc.anchor = GridBagConstraints.EAST;
			commandName = new JTextField(externalCommandButton.commandName, 30);
			add(commandName, gbc);
			gbc.gridx--;
			gbc.gridy++;

			gbc.anchor = GridBagConstraints.WEST;
			label = new JLabel("Command String: ");
			add(label, gbc);
			gbc.gridx++;

			gbc.anchor = GridBagConstraints.EAST;
			commandString = new JTextField(
					externalCommandButton.externalCommand, 30);
			add(commandString, gbc);
			gbc.gridx--;
			gbc.gridy++;

			okButton = new JButton("OK");
			okButton.addActionListener(this);
			gbc.gridwidth = 2;
			gbc.fill = GridBagConstraints.NONE;
			gbc.anchor = GridBagConstraints.CENTER;
			add(okButton, gbc);
		}

		public void actionPerformed(ActionEvent ae) {
			externalCommandButton.setCommandName(commandName.getText());
			externalCommandButton.setExternalCommand(commandString.getText());
			this.dispose();
		}
	}

	public class WindowMenu extends JMenu {

		private MIOSStudioGUI miosStudio;

		private JDesktopPane desktop;

		private JMenuItem cascadeMenuItem;

		public WindowMenu(MIOSStudioGUI miosStudio) {
			this.miosStudio = miosStudio;
			this.desktop = miosStudio.desktop;
			setText("Window");
			addMenuListener(MIOSStudioGUI.this);
		}

		private JInternalFrame[] getFrames() {
			return desktop.getAllFrames();
		}

		private void buildChildMenus() {
			int i;
			ChildMenuItem menuItem;
			JInternalFrame[] frames = getFrames();

			cascadeMenuItem = new JMenuItem("Cascade Windows");

			cascadeMenuItem.addActionListener(new ActionListener() {

				public void actionPerformed(ActionEvent ae) {

					JInternalFrame[] frames = getFrames();

					for (int x = 0; x < frames.length; x++) {
						frames[frames.length - 1 - x].setLocation(x * 20,
								x * 20);
					}
				}
			});

			cascadeMenuItem.setEnabled(frames.length > 0);

			add(cascadeMenuItem);

			addSeparator();

			for (i = 0; i < frames.length; i++) {
				menuItem = new ChildMenuItem(frames[i]);
				menuItem.setState(i == 0);
				menuItem.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent ae) {
						JInternalFrame frame = ((ChildMenuItem) ae.getSource())
								.getFrame();
						miosStudio.showFrame(frame);
					}
				});
				menuItem.setIcon(frames[i].getFrameIcon());
				add(menuItem);
			}
		}

		public class ChildMenuItem extends JCheckBoxMenuItem {
			private JInternalFrame frame;

			public ChildMenuItem(JInternalFrame frame) {
				super(frame.getTitle());
				this.frame = frame;
			}

			public JInternalFrame getFrame() {
				return frame;
			}
		}
	}

	class WorkspaceOptionDialog extends JDialog {

		File file;

		JCheckBox modelCheckBox;

		JCheckBox guiCheckBox;

		JCheckBox externalCommandsCheckBox;

		JCheckBox lookAndFeelCheckBox;

		JButton okButton;

		public WorkspaceOptionDialog() {

			super(DialogOwner.getFrame(), "Workspace Options", true);

			JPanel panel = new JPanel(new GridBagLayout());

			GridBagConstraints gbc = new GridBagConstraints();

			gbc.gridx = 0;
			gbc.gridy = 0;
			gbc.gridwidth = 1;
			gbc.gridheight = 1;
			gbc.anchor = (GridBagConstraints.WEST);
			gbc.insets = new Insets(5, 5, 5, 5);

			JLabel label = new JLabel("Load/save workspace: ");
			panel.add(label, gbc);
			gbc.gridy++;

			modelCheckBox = new JCheckBox("MIOS Studio setup", true);
			panel.add(modelCheckBox, gbc);
			gbc.gridy++;

			guiCheckBox = new JCheckBox("Graphical layout", true);
			panel.add(guiCheckBox, gbc);
			gbc.gridy++;

			externalCommandsCheckBox = new JCheckBox(
					"External Command buttons", true);
			panel.add(externalCommandsCheckBox, gbc);
			gbc.gridy++;

			lookAndFeelCheckBox = new JCheckBox("Look & Feel", true);
			panel.add(lookAndFeelCheckBox, gbc);
			gbc.gridy++;

			gbc.anchor = (GridBagConstraints.CENTER);

			okButton = new JButton("OK");
			panel.add(okButton, gbc);

			setContentPane(panel);
		}
	}
}
