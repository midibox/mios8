/*
 * @(#)MidiParameterProperties.java	beta8	2006/04/23
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
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.JToggleButton;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import org.midibox.midi.MidiParameter;
import org.midibox.midi.MidiParameterControl;
import org.midibox.midi.MidiUtils;

public class MidiParameterProperties extends JPanel implements Observer,
		ActionListener, ListSelectionListener, ChangeListener {

	protected MidiParameterControl midiParameterControl;

	protected JToggleButton midiLearn;

	protected JSpinner channelSpinner;

	protected JComboBox statusComboBox;

	protected JList nameList;

	protected JScrollPane nameScrollPane;

	protected JLabel defaultValueLabel;

	protected JSpinner defaultValueSpinner;

	protected JCheckBox highResolution;

	protected JPanel statusPanel;

	protected JPanel midiLearnPanel;

	protected JPanel channelPanel;

	protected JPanel defaultPanel;

	protected JPanel mainPanel;

	protected JPanel numberPanel;

	protected boolean update = true;

	public MidiParameterProperties(MidiParameterControl midiParameterControl) {
		super(new BorderLayout());
		this.midiParameterControl = midiParameterControl;
		midiParameterControl.addObserver(this);

		add(createMainPanel(), BorderLayout.CENTER);

		midiLearnPanel = new JPanel(new BorderLayout());

		midiLearn = new JToggleButton("MIDI Learn");
		midiLearn.addActionListener(this);

		midiLearnPanel.add(midiLearn);
		midiLearnPanel
				.setBorder(BorderFactory.createTitledBorder("MIDI Learn"));

		add(midiLearnPanel, BorderLayout.NORTH);

		updateControls();
	}

	protected JPanel createMainPanel() {

		statusPanel = new JPanel(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.anchor = GridBagConstraints.WEST;
		gbc.gridy = 0;

		statusComboBox = new JComboBox(new String[] { "Note Off/On",
				"Aftertouch", "Control Change", "Program Change",
				"Channel Pressure", "Pitch Bend" });
		statusComboBox.addActionListener(this);

		statusPanel.add(statusComboBox, gbc);
		gbc.gridy++;
		statusPanel.setBorder(BorderFactory.createTitledBorder("Status"));

		highResolution = new JCheckBox("14 bit");
		highResolution.addActionListener(this);
		statusPanel.add(highResolution, gbc);

		channelPanel = new JPanel(new GridBagLayout());
		gbc = new GridBagConstraints();
		gbc.anchor = GridBagConstraints.WEST;
		gbc.gridy = 0;

		channelSpinner = new JSpinner(new SpinnerNumberModel(1, 1, 16, 1));
		channelSpinner.addChangeListener(this);
		channelPanel.add(channelSpinner, gbc);

		channelPanel.setBorder(BorderFactory.createTitledBorder("Channel"));

		defaultPanel = new JPanel();

		defaultValueLabel = new JLabel("Channel Pressure Value: ", JLabel.LEFT);
		defaultPanel.add(defaultValueLabel);

		defaultValueSpinner = new JSpinner(new SpinnerNumberModel(0, 0,
				midiParameterControl.getMidiMaxValue(), 1));
		defaultValueSpinner.addChangeListener(this);
		defaultPanel.add(defaultValueSpinner);

		defaultPanel.setBorder(BorderFactory
				.createTitledBorder("Default \"On\" Value"));

		numberPanel = new JPanel(new BorderLayout(0, 4));

		nameList = new JList();
		nameList.addListSelectionListener(this);
		nameList.setVisibleRowCount(4);
		nameScrollPane = new JScrollPane(nameList);
		nameScrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		numberPanel.add(nameScrollPane);

		numberPanel.setBorder(BorderFactory.createTitledBorder("Number"));

		mainPanel = new JPanel(new GridBagLayout());
		gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridheight = 1;
		gbc.gridwidth = 1;
		gbc.weightx = 1.0;

		gbc.anchor = GridBagConstraints.NORTHWEST;
		gbc.fill = GridBagConstraints.BOTH;

		mainPanel.add(statusPanel, gbc);
		gbc.gridx++;

		mainPanel.add(channelPanel, gbc);

		gbc.gridx = 0;
		gbc.gridy++;
		gbc.gridwidth = 2;

		mainPanel.add(defaultPanel, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;

		mainPanel.add(numberPanel, gbc);

		return mainPanel;
	}

	public void updateControls() {

		boolean isLearn = midiParameterControl.isLearn();

		midiLearn.setSelected(isLearn);

		int status = midiParameterControl.getMidiStatus();

		statusComboBox
				.setSelectedIndex((midiParameterControl.getMidiStatus() >> 4) - 9);
		statusComboBox.setEnabled(!isLearn);

		String[] listData = new String[128];

		for (int i = 0; i < listData.length; i++) {
			listData[i] = i + ": " + MidiUtils.getKeyName(i);
		}

		if (status == MidiParameter.NOTE_ON) {
			nameList.setEnabled(true && !isLearn);
			nameScrollPane.setEnabled(true);

		} else if (status == MidiParameter.AFTERTOUCH) {
			nameList.setEnabled(true && !isLearn);
			nameScrollPane.setEnabled(true && !isLearn);

		} else if (status == MidiParameter.CONTROL_CHANGE) {
			nameList.setEnabled(true && !isLearn);
			nameScrollPane.setEnabled(true && !isLearn);

			if (midiParameterControl.isHighResolution()) {
				for (int i = 0; i < listData.length; i++) {
					listData[i] = i + ": " + MidiUtils.cc14BitNames[i];
				}
			} else {
				for (int i = 0; i < listData.length; i++) {
					listData[i] = i + ": " + MidiUtils.ccNames[i];
				}
			}
		} else if (status == MidiParameter.PROGRAM_CHANGE) {
			nameList.setEnabled(false);
			nameScrollPane.setEnabled(false);

		} else if (status == MidiParameter.CHANNEL_PRESSURE) {
			nameList.setEnabled(false);
			nameScrollPane.setEnabled(false);

		} else if (status == MidiParameter.PITCH_BEND) {
			nameList.setEnabled(false);
			nameScrollPane.setEnabled(false);
		}

		nameList.setListData(listData);
		nameList.setSelectedIndex(midiParameterControl.getMidiNumber());
		nameList.ensureIndexIsVisible(midiParameterControl.getMidiNumber());

		if (status == MidiParameter.NOTE_ON) {
			defaultValueLabel.setText("Note On Velocity:       ");

		} else if (status == MidiParameter.AFTERTOUCH) {
			defaultValueLabel.setText("Aftertouch Pressure:    ");

		} else if (status == MidiParameter.CONTROL_CHANGE) {
			defaultValueLabel.setText("Control Change Value:   ");

		} else if (status == MidiParameter.PROGRAM_CHANGE) {
			defaultValueLabel.setText("Program Change No:      ");

		} else if (status == MidiParameter.CHANNEL_PRESSURE) {
			defaultValueLabel.setText("Channel Pressure Value: ");

		} else if (status == MidiParameter.PITCH_BEND) {
			defaultValueLabel.setText("Pitch Bend Value:       ");
		}

		channelSpinner.getModel().setValue(
				new Integer(midiParameterControl.getMidiChannel() + 1));
		channelSpinner.setEnabled(!isLearn);

		((SpinnerNumberModel) defaultValueSpinner.getModel())
				.setValue(new Integer(midiParameterControl.getMidiDefaultValue()));
		((SpinnerNumberModel) defaultValueSpinner.getModel())
				.setMaximum(new Integer(midiParameterControl.getMidiMaxValue()));
		defaultValueSpinner.setEnabled(!isLearn);

		highResolution.setSelected(((MidiParameterControl) midiParameterControl)
				.isHighResolution());
		highResolution
				.setEnabled(!((MidiParameterControl) midiParameterControl).isLearn()
						&& ((MidiParameterControl) midiParameterControl)
								.getMidiStatus() == MidiParameterControl.CONTROL_CHANGE);
	}

	public void stateChanged(ChangeEvent ce) {
		if (update) {
			update = false;
			Object source = ce.getSource();

			if (source == channelSpinner) {
				midiParameterControl.setMidiChannel(((Integer) channelSpinner
						.getModel().getValue()).intValue() - 1);
			}
			if (source == defaultValueSpinner) {
				midiParameterControl
						.setMidiDefaultValue(((Integer) defaultValueSpinner
								.getModel().getValue()).intValue());
			}
			update = true;
		}
	}

	public void valueChanged(ListSelectionEvent lse) {
		if (update) {
			update = false;
			if (nameList.getSelectedIndex() >= 0) {
				midiParameterControl.setMidiNumber(nameList.getSelectedIndex());
			}
			update = true;
		}
	}

	public void update(Observable observable, Object object) {
		if (observable == midiParameterControl) {
			if (object != MidiParameter.VALUE) {
				updateControls();
			}
		}
	}

	public void actionPerformed(ActionEvent ae) {
		if (update) {
			update = false;

			Object source = ae.getSource();

			if (source == statusComboBox) {
				midiParameterControl
						.setMidiStatus((statusComboBox.getSelectedIndex() + 9) << 4);
			} else if (source == midiLearn) {
				midiParameterControl.setLearn(midiLearn.isSelected());
			} else if (source == highResolution) {
				midiParameterControl.setHighResolution(highResolution.isSelected());
			}
			update = true;
		}
	}
}