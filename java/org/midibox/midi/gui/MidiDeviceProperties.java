/*
 * @(#)MidiDeviceProperties.java	beta7	2006/04/23
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

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import javax.sound.midi.MidiDevice;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class MidiDeviceProperties extends JPanel {

	protected MidiDevice midiDevice;

	public MidiDeviceProperties(MidiDevice midiDevice) {
		super(new GridBagLayout());
		this.midiDevice = midiDevice;

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.insets = new Insets(5, 5, 5, 5);
		gbc.fill = GridBagConstraints.BOTH;

		add(new JLabel("Name: "), gbc);
		gbc.gridx++;

		add(new JLabel(midiDevice.getDeviceInfo().getName()), gbc);
		gbc.gridx--;
		gbc.gridy++;

		add(new JLabel("Vendor: "), gbc);
		gbc.gridx++;

		add(new JLabel(midiDevice.getDeviceInfo().getVendor()), gbc);
		gbc.gridx--;
		gbc.gridy++;

		add(new JLabel("Version: "), gbc);
		gbc.gridx++;

		add(new JLabel(midiDevice.getDeviceInfo().getVersion()), gbc);
		gbc.gridx--;
		gbc.gridy++;

		add(new JLabel("Description: "), gbc);
		gbc.gridx++;

		add(new JLabel(midiDevice.getDeviceInfo().getDescription()), gbc);
	}
}
