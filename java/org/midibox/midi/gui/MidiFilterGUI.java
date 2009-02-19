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
import java.io.File;
import java.util.Observable;
import java.util.Observer;

import javax.sound.midi.MetaMessage;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.SysexMessage;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JToolBar;
import javax.swing.table.DefaultTableModel;

import org.midibox.midi.MidiFilter;
import org.midibox.midi.MidiUtils;
import org.midibox.midi.xml.MidiFilterXML;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SimpleFileChooserFilter;

public class MidiFilterGUI extends JPanel implements Observer, ActionListener {

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

	private JCheckBox polyPressure;

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

	private static String currentDirectory = "";

	private static JFileChooser fc = null;

	public MidiFilterGUI(MidiFilter midiFilter) {
		super(new BorderLayout(5, 5));
		this.midiFilter = midiFilter;

		midiFilter.addObserver(this);

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

		noteOff = new JCheckBox("Note Off", midiFilter
				.getVoiceMessage(ShortMessage.NOTE_OFF));
		noteOff.addActionListener(this);
		voicePanel.add(noteOff, gbc);
		gbc.gridy++;

		noteOn = new JCheckBox("Note On", midiFilter
				.getVoiceMessage(ShortMessage.NOTE_ON));
		noteOn.addActionListener(this);
		voicePanel.add(noteOn, gbc);
		gbc.gridy++;

		polyPressure = new JCheckBox("Aftertouch", midiFilter
				.getVoiceMessage(ShortMessage.POLY_PRESSURE));
		polyPressure.addActionListener(this);
		voicePanel.add(polyPressure, gbc);
		gbc.gridy++;

		controlChange = new JCheckBox("Control Change", midiFilter
				.getVoiceMessage(ShortMessage.CONTROL_CHANGE));
		controlChange.addActionListener(this);
		voicePanel.add(controlChange, gbc);
		gbc.gridy++;

		programChange = new JCheckBox("Program Change", midiFilter
				.getVoiceMessage(ShortMessage.PROGRAM_CHANGE));
		programChange.addActionListener(this);
		voicePanel.add(programChange, gbc);
		gbc.gridy++;

		channelPressure = new JCheckBox("Channel Pressure", midiFilter
				.getVoiceMessage(ShortMessage.CHANNEL_PRESSURE));
		channelPressure.addActionListener(this);
		voicePanel.add(channelPressure, gbc);
		gbc.gridy++;

		pitchBend = new JCheckBox("Pitch Bend", midiFilter
				.getVoiceMessage(ShortMessage.PITCH_BEND));
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

		sysexMessage = new JCheckBox("Sysex Message", midiFilter
				.isSysexMessages());
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

		midiTimeCode = new JCheckBox("MIDI Time Code", midiFilter
				.getSystemCommonMessage(ShortMessage.MIDI_TIME_CODE));
		midiTimeCode.addActionListener(this);
		sysCommon.add(midiTimeCode, gbc);
		gbc.gridy++;

		songPositionPointer = new JCheckBox("Song Position Pointer", midiFilter
				.getSystemCommonMessage(ShortMessage.SONG_POSITION_POINTER));
		songPositionPointer.addActionListener(this);
		sysCommon.add(songPositionPointer, gbc);
		gbc.gridy++;

		songSelect = new JCheckBox("Song Select", midiFilter
				.getSystemCommonMessage(ShortMessage.SONG_SELECT));
		songSelect.addActionListener(this);
		sysCommon.add(songSelect, gbc);
		gbc.gridy++;

		tuneRequest = new JCheckBox("Tune Request", midiFilter
				.getSystemCommonMessage(ShortMessage.TUNE_REQUEST));
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

		timingClock = new JCheckBox("Timing Clock", midiFilter
				.getSystemRealtimeMessage(ShortMessage.TIMING_CLOCK));
		timingClock.addActionListener(this);
		sysReal.add(timingClock, gbc);
		gbc.gridy++;

		start = new JCheckBox("Start", midiFilter
				.getSystemRealtimeMessage(ShortMessage.START));
		start.addActionListener(this);
		sysReal.add(start, gbc);
		gbc.gridy++;

		continueMessage = new JCheckBox("Continue", midiFilter
				.getSystemRealtimeMessage(ShortMessage.CONTINUE));
		continueMessage.addActionListener(this);
		sysReal.add(continueMessage, gbc);
		gbc.gridy++;

		stop = new JCheckBox("Stop", midiFilter
				.getSystemRealtimeMessage(ShortMessage.STOP));
		stop.addActionListener(this);
		sysReal.add(stop, gbc);
		gbc.gridy++;

		activeSensing = new JCheckBox("Active Sensing", midiFilter
				.getSystemRealtimeMessage(ShortMessage.ACTIVE_SENSING));
		activeSensing.addActionListener(this);
		sysReal.add(activeSensing, gbc);
		gbc.gridy++;

		systemReset = new JCheckBox("System Reset", midiFilter
				.getSystemRealtimeMessage(ShortMessage.SYSTEM_RESET));
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

		metaMessage = new JCheckBox("Meta Message", midiFilter.isMetaMessages());
		metaMessage.addActionListener(this);
		metaPanel.add(metaMessage, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		metaPanel.add(Box.createVerticalGlue(), gbc);

		channelsModel = new MyTableModel(
				new String[] { "Selected", "Channel" }, 0);
		channels = new JTable(channelsModel);

		for (int i = 1; i <= 16; i++) {
			channelsModel.addRow(new Object[] {
					new Boolean(midiFilter.getChannel(i)), "Channel " + i });
		}

		JScrollPane channelsScroll = new JScrollPane(channels);
		channelsScroll
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		channelsScroll.setPreferredSize(new Dimension(150, 200));
		channelsScroll.setBorder(BorderFactory.createTitledBorder("Channels"));
		channels.setCellSelectionEnabled(false);
		channels.setShowGrid(false);

		ccModel = new MyTableModel(new String[] { "Selected", "Controller" }, 0);
		cc = new JTable(ccModel);

		for (int i = 0; i < 128; i++) {
			ccModel.addRow(new Object[] {
					new Boolean(midiFilter.getControlChange(i)),
					i + ": " + MidiUtils.ccNames[i] });
		}

		JScrollPane ccScroll = new JScrollPane(cc);
		ccScroll
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		ccScroll.setPreferredSize(new Dimension(200, 200));
		ccScroll.setBorder(BorderFactory.createTitledBorder("Control Change"));
		cc.setRowSelectionAllowed(false);
		cc.setShowGrid(false);

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

		add(createToolBar(), BorderLayout.NORTH);
	}

	private JToolBar createToolBar() {

		JToolBar toolBar = new JToolBar();

		toolBar.setRollover(true);

		toolBar.setFloatable(false);

		JButton button = new JButton(ImageLoader.getImageIcon("open.png"));

		button.addActionListener(this);

		button.setMargin(new Insets(2, 2, 2, 2));

		button.setToolTipText("Load MIDI filter definition");

		button.setActionCommand("load");

		toolBar.add(button);

		button = new JButton(ImageLoader.getImageIcon("save.png"));

		button.setToolTipText("Save MIDI filter definition");

		button.setActionCommand("save");

		button.addActionListener(this);

		button.setMargin(new Insets(2, 2, 2, 2));

		toolBar.add(button);

		return toolBar;
	}

	protected void loadFilterDefinition() {

		if (fc == null) {
			fc = new JFileChooser(currentDirectory);
			SimpleFileChooserFilter fileFilter = new SimpleFileChooserFilter(
					"XML files", "xml", "XML Filter Definition");
			fc.addChoosableFileFilter(fileFilter);
			fc.setAcceptAllFileFilterUsed(false);
		}

		File noFile = new File("");
		File noFiles[] = { noFile };
		fc.setSelectedFile(noFile);
		fc.setSelectedFiles(noFiles);

		int nRetVal = fc.showOpenDialog(this);

		if (nRetVal == JFileChooser.APPROVE_OPTION) {
			File file = fc.getSelectedFile();

			MidiFilterXML.loadMidiFilter(midiFilter, file);

			setButtonStates();

			currentDirectory = fc.getCurrentDirectory().toString();
		}
	}

	protected void saveFilterDefinition() {

		if (fc == null) {
			fc = new JFileChooser(currentDirectory);
			SimpleFileChooserFilter fileFilter = new SimpleFileChooserFilter(
					"XML files", "xml", "XML Filter Definition");
			fc.addChoosableFileFilter(fileFilter);
			fc.setAcceptAllFileFilterUsed(false);
		}

		File noFile = new File("");
		File noFiles[] = { noFile };
		fc.setSelectedFile(noFile);
		fc.setSelectedFiles(noFiles);

		int nRetVal = fc.showSaveDialog(this);

		if (nRetVal == JFileChooser.APPROVE_OPTION) {
			File file = fc.getSelectedFile();

			MidiFilterXML.saveMidiFilter(midiFilter, file);

			currentDirectory = fc.getCurrentDirectory().toString();
		}
	}

	public void update(Observable observable, Object object) {

		setButtonStates();
	}

	public void actionPerformed(ActionEvent ae) {

		Object source = ae.getSource();

		if (source == noteOff)
			midiFilter.setVoiceMessage(ShortMessage.NOTE_OFF, noteOff
					.isSelected());

		if (source == noteOn)
			midiFilter.setVoiceMessage(ShortMessage.NOTE_ON, noteOn
					.isSelected());

		if (source == polyPressure)
			midiFilter.setVoiceMessage(ShortMessage.POLY_PRESSURE, polyPressure
					.isSelected());

		if (source == controlChange)
			midiFilter.setVoiceMessage(ShortMessage.CONTROL_CHANGE,
					controlChange.isSelected());

		if (source == programChange)
			midiFilter.setVoiceMessage(ShortMessage.PROGRAM_CHANGE,
					programChange.isSelected());

		if (source == channelPressure)
			midiFilter.setVoiceMessage(ShortMessage.CHANNEL_PRESSURE,
					channelPressure.isSelected());

		if (source == pitchBend)
			midiFilter.setVoiceMessage(ShortMessage.PITCH_BEND, pitchBend
					.isSelected());

		if (source == midiTimeCode)
			midiFilter.setSystemCommonMessage(ShortMessage.MIDI_TIME_CODE,
					midiTimeCode.isSelected());

		if (source == songPositionPointer)
			midiFilter.setSystemCommonMessage(
					ShortMessage.SONG_POSITION_POINTER, songPositionPointer
							.isSelected());

		if (source == songSelect)
			midiFilter.setSystemCommonMessage(ShortMessage.SONG_SELECT,
					songSelect.isSelected());

		if (source == tuneRequest)
			midiFilter.setSystemCommonMessage(ShortMessage.TUNE_REQUEST,
					tuneRequest.isSelected());

		if (source == timingClock)
			midiFilter.setSystemRealtimeMessage(ShortMessage.TIMING_CLOCK,
					timingClock.isSelected());

		if (source == start)
			midiFilter.setSystemRealtimeMessage(ShortMessage.START, start
					.isSelected());

		if (source == continueMessage)
			midiFilter.setSystemRealtimeMessage(ShortMessage.CONTINUE,
					continueMessage.isSelected());

		if (source == stop)
			midiFilter.setSystemRealtimeMessage(ShortMessage.STOP, stop
					.isSelected());

		if (source == activeSensing)
			midiFilter.setSystemRealtimeMessage(ShortMessage.ACTIVE_SENSING,
					activeSensing.isSelected());

		if (source == systemReset)
			midiFilter.setSystemRealtimeMessage(ShortMessage.SYSTEM_RESET,
					systemReset.isSelected());

		if (source == sysexMessage)
			midiFilter.setSysexMessages(sysexMessage.isSelected());

		if (source == metaMessage)
			midiFilter.setMetaMessages(metaMessage.isSelected());

		if (ae.getActionCommand() == "load")
			loadFilterDefinition();

		if (ae.getActionCommand() == "save")
			saveFilterDefinition();
	}

	protected void setButtonStates() {

		noteOff.setSelected(midiFilter.getVoiceMessage(ShortMessage.NOTE_OFF));

		noteOn.setSelected(midiFilter.getVoiceMessage(ShortMessage.NOTE_ON));

		polyPressure.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.POLY_PRESSURE));

