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

package org.midibox.sidedit.gui.lead;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;

import org.midibox.sidedit.gui.controls.SIDSysexParameterControlGUI;

public class ArpGUI extends JPanel {
	protected ArpGUI(Vector V1GUIv, Vector V2GUIv, Vector V3GUIv,
			Vector V4GUIv, Vector V5GUIv, Vector V6GUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);

		JPanel panel1 = new JPanel();
		panel1.setLayout(new BoxLayout(panel1, BoxLayout.X_AXIS));
		panel1.setOpaque(false);

		JPanel panel2 = new JPanel();
		panel2.setLayout(new BoxLayout(panel2, BoxLayout.Y_AXIS));
		panel2.setOpaque(false);

		panel2.add(createArp(V1GUIv, "ARP (voice1 - L)"));
		panel2.add(createArp(V2GUIv, "ARP (voice2 - L)"));
		panel2.add(createArp(V3GUIv, "ARP (voice3 - L)"));

		JPanel panel3 = new JPanel();
		panel3.setLayout(new BoxLayout(panel3, BoxLayout.Y_AXIS));
		panel3.setOpaque(false);

		panel3.add(createArp(V4GUIv, "ARP (voice4 - R)"));
		panel3.add(createArp(V5GUIv, "ARP (voice5 - R)"));
		panel3.add(createArp(V6GUIv, "ARP (voice6 - R)"));

		panel1.add(panel2);
		panel1.add(panel3);

		this.add(panel1, BorderLayout.NORTH);
	}

	protected JPanel createArp(Vector vGUI, String s) {
		JPanel arpPanel = new JPanel();
		arpPanel.setLayout(new BoxLayout(arpPanel, BoxLayout.X_AXIS));
		arpPanel.setBorder(BorderFactory.createEtchedBorder());
		arpPanel.setBorder(BorderFactory.createTitledBorder(s));
		arpPanel.setOpaque(false);

		JPanel subPanel1 = new JPanel(new GridBagLayout());
		subPanel1.setOpaque(false);
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.ipadx = 10;
		// First row
		c.gridx = 0;
		c.gridy = 0;
		c.gridwidth = 1;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(14), c);
		c.gridx = 1;
		c.gridy = 0;
		c.gridwidth = 2;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(15), c);

		// Second row
		c.gridx = 0;
		c.gridy = 1;
		c.gridwidth = 1;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(20), c);
		c.gridx = 1;
		c.gridy = 1;
		c.gridwidth = 1;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(23), c);
		c.gridx = 2;
		c.gridy = 1;
		c.gridwidth = 1;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(22), c);

		JPanel subPanel2 = new JPanel();
		subPanel2.setLayout(new GridLayout(2, 3));
		subPanel2.setOpaque(false);

		subPanel2.add((SIDSysexParameterControlGUI) vGUI.elementAt(16));
		subPanel2.add((SIDSysexParameterControlGUI) vGUI.elementAt(17));
		subPanel2.add((SIDSysexParameterControlGUI) vGUI.elementAt(18));
		subPanel2.add((SIDSysexParameterControlGUI) vGUI.elementAt(21));
		subPanel2.add((SIDSysexParameterControlGUI) vGUI.elementAt(19));
		arpPanel.add(subPanel1);
		arpPanel.add(subPanel2);

		return arpPanel;
	}
}
