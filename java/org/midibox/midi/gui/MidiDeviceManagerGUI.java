/*
 * @(#)MidiDeviceManagerGUI.java	beta8	2006/04/23
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

import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.LinkedHashMap;
import java.util.Observable;
import java.util.Observer;

import javax.sound.midi.MidiDevice;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.table.DefaultTableModel;

import org.midibox.midi.MidiDeviceManager;

public class MidiDeviceManagerGUI extends JPanel implements Observer,
		ActionListener {

	private MidiDeviceManager midiDeviceManager;

	private JTable midiReadDevices;

	private JTable midiWriteDevices;

	public MidiDeviceManagerGUI(MidiDeviceManager midiDeviceManager) {
		super(new GridBagLayout());

		this.midiDeviceManager = midiDeviceManager;
		midiDeviceManager.addObserver(this);

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;
		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.weightx = 1.0;
		gbc.insets = new Insets(5, 5, 5, 5);

		gbc.weighty = 0.0;
		add(new JLabel("MIDI Devices - Readable", JLabel.CENTER), gbc);
		gbc.gridx++;

		add(new JLabel("MIDI Devices - Writeable", JLabel.CENTER), gbc);
		gbc.gridx--;
		gbc.gridy++;

		gbc.fill = GridBagConstraints.BOTH;
		gbc.weighty = 1.0;
		midiReadDevices = new JTable(new MidiDeviceTableModel(midiDeviceManager
				.getMidiReadDevices(), midiDeviceManager
				.getSelectedMidiReadDevices()));
		midiReadDevices.setPreferredScrollableViewportSize(new Dimension(200,
				200));
		midiReadDevices.setCellSelectionEnabled(false);
		midiReadDevices.setShowGrid(false);

		JScrollPane scrollPane = new JScrollPane(midiReadDevices);
		scrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		add(scrollPane, gbc);
		gbc.gridx++;

		midiWriteDevices = new JTable(new MidiDeviceTableModel(
				midiDeviceManager.getMidiWriteDevices(), midiDeviceManager
						.getSelectedMidiWriteDevices()));
		midiWriteDevices.setPreferredScrollableViewportSize(new Dimension(200,
				200));
		midiWriteDevices.setCellSelectionEnabled(false);
		midiWriteDevices.setShowGrid(false);

		scrollPane = new JScrollPane(midiWriteDevices);
		scrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		add(scrollPane, gbc);

		gbc.gridx = 0;
		gbc.gridy++;
		gbc.gridwidth = 2;
		gbc.fill = GridBagConstraints.NONE;
		gbc.weightx = 0.0;
		gbc.weighty = 0.0;
		gbc.anchor = GridBagConstraints.WEST;

		JButton button = new JButton("Rescan Devices");
		button.setActionCommand("rescan");
		button.addActionListener(this);
		add(button, gbc);
	}

	public void update(Observable observable, Object object) {
		if (observable == midiDeviceManager) {
			((DefaultTableModel) midiReadDevices.getModel())
					.fireTableDataChanged();
			((DefaultTableModel) midiWriteDevices.getModel())
					.fireTableDataChanged();
		}
	}

	public void actionPerformed(ActionEvent ae) {
		if (ae.getActionCommand().equals("rescan")) {
			midiDeviceManager.rescanDevices();
		}
	}

	class MidiDeviceTableModel extends DefaultTableModel {
		private String[] headers = { "Selected", "MIDI Device Name" };

		private LinkedHashMap midiDevices;

		private LinkedHashMap selectedMidiDevices;

		public MidiDeviceTableModel(LinkedHashMap midiDevices,
				LinkedHashMap selectedMidiDevices) {
			this.midiDevices = midiDevices;
			this.selectedMidiDevices = selectedMidiDevices;
		}

		public int getColumnCount() {
			return headers.length;
		}

		public int getRowCount() {
			if (midiDevices != null) {
				return midiDevices.size();
			}
			return 0;
		}

		public String getColumnName(int col) {
			return headers[col];
		}

		public Object getValueAt(int row, int col) {

			MidiDevice midiDevice = (MidiDevice) midiDevices.values().toArray()[row];

			if (col == 0) {
				return new Boolean(selectedMidiDevices
						.containsValue(midiDevices.values().toArray()[row]));
			} else if (col == 1) {
				return midiDevice.getDeviceInfo().getName();
			}
			return null;
		}

		public void setValueAt(Object aValue, int row, int column) {
			if (column == 0) {
				if (((Boolean) aValue).booleanValue()) {

					if (midiDevices == midiDeviceManager.getMidiReadDevices()) {
						midiDeviceManager
								.selectMidiReadDevice((MidiDevice) midiDevices
										.values().toArray()[row]);

					} else {
						midiDeviceManager
								.selectMidiWriteDevice((MidiDevice) midiDevices
										.values().toArray()[row]);
					}
				} else {
					if (midiDevices == midiDeviceManager.getMidiReadDevices()) {
						midiDeviceManager
								.deselectMidiReadDevice((MidiDevice) midiDevices
										.values().toArray()[row]);
					} else {
						midiDeviceManager
								.deselectMidiWriteDevice((MidiDevice) midiDevices
										.values().toArray()[row]);
					}
				}
			}
		}

		public Class getColumnClass(int c) {
			return getValueAt(0, c).getClass();
		}

		public boolean isCellEditable(int row, int col) {
			return col == 0;
		}
	}
}
