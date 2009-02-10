/*
 * @(#)MidiFilterGUI.java	beta8	2006/04/23
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
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JToolBar;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableModel;

import org.midibox.midi.MidiFilter;
import org.midibox.midi.MidiUtils;
import org.midibox.utils.gui.ImageLoader;

public class MidiFilterGUI extends JPanel implements ActionListener,
		TableModelListener, Observer {

	private MidiFilter midiFilter;

	// sysex messages
	private JCheckBox sysexMessage;

	// meta messages
	private JCheckBox metaMessage;

	// short messages
	private JCheckBox activeSensing;

	private JCheckBox channelPressure;

	private JCheckBox continueMessage;

	private JCheckBox controlChange;

	private JCheckBox midiTimeCode;

	private JCheckBox noteOff;

	private JCheckBox noteOn;

	private JCheckBox pitchBend;

	private JCheckBox pollyPressure;

	private JCheckBox programChange;

	private JCheckBox songPositionPointer;

	private JCheckBox songSelect;

	private JCheckBox start;

	private JCheckBox stop;

	private JCheckBox systemReset;

	private JCheckBox timingClock;

	private JCheckBox tuneRequest;

	private JTable cc;

	private DefaultTableModel ccModel;

	private JTable channels;

	private DefaultTableModel channelsModel;

	public MidiFilterGUI(MidiFilter midiFilter) {
		super(new BorderLayout(5, 5));
		this.midiFilter = midiFilter;

		setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

		// voice messages
		JPanel voicePanel = new JPanel(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridy = 0;
		gbc.anchor = GridBagConstraints.NORTHWEST;
		gbc.fill = GridBagConstraints.BOTH;
		gbc.weightx = 1.0;

		voicePanel
				.setBorder(BorderFactory.createTitledBorder("Voice Messages"));

		noteOff = new JCheckBox("Note Off", midiFilter.noteOff);
		noteOff.addActionListener(this);
		voicePanel.add(noteOff, gbc);
		gbc.gridy++;

		noteOn = new JCheckBox("Note On", midiFilter.noteOn);
		noteOn.addActionListener(this);
		voicePanel.add(noteOn, gbc);
		gbc.gridy++;

		pollyPressure = new JCheckBox("Aftertouch", midiFilter.pollyPressure);
		pollyPressure.addActionListener(this);
		voicePanel.add(pollyPressure, gbc);
		gbc.gridy++;

		controlChange = new JCheckBox("Control Change",
				midiFilter.controlChange);
		controlChange.addActionListener(this);
		voicePanel.add(controlChange, gbc);
		gbc.gridy++;

		programChange = new JCheckBox("Program Change",
				midiFilter.programChange);
		programChange.addActionListener(this);
		voicePanel.add(programChange, gbc);
		gbc.gridy++;

		channelPressure = new JCheckBox("Channel Pressure",
				midiFilter.channelPressure);
		channelPressure.addActionListener(this);
		voicePanel.add(channelPressure, gbc);
		gbc.gridy++;

		pitchBend = new JCheckBox("Pitch Bend", midiFilter.pitchBend);
		pitchBend.addActionListener(this);
		voicePanel.add(pitchBend, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		voicePanel.add(Box.createVerticalGlue(), gbc);

		// sysex messages
		JPanel sysexPanel = new JPanel(new GridBagLayout());
		gbc.gridy = 0;
		gbc.weighty = 0.0;
		sysexPanel
				.setBorder(BorderFactory.createTitledBorder("Sysex Messages"));

		sysexMessage = new JCheckBox("Sysex Message", midiFilter.sysexMessage);
		sysexMessage.addActionListener(this);
		sysexPanel.add(sysexMessage, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		sysexPanel.add(Box.createVerticalGlue(), gbc);

		// system common
		JPanel sysCommon = new JPanel(new GridBagLayout());
		gbc.gridy = 0;
		gbc.weighty = 0.0;
		sysCommon.setBorder(BorderFactory.createTitledBorder("System Common"));

		midiTimeCode = new JCheckBox("MIDI Time Code", midiFilter.midiTimeCode);
		midiTimeCode.addActionListener(this);
		sysCommon.add(midiTimeCode, gbc);
		gbc.gridy++;

		songPositionPointer = new JCheckBox("Song Position Pointer",
				midiFilter.songPositionPointer);
		songPositionPointer.addActionListener(this);
		sysCommon.add(songPositionPointer, gbc);
		gbc.gridy++;

		songSelect = new JCheckBox("Song Select", midiFilter.songSelect);
		songSelect.addActionListener(this);
		sysCommon.add(songSelect, gbc);
		gbc.gridy++;

		tuneRequest = new JCheckBox("Tune Request", midiFilter.tuneRequest);
		tuneRequest.addActionListener(this);
		sysCommon.add(tuneRequest, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		sysCommon.add(Box.createVerticalGlue(), gbc);

		// system realtime
		JPanel sysReal = new JPanel(new GridBagLayout());
		gbc.gridy = 0;
		gbc.weighty = 0.0;
		sysReal.setBorder(BorderFactory.createTitledBorder("System Realtime"));

		timingClock = new JCheckBox("Timing Clock", midiFilter.timingClock);
		timingClock.addActionListener(this);
		sysReal.add(timingClock, gbc);
		gbc.gridy++;

		start = new JCheckBox("Start", midiFilter.start);
		start.addActionListener(this);
		sysReal.add(start, gbc);
		gbc.gridy++;

		continueMessage = new JCheckBox("Continue", midiFilter.continueMessage);
		continueMessage.addActionListener(this);
		sysReal.add(continueMessage, gbc);
		gbc.gridy++;

		stop = new JCheckBox("Stop", midiFilter.stop);
		stop.addActionListener(this);
		sysReal.add(stop, gbc);
		gbc.gridy++;

		activeSensing = new JCheckBox("Active Sensing",
				midiFilter.activeSensing);
		activeSensing.addActionListener(this);
		sysReal.add(activeSensing, gbc);
		gbc.gridy++;

		systemReset = new JCheckBox("System Reset", midiFilter.systemReset);
		systemReset.addActionListener(this);
		sysReal.add(systemReset, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		sysReal.add(Box.createVerticalGlue(), gbc);

		// meta messages
		JPanel metaPanel = new JPanel(new GridBagLayout());
		gbc.gridy = 0;
		gbc.weighty = 0.0;
		metaPanel.setBorder(BorderFactory.createTitledBorder("Meta Messages"));

		metaMessage = new JCheckBox("Meta Message", midiFilter.metaMessage);
		metaMessage.addActionListener(this);
		metaPanel.add(metaMessage, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		metaPanel.add(Box.createVerticalGlue(), gbc);

		channelsModel = new MyTableModel(
				new String[] { "Selected", "Channel" }, 0);
		channels = new JTable(channelsModel);

		for (int i = 0; i < 16; i++) {
			channelsModel.addRow(new Object[] {
					new Boolean(midiFilter.channel[i]), "Channel " + (i + 1) });
		}

		JScrollPane channelsScroll = new JScrollPane(channels);
		channelsScroll
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		channelsScroll.setPreferredSize(new Dimension(150, 200));
		channelsScroll.setBorder(BorderFactory.createTitledBorder("Channels"));
		channels.setCellSelectionEnabled(false);
		channels.setShowGrid(false);
		channelsModel.addTableModelListener(this);

		ccModel = new MyTableModel(new String[] { "Selected", "Controller" }, 0);
		cc = new JTable(ccModel);

		for (int i = 0; i < 128; i++) {
			ccModel.addRow(new Object[] { new Boolean(midiFilter.cc[i]),
					i + ": " + MidiUtils.ccNames[i] });
		}

		JScrollPane ccScroll = new JScrollPane(cc);
		ccScroll
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		ccScroll.setPreferredSize(new Dimension(200, 200));
		ccScroll.setBorder(BorderFactory.createTitledBorder("Control Change"));
		cc.setRowSelectionAllowed(false);
		cc.setShowGrid(false);
		ccModel.addTableModelListener(this);

		JPanel mainPanel = new JPanel(new GridBagLayout());
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridheight = 2;

		mainPanel.add(voicePanel, gbc);
		gbc.gridx++;

		gbc.gridheight = 1;

		mainPanel.add(sysCommon, gbc);
		gbc.gridy++;

		mainPanel.add(sysexPanel, gbc);
		gbc.gridx++;
		gbc.gridy = 0;

		mainPanel.add(sysReal, gbc);
		gbc.gridy++;

		mainPanel.add(metaPanel, gbc);

		gbc.gridx++;
		gbc.gridy = 0;

		gbc.gridheight = 2;

		mainPanel.add(channelsScroll, gbc);
		gbc.gridx++;

		mainPanel.add(ccScroll, gbc);

		add(mainPanel, BorderLayout.CENTER);

		//add(createToolBar(), BorderLayout.NORTH);
	}

	private JToolBar createToolBar() {

		JToolBar toolBar = new JToolBar();

		toolBar.setRollover(true);

		toolBar.setFloatable(false);

		JButton button = new JButton(ImageLoader.getImageIcon("open.png"));

		button.addActionListener(this);

		button.setMargin(new Insets(2, 2, 2, 2));

		button.setToolTipText("Load MIDI filter definition");

		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("save.png"));

		button.setToolTipText("Save MIDI filter definition");

		button.addActionListener(this);

		button.setMargin(new Insets(2, 2, 2, 2));

		toolBar.add(button);

		return toolBar;
	}

	public void actionPerformed(ActionEvent ae) {

		Object source = ae.getSource();

		if (source == sysexMessage)
			midiFilter.sysexMessage = sysexMessage.isSelected();

		if (source == metaMessage)
			midiFilter.metaMessage = metaMessage.isSelected();

		if (source == activeSensing)
			midiFilter.activeSensing = activeSensing.isSelected();

		if (source == channelPressure)
			midiFilter.channelPressure = channelPressure.isSelected();

		if (source == continueMessage)
			midiFilter.continueMessage = continueMessage.isSelected();

		if (source == controlChange)
			midiFilter.controlChange = controlChange.isSelected();

		if (source == midiTimeCode)
			midiFilter.midiTimeCode = midiTimeCode.isSelected();

		if (source == noteOff)
			midiFilter.noteOff = noteOff.isSelected();

		if (source == noteOn)
			midiFilter.noteOn = noteOn.isSelected();

		if (source == pitchBend)
			midiFilter.pitchBend = pitchBend.isSelected();

		if (source == pollyPressure)
			midiFilter.pollyPressure = pollyPressure.isSelected();

		if (source == programChange)
			midiFilter.programChange = programChange.isSelected();

		if (source == songPositionPointer)
			midiFilter.songPositionPointer = songPositionPointer.isSelected();

		if (source == songSelect)
			midiFilter.songSelect = songSelect.isSelected();

		if (source == start)
			midiFilter.start = start.isSelected();

		if (source == stop)
			midiFilter.stop = stop.isSelected();

		if (source == systemReset)
			midiFilter.systemReset = systemReset.isSelected();

		if (source == timingClock)
			midiFilter.timingClock = timingClock.isSelected();

		if (source == tuneRequest)
			midiFilter.tuneRequest = tuneRequest.isSelected();
	}

	public void tableChanged(TableModelEvent e) {
		int row = e.getFirstRow();

		TableModel model = (TableModel) e.getSource();
		Boolean data = (Boolean) model.getValueAt(row, 0);

		if (model == channelsModel) {
			midiFilter.channel[row] = data.booleanValue();
		} else {
			midiFilter.cc[row] = data.booleanValue();
		}
	}

	public void update(Observable observable, Object object) {

	}

	class MyTableModel extends DefaultTableModel {

		public MyTableModel(Object[] a, int b) {
			super(a, b);
		}

		public Class getColumnClass(int c) {
			return getValueAt(0, c).getClass();
		}

		public boolean isCellEditable(int row, int col) {
			if (col > 0) {
				return false;
			} else {
				return true;
			}
		}
	}
}
