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
import java.awt.Dimension;
import javax.swing.*;

import org.midibox.sidedit.*;

import java.awt.Font;

public class SIDSysexParameterControlCombo extends SIDSysexParameterControlGUI {

	public static float mouseWheelResolution = 10.0f;
	private JComboBox combo;

	public SIDSysexParameterControlCombo(SIDSysexParameterControl midiParameter, JComboBox combo,
			boolean showLabel, String labelLocation, boolean valueBelow, boolean showValue) {
		super(midiParameter, showLabel, labelLocation, valueBelow, showValue);
		this.combo = combo;
		combo.setPreferredSize(new Dimension(110,20));
		combo.setFont(new Font("sansserif", Font.BOLD, 11));
		JPanel panel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
		panel.setOpaque(false);
		panel.add(combo);
		add(panel, BorderLayout.CENTER);
		combo.addActionListener(this);
		combo.addMouseListener(this);
		updateGraphics();		
	}
	
	public void actionPerformed(ActionEvent ae) {

		super.actionPerformed(ae);

		if (ae.getSource() == combo) {
			if (update) {
				update = false;
				int newval = midiParameter.lookUpAlias((String)combo.getSelectedItem());
				midiParameter.setMidiValue(newval, true);
				
				for (int c = 0; c < midiParameters.size(); c++) {
					SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters.elementAt(c);
					mp.setMidiValue(newval, false);
				}
				
				update = true;
			}
		}
	}
	
	public void updateGraphics() {
		super.updateGraphics();
		if (update) {
			update = false;
			combo.setSelectedIndex(midiParameter.lookUpValue(midiParameter.getMidiValue()));
			update = true;
		}
	}
}