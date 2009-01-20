/*
 * @(#)MidiParameterControlToggleButton.java	beta8	2006/04/23
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
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;

import javax.swing.JPanel;
import javax.swing.JToggleButton;

import org.midibox.midi.MidiParameterControl;

public class MidiParameterControlToggleButton extends MidiParameterControlGUI {

	private JToggleButton toggleButton;

	private boolean momentary = false;

	public MidiParameterControlToggleButton(MidiParameterControl midiParameter,
			JToggleButton toggleButton, boolean momentary, boolean showLabel,
			String labelLocation, boolean valueBelow, boolean showValue) {
		super(midiParameter, showLabel, labelLocation, valueBelow, showValue);
		this.toggleButton = toggleButton;
		this.momentary = momentary;
		JPanel panel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
		panel.setOpaque(false);
		panel.add(toggleButton);
		add(panel, BorderLayout.CENTER);
		toggleButton.addActionListener(this);
		toggleButton.addMouseListener(this);
		updateGraphics();
	}

	public void actionPerformed(ActionEvent ae) {

		super.actionPerformed(ae);

		if (ae.getSource() == toggleButton) {
			if (update) {
				update = false;
				midiParameter.setMidiValueOn(toggleButton.isSelected(), true);
				update = true;
			}
			if (momentary && toggleButton.isSelected()) {
				toggleButton.doClick();
			}
		}
	}

	public void updateGraphics() {
		super.updateGraphics();
		if (update) {
			update = false;
			toggleButton.setSelected(midiParameter.isMidiValueOn());
			update = true;
		}
	}
}