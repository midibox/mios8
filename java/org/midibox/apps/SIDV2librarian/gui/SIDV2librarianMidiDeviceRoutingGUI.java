/*
 * @(#)MidiDeviceRoutingGUI.java	beta7	2006/04/23
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

package org.midibox.apps.SIDV2librarian.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;

import javax.sound.midi.MidiDevice;
import javax.swing.BorderFactory;
import javax.swing.DefaultListCellRenderer;
import javax.swing.DefaultListModel;
import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.SwingConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import org.midibox.apps.SIDV2librarian.SIDV2librarian;
import org.midibox.midi.MidiDeviceRouting;
import org.midibox.utils.gui.GuiUtils;
import org.midibox.utils.gui.ImageLoader;

public class SIDV2librarianMidiDeviceRoutingGUI extends JPanel implements
		ChangeListener, ListSelectionListener, ActionListener, Observer {

	private SIDV2librarian sidv2librarian;

	private static Icon openIcon = ImageLoader.getImageIcon("midiPortOpen.png");

	private static Icon closedIcon = ImageLoader
			.getImageIcon("midiPortClosed.png");

	private Hashtable icons = new Hashtable();

	private DefaultListModel midiReadDevicesListModel;

	private JList midiReadDevicesList;

	private JScrollPane midiReadDevicesScroller;

	private DefaultListModel midiWriteDevicesListModel;

	private JList midiWriteDevicesList;

	private JScrollPane midiWriteDevicesScroller;

	private JButton rescan;

	public SIDV2librarianMidiDeviceRoutingGUI(SIDV2librarian sidv2librarian) {
		super(new BorderLayout());

		this.sidv2librarian = sidv2librarian;
		sidv2librarian.getMidiDeviceRouting().addObserver(this);

		midiReadDevicesListModel = new DefaultListModel();
		midiWriteDevicesListModel = new DefaultListModel();

		midiReadDevicesList = new JList(midiReadDevicesListModel);
		midiReadDevicesList
				.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		midiReadDevicesList.setLayoutOrientation(JList.VERTICAL);
		midiReadDevicesList.setVisibleRowCount(10);
		midiReadDevicesList.setCellRenderer(new MyListCellRenderer());
		midiReadDevicesList.setBackground(Color.WHITE);
		midiWriteDevicesList = new JList(midiWriteDevicesListModel);
		midiWriteDevicesList
				.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		midiWriteDevicesList.setLayoutOrientation(JList.VERTICAL);
		midiWriteDevicesList.setVisibleRowCount(10);
		midiWriteDevicesList.setCellRenderer(new MyListCellRenderer());
		midiWriteDevicesList.setBackground(Color.WHITE);

		JPanel listPane = new JPanel(new GridBagLayout());

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 1;
		gbc.gridy = 1;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;
		gbc.insets = new Insets(2, 0, 2, 0);
		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.weightx = 1.0;
		gbc.weighty = 0.0;

		listPane.setOpaque(false);

		listPane.add(new JLabel("MIDI Input devices", SwingConstants.CENTER),
				gbc);
		gbc.gridy++;

		midiReadDevicesScroller = new JScrollPane(midiReadDevicesList);
		midiReadDevicesScroller.getViewport().addChangeListener(this);
		midiReadDevicesScroller
				.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		midiReadDevicesScroller
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

		gbc.fill = GridBagConstraints.BOTH;
		gbc.weighty = 1.0;
		gbc.weighty = 1.0;

		listPane.add(midiReadDevicesScroller, gbc);
		gbc.gridx++;

		gbc.gridx++;
		gbc.gridy--;

		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.weightx = 1.0;
		gbc.weighty = 0.0;

		listPane.add(new JLabel("MIDI Output devices", SwingConstants.CENTER),
				gbc);
		gbc.gridy++;

		midiWriteDevicesScroller = new JScrollPane(midiWriteDevicesList);
		midiWriteDevicesScroller
				.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		midiWriteDevicesScroller
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		midiWriteDevicesScroller.getViewport().addChangeListener(this);

		gbc.fill = GridBagConstraints.BOTH;
		gbc.weightx = 1.0;
		gbc.weighty = 1.0;

		listPane.add(midiWriteDevicesScroller, gbc);

		int maxPreferredWidth = Math.max(midiReadDevicesScroller
				.getPreferredSize().width, midiWriteDevicesScroller
				.getPreferredSize().width);
		int maxPreferredHeight = Math.max(midiReadDevicesScroller
				.getPreferredSize().height, midiWriteDevicesScroller
				.getPreferredSize().height);

		GuiUtils.setComponentSize(midiReadDevicesScroller, maxPreferredWidth,
				maxPreferredHeight);
		GuiUtils.setComponentSize(midiWriteDevicesScroller, maxPreferredWidth,
				maxPreferredHeight);

		midiReadDevicesList.addListSelectionListener(this);
		midiWriteDevicesList.addListSelectionListener(this);

		listPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
		listPane.setOpaque(false);
		add(listPane, BorderLayout.CENTER);
		rescan = new JButton("Rescan");
		rescan.addActionListener(this);
		add(rescan, BorderLayout.SOUTH);
		populateTrees();
	}

	public MidiDeviceRouting getMidiDeviceRouting() {
		return sidv2librarian.getMidiDeviceRouting();
	}

	public Icon getMidiDeviceIcon(MidiDevice md) {
		if (icons.containsKey(md)) {
			return (Icon) icons.get(md);
		}

		if (icons.containsKey(md.getClass())) {
			return (Icon) icons.get(md.getClass());
		}

		try {
			if (md.isOpen()) {
				return openIcon;
			}

			return closedIcon;

		} catch (Exception e) {
			return closedIcon;
		}
	}

	public void redrawAll() {
		midiReadDevicesList.repaint();
		midiWriteDevicesList.repaint();
	}

	private void populateTrees() {
		midiReadDevicesListModel.removeAllElements();
		Iterator it = sidv2librarian.getMidiDeviceRouting()
				.getMidiReadDevices().iterator();

		while (it.hasNext()) {
			midiReadDevicesListModel.addElement(((MidiDevice) it.next())
					.getDeviceInfo().getName());
		}
		midiReadDevicesList.setSelectedIndex(0);
		midiWriteDevicesListModel.removeAllElements();
		it = sidv2librarian.getMidiDeviceRouting().getMidiWriteDevices()
				.iterator();
		while (it.hasNext()) {
			midiWriteDevicesListModel.addElement(((MidiDevice) it.next())
					.getDeviceInfo().getName());
		}
		midiWriteDevicesList.setSelectedIndex(0);
		updateUI();
	}

	public void valueChanged(ListSelectionEvent lse) {
		if (lse.getSource() == midiReadDevicesList) {
			if (midiReadDevicesList.getSelectedIndex() > 0) {
				sidv2librarian.setInputDeviceIndex(midiReadDevicesList
						.getSelectedIndex());
			}
		} else if (lse.getSource() == midiWriteDevicesList) {
			if (midiWriteDevicesList.getSelectedIndex() > 0) {
				sidv2librarian.setOutputDeviceIndex(midiWriteDevicesList
						.getSelectedIndex());
			}
		}
	}

	public void stateChanged(ChangeEvent ce) {
	}

	public Dimension getMinimumSize() {
		return getPreferredSize();
	}

	public void update(Observable observable, Object object) {
		if (observable == sidv2librarian.getMidiDeviceRouting()) {
			if (object == sidv2librarian.getMidiDeviceRouting()
					.getMidiReadDevices()
					|| object == sidv2librarian.getMidiDeviceRouting()
							.getMidiWriteDevices()) {
				populateTrees();
			}
			midiReadDevicesList.setSelectedIndex(sidv2librarian
					.getInputDeviceIndex());
			midiWriteDevicesList.setSelectedIndex(sidv2librarian
					.getOutputDeviceIndex());
			redrawAll();
		}

	}

	public void actionPerformed(ActionEvent ae) {
		if (ae.getSource() == rescan) {
			sidv2librarian.getMidiDeviceManager().rescanDevices();
		}
	}

	public class MyListCellRenderer extends DefaultListCellRenderer {
		public Component getListCellRendererComponent(JList list, Object value,
				int index, boolean isSelected, boolean hasFocus) {

			super.getListCellRendererComponent(list, value, index, isSelected,
					hasFocus);

			setIcon(getMidiDeviceIcon(((MidiDevice) ((list == midiWriteDevicesList) ? sidv2librarian
					.getMidiDeviceRouting().getMidiWriteDevices()
					: sidv2librarian.getMidiDeviceRouting()
							.getMidiReadDevices()).elementAt(index))));

			return this;
		}
	}
}
