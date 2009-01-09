/*
 * @(#)MidiRouterDeviceManagerGUI.java	beta8	2006/04/23
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
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.DefaultTableModel;

import org.midibox.midi.MidiRouterDevice;
import org.midibox.midi.MidiRouterDeviceManager;

public class MidiRouterDeviceManagerGUI extends JPanel implements Observer,
		ActionListener, ListSelectionListener {

	private MidiRouterDeviceManager midiRouterDeviceManager;

	private JTable routerTable;

	private RouterDataModel routerDataModel;

	private JButton removeButton;

	public MidiRouterDeviceManagerGUI(
			MidiRouterDeviceManager midiRouterDeviceManager) {
		super(new BorderLayout());
		this.midiRouterDeviceManager = midiRouterDeviceManager;
		midiRouterDeviceManager.addObserver(this);
		add(createMainPanel(), BorderLayout.CENTER);
		add(createButtonPanel(), BorderLayout.SOUTH);
		setButtonStates();
	}

	private JPanel createMainPanel() {
		JPanel mainPanel = new JPanel(new BorderLayout());
		routerDataModel = new RouterDataModel(midiRouterDeviceManager
				.getMidiRouterDevices());
		routerTable = new JTable(routerDataModel);
		routerTable.getSelectionModel().setSelectionMode(
				ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
		routerTable.getSelectionModel().addListSelectionListener(this);
		routerTable.setPreferredScrollableViewportSize(new Dimension(200, 200));
		JScrollPane scrollPane = new JScrollPane(routerTable);
		scrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		mainPanel.add(scrollPane);
		return mainPanel;
	}

	private JPanel createButtonPanel() {
		JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));

		JButton button = new JButton("Add Virtual Port");
		button.setActionCommand("add");
		button.addActionListener(this);
		buttonPanel.add(button);

		removeButton = new JButton("Remove Virtual Port");
		removeButton.setActionCommand("remove");
		removeButton.addActionListener(this);
		buttonPanel.add(removeButton);
		return buttonPanel;
	}

	public MidiRouterDeviceManager getMidiRouterDeviceManager() {
		return midiRouterDeviceManager;
	}

	private void setButtonStates() {
		boolean rowSelected = routerTable.getSelectedRowCount() > 0;
		removeButton.setEnabled(rowSelected);
	}

	public void valueChanged(ListSelectionEvent lse) {
		setButtonStates();
	}

	public void update(Observable observable, Object object) {
		if (observable == midiRouterDeviceManager) {
			routerDataModel.fireTableDataChanged();
		}
	}

	public void actionPerformed(ActionEvent ae) {
		String actionCommand = ae.getActionCommand();

		if (actionCommand.equals("add")) {
			midiRouterDeviceManager.addMidiRouterDevice();
		} else if (actionCommand.equals("remove")) {

			int[] selectedRows = routerTable.getSelectedRows();

			for (int v = selectedRows.length - 1; v >= 0; v--) {

				midiRouterDeviceManager
						.removeMidiRouter((MidiRouterDevice) midiRouterDeviceManager
								.getMidiRouterDevices().elementAt(
										selectedRows[v]));

			}
		}
	}

	class RouterDataModel extends DefaultTableModel {
		private String[] headers = { "Virtual Port Name" };

		private Vector midiRouters;

		public RouterDataModel(Vector midiRouters) {
			this.midiRouters = midiRouters;
		}

		public int getColumnCount() {
			return headers.length;
		}

		public int getRowCount() {
			if (midiRouters != null) {
				return midiRouters.size();
			}
			return 0;
		}

		public String getColumnName(int col) {
			return headers[col];
		}

		public Object getValueAt(int row, int col) {

			MidiRouterDevice midiRouter = (MidiRouterDevice) midiRouters
					.elementAt(row);

			if (col == 0) {
				return midiRouter.getDeviceInfo().getName();
			}
			return null;
		}

		public Class getColumnClass(int c) {
			return getValueAt(0, c).getClass();
		}

		public boolean isCellEditable(int row, int col) {
			return false;
		}
	}
}
