/*
 * @(#)SIDV2librarian.java	beta1	2008/01/21
 *
 * Copyright (C) 2008    Rutger Vlek (rutgervlek@hotmail.com)
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

package org.midibox.apps.sidv2editor.gui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Observable;
import java.util.Observer;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JDialog;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JToolBar;
import javax.swing.KeyStroke;

import org.midibox.apps.sidv2editor.SIDV2Editor;
import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.gui.MBSIDV2EditorGUI;
import org.midibox.sidlibr.Patch;
import org.midibox.sidlibr.SIDLibController;
import org.midibox.sidlibr.gui.LibraryGUI;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SplitButton;

public class SIDV2EditorGUI extends JPanel implements Observer,
		ActionListener, ItemListener {

	JCheckBoxMenuItem cbMenuItem1, cbMenuItem2, cbMenuItem3, cbMenuItem4;

	private SIDV2Editor sidv2librarian;
	private SIDLibController sidLibController;

	private LibraryGUI libraryGUI;
	private JDialog midiRoutingDialog;
	private MBSIDV2EditorGUI mbsidV2EditorGUI;
	private JDialog mbsidV2EditorGUIDialog;

	// private SIDEditController sidEditController;

	private SIDV2EditorMidiDeviceRoutingGUI midiDeviceRoutingGUI;

	private JMenu fileMenu;

	private JMenu editMenu;

	public SIDV2EditorGUI(SIDV2Editor sidv2librarian) {

		this.sidv2librarian = sidv2librarian;

		this.sidLibController = sidv2librarian.getSIDLibController();

		sidLibController.addObserver(this);

		// Setup frame
		setLayout(new BorderLayout());

		add(createToolbar(), BorderLayout.PAGE_START);

		// Setup Librarian GUI (table view)
		libraryGUI = new LibraryGUI(sidLibController);

		this.add(libraryGUI, BorderLayout.CENTER);

		mbsidV2EditorGUI = new MBSIDV2EditorGUI();

		mbsidV2EditorGUIDialog = new JDialog(DialogOwner.getFrame(),
				"MidiBox SID V2 Editor - no patch selected", true);

		mbsidV2EditorGUIDialog
				.setDefaultCloseOperation(JDialog.DO_NOTHING_ON_CLOSE);
		mbsidV2EditorGUIDialog.setResizable(false);

		mbsidV2EditorGUIDialog.addWindowListener(new WindowAdapter() {

			public void windowClosing(WindowEvent we) {
				int result = JOptionPane
						.showConfirmDialog(
								null,
								"The current patch has been changed. Would you like to save?",
								"Save patch?",
								JOptionPane.YES_NO_CANCEL_OPTION,
								JOptionPane.QUESTION_MESSAGE);
				switch (result) {
				case JOptionPane.YES_OPTION:
					mbsidV2EditorGUI.getSIDEditController().Save();
					mbsidV2EditorGUIDialog.setVisible(false);
					break;
				case JOptionPane.NO_OPTION:
					mbsidV2EditorGUIDialog.setVisible(false);
					break;
				case JOptionPane.CANCEL_OPTION:
					break;
				default:
					break;
				}
			}
		});

		mbsidV2EditorGUIDialog.setContentPane(mbsidV2EditorGUI);

		if (sidv2librarian.getMidiDeviceRouting() != null) {

			// Setup midiDevice routing GUI
			midiDeviceRoutingGUI = new SIDV2EditorMidiDeviceRoutingGUI(
					sidv2librarian);
			// midiDeviceRoutingGUI.addMidiDeviceIcon(sidv2librarian.getSysExControllerDevice(),
			// new ImageIcon(ImageLoader.getImageIcon("sid.png").getImage()));

			// Setup Midi Routing Dialog

			midiRoutingDialog = new JDialog(DialogOwner.getFrame(),
					"Midi Routing", true);
			midiRoutingDialog.setContentPane(midiDeviceRoutingGUI);
			midiRoutingDialog.pack();
		}
	}

	public SIDV2Editor getSidv2librarian() {
		return sidv2librarian;
	}

	private void showEditGUI() {

		SIDEditController sidEditController = mbsidV2EditorGUI
				.getSIDEditController();

		if (sidEditController != null) {

			sidEditController.deleteObserver(this);
		}

		sidv2librarian.reconnectAllDevices(); // java.sound.midi SysEx bug
		// workaround

		sidEditController = new SIDEditController(sidLibController.getCurrentPatch());	
		sidEditController.addObserver(this);
		mbsidV2EditorGUI.editThis(sidEditController, sidLibController.getCores());
		Patch p = mbsidV2EditorGUI.getSIDEditController().getPatch();
		if (p.isEnsemble()) {
			mbsidV2EditorGUIDialog.setTitle("MidiBox SID V2 Editor - " + "Ensemble " + intToStr3(sidLibController.getCurrentPatchNumber()+1));
		} else {
			mbsidV2EditorGUIDialog.setTitle("MidiBox SID V2 Editor - " + p.getEngineStr() + " engine: " + p.getPatchName());
		}
		mbsidV2EditorGUIDialog.pack();
		mbsidV2EditorGUIDialog.setLocationRelativeTo(this);
		mbsidV2EditorGUIDialog.setVisible(true);
	}
	
	private String intToStr3(int i) {
		String s = Integer.toString(i);
		switch (s.length()) {
		case 1:
			s = "00" + s;
			break;
		case 2:
			s = "0" + s;
			break;
		}
		return s;
	}

	public void actionPerformed(ActionEvent ae) {

		sidLibController.actionPerformed(ae);
	}

	public void update(Observable observable, Object object) {
		if (object == "Edit") {
			showEditGUI();
		} else if (object == "Save editor patch") {
			sidLibController.setPatchAt(mbsidV2EditorGUI.getSIDEditController().getPatch(), sidLibController.getCurrentPatchNumber(),sidLibController.getCurrentBankNumber());
		}
	}

	public void itemStateChanged(ItemEvent e) {
		if ((e.getItem() == cbMenuItem1) || (e.getItem() == cbMenuItem2)
				|| (e.getItem() == cbMenuItem3) || (e.getItem() == cbMenuItem4)) {
			int tempVal = 0;
			if (cbMenuItem1.isSelected()) {
				tempVal += 1;
			}
			;
			if (cbMenuItem2.isSelected()) {
				tempVal += 2;
			}
			;
			if (cbMenuItem3.isSelected()) {
				tempVal += 4;
			}
			;
			if (cbMenuItem4.isSelected()) {
				tempVal += 8;
			}
			;
			sidLibController.setCores(tempVal);
		}
	}

	public JMenuBar createMenu() {
		JMenuBar menuBar = new JMenuBar();
		JMenu menu, submenu;
		JMenuItem menuItem;

		// File menu
		fileMenu = new JMenu("File");
		fileMenu.setMnemonic(KeyEvent.VK_F);
		menuBar.add(fileMenu);

		// File menu items:
		menuItem = new JMenuItem("Load patch", KeyEvent.VK_L);
		menuItem.setActionCommand("Load patch");
		menuItem.addActionListener(this);
		fileMenu.add(menuItem);

		menuItem = new JMenuItem("Save patch", KeyEvent.VK_S);
		menuItem.setActionCommand("Save patch");
		menuItem.addActionListener(this);
		fileMenu.add(menuItem);

		fileMenu.addSeparator();

		menuItem = new JMenuItem("Load bank", KeyEvent.VK_O);
		menuItem.setActionCommand("Load bank");
		menuItem.addActionListener(this);
		fileMenu.add(menuItem);

		menuItem = new JMenuItem("Save bank", KeyEvent.VK_A);
		menuItem.setActionCommand("Save bank");
		menuItem.addActionListener(this);
		fileMenu.add(menuItem);

		// Build second menu in the menu bar.
		editMenu = new JMenu("Edit");
		editMenu.setMnemonic(KeyEvent.VK_E);
		menuBar.add(editMenu);

		// Edit menu items:
		menuItem = new JMenuItem("Edit", KeyEvent.VK_E);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_E,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Edit");
		menuItem.addActionListener(this);
		editMenu.add(menuItem);

		menuItem = new JMenuItem("Rename", KeyEvent.VK_R);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Rename");
		menuItem.addActionListener(this);
		editMenu.add(menuItem);

		editMenu.addSeparator();

		menuItem = new JMenuItem("Cut", KeyEvent.VK_U);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Cut");
		menuItem.addActionListener(this);
		editMenu.add(menuItem);

		menuItem = new JMenuItem("Copy", KeyEvent.VK_C);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Copy");
		menuItem.addActionListener(this);
		editMenu.add(menuItem);

		menuItem = new JMenuItem("Paste", KeyEvent.VK_P);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V,
				ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Paste");
		menuItem.addActionListener(this);
		editMenu.add(menuItem);

		menuItem = new JMenuItem("Clear", KeyEvent.VK_L);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0));
		menuItem.setActionCommand("Clear");
		menuItem.addActionListener(this);
		editMenu.add(menuItem);

		editMenu.addSeparator();

		submenu = new JMenu("Init current patch");
		submenu.setMnemonic(KeyEvent.VK_I);
		editMenu.add(submenu);

		menuItem = new JMenuItem("LEAD engine", KeyEvent.VK_P);
		menuItem.setActionCommand("Init LEAD patch");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		menuItem = new JMenuItem("BASSLINE engine", KeyEvent.VK_P);
		menuItem.setActionCommand("Init BASSLINE patch");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		menuItem = new JMenuItem("DRUM engine", KeyEvent.VK_P);
		menuItem.setActionCommand("Init DRUM patch");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		menuItem = new JMenuItem("MULTI engine", KeyEvent.VK_P);
		menuItem.setActionCommand("Init MULTI patch");
		menuItem.addActionListener(this);
		submenu.add(menuItem);
		
		menuItem = new JMenuItem("ENSEMBLE", KeyEvent.VK_P);
		menuItem.setActionCommand("Init ensemble");
		menuItem.addActionListener(this);
		submenu.add(menuItem);

		menuItem = new JMenuItem("Init current bank", KeyEvent.VK_N);
		menuItem.setActionCommand("Init current bank");
		menuItem.addActionListener(this);
		editMenu.add(menuItem);

		// Build third menu in the menu bar.
		menu = new JMenu("Transmit");
		menu.setMnemonic(KeyEvent.VK_T);
		menuBar.add(menu);

		// Transmit menu items:
		menuItem = new JMenuItem("Patch to buffer", KeyEvent.VK_B);
		menuItem.setActionCommand("Transmit patch to buffer");
		menuItem.addActionListener(this);
		menu.add(menuItem);

		menuItem = new JMenuItem("Patch to memory", KeyEvent.VK_M);
		menuItem.setActionCommand("Transmit patch to memory");
		menuItem.addActionListener(this);
		menu.add(menuItem);

		menu.addSeparator();

		menuItem = new JMenuItem("Bank", KeyEvent.VK_A);
		menuItem.setActionCommand("Transmit bank to memory");
		menuItem.addActionListener(this);
		menu.add(menuItem);

		// Build fourth menu in the menu bar.
		menu = new JMenu("Receive");
		menu.setMnemonic(KeyEvent.VK_R);
		menuBar.add(menu);

		menuItem = new JMenuItem("Patch from buffer", KeyEvent.VK_B);
		menuItem.setActionCommand("Receive patch from buffer");
		menuItem.addActionListener(this);
		menu.add(menuItem);

		menuItem = new JMenuItem("Patch from memory", KeyEvent.VK_M);
		menuItem.setActionCommand("Receive patch from memory");
		menuItem.addActionListener(this);
		menu.add(menuItem);

		menu.addSeparator();

		menuItem = new JMenuItem("Bank", KeyEvent.VK_A);
		menuItem.setActionCommand("Receive bank from memory");
		menuItem.addActionListener(this);
		menu.add(menuItem);

		// Build fifth menu in the menu bar.
		menu = new JMenu("Cores");
		menu.setMnemonic(KeyEvent.VK_C);
		menuBar.add(menu);

		cbMenuItem1 = new JCheckBoxMenuItem("Edit on core 1");
		cbMenuItem1.setSelected(true);
		cbMenuItem1.setMnemonic(KeyEvent.VK_1);
		cbMenuItem1.addItemListener(this);
		menu.add(cbMenuItem1);

		cbMenuItem2 = new JCheckBoxMenuItem("Edit on core 2");
		cbMenuItem2.setMnemonic(KeyEvent.VK_2);
		cbMenuItem2.addItemListener(this);
		menu.add(cbMenuItem2);

		cbMenuItem3 = new JCheckBoxMenuItem("Edit on core 3");
		cbMenuItem3.setMnemonic(KeyEvent.VK_3);
		cbMenuItem3.addItemListener(this);
		menu.add(cbMenuItem3);

		cbMenuItem4 = new JCheckBoxMenuItem("Edit on core 4");
		cbMenuItem4.setMnemonic(KeyEvent.VK_4);
		cbMenuItem4.addItemListener(this);
		menu.add(cbMenuItem4);

		menu.addSeparator();

		menuItem = new JMenuItem("Scan hardware", KeyEvent.VK_S);
		menuItem.setActionCommand("Scan hardware");
		menuItem.addActionListener(this);
		menu.add(menuItem);

		return menuBar;
	}

	private JToolBar createToolbar() {
		JToolBar toolBar = new JToolBar();
		toolBar.setFloatable(false);
		toolBar.add(new SplitButton(makeButton("init_lead.png",
				"Init LEAD patch", "Init LEAD patch"), dropInit()));
		toolBar.add(new SplitButton(makeButton("open.png", "Load bank",
				"Load bank"), dropLoad()));
		toolBar.add(new SplitButton(makeButton("save.png", "Save patch",
				"Save patch"), dropSave()));
		toolBar.addSeparator();
		toolBar.add(makeButton("cut.png", "Cut", "Cut"));
		toolBar.add(makeButton("copy.png", "Copy", "Copy"));
		toolBar.add(makeButton("paste.png", "Paste", "Paste"));
		toolBar.addSeparator();
		toolBar.add(new SplitButton(makeButton("transmit.png",
				"Transmit patch to buffer", "Transmit patch to buffer"),
				dropTransmit()));
		toolBar.add(new SplitButton(makeButton("receive.png",
				"Receive patch from buffer", "Receive patch from buffer"),
				dropReceive()));
		toolBar.addSeparator();

		// toolBar.add(makeButton("init_lead.png","Init LEAD patch","Init LEAD patch"));
		// toolBar.add(makeButton("init_bassline.png","Init BASSLINE patch","Init BASSLINE patch"));
		// toolBar.add(makeButton("init_drum.png","Init DRUM patch","Init DRUM patch"));
		// toolBar.add(makeButton("init_multi.png","Init MULTI patch","Init MULTI patch"));
		return toolBar;
	}

	public JMenu getFileMenu() {
		return fileMenu;
	}

	public JMenu getEditMenu() {
		return editMenu;
	}

	public void showMidiDeviceRoutingDialog() {
		midiRoutingDialog.setVisible(true);
	}

	protected JButton makeButton(String imageName, String actionCommand,
			String toolTipText) {
		// Create and initialize the button.
		ImageIcon icon = ImageLoader.getImageIcon(imageName);
		int s = Math.max(icon.getIconWidth(), icon.getIconHeight()) + 4;
		JButton button = new JButton();
		button.setPreferredSize(new Dimension(s, s));
		button.setActionCommand(actionCommand);
		button.setToolTipText(toolTipText);
		button.addActionListener(this);
		button.setIcon(icon);
		button.setFocusPainted(false);
		return button;
	}

	protected JPopupMenu dropInit() {
		JPopupMenu ToolbarMenu = new JPopupMenu();

		JMenuItem item1 = new JMenuItem();
		item1.setText("Init LEAD");
		item1.setIcon(ImageLoader.getImageIcon("init_lead.png"));
		item1.setActionCommand("Init LEAD patch");
		item1.addActionListener(this);
		ToolbarMenu.add(item1);

		JMenuItem item2 = new JMenuItem();
		item2.setText("Init BASSLINE");
		item2.setFont(item2.getFont().deriveFont(Font.PLAIN));
		item2.setIcon(ImageLoader.getImageIcon("init_bassline.png"));
		item2.setActionCommand("Init BASSLINE patch");
		item2.addActionListener(this);
		ToolbarMenu.add(item2);

		JMenuItem item3 = new JMenuItem();
		item3.setText("Init DRUM");
		item3.setFont(item3.getFont().deriveFont(Font.PLAIN));
		item3.setIcon(ImageLoader.getImageIcon("init_drum.png"));
		item3.setActionCommand("Init DRUM patch");
		item3.addActionListener(this);
		ToolbarMenu.add(item3);

		JMenuItem item4 = new JMenuItem();
		item4.setText("Init MULTI");
		item4.setFont(item4.getFont().deriveFont(Font.PLAIN));
		item4.setIcon(ImageLoader.getImageIcon("init_multi.png"));
		item4.setActionCommand("Init MULTI patch");
		item4.addActionListener(this);
		ToolbarMenu.add(item4);
				
		return ToolbarMenu;
	}

	protected JPopupMenu dropTransmit() {
		JPopupMenu toolbarMenu = new JPopupMenu();

		JMenuItem item1 = new JMenuItem();
		item1.setText("Patch to buffer");
		item1.setIcon(ImageLoader.getImageIcon("transmit.png"));
		item1.setActionCommand("Transmit patch to buffer");
		item1.addActionListener(this);
		toolbarMenu.add(item1);

		JMenuItem item2 = new JMenuItem();
		item2.setText("Patch to memory");
		item2.setFont(item2.getFont().deriveFont(Font.PLAIN));
		item2.setIcon(ImageLoader.getImageIcon("transmit.png"));
		item2.setActionCommand("Transmit patch to memory");
		item2.addActionListener(this);
		toolbarMenu.add(item2);

		toolbarMenu.addSeparator();

		JMenuItem item3 = new JMenuItem();
		item3.setText("Bank");
		item3.setFont(item3.getFont().deriveFont(Font.PLAIN));
		item3.setIcon(ImageLoader.getImageIcon("transmit.png"));
		item3.setActionCommand("Transmit bank to memory");
		item3.addActionListener(this);
		toolbarMenu.add(item3);

		return toolbarMenu;
	}

	protected JPopupMenu dropReceive() {
		JPopupMenu toolbarMenu = new JPopupMenu();

		JMenuItem item1 = new JMenuItem();
		item1.setText("Patch from buffer");
		item1.setIcon(ImageLoader.getImageIcon("receive.png"));
		item1.setActionCommand("Receive patch from buffer");
		item1.addActionListener(this);
		toolbarMenu.add(item1);

		JMenuItem item2 = new JMenuItem();
		item2.setText("Patch from memory");
		item2.setFont(item2.getFont().deriveFont(Font.PLAIN));
		item2.setIcon(ImageLoader.getImageIcon("receive.png"));
		item2.setActionCommand("Receive patch from memory");
		item2.addActionListener(this);
		toolbarMenu.add(item2);

		toolbarMenu.addSeparator();

		JMenuItem item3 = new JMenuItem();
		item3.setText("Bank");
		item3.setFont(item3.getFont().deriveFont(Font.PLAIN));
		item3.setIcon(ImageLoader.getImageIcon("receive.png"));
		item3.setActionCommand("Receive bank from memory");
		item3.addActionListener(this);
		toolbarMenu.add(item3);

		return toolbarMenu;
	}

	protected JPopupMenu dropLoad() {
		JPopupMenu toolbarMenu = new JPopupMenu();

		JMenuItem item1 = new JMenuItem();
		item1.setText("Load bank");
		item1.setIcon(ImageLoader.getImageIcon("open.png"));
		item1.setActionCommand("Load bank");
		item1.addActionListener(this);
		toolbarMenu.add(item1);

		JMenuItem item2 = new JMenuItem();
		item2.setText("Load patch");
		item2.setFont(item2.getFont().deriveFont(Font.PLAIN));
		item2.setIcon(ImageLoader.getImageIcon("open.png"));
		item2.setActionCommand("Load patch");
		item2.addActionListener(this);
		toolbarMenu.add(item2);

		return toolbarMenu;
	}

	protected JPopupMenu dropSave() {
		JPopupMenu toolbarMenu = new JPopupMenu();

		JMenuItem item1 = new JMenuItem();
		item1.setText("Save bank");
		item1.setIcon(ImageLoader.getImageIcon("save.png"));
		item1.setActionCommand("Save bank");
		item1.addActionListener(this);
		toolbarMenu.add(item1);

		JMenuItem item2 = new JMenuItem();
		item2.setText("Save patch");
		item2.setFont(item2.getFont().deriveFont(Font.PLAIN));
		item2.setIcon(ImageLoader.getImageIcon("save.png"));
		item2.setActionCommand("Save patch");
		item2.addActionListener(this);
		toolbarMenu.add(item2);

		return toolbarMenu;
	}
}
