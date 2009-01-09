/*
 * @(#)MidiFilterDeviceManagerGUI.java	beta8	2006/04/23
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

import org.midibox.midi.MidiFilterDevice;
import org.midibox.midi.MidiFilterDeviceManager;

public class MidiFilterDeviceManagerGUI extends JPanel implements Observer,
		ActionListener, ListSelectionListener {

	private MidiFilterDeviceManager midiFilterDeviceManager;

	private JTable midiFiltersTable;

	private FiltersDataModel filtersDataModel;

	private JButton propertiesButton;

	private JButton removeButton;

	private Hashtable filterPropertiesDialogs;

	public MidiFilterDeviceManagerGUI(MidiFilterDeviceManager midiFilterManager) {
		super(new BorderLayout());
		this.midiFilterDeviceManager = midiFilterManager;
		midiFilterManager.addObserver(this);
		add(createMainPanel(), BorderLayout.CENTER);
		add(createButtonPanel(), BorderLayout.SOUTH);
		filterPropertiesDialogs = new Hashtable();
		setButtonStates();
	}

	private JPanel createMainPanel() {
		JPanel mainPanel = new JPanel(new BorderLayout());
		filtersDataModel = new FiltersDataModel(midiFilterDeviceManager
				.getMidiFilterDevices());
		midiFiltersTable = new JTable(filtersDataModel);
		midiFiltersTable.getSelectionModel().setSelectionMode(
				ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
		midiFiltersTable.getSelectionModel().addListSelectionListener(this);
		midiFiltersTable.setPreferredScrollableViewportSize(new Dimension(200,
				200));
		JScrollPane scrollPane = new JScrollPane(midiFiltersTable);
		scrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		mainPanel.add(scrollPane);
		return mainPanel;
	}

	private JPanel createButtonPanel() {
		JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));

		JButton button = new JButton("Add Filter");
		button.setActionCommand("add");
		button.addActionListener(this);
		buttonPanel.add(button);

		propertiesButton = new JButton("Filter Properites");
		propertiesButton.setActionCommand("properties");
		propertiesButton.addActionListener(this);
		buttonPanel.add(propertiesButton);

		removeButton = new JButton("Remove Filter");
		removeButton.setActionCommand("remove");
		removeButton.addActionListener(this);
		buttonPanel.add(removeButton);
		return buttonPanel;
	}

	public MidiFilterDeviceManager getMidiFilterDeviceManager() {
		return midiFilterDeviceManager;
	}

	private void showFilterProperties() {

		int[] selectedRows = midiFiltersTable.getSelectedRows();

		for (int f = 0; f < selectedRows.length; f++) {

			final MidiFilterDevice midiFilterDevice = (MidiFilterDevice) midiFilterDeviceManager
					.getMidiFilterDevices().elementAt(selectedRows[f]);

			if (!filterPropertiesDialogs.containsKey(midiFilterDevice)) {

				final MidiFilterGUI midiFilterGUI = new MidiFilterGUI(
						midiFilterDevice.getMidiFilter());

				JDialog filterPropertiesDialog = new JDialog();
				filterPropertiesDialog.setContentPane(midiFilterGUI);
				filterPropertiesDialog.setModal(false);
				filterPropertiesDialog.setTitle(midiFilterDevice
						.getDeviceInfo().getName());
				filterPropertiesDialog.pack();
				filterPropertiesDialog.setLocationRelativeTo(this);
				Point location = filterPropertiesDialog.getLocation();
				filterPropertiesDialog.setLocation(location.x + f * 20,
						location.y + f * 20);
				filterPropertiesDialog.setVisible(true);

				filterPropertiesDialogs.put(midiFilterDevice,
						filterPropertiesDialog);

				filterPropertiesDialog.addWindowListener(new WindowAdapter() {
					public void windowClosing(WindowEvent we) {
						midiFilterDevice.deleteObserver(midiFilterGUI);
						filterPropertiesDialogs.remove(midiFilterDevice);
					}
				});
			}
			((JDialog) filterPropertiesDialogs.get(midiFilterDevice))
					.requestFocus();

		}
	}

	private void setButtonStates() {
		boolean rowSelected = midiFiltersTable.getSelectedRowCount() > 0;
		propertiesButton.setEnabled(rowSelected);
		removeButton.setEnabled(rowSelected);
	}

	public void valueChanged(ListSelectionEvent lse) {
		setButtonStates();
	}

	public void update(Observable observable, Object object) {
		if (observable == midiFilterDeviceManager) {
			filtersDataModel.fireTableDataChanged();
		}
	}

	public void actionPerformed(ActionEvent ae) {
		String actionCommand = ae.getActionCommand();

		if (actionCommand.equals("add")) {
			midiFilterDeviceManager.addMidiFilterDevice();
		} else if (actionCommand.equals("properties")) {
			showFilterProperties();
		} else if (actionCommand.equals("remove")) {

			int[] selectedRows = midiFiltersTable.getSelectedRows();

			for (int f = selectedRows.length - 1; f >= 0; f--) {

				midiFilterDeviceManager
						.removeMidiFilterDevice((MidiFilterDevice) midiFilterDeviceManager
								.getMidiFilterDevices().elementAt(
										selectedRows[f]));
			}
		}
	}

	class FiltersDataModel extends DefaultTableModel {
		private String[] headers = { "Filter Name" };

		private Vector midiFilterDevices;

		public FiltersDataModel(Vector midiFilterDevices) {
			this.midiFilterDevices = midiFilterDevices;
		}

		public int getColumnCount() {
			return headers.length;
		}

		public int getRowCount() {
			if (midiFilterDevices != null) {
				return midiFilterDevices.size();
			}
			return 0;
		}

		public String getColumnName(int col) {
			return headers[col];
		}

		public Object getValueAt(int row, int col) {

			MidiFilterDevice midiFilterDevice = (MidiFilterDevice) midiFilterDevices
					.elementAt(row);

			if (col == 0) {
				return midiFilterDevice.getDeviceInfo().getName();
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