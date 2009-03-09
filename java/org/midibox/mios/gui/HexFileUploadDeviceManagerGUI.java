/*
 * @(#)HexFileUploadDeviceManagerGUI.java	beta8	2006/04/23
 *
 * Copyright (C) 2008   Adam King (adamjking@optusnet.com.au)
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

package org.midibox.mios.gui;

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

import org.midibox.mios.HexFileUploadDevice;
import org.midibox.mios.HexFileUploadDeviceManager;
import org.midibox.utils.gui.ImageLoader;

public class HexFileUploadDeviceManagerGUI extends JPanel implements
		ActionListener, Observer {

	private HexFileUploadDeviceManager hexFileUploadDeviceManager;

	private JTabbedPane tabbedPane;

	private Vector hexFileUploadGUIs;

	public HexFileUploadDeviceManagerGUI(
			HexFileUploadDeviceManager hexFileUploadDeviceManager) {
		super(new BorderLayout());

		this.hexFileUploadDeviceManager = hexFileUploadDeviceManager;
		hexFileUploadDeviceManager.addObserver(this);

		hexFileUploadGUIs = new Vector();

		add(createButtonPanel(), BorderLayout.NORTH);
		add(createTabPanel(), BorderLayout.CENTER);

		createGUIs();
	}
	
	public JTabbedPane getTabbedPane() {
		return tabbedPane;
	}

	private JPanel createButtonPanel() {
		JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 4, 4));

		JLabel label = new JLabel("MIOS Hex Upload:", JLabel.LEFT);
		label.setOpaque(true);
		buttonPanel.add(label);
		buttonPanel.add(Box.createHorizontalStrut(10));

		JButton button = new JButton("New Tab");
		button.setActionCommand("newUpload");
		button.setToolTipText("Open new upload tab");
		button.addActionListener(this);
		buttonPanel.add(button);

		button = new JButton("Close Tab");
		button.setActionCommand("closeUpload");
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

		Iterator it = hexFileUploadDeviceManager.getHexFileUploadDevices()
				.iterator();

		while (it.hasNext()) {
			HexFileUploadDevice hexFileUploadDevice = (HexFileUploadDevice) it
					.next();

			boolean hasGUI = false;

			Iterator it2 = hexFileUploadGUIs.iterator();

			while (it2.hasNext()) {

				HexFileUploadGUI hexFileUploadGUI = (HexFileUploadGUI) it2
						.next();

				if (hexFileUploadGUI.getHexFileUpload() == hexFileUploadDevice
						.getHexFileUpload()) {
					hasGUI = true;
				}
			}

			if (!hasGUI) {

				HexFileUploadGUI hexFileUploadGUI = new HexFileUploadGUI(
						hexFileUploadDevice.getHexFileUpload());

				hexFileUploadGUIs.add(hexFileUploadGUI);

				tabbedPane.addTab(
						hexFileUploadDevice.getDeviceInfo().getName(),
						ImageLoader.getImageIcon("hex.png"), hexFileUploadGUI);
			}
		}

		it = ((Vector) hexFileUploadGUIs.clone()).iterator();

		while (it.hasNext()) {

			HexFileUploadGUI hexFileUploadGUI = (HexFileUploadGUI) it.next();

			boolean hasDevice = false;

			Iterator it2 = hexFileUploadDeviceManager.getHexFileUploadDevices()
					.iterator();

			while (it2.hasNext()) {

				HexFileUploadDevice hexFileUploadDevice = (HexFileUploadDevice) it2
						.next();

				if (hexFileUploadGUI.getHexFileUpload() == hexFileUploadDevice
						.getHexFileUpload()) {
					hasDevice = true;
				}
			}

			if (!hasDevice) {

				hexFileUploadGUIs.remove(hexFileUploadGUI);

				tabbedPane.remove(hexFileUploadGUI);
			}
		}

		for (int i = 0; i < hexFileUploadDeviceManager
				.getHexFileUploadDevices().size(); i++) {

			HexFileUploadDevice hexFileUploadDevice = (HexFileUploadDevice) hexFileUploadDeviceManager
					.getHexFileUploadDevices().elementAt(i);

			tabbedPane.setTitleAt(i, hexFileUploadDevice.getDeviceInfo()
					.getName());
		}
	}

	public void addNewUploadTab() {
		hexFileUploadDeviceManager.newHexFileUploadDevice();
	}

	public void closeUploadTab() {
		int selectedTab = tabbedPane.getSelectedIndex();

		if (selectedTab > -1) {
			hexFileUploadDeviceManager
					.removeHexFileUploadDevice((HexFileUploadDevice) hexFileUploadDeviceManager
							.getHexFileUploadDevices().elementAt(selectedTab));
		}
	}

	public void update(Observable observable, Object object) {

		if (observable == hexFileUploadDeviceManager) {

			createGUIs();
		}
	}

	public void actionPerformed(ActionEvent ae) {

		String command = ae.getActionCommand();

		if (command.equals("newUpload")) {
			addNewUploadTab();
		}

		if (command.equals("closeUpload")) {
			closeUploadTab();
		}
	}
}
