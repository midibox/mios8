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

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.util.Vector;

import javax.swing.ButtonGroup;
import javax.swing.JPanel;
import javax.swing.JToggleButton;

import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.MyButtonUI;

public class SIDSysexParameterControlLFOSelect extends
		SIDSysexParameterControlGUI {
	private static String[] onFig = { "sin_on.png", "tri_on.png", "saw_on.png",
			"pulse_on.png", "random_on.png", "sin_pos_on.png",
			"tri_pos_on.png", "saw_pos_on.png", "pulse_pos_on.png" };
	private static String[] offFig = { "sin.png", "tri.png", "saw.png",
			"pulse.png", "random.png", "sin_pos.png", "tri_pos.png",
			"saw_pos.png", "pulse_pos.png" };
	private Vector buttonVec = new Vector();

	public SIDSysexParameterControlLFOSelect(
			SIDSysexParameterControl midiParameter, boolean showLabel,
			String labelLocation, boolean valueBelow, boolean showValue) {
		super(midiParameter, showLabel, labelLocation, valueBelow, showValue);

		JPanel panel = new JPanel();
		panel.setLayout(new GridLayout(2, 5));
		panel.setOpaque(false);

		ButtonGroup group = new ButtonGroup();
		for (int c = 0; c < 9; c++) {
			JToggleButton toggle = new JToggleButton();
			toggle.setUI(new MyButtonUI(ImageLoader.getImageIcon(onFig[c]),
					ImageLoader.getImageIcon(offFig[c])));
			toggle.addMouseListener(this);
			panel.add(toggle);
			group.add(toggle);
			buttonVec.add(toggle);
			toggle.addActionListener(this);
		}
		add(panel);
		updateGraphics();
	}

	public void actionPerformed(ActionEvent ae) {
		super.actionPerformed(ae);
		for (int c = 0; c < buttonVec.size(); c++) {
			JToggleButton toggle = (JToggleButton) buttonVec.elementAt(c);
			if (ae.getSource() == toggle) {
				if (update) {
					update = false;
					int newval = 0;
					if (toggle.isSelected()) {
						newval = c;
					}
					midiParameter.setMidiValue(newval, true);
					for (int d = 0; d < midiParameters.size(); d++) {
						SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
								.elementAt(d);
						mp.setMidiValue(newval, false);
					}
					update = true;
				}
			}
		}
	}

	public void updateGraphics() {
		super.updateGraphics();
		if (update) {
			update = false;
			((JToggleButton) buttonVec.elementAt(midiParameter.getMidiValue()))
					.setSelected(true);
			update = true;
		}
	}
}