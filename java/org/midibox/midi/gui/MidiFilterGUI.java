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
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.sound.midi.ShortMessage;
import javax.swing.AbstractCellEditor;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JToolBar;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableCellEditor;

import org.midibox.midi.MidiFilter;
import org.midibox.midi.MidiUtils;
import org.midibox.midi.xml.MidiFilterXML;
import org.midibox.mios.gui.HexFileUploadGUI;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SimpleFileChooserFilter;
import org.midibox.utils.gui.SplitButton;

public class MidiFilterGUI extends JPanel implements Observer, ActionListener {

	private MidiFilter midiFilter;

	private JCheckBox voiceMessages;

	private JCheckBox systemCommonMessages;

	private JCheckBox systemRealtimeMessages;

	private JCheckBox sysexMessages;

	private JCheckBox metaMessages;

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

	private JTable controlChangeTable;

	private DefaultTableModel controlChangeModel;

	private JTable channelsTable;

	private DefaultTableModel channelsModel;
	
	private JPopupMenu MRUPopupMenu;
	
	private SplitButton openMRUButton;

	private static int maxMRU = 10;	

	private static String currentDirectory = "";
	
	private static Vector MRU = new Vector();

	private static JFileChooser fc = null;

	public MidiFilterGUI(MidiFilter midiFilter) {
		super(new BorderLayout(5, 5));
		this.midiFilter = midiFilter;

		midiFilter.addObserver(this);

		setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

		// voice messages
		JPanel masterPanel = new JPanel(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.anchor = GridBagConstraints.NORTHWEST;
		gbc.fill = GridBagConstraints.BOTH;
		gbc.weightx = 1.0;
		gbc.insets = new Insets(0, 2, 0, 2);

		masterPanel.setBorder(BorderFactory.createTitledBorder("Message Type"));

		voiceMessages = new JCheckBox("Voice", midiFilter.isVoiceMessages());
		voiceMessages.addActionListener(this);

		masterPanel.add(voiceMessages, gbc);
		gbc.gridy++;

		systemCommonMessages = new JCheckBox("System Common", midiFilter
				.isSystemCommonMessages());
		systemCommonMessages.addActionListener(this);

		masterPanel.add(systemCommonMessages, gbc);
		gbc.gridy++;

		systemRealtimeMessages = new JCheckBox("System Realtime", midiFilter
				.isSystemRealtimeMessages());
		systemRealtimeMessages.addActionListener(this);

		masterPanel.add(systemRealtimeMessages, gbc);
		gbc.gridy++;

		sysexMessages = new JCheckBox("SysEx", midiFilter.isSysexMessages());
		sysexMessages.addActionListener(this);

		masterPanel.add(sysexMessages, gbc);
		gbc.gridy++;

		metaMessages = new JCheckBox("Meta", midiFilter.isMetaMessages());
		metaMessages.addActionListener(this);

		masterPanel.add(metaMessages, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		masterPanel.add(Box.createVerticalGlue(), gbc);

		JPanel voicePanel = new JPanel(new GridBagLayout());
		gbc.gridy = 0;
		gbc.weighty = 0.0;

		voicePanel.setBorder(BorderFactory.createTitledBorder("Voice"));

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

		gbc.gridheight = 8;
		gbc.weighty = 0.0;
		gbc.gridy = 0;

		gbc.gridx++;

		controlChangeModel = new MidiFilterTableModel(
				new String[] { "Controller" }, 128);
		controlChangeTable = new JTable(controlChangeModel);

		JScrollPane ccScroll = new JScrollPane(controlChangeTable);
		ccScroll
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		ccScroll.setPreferredSize(new Dimension(200, 200));
		controlChangeTable.setRowSelectionAllowed(false);
		controlChangeTable.setShowGrid(false);
		controlChangeTable.setDefaultRenderer(Boolean.class,
				new MidiFilterTableCellRenderer());
		controlChangeTable.setDefaultEditor(Boolean.class,
				new MidiFilterTableCellEditor());

		voicePanel.add(ccScroll, gbc);

		gbc.gridx++;

		channelsModel = new MidiFilterTableModel(new String[] { "Channel" }, 16);
		channelsTable = new JTable(channelsModel);

		JScrollPane channelsScroll = new JScrollPane(channelsTable);
		channelsScroll
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		channelsScroll.setPreferredSize(new Dimension(150, 200));
		channelsTable.setCellSelectionEnabled(false);
		channelsTable.setShowGrid(false);
		channelsTable.setDefaultRenderer(Boolean.class,
				new MidiFilterTableCellRenderer());
		channelsTable.setDefaultEditor(Boolean.class,
				new MidiFilterTableCellEditor());

		voicePanel.add(channelsScroll, gbc);

		// system common
		JPanel systemCommonPanel = new JPanel(new GridBagLayout());
		gbc.gridy = 0;
		gbc.weighty = 0.0;
		gbc.gridheight = 1;
		systemCommonPanel.setBorder(BorderFactory
				.createTitledBorder("System Common"));

		midiTimeCode = new JCheckBox("MTC Quarter Frame", midiFilter
				.getSystemCommonMessage(ShortMessage.MIDI_TIME_CODE));
		midiTimeCode.addActionListener(this);
		systemCommonPanel.add(midiTimeCode, gbc);
		gbc.gridy++;

		songPositionPointer = new JCheckBox("Song Position Pointer", midiFilter
				.getSystemCommonMessage(ShortMessage.SONG_POSITION_POINTER));
		songPositionPointer.addActionListener(this);
		systemCommonPanel.add(songPositionPointer, gbc);
		gbc.gridy++;

		songSelect = new JCheckBox("Song Select", midiFilter
				.getSystemCommonMessage(ShortMessage.SONG_SELECT));
		songSelect.addActionListener(this);
		systemCommonPanel.add(songSelect, gbc);
		gbc.gridy++;

		tuneRequest = new JCheckBox("Tune Request", midiFilter
				.getSystemCommonMessage(ShortMessage.TUNE_REQUEST));
		tuneRequest.addActionListener(this);
		systemCommonPanel.add(tuneRequest, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		systemCommonPanel.add(Box.createVerticalGlue(), gbc);

		// system realtime
		JPanel systemRealtimePanel = new JPanel(new GridBagLayout());
		gbc.gridy = 0;
		gbc.weighty = 0.0;
		systemRealtimePanel.setBorder(BorderFactory
				.createTitledBorder("System Realtime"));

		timingClock = new JCheckBox("Timing Clock", midiFilter
				.getSystemRealtimeMessage(ShortMessage.TIMING_CLOCK));
		timingClock.addActionListener(this);
		systemRealtimePanel.add(timingClock, gbc);
		gbc.gridy++;

		start = new JCheckBox("Start", midiFilter
				.getSystemRealtimeMessage(ShortMessage.START));
		start.addActionListener(this);
		systemRealtimePanel.add(start, gbc);
		gbc.gridy++;

		continueMessage = new JCheckBox("Continue", midiFilter
				.getSystemRealtimeMessage(ShortMessage.CONTINUE));
		continueMessage.addActionListener(this);
		systemRealtimePanel.add(continueMessage, gbc);
		gbc.gridy++;

		stop = new JCheckBox("Stop", midiFilter
				.getSystemRealtimeMessage(ShortMessage.STOP));
		stop.addActionListener(this);
		systemRealtimePanel.add(stop, gbc);
		gbc.gridy++;

		activeSensing = new JCheckBox("Active Sensing", midiFilter
				.getSystemRealtimeMessage(ShortMessage.ACTIVE_SENSING));
		activeSensing.addActionListener(this);
		systemRealtimePanel.add(activeSensing, gbc);
		gbc.gridy++;

		systemReset = new JCheckBox("System Reset", midiFilter
				.getSystemRealtimeMessage(ShortMessage.SYSTEM_RESET));
		systemReset.addActionListener(this);
		systemRealtimePanel.add(systemReset, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		systemRealtimePanel.add(Box.createVerticalGlue(), gbc);

		JPanel mainPanel = new JPanel(new GridBagLayout());
		gbc.gridx = 0;
		gbc.gridy = 0;

		mainPanel.add(masterPanel, gbc);
		gbc.gridx++;

		mainPanel.add(voicePanel, gbc);
		gbc.gridx++;

		mainPanel.add(systemCommonPanel, gbc);
		gbc.gridx++;

		mainPanel.add(systemRealtimePanel, gbc);

		add(mainPanel, BorderLayout.CENTER);

		add(createToolBar(), BorderLayout.NORTH);

		setButtonStates();
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

		MRUPopupMenu = new JPopupMenu();
		MRUPopupMenu.addPopupMenuListener(new PopupMenuListener() {

			public void popupMenuCanceled(PopupMenuEvent e) {

			}

			public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {

			}

			public void popupMenuWillBecomeVisible(PopupMenuEvent e) {

				MidiFilterGUI.this.buildMRUMenu(MRUPopupMenu);

			}
		});

		openMRUButton = new SplitButton(button, MRUPopupMenu);
		openMRUButton.setRollover(true);
		
		toolBar.add(openMRUButton);

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

			loadFilterDefinition(file);

			setButtonStates();

			currentDirectory = fc.getCurrentDirectory().toString();
		}
	}
	
	protected void loadFilterDefinition(File file) {
		
		if (file.exists()) {
			
			MidiFilterXML midiFilterXML = new MidiFilterXML(midiFilter,
					MidiFilterXML.TAG_ROOT_ELEMENT);

			midiFilterXML.loadXML(file);
			
			saveMRU(file.getPath());
			
		} else {
			JOptionPane.showMessageDialog(this,
					"MIDI filter definition no longer exists",
					"File does not exist", JOptionPane.ERROR_MESSAGE);
		}
	}
	
	private void buildMRUMenu(JComponent menu) {

		menu.removeAll();

		Iterator it = MRU.iterator();

		while (it.hasNext()) {

			final JMenuItem menuItem = new JMenuItem((String) it.next());

			menuItem.addActionListener(new ActionListener() {

				public void actionPerformed(ActionEvent ae) {

					File file = new File(menuItem.getText());

					loadFilterDefinition(file);
				}
			});

			menu.add(menuItem, 0);
		}
	}
	
	public static void saveMRU(String file) {

		MRU.remove(file);

		MRU.add(file);

		for (int i = MRU.size() - maxMRU; i > 0; i--) {

			MRU.removeElementAt(i - 1);
		}
	}
	
	public static Vector getMRU() {
		return MRU;
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

			MidiFilterXML midiFilterXML = new MidiFilterXML(midiFilter,
					MidiFilterXML.TAG_ROOT_ELEMENT);

			midiFilterXML.saveXML(file);

			saveMRU(file.getPath());
				
			currentDirectory = fc.getCurrentDirectory().toString();
		}
	}

	public static String getCurrentDirectory() {
		return currentDirectory;
	}

	public static void setCurrentDirectory(String currentDirectory) {
		MidiFilterGUI.currentDirectory = currentDirectory;
	}

	public void update(Observable observable, Object object) {

		setButtonStates();
	}

	public void actionPerformed(ActionEvent ae) {

		Object source = ae.getSource();

		if (source == voiceMessages)
			midiFilter.setVoiceMessages(voiceMessages.isSelected());

		if (source == systemCommonMessages)
			midiFilter.setSystemCommonMessages(systemCommonMessages
					.isSelected());

		if (source == systemRealtimeMessages)
			midiFilter.setSystemRealtimeMessages(systemRealtimeMessages
					.isSelected());

		if (source == sysexMessages)
			midiFilter.setSysexMessages(sysexMessages.isSelected());

		if (source == metaMessages)
			midiFilter.setMetaMessages(metaMessages.isSelected());

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

		if (ae.getActionCommand() == "load")
			loadFilterDefinition();

		if (ae.getActionCommand() == "save")
			saveFilterDefinition();
	}

	protected void setButtonStates() {

		voiceMessages.setSelected(midiFilter.isVoiceMessages());

		systemCommonMessages.setSelected(midiFilter.isSystemCommonMessages());

		systemRealtimeMessages.setSelected(midiFilter
				.isSystemRealtimeMessages());

		sysexMessages.setSelected(midiFilter.isSysexMessages());

		metaMessages.setSelected(midiFilter.isMetaMessages());

		noteOff.setSelected(midiFilter.getVoiceMessage(ShortMessage.NOTE_OFF));
		noteOff.setEnabled(midiFilter.isVoiceMessages());

		noteOn.setSelected(midiFilter.getVoiceMessage(ShortMessage.NOTE_ON));
		noteOn.setEnabled(midiFilter.isVoiceMessages());

		polyPressure.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.POLY_PRESSURE));
		polyPressure.setEnabled(midiFilter.isVoiceMessages());

		controlChange.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.CONTROL_CHANGE));
		controlChange.setEnabled(midiFilter.isVoiceMessages());

		programChange.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.PROGRAM_CHANGE));
		programChange.setEnabled(midiFilter.isVoiceMessages());

		channelPressure.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.CHANNEL_PRESSURE));
		channelPressure.setEnabled(midiFilter.isVoiceMessages());

		pitchBend.setSelected(midiFilter
				.getVoiceMessage(ShortMessage.PITCH_BEND));
		pitchBend.setEnabled(midiFilter.isVoiceMessages());

		controlChangeModel.fireTableDataChanged();

		channelsModel.fireTableDataChanged();

		midiTimeCode.setSelected(midiFilter
				.getSystemCommonMessage(ShortMessage.MIDI_TIME_CODE));
		midiTimeCode.setEnabled(midiFilter.isSystemCommonMessages());

		songPositionPointer.setSelected(midiFilter
				.getSystemCommonMessage(ShortMessage.SONG_POSITION_POINTER));
		songPositionPointer.setEnabled(midiFilter.isSystemCommonMessages());

		songSelect.setSelected(midiFilter
				.getSystemCommonMessage(ShortMessage.SONG_SELECT));
		songSelect.setEnabled(midiFilter.isSystemCommonMessages());

		tuneRequest.setSelected(midiFilter
				.getSystemCommonMessage(ShortMessage.TUNE_REQUEST));
		tuneRequest.setEnabled(midiFilter.isSystemCommonMessages());

		timingClock.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.TIMING_CLOCK));
		timingClock.setEnabled(midiFilter.isSystemRealtimeMessages());

		start.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.START));
		start.setEnabled(midiFilter.isSystemRealtimeMessages());

		continueMessage.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.CONTINUE));
		continueMessage.setEnabled(midiFilter.isSystemRealtimeMessages());

		stop
				.setSelected(midiFilter
						.getSystemRealtimeMessage(ShortMessage.STOP));
		stop.setEnabled(midiFilter.isSystemRealtimeMessages());

		activeSensing.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.ACTIVE_SENSING));
		activeSensing.setEnabled(midiFilter.isSystemRealtimeMessages());

		systemReset.setSelected(midiFilter
				.getSystemRealtimeMessage(ShortMessage.SYSTEM_RESET));
		systemReset.setEnabled(midiFilter.isSystemRealtimeMessages());
	}

	class MidiFilterTableCellEditor extends AbstractCellEditor implements
			TableCellEditor {

		private JCheckBox checkBox;

		public Component getTableCellEditorComponent(JTable table,
				Object value, boolean isSelected, int row, int column) {

			if (table.getRowCount() == 128) {

				checkBox = new JCheckBox(row + ": " + MidiUtils.ccNames[row],
						midiFilter.getControlChange(row));

				checkBox.setEnabled(midiFilter.isVoiceMessages()
						&& midiFilter
								.getVoiceMessage(ShortMessage.CONTROL_CHANGE));

			} else {

				checkBox = new JCheckBox("Channel " + (row + 1), midiFilter
						.getChannel(row));
			}

			checkBox.setOpaque(false);

			checkBox.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ae) {
					fireEditingStopped();
				}
			});

			return checkBox;
		}

		public Object getCellEditorValue() {

			return new Boolean(checkBox.isSelected());
		}
	}

	class MidiFilterTableCellRenderer extends DefaultTableCellRenderer {

		public Component getTableCellRendererComponent(JTable table,
				Object value, boolean isSelected, boolean hasFocus, int row,
				int column) {

			JCheckBox checkBox;

			if (table.getRowCount() == 128) {

				checkBox = new JCheckBox(row + ": " + MidiUtils.ccNames[row],
						midiFilter.getControlChange(row));

				checkBox.setEnabled(midiFilter.isVoiceMessages()
						&& midiFilter
								.getVoiceMessage(ShortMessage.CONTROL_CHANGE));

			} else {

				checkBox = new JCheckBox("Channel " + (row + 1), midiFilter
						.getChannel(row));

				checkBox.setEnabled(midiFilter.isVoiceMessages());
			}

			checkBox.setOpaque(false);

			return checkBox;
		}
	}

	class MidiFilterTableModel extends DefaultTableModel {

		public MidiFilterTableModel(Object[] a, int b) {
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

					return new Boolean(midiFilter.getChannel(row));
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

					midiFilter
							.setChannel(row, ((Boolean) value).booleanValue());
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
