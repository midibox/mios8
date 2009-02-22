/*
 * @(#)MidiParameterControlProperties.java	beta8	2006/04/23
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

import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import javax.swing.JPanel;

import org.midibox.midi.MidiParameterControl;

public class MidiParameterControlProperties extends MidiParameterProperties {

	protected JCheckBox receiveCheckBox;

	protected JCheckBox sendCheckBox;

	protected JCheckBox globalCheckBox;

	public MidiParameterControlProperties(MidiParameterControl midiParameter) {
		super(midiParameter);
		globalCheckBox = new JCheckBox("Global");
		globalCheckBox.addActionListener(this);
		GridBagConstraints gbc = ((GridBagLayout) channelPanel.getLayout())
				.getConstraints(channelSpinner);
		gbc.gridy++;
		channelPanel.add(globalCheckBox, gbc);
		add(createBottomPanel(), BorderLayout.SOUTH);
		updateControls();
	}

	public JPanel createBottomPanel() {

		JPanel messagesPanel = new JPanel(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.anchor = GridBagConstraints.WEST;
		gbc.gridy = 0;

		receiveCheckBox = new JCheckBox("Receive MIDI Messages");
		receiveCheckBox.addActionListener(this);
		messagesPanel.add(receiveCheckBox, gbc);
		gbc.gridy++;

		sendCheckBox = new JCheckBox("Send MIDI Messages");
		sendCheckBox.addActionListener(this);
		messagesPanel.add(sendCheckBox, gbc);

		messagesPanel.setBorder(BorderFactory
				.createTitledBorder("MIDI Controller"));

		return messagesPanel;
	}

	public void updateControls() {

		super.updateControls();

		if (sendCheckBox != null && receiveCheckBox != null
				&& globalCheckBox != null) {

			sendCheckBox.setSelected(((MidiParameterControl) midiParameterControl)
					.isSend());
			sendCheckBox.setEnabled(!((MidiParameterControl) midiParameterControl)
					.isLearn());
			receiveCheckBox.setSelected(((MidiParameterControl) midiParameterControl)
					.isReceive());
			receiveCheckBox.setEnabled(!((MidiParameterControl) midiParameterControl)
					.isLearn());

			globalCheckBox.setSelected(((MidiParameterControl) midiParameterControl)
					.isGlobal());
			globalCheckBox.setEnabled(!((MidiParameterControl) midiParameterControl)
					.isLearn());

			channelSpinner.setEnabled(!((MidiParameterControl) midiParameterControl)
					.isGlobal()
					&& !((MidiParameterControl) midiParameterControl).isLearn());
		}
	}

	public void actionPerformed(ActionEvent ae) {

		super.actionPerformed(ae);

		if (update) {
			update = false;

			Object source = ae.getSource();

			if (source == receiveCheckBox) {
				((MidiParameterControl) midiParameterControl)
						.setReceive(receiveCheckBox.isSelected());
			} else if (source == sendCheckBox) {
				((MidiParameterControl) midiParameterControl).setSend(sendCheckBox
						.isSelected());
			} else if (source == globalCheckBox) {
				((MidiParameterControl) midiParameterControl).setGlobal(globalCheckBox
						.isSelected());
			}
			update = true;
		}
	}
}
