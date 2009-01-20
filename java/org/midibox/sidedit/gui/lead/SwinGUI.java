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
import java.awt.GridLayout;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;

import org.midibox.sidedit.gui.controls.SIDSysexParameterControlGUI;

public class SwinGUI extends JPanel {
	protected SwinGUI(Vector V1GUIv, Vector V2GUIv, Vector V3GUIv,
			Vector V4GUIv, Vector V5GUIv, Vector V6GUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);

		JPanel panel1 = new JPanel();
		panel1.setLayout(new BoxLayout(panel1, BoxLayout.X_AXIS));
		panel1.setOpaque(false);

		JPanel panel2 = new JPanel();
		panel2.setLayout(new BoxLayout(panel2, BoxLayout.Y_AXIS));
		panel2.setOpaque(false);

		panel2.add(createSwin(V1GUIv, "SwinSID (voice1 - L)"));
		panel2.add(createSwin(V2GUIv, "SwinSID (voice2 - L)"));
		panel2.add(createSwin(V3GUIv, "SwinSID (voice3 - L)"));

		JPanel panel3 = new JPanel();
		panel3.setLayout(new BoxLayout(panel3, BoxLayout.Y_AXIS));
		panel3.setOpaque(false);

		panel3.add(createSwin(V4GUIv, "SwinSID (voice4 - R)"));
		panel3.add(createSwin(V5GUIv, "SwinSID (voice5 - R)"));
		panel3.add(createSwin(V6GUIv, "SwinSID (voice6 - R)"));

		panel1.add(panel2);
		panel1.add(panel3);

		this.add(panel1, BorderLayout.NORTH);
	}

	protected JPanel createSwin(Vector vGUI, String s) {

		JPanel swinPanel = new JPanel();
		swinPanel.setLayout(new GridLayout(1, 5));
		swinPanel.setBorder(BorderFactory.createEtchedBorder());
		swinPanel.setBorder(BorderFactory.createTitledBorder(s));
		swinPanel.setOpaque(false);

		swinPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(24));
		swinPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(28));
		swinPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(25));
		swinPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(27));
		swinPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(26));

		return swinPanel;
	}
}
