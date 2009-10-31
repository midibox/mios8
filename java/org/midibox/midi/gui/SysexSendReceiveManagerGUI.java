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

import org.midibox.midi.SysexSendReceive;
import org.midibox.midi.SysexSendReceiveManager;
import org.midibox.utils.gui.ImageLoader;

public class SysexSendReceiveManagerGUI extends JPanel implements
		ActionListener, Observer {

	private SysexSendReceiveManager sysexSendReceiveManager;

	private JTabbedPane tabbedPane;

	private Vector sysexSendReceiveGUIs;

	public SysexSendReceiveManagerGUI(SysexSendReceiveManager uploadTaskManager) {
		super(new BorderLayout());

		this.sysexSendReceiveManager = uploadTaskManager;
		uploadTaskManager.addObserver(this);

		sysexSendReceiveGUIs = new Vector();

		add(createButtonPanel(), BorderLayout.NORTH);
		add(createTabPanel(), BorderLayout.CENTER);

		createGUIs();
	}

	private JPanel createButtonPanel() {
		JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 4, 4));

		JLabel label = new JLabel("SysEx Send/Receive:", JLabel.LEFT);
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

		Iterator it = sysexSendReceiveManager.getSysexSendReceives().iterator();

		while (it.hasNext()) {

			SysexSendReceive sysexSendReceive = (SysexSendReceive) it.next();

			boolean hasGUI = false;

			Iterator it2 = sysexSendReceiveGUIs.iterator();

			while (it2.hasNext()) {

				SysexSendReceiveGUI sysexSendReceiveGUI = (SysexSendReceiveGUI) it2
						.next();

				if (sysexSendReceiveGUI.getSysexSendReceive() == sysexSendReceive) {
					hasGUI = true;
				}
			}

			if (!hasGUI) {

				SysexSendReceiveGUI sysexSendReceiveGUI = new SysexSendReceiveGUI(
						sysexSendReceive);

				sysexSendReceiveGUIs.add(sysexSendReceiveGUI);

				tabbedPane.addTab("SysEx Send/Receive"
						+ (sysexSendReceiveManager.getSysexSendReceives()
								.indexOf(sysexSendReceive) + 1), ImageLoader
						.getImageIcon("sysex.png"), sysexSendReceiveGUI);
			}
		}

		it = ((Vector) sysexSendReceiveGUIs.clone()).iterator();

		while (it.hasNext()) {

			SysexSendReceiveGUI sysexSendReceiveGUI = (SysexSendReceiveGUI) it
					.next();

			boolean hasDevice = false;

			Iterator it2 = sysexSendReceiveManager.getSysexSendReceives()
					.iterator();

			while (it2.hasNext()) {

				SysexSendReceive sysexSendReceive = (SysexSendReceive) it2
						.next();

				if (sysexSendReceiveGUI.getSysexSendReceive() == sysexSendReceive) {
					hasDevice = true;
				}
			}

			if (!hasDevice) {
				sysexSendReceiveGUIs.remove(sysexSendReceiveGUI);
				tabbedPane.remove(sysexSendReceiveGUI);
			}
		}

		for (int i = 0; i < sysexSendReceiveManager.getSysexSendReceives()
				.size(); i++) {

			tabbedPane.setTitleAt(i, "SysEx Send/Receive " + (i + 1));
		}
	}

	public void addNewSendReceiveTab() {
		sysexSendReceiveManager.newSysexSendReceive();
	}

	public void closeSendReceiveTab() {
		int selectedTab = tabbedPane.getSelectedIndex();

		if (selectedTab > -1) {
			sysexSendReceiveManager
					.removeSysexSendReceive((SysexSendReceive) sysexSendReceiveManager
							.getSysexSendReceives().elementAt(selectedTab));
		}
	}

	public JTabbedPane getTabbedPane() {
		return tabbedPane;
	}

	public void update(Observable observable, Object object) {
		if (observable == sysexSendReceiveManager) {
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
