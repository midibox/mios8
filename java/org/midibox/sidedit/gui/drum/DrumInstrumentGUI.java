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

package org.midibox.sidedit.gui.drum;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;

import org.midibox.sidedit.SIDSysexInfo;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlGUI;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlKnob;

public class DrumInstrumentGUI extends JPanel {
	protected DrumInstrumentGUI(Vector V1GUIv, boolean second) {
		setLayout(new BorderLayout());
		this.setOpaque(false);

		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		panel.setOpaque(false);

		JPanel panel1 = new JPanel();
		panel1.setLayout(new GridLayout(4, 2));
		panel1.setOpaque(false);

		if (!second) {
			panel1.add(createVoice(V1GUIv, "Drum 1 (ch1 primary)", 0));
			panel1.add(createVoice(V1GUIv, "Drum 2 (ch1 seconday)", 11));
			panel1.add(createVoice(V1GUIv, "Drum 3 (ch2 primary)", 22));
			panel1.add(createVoice(V1GUIv, "Drum 4 (ch2 seconday)", 33));
			panel1.add(createVoice(V1GUIv, "Drum 5 (ch3 primary)", 44));
			panel1.add(createVoice(V1GUIv, "Drum 6 (ch3 seconday)", 55));
			panel1.add(createVoice(V1GUIv, "Drum 7 (ch4 primary)", 66));
			panel1.add(createVoice(V1GUIv, "Drum 8 (ch4 seconday)", 77));
		} else {
			panel1.add(createVoice(V1GUIv, "Drum 9 (ch5 primary)", 88));
			panel1.add(createVoice(V1GUIv, "Drum 10 (ch5 seconday)", 99));
			panel1.add(createVoice(V1GUIv, "Drum 11 (ch6 primary)", 110));
			panel1.add(createVoice(V1GUIv, "Drum 12 (ch6 seconday)", 121));
			panel1.add(createVoice(V1GUIv, "Drum 13 (ch7 primary)", 132));
			panel1.add(createVoice(V1GUIv, "Drum 14 (ch7 seconday)", 143));
			panel1.add(createVoice(V1GUIv, "Drum 15 (ch8 primary)", 154));
			panel1.add(createVoice(V1GUIv, "Drum 16 (ch8 seconday)", 165));
		}
		panel.add(panel1);
		this.add(panel, BorderLayout.NORTH);
	}

	protected JPanel createVoice(Vector vGUI, String s, int offset) {
		JPanel voicePanel = new JPanel();
		voicePanel.setLayout(new GridBagLayout());
		voicePanel.setBorder(BorderFactory.createEtchedBorder());
		voicePanel.setBorder(BorderFactory.createTitledBorder(s));
		voicePanel.setOpaque(false);

		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.ipadx = 5;
		c.ipady = 5;

		// Voice assign
		c.gridx = 0;
		c.gridy = 0;
		c.gridwidth = 1;
		c.gridheight = 1;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(0 + offset), c);
		((SIDSysexParameterControlKnob) vGUI.elementAt(0 + offset))
				.setSnapVals(new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8 },
						SIDSysexInfo.voiceAssign);
		((SIDSysexParameterControlKnob) vGUI.elementAt(0 + offset))
				.setSnap(true);
		c.gridx = 1;
		c.gridy = 0;
		c.gridwidth = 1;
		c.gridheight = 1;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(1 + offset), c);
		((SIDSysexParameterControlKnob) vGUI.elementAt(1 + offset))
				.setSnapVals(new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
						12, 13, 14, 15, 16, 17, 18, 19 },
						SIDSysexInfo.drumModel);
		((SIDSysexParameterControlKnob) vGUI.elementAt(1 + offset))
				.setSnap(true);
		c.gridx = 2;
		c.gridy = 0;
		c.gridwidth = 2;
		c.gridheight = 1;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI
				.elementAt(10 + offset), c);

		// Knobs
		c.gridx = 0;
		c.gridy = 1;
		c.gridwidth = 1;
		c.gridheight = 1;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(6 + offset), c);
		c.gridx = 1;
		c.gridy = 1;
		c.gridwidth = 1;
		c.gridheight = 1;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(7 + offset), c);
		c.gridx = 2;
		c.gridy = 1;
		c.gridwidth = 1;
		c.gridheight = 1;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(8 + offset), c);
		c.gridx = 3;
		c.gridy = 1;
		c.gridwidth = 1;
		c.gridheight = 1;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(9 + offset), c);

		// ADSR
		c.gridx = 4;
		c.gridy = 0;
		c.gridwidth = 1;
		c.gridheight = 2;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(2 + offset), c);
		c.gridx = 5;
		c.gridy = 0;
		c.gridwidth = 1;
		c.gridheight = 2;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(3 + offset), c);
		c.gridx = 6;
		c.gridy = 0;
		c.gridwidth = 1;
		c.gridheight = 2;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(4 + offset), c);
		c.gridx = 7;
		c.gridy = 0;
		c.gridwidth = 1;
		c.gridheight = 2;
		voicePanel.add(
				(SIDSysexParameterControlGUI) vGUI.elementAt(5 + offset), c);

		return voicePanel;
	}
}