		controlChange.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.CONTROL_CHANGE));

		programChange.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.PROGRAM_CHANGE));

		channelPressure.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.CHANNEL_PRESSURE));

		pitchBend.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.PITCH_BEND));

		midiTimeCode.setSelected(midiFilter
				.getSystemCommonMessage(ShortMessage.MIDI_TIME_CODE));

		songPositionPointer.setSelected(midiFilter
				.getSystemCommonMessage(ShortMessage.SONG_POSITION_POINTER));

		songSelect.setSelected(midiFilter
				.getSystemCommonMessage(ShortMessage.SONG_SELECT));

		tuneRequest.setSelected(midiFilter
				.getSystemCommonMessage(ShortMessage.TUNE_REQUEST));

		timingClock.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.TIMING_CLOCK));

		start.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.START));

		continueMessage.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.CONTINUE));

		stop
				.setSelected(midiFilter
						.getSystemRealtimeMessage(ShortMessage.STOP));

		activeSensing.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.ACTIVE_SENSING));

		systemReset.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.SYSTEM_RESET));

		sysexMessage.setSelected(midiFilter.isSysexMessages());

		metaMessage.setSelected(midiFilter.isMetaMessages());

		ccModel.fireTableDataChanged();

		channelsModel.fireTableDataChanged();

		cc.setEnabled(midiFilter.getVoiceMessage(ShortMessage.CONTROL_CHANGE));
	}

	class MyTableModel extends DefaultTableModel {

		public MyTableModel(Object[] a, int b) {
			super(a, b);
		}

		public Class getColumnClass(int c) {
			return getValueAt(0, c).getClass();
		}

		public Object getValueAt(int row, int column) {

			if (column == 0) {

				if (getRowCount() == 128) {

					return new Boolean(midiFilter.getControlChange(row));
				} else {

					return new Boolean(midiFilter.getChannel(row + 1));
				}
			}

			return super.getValueAt(row, column);
		}

		public void setValueAt(Object value, int row, int column) {

			if (column == 0) {

				if (getRowCount() == 128) {

					midiFilter.setControlChange(row, ((Boolean) value)
							.booleanValue());
				} else {

					midiFilter.setChannel(row + 1, ((Boolean) value)
							.booleanValue());
				}
			} else {

				super.setValueAt(value, row, column);
			}
		}

		public boolean isCellEditable(int row, int col) {

			if (col == 0) {
				return true;
			}
			return false;
		}
	}
}
