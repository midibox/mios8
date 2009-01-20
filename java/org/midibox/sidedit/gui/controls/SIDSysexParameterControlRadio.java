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
import java.awt.event.ActionEvent;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import org.midibox.sidedit.SIDSysexParameterControl;

public class SIDSysexParameterControlRadio extends SIDSysexParameterControlGUI {

	public static float mouseWheelResolution = 10.0f;
	private JRadioButton[] radioButtons;

	public SIDSysexParameterControlRadio(
			SIDSysexParameterControl midiParameter, boolean showLabel,
			String labelLocation, boolean valueBelow, boolean showValue) {
		super(midiParameter, showLabel, labelLocation, valueBelow, showValue);

		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		panel.setOpaque(false);
		ButtonGroup bg = new ButtonGroup();
		String[] str = midiParameter.getSparseValAlias();
		radioButtons = new JRadioButton[str.length];
		for (int i = 0; i < str.length; i++) {
			radioButtons[i] = new JRadioButton(str[i], false);
			radioButtons[i].addActionListener(this);
			radioButtons[i].setOpaque(false);
			bg.add(radioButtons[i]);
			panel.add(radioButtons[i]);
		}
		updateGraphics();
		add(panel, BorderLayout.CENTER);
		updateGraphics();
	}

	public void actionPerformed(ActionEvent ae) {
		super.actionPerformed(ae);
		for (int i = 0; i < radioButtons.length; i++) {
			if ((ae.getSource() == radioButtons[i])
					&& (radioButtons[i].isSelected())) {
				if (update) {
					update = false;
					int newval = midiParameter
							.lookUpAlias((String) radioButtons[i].getText());
					midiParameter.setMidiValue(newval, true);
					for (int c = 0; c < midiParameters.size(); c++) {
						SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
								.elementAt(c);
						mp.setMidiValue(newval, false);
					}
					update = true;
					break;
				}
			}
		}
	}

	public void updateGraphics() {
		super.updateGraphics();
		if (update) {
			update = false;
			for (int i = 0; i < radioButtons.length; i++) {
				if (i == midiParameter
						.lookUpValue(midiParameter.getMidiValue())) {
					radioButtons[i].setSelected(true);
				} else {
					radioButtons[i].setSelected(false);
				}
			}
			update = true;
		}
	}
}