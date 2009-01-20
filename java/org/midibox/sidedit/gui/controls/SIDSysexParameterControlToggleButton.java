/*
 * @(#)SIDV2librarian.java	beta1	2008/01/21
 *
 * Copyright (C) 2008    Rutger Vlek (rutgervlek@hotmail.com)
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

package org.midibox.sidedit.gui.controls;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;

import javax.swing.JPanel;
import javax.swing.JToggleButton;

import org.midibox.sidedit.SIDSysexParameterControl;

public class SIDSysexParameterControlToggleButton extends
		SIDSysexParameterControlGUI {

	private JToggleButton toggleButton;

	private boolean momentary = false;

	private SIDSysexParameterControlGUI snapGUI = null;

	public SIDSysexParameterControlToggleButton(
			SIDSysexParameterControl midiParameter, JToggleButton toggleButton,
			boolean momentary, boolean showLabel, String labelLocation,
			boolean valueBelow, boolean showValue) {
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

	public JToggleButton getToggleButton() {
		return toggleButton;
	}

	public void actionPerformed(ActionEvent ae) {

		super.actionPerformed(ae);

		if (ae.getSource() == toggleButton) {
			if (update) {
				update = false;
				midiParameter.setMidiValue((toggleButton.isSelected()) ? 1 : 0,
						true);
				for (int c = 0; c < midiParameters.size(); c++) {
					SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
							.elementAt(c);
					mp.setMidiValue((toggleButton.isSelected()) ? 1 : 0, false);
				}
				if (snapGUI != null) {
					snapGUI.setSnap(toggleButton.isSelected());
				}
				update = true;
			}
			if (momentary && toggleButton.isSelected()) {
				toggleButton.doClick();
			}
		}
	}

	public void setSnapParameter(SIDSysexParameterControlGUI snapGUI) {
		this.snapGUI = snapGUI;
	}

	public void updateGraphics() {
		super.updateGraphics();
		if (update) {
			update = false;
			toggleButton.setSelected((midiParameter.getMidiValue() != 0));
			if (snapGUI != null) {
				snapGUI.setSnap(toggleButton.isSelected());
			}
			update = true;
		}
	}
}