/*
 * @(#)MidiMapDeviceManagerGUI.java	beta7	2006/04/23
 *
 * Copyright (C) 2006    Adam King (adamjking@optusnet.com.au)
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
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Hashtable;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.DefaultTableModel;

import org.midibox.midi.MidiMapDevice;
import org.midibox.midi.MidiMapDeviceManager;

public class MidiMapDeviceManagerGUI extends JPanel implements Observer,
		ActionListener, ListSelectionListener {

	private MidiMapDeviceManager midiMapDeviceManager;

	private JTable midiMapsTable;

	private FiltersDataModel filtersDataModel;

	private JButton propertiesButton;

	private JButton removeButton;

	private Hashtable mapPropertiesDialogs;

	public MidiMapDeviceManagerGUI(MidiMapDeviceManager midiMapManager) {
		super(new BorderLayout());
		this.midiMapDeviceManager = midiMapManager;
		midiMapManager.addObserver(this);
		add(createMainPanel(), BorderLayout.CENTER);
		add(createButtonPanel(), BorderLayout.SOUTH);
		mapPropertiesDialogs = new Hashtable();
		setButtonStates();
	}

	private JPanel createMainPanel() {
		JPanel mainPanel = new JPanel(new BorderLayout());
		filtersDataModel = new FiltersDataModel(midiMapDeviceManager
				.getMidiMapDevices());
		midiMapsTable = new JTable(filtersDataModel);
		midiMapsTable.getSelectionModel().setSelectionMode(
				ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
		midiMapsTable.getSelectionModel().addListSelectionListener(this);
		midiMapsTable
				.setPreferredScrollableViewportSize(new Dimension(200, 200));
		JScrollPane scrollPane = new JScrollPane(midiMapsTable);
		scrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		mainPanel.add(scrollPane);
		return mainPanel;
	}

	private JPanel createButtonPanel() {
		JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));

		JButton button = new JButton("Add Map");
		button.setActionCommand("add");
		button.addActionListener(this);
		buttonPanel.add(button);

		propertiesButton = new JButton("Map Properites");
		propertiesButton.setActionCommand("properties");
		propertiesButton.addActionListener(this);
		buttonPanel.add(propertiesButton);

		removeButton = new JButton("Remove Map");
		removeButton.setActionCommand("remove");
		removeButton.addActionListener(this);
		buttonPanel.add(removeButton);
		return buttonPanel;
	}

	public MidiMapDeviceManager getMidiMapDeviceManager() {
		return midiMapDeviceManager;
	}

	private void showMapProperties() {

		int[] selectedRows = midiMapsTable.getSelectedRows();

		for (int m = 0; m < selectedRows.length; m++) {

			final MidiMapDevice midiMapDevice = (MidiMapDevice) midiMapDeviceManager
					.getMidiMapDevices().elementAt(selectedRows[m]);

			if (!mapPropertiesDialogs.containsKey(midiMapDevice)) {

				final MidiMapGUI midiMapGUI = new MidiMapGUI(midiMapDevice
						.getMidiMap());

				JDialog mapPropertiesDialog = new JDialog();
				mapPropertiesDialog.setContentPane(midiMapGUI);
				mapPropertiesDialog.setModal(false);
				mapPropertiesDialog.setTitle(midiMapDevice.getDeviceInfo()
						.getName());
				mapPropertiesDialog.pack();
				mapPropertiesDialog.setLocationRelativeTo(this);
				Point location = mapPropertiesDialog.getLocation();
				mapPropertiesDialog.setLocation(location.x + m * 20, location.y
						+ m * 20);
				mapPropertiesDialog.setVisible(true);

				mapPropertiesDialogs.put(midiMapDevice, mapPropertiesDialog);

				mapPropertiesDialog.addWindowListener(new WindowAdapter() {
					public void windowClosing(WindowEvent we) {
						midiMapDevice.deleteObserver(midiMapGUI);
						mapPropertiesDialogs.remove(midiMapDevice);
					}
				});
			}
			((JDialog) mapPropertiesDialogs.get(midiMapDevice)).requestFocus();
		}
	}

	private void setButtonStates() {
		boolean rowSelected = midiMapsTable.getSelectedRowCount() > 0;
		propertiesButton.setEnabled(rowSelected);
		removeButton.setEnabled(rowSelected);
	}

	public void valueChanged(ListSelectionEvent lse) {
		setButtonStates();
	}

	public void update(Observable observable, Object object) {
		if (observable == midiMapDeviceManager) {
			filtersDataModel.fireTableDataChanged();
		}
	}

	public void actionPerformed(ActionEvent ae) {
		String actionCommand = ae.getActionCommand();

		if (actionCommand.equals("add")) {
			midiMapDeviceManager.addMidiMapDevice();
		} else if (actionCommand.equals("properties")) {
			showMapProperties();
		} else if (actionCommand.equals("remove")) {

			int[] selectedRows = midiMapsTable.getSelectedRows();

			for (int m = selectedRows.length - 1; m >= 0; m--) {
				midiMapDeviceManager
						.removeMidiMapDevice((MidiMapDevice) midiMapDeviceManager
								.getMidiMapDevices().elementAt(selectedRows[m]));
			}
		}
	}

	class FiltersDataModel extends DefaultTableModel {
		private String[] headers = { "Map Name" };

		private Vector midiMaps;

		public FiltersDataModel(Vector midiMaps) {
			this.midiMaps = midiMaps;
		}

		public int getColumnCount() {
			return headers.length;
		}

		public int getRowCount() {
			if (midiMaps != null) {
				return midiMaps.size();
			}
			return 0;
		}

		public String getColumnName(int col) {
			return headers[col];
		}

		public Object getValueAt(int row, int col) {

			MidiMapDevice midiMapDevice = (MidiMapDevice) midiMaps
					.elementAt(row);

			if (col == 0) {
				return midiMapDevice.getDeviceInfo().getName();
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