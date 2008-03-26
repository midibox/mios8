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

package org.midibox.apps.SIDV2librarian;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.util.Observable;
import java.util.Observer;
import javax.swing.ImageIcon;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.KeyStroke;
import org.midibox.midi.gui.MidiDeviceRoutingGUI;
import org.midibox.sidedit.SIDEditController;
import org.midibox.sidlibr.SIDLibController;
import org.midibox.sidlibr.gui.LibraryGUI;
import org.midibox.utils.gui.ImageLoader;
import javax.swing.JDialog;
import org.midibox.sidedit.gui.MBSIDV2EditorGUI;
import java.awt.event.*;
public class SIDV2librarianGUI extends JFrame implements Observer, ActionListener, ItemListener{

	JCheckBoxMenuItem cbMenuItem1, cbMenuItem2, cbMenuItem3, cbMenuItem4;

	private SIDV2librarian sidv2librarian;
	private SIDLibController sidLibController;
	
	private LibraryGUI libraryGUI;
	private JDialog midiRoutingDialog; 
	private MBSIDV2EditorGUI mbsidV2EditorGUI;
		
	private SIDEditController sidEditController;
		
	private MidiDeviceRoutingGUI midiDeviceRoutingGUI;

	public SIDV2librarianGUI(SIDV2librarian sidv2librarian) {
		super("MidiBox SID V2 Editor");		
		this.sidv2librarian = sidv2librarian;
		this.sidLibController = sidv2librarian.getSIDLibController();
		sidLibController.addObserver(this);
		
		// Setup frame
		setLayout(new BorderLayout());
		setIconImage(ImageLoader.getImageIcon("sid.png").getImage());
		setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent ev) {
                tryExit();
            }
        });
		setResizable(false);
		setJMenuBar(createMenu());			
				
		// Setup Librarian GUI (table view)
		libraryGUI = new LibraryGUI(sidLibController);
		
		// Setup midiDevice routing GUI
		midiDeviceRoutingGUI = sidv2librarian.getMidiDeviceRoutingGUI();		
		midiDeviceRoutingGUI.addMidiDeviceIcon(sidv2librarian.getSysExControllerDevice(), new ImageIcon(ImageLoader.getImageIcon("sid.png").getImage()));
		
		this.add(libraryGUI, BorderLayout.NORTH);
		
		// Setup Midi Routing Dialog
		midiRoutingDialog = new JDialog(this,"Midi Routing", false);
		midiRoutingDialog.setContentPane(midiDeviceRoutingGUI);
		midiRoutingDialog.pack();
		
		mbsidV2EditorGUI = new MBSIDV2EditorGUI(this,true);
		
		pack();
		setVisible(true);
	}
	
	private void showEditGUI() {	
		sidv2librarian.getMidiDeviceRouting().reconnectAllDevices();	// java.sound.midi SysEx bug workaround
		sidEditController = new SIDEditController(sidLibController.getCurrentPatch());
		sidEditController.setCores(sidLibController.getCores());		
		sidEditController.addObserver(this);
		mbsidV2EditorGUI.editThis(sidEditController);		
	}
	
	public void actionPerformed(ActionEvent ae) {
		if (ae.getActionCommand().equals("Preferences")) {
			midiRoutingDialog.setVisible(true);			
		} else if (ae.getActionCommand().equals("Exit")) {
			tryExit();
		}
		sidLibController.actionPerformed(ae);		
	}
	
	private void tryExit() {
		int n = JOptionPane.showConfirmDialog(null,"Are you sure you want to exit?","Exit?",JOptionPane.YES_NO_OPTION);
		if (n==JOptionPane.YES_OPTION) {
			sidv2librarian.storeConnections();
			System.exit(0);
		}
	}
	
	public void update(Observable observable, Object object) {
		if (object=="Edit") {
			showEditGUI();			
		} else if (object=="Save editor patch") {
			sidLibController.setPatchAt(sidEditController.getPatch(), sidLibController.getCurrentPatchNumber(), sidLibController.getCurrentBankNumber());
		}
	}
	
	public void itemStateChanged(ItemEvent e) {
		if (e.getItem()==cbMenuItem1) {
			if (cbMenuItem1.isSelected()) {
				sidLibController.setCores(0, true);
			} else {
				sidLibController.setCores(0, false);
			}
		} else if (e.getItem()==cbMenuItem2) {
			if (cbMenuItem2.isSelected()) {
				sidLibController.setCores(1, true);
			} else {
				sidLibController.setCores(1, false);
			}
		} else if (e.getItem()==cbMenuItem3) {
			if (cbMenuItem3.isSelected()) {
				sidLibController.setCores(2, true);
			} else {
				sidLibController.setCores(2, false);
			}
		} else if (e.getItem()==cbMenuItem4) {
			if (cbMenuItem4.isSelected()) {
				sidLibController.setCores(3, true);
			} else {
				sidLibController.setCores(3, false);
			}
		}
    } 
	
	private JMenuBar createMenu() {
		JMenuBar menuBar = new JMenuBar();
		JMenu menu, submenu;
		JMenuItem menuItem;		
				
		//File menu
		menu = new JMenu("File");
		menu.setMnemonic(KeyEvent.VK_F);
		menuBar.add(menu);

		//File menu items:		
		menuItem = new JMenuItem("Load patch",KeyEvent.VK_L);
		menuItem.setActionCommand("Load patch");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("Save patch",KeyEvent.VK_S);
		menuItem.setActionCommand("Save patch");
		menuItem.addActionListener(this);
		menu.add(menuItem);
				
		menu.addSeparator();
		
		menuItem = new JMenuItem("Load bank",KeyEvent.VK_O);
		menuItem.setActionCommand("Load bank");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("Save bank",KeyEvent.VK_A);
		menuItem.setActionCommand("Save bank");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menu.addSeparator();
				
		menuItem = new JMenuItem("Exit",KeyEvent.VK_X);
		menuItem.setActionCommand("Exit");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		//Build second menu in the menu bar.
		menu = new JMenu("Edit");
		menu.setMnemonic(KeyEvent.VK_E);
		menuBar.add(menu);
		
		//Edit menu items:
		menuItem = new JMenuItem("Edit",KeyEvent.VK_E);
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_E, ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Edit");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("Rename",KeyEvent.VK_R);
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R, ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Rename");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menu.addSeparator();	
		
		menuItem = new JMenuItem("Cut",KeyEvent.VK_U);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Cut");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("Copy",KeyEvent.VK_C);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Copy");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("Paste",KeyEvent.VK_P);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V, ActionEvent.CTRL_MASK));
		menuItem.setActionCommand("Paste");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("Clear",KeyEvent.VK_L);
		menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE,0));
		menuItem.setActionCommand("Clear");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menu.addSeparator();	
		
		submenu = new JMenu("Init current patch");
		submenu.setMnemonic(KeyEvent.VK_I);
		menu.add(submenu);
		
		menuItem = new JMenuItem("LEAD engine",KeyEvent.VK_P);
		menuItem.setActionCommand("Init LEAD patch");
		menuItem.addActionListener(this);
		submenu.add(menuItem);
		
		menuItem = new JMenuItem("BASSLINE engine",KeyEvent.VK_P);
		menuItem.setActionCommand("Init BASSLINE patch");
		menuItem.addActionListener(this);
		submenu.add(menuItem);
		
		menuItem = new JMenuItem("DRUM engine",KeyEvent.VK_P);
		menuItem.setActionCommand("Init DRUM patch");
		menuItem.addActionListener(this);
		submenu.add(menuItem);
		
		menuItem = new JMenuItem("MULTI engine",KeyEvent.VK_P);
		menuItem.setActionCommand("Init MULTI patch");
		menuItem.addActionListener(this);
		submenu.add(menuItem);
		
		menuItem = new JMenuItem("Init current bank",KeyEvent.VK_N);
		menuItem.setActionCommand("Init current bank");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menu.addSeparator();	
		
		menuItem = new JMenuItem("Preferences",KeyEvent.VK_R);
		menuItem.setActionCommand("Preferences");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		//Build third menu in the menu bar.
		menu = new JMenu("Transmit");
		menu.setMnemonic(KeyEvent.VK_T);
		menuBar.add(menu);
		
		//Transmit menu items:
		menuItem = new JMenuItem("Patch to buffer",KeyEvent.VK_B);
		menuItem.setActionCommand("Transmit patch to buffer");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("Patch to memory",KeyEvent.VK_M);
		menuItem.setActionCommand("Transmit patch to memory");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menu.addSeparator();	
				
		menuItem = new JMenuItem("Bank",KeyEvent.VK_A);
		menuItem.setActionCommand("Transmit bank to memory");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		//Build fourth menu in the menu bar.
		menu = new JMenu("Receive");
		menu.setMnemonic(KeyEvent.VK_R);
		menuBar.add(menu);
		
		menuItem = new JMenuItem("Patch from buffer",KeyEvent.VK_B);
		menuItem.setActionCommand("Receive patch from buffer");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("Patch from memory",KeyEvent.VK_M);
		menuItem.setActionCommand("Receive patch from memory");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menu.addSeparator();	
				
		menuItem = new JMenuItem("Bank",KeyEvent.VK_A);
		menuItem.setActionCommand("Receive bank from memory");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		//Build fifth menu in the menu bar.
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
		
		return menuBar;
	}

	public static void main(String[] args) {
		final SIDV2librarian sidv2librarian = new SIDV2librarian();
		SIDV2librarianGUI sidv2librarianGUI = new SIDV2librarianGUI(sidv2librarian);		
	}
}
