/*
 * @(#)SysexSendReceiveDeviceManagerGUI.java	beta8	2006/04/23
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

package org.midibox.midi.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JTabbedPane;

import org.midibox.midi.SysexSendReceiveDevice;
import org.midibox.midi.SysexSendReceiveDeviceManager;
import org.midibox.utils.gui.ImageLoader;

public class SysexSendReceiveDeviceManagerGUI extends JPanel implements
		ActionListener, Observer {

	private SysexSendReceiveDeviceManager sysexSendReceiveDeviceManager;

	private JTabbedPane tabbedPane;

	private Vector sysexSendReceiveGUIs;

	public SysexSendReceiveDeviceManagerGUI(
			SysexSendReceiveDeviceManager uploadTaskManager) {
		super(new BorderLayout());

		this.sysexSendReceiveDeviceManager = uploadTaskManager;
		uploadTaskManager.addObserver(this);

		sysexSendReceiveGUIs = new Vector();

		add(createButtonPanel(), BorderLayout.NORTH);
		add(createTabPanel(), BorderLayout.CENTER);

		createGUIs();
	}

	private JPanel createButtonPanel() {
		JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 4, 4));

		JLabel label = new JLabel("Sysex Send/Receive:", JLabel.LEFT);
		label.setOpaque(true);
		buttonPanel.add(label);
		buttonPanel.add(Box.createHorizontalStrut(10));

		JButton button = new JButton("New Tab");
		button.setActionCommand("newSendReceive");
		button.setToolTipText("Open new upload tab");
		button.addActionListener(this);
		buttonPanel.add(button);

		button = new JButton("Close Tab");
		button.setActionCommand("closeSendReceive");
		button.setToolTipText("Close current upload tab");
		button.addActionListener(this);
		buttonPanel.add(button);

		return buttonPanel;
	}

	private JPanel createTabPanel() {
		JPanel tabPanel = new JPanel(new BorderLayout());
		tabbedPane = new JTabbedPane();
		tabPanel.add(tabbedPane, BorderLayout.CENTER);
		tabPanel.add(new JSeparator(JSeparator.HORIZONTAL), BorderLayout.NORTH);

		return tabPanel;
	}

	private void createGUIs() {

		Iterator it = sysexSendReceiveDeviceManager
				.getSysexSendReceiveDevices().iterator();

		while (it.hasNext()) {

			SysexSendReceiveDevice ssrd = (SysexSendReceiveDevice) it.next();

			boolean hasGUI = false;

			Iterator it2 = sysexSendReceiveGUIs.iterator();

			while (it2.hasNext()) {
				SysexSendReceiveGUI ssrg = (SysexSendReceiveGUI) it2.next();
				if (ssrg.getSysexSendReceive() == ssrd.getSysexSendReceive()) {
					hasGUI = true;
				}
			}

			if (!hasGUI) {
				SysexSendReceiveGUI ssrg = new SysexSendReceiveGUI(ssrd
						.getSysexSendReceive());
				sysexSendReceiveGUIs.add(ssrg);
				tabbedPane.addTab(ssrd.getDeviceInfo().getName(), ImageLoader
						.getImageIcon("sysex.png"), ssrg);
			}
		}

		it = ((Vector) sysexSendReceiveGUIs.clone()).iterator();

		while (it.hasNext()) {

			SysexSendReceiveGUI ssrg = (SysexSendReceiveGUI) it.next();

			boolean hasDevice = false;

			Iterator it2 = sysexSendReceiveDeviceManager
					.getSysexSendReceiveDevices().iterator();

			while (it2.hasNext()) {
				SysexSendReceiveDevice ssrd = (SysexSendReceiveDevice) it2
						.next();
				if (ssrg.getSysexSendReceive() == ssrd.getSysexSendReceive()) {
					hasDevice = true;
				}
			}

			if (!hasDevice) {
				sysexSendReceiveGUIs.remove(ssrg);
				tabbedPane.remove(ssrg);
			}
		}
	}

	public void addNewSendReceiveTab() {
		sysexSendReceiveDeviceManager.newSysexSendReceive();
	}

	public void closeSendReceiveTab() {
		int selectedTab = tabbedPane.getSelectedIndex();

		if (selectedTab > -1) {
			sysexSendReceiveDeviceManager
					.removeSysexSendReceiveDevice((SysexSendReceiveDevice) sysexSendReceiveDeviceManager
							.getSysexSendReceiveDevices()
							.elementAt(selectedTab));
		}
	}
	
	

	public JTabbedPane getTabbedPane() {
		return tabbedPane;
	}

	public void update(Observable observable, Object object) {
		if (observable == sysexSendReceiveDeviceManager) {
			createGUIs();
		}
	}

	public void actionPerformed(ActionEvent ae) {

		String command = ae.getActionCommand();

		if (command.equals("newSendReceive")) {
			addNewSendReceiveTab();
		}

		if (command.equals("closeSendReceive")) {
			closeSendReceiveTab();
		}
	}
}
