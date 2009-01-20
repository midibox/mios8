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

package org.midibox.sidedit.gui;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;

import org.midibox.sidedit.gui.controls.SIDSysexParameterControlGUI;

public class GlobalGUI extends JPanel {

	public GlobalGUI(Vector globalGUIv, Vector extGUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);

		JPanel panel1 = new JPanel();
		panel1.setLayout(new BoxLayout(panel1, BoxLayout.Y_AXIS));

		panel1.add(createHardware(globalGUIv));
		panel1.add(createKnobs(globalGUIv));
		panel1.add(createExt(extGUIv));

		panel1.setOpaque(false);
		this.add(panel1, BorderLayout.NORTH);
	}

	protected JPanel createHardware(Vector midiParameterGUI) {
		JPanel hardwarePanel = new JPanel();
		hardwarePanel.setLayout(new BoxLayout(hardwarePanel, BoxLayout.X_AXIS));
		hardwarePanel.setOpaque(false);

		JPanel subPanel2 = new JPanel();
		subPanel2.setLayout(new BoxLayout(subPanel2, BoxLayout.X_AXIS));
		subPanel2.setBorder(BorderFactory.createEtchedBorder());
		subPanel2.setBorder(BorderFactory
				.createTitledBorder("Hardware Information Flags"));
		subPanel2.setOpaque(false);

		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(0));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(1));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(2));

		JPanel subPanel3 = new JPanel();
		subPanel3.setLayout(new BoxLayout(subPanel3, BoxLayout.Y_AXIS));
		subPanel3.setBorder(BorderFactory.createEtchedBorder());
		subPanel3.setBorder(BorderFactory
				.createTitledBorder("ADSR Bug Workaround"));
		subPanel3.setOpaque(false);
		subPanel3.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(3));

		JPanel subPanel4 = new JPanel();
		subPanel4.setLayout(new BoxLayout(subPanel4, BoxLayout.Y_AXIS));
		subPanel4.setBorder(BorderFactory.createEtchedBorder());
		subPanel4.setBorder(BorderFactory.createTitledBorder("Master Volume"));
		subPanel4.setOpaque(false);
		subPanel4.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(4));

		hardwarePanel.add(subPanel2);
		hardwarePanel.add(subPanel3);
		hardwarePanel.add(subPanel4);
		return hardwarePanel;
	}

	protected JPanel createExt(Vector midiParameterGUI) {
		JPanel extPanel = new JPanel();
		extPanel.setLayout(new GridLayout(2, 8));
		extPanel.setBorder(BorderFactory.createEtchedBorder());
		extPanel.setBorder(BorderFactory
				.createTitledBorder("External Switch / CV"));
		extPanel.setOpaque(false);

		for (int c = 0; c < midiParameterGUI.size(); c++) {
			extPanel.add((SIDSysexParameterControlGUI) midiParameterGUI
					.elementAt(c));
		}
		return extPanel;
	}

	protected JPanel createKnobs(Vector midiParameterGUI) {
		JPanel knobsPanel = new JPanel(new GridLayout(1, 8));
		knobsPanel.setOpaque(false);
		knobsPanel.add(createKnob(midiParameterGUI, 0, "Modulation Wheel"));
		knobsPanel.add(createKnob(midiParameterGUI, 5, "CC#16"));
		knobsPanel.add(createKnob(midiParameterGUI, 10, "CC#17"));
		knobsPanel.add(createKnob(midiParameterGUI, 15, "CC#18"));
		knobsPanel.add(createKnob(midiParameterGUI, 20, "CC#19"));
		knobsPanel.add(createKnob(midiParameterGUI, 25, "Velocity"));
		knobsPanel.add(createKnob(midiParameterGUI, 30, "Pitch Bender"));
		knobsPanel.add(createKnob(midiParameterGUI, 35, "Aftertouch"));
		return knobsPanel;
	}

	protected JPanel createKnob(Vector midiParameterGUI, int offset, String s) {
		JPanel knobPanel = new JPanel();
		knobPanel.setLayout(new BoxLayout(knobPanel, BoxLayout.Y_AXIS));
		knobPanel.setBorder(BorderFactory.createEtchedBorder());
		knobPanel.setBorder(BorderFactory.createTitledBorder(s));
		knobPanel.setOpaque(false);

		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(5 + offset));
		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(6 + offset));
		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(7 + offset));
		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(8 + offset));
		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI
				.elementAt(9 + offset));

		return knobPanel;
	}
}
