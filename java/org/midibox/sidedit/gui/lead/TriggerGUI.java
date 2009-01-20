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
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.util.Vector;

import javax.swing.JLabel;
import javax.swing.JPanel;

import org.midibox.sidedit.SIDSysexInfo;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlGUI;
import org.midibox.utils.gui.JVertLabel;

public class TriggerGUI extends JPanel {
	protected TriggerGUI(Vector trigGUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);

		JPanel topanel = new JPanel();
		topanel.setLayout(new BorderLayout());
		topanel.setOpaque(false);

		JPanel panel1 = new JPanel();
		panel1.setLayout(new GridBagLayout());
		panel1.setOpaque(false);
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;

		for (int row = 0; row < 25; row++) {
			for (int col = 0; col < 15; col++) {
				if ((col == 0) && (row == 0)) {
					JLabel label = new JLabel();
					label.setOpaque(false);
					c.gridx = col;
					c.gridy = row;
					panel1.add(label, c);
				} else if (row == 0) {
					JLabel label = new JVertLabel(
							SIDSysexInfo.trigColumnNames[col - 1]);
					label.setFont(new Font("serif", Font.BOLD, 10));
					label.setOpaque(false);
					c.gridx = col;
					c.gridy = row;
					panel1.add(label, c);
				} else if (col == 0) {
					JLabel label = new JLabel(
							SIDSysexInfo.trigRowNames[row - 1]);
					label.setFont(new Font("serif", Font.BOLD, 10));
					label.setOpaque(false);
					label.setPreferredSize(new Dimension(70, 10));
					c.gridx = col;
					c.gridy = row;
					panel1.add(label, c);
				} else {
					c.gridx = col;
					c.gridy = row;
					panel1.add((SIDSysexParameterControlGUI) trigGUIv
							.elementAt((row - 1) + ((col - 1) * 24)), c);
				}
			}
		}

		topanel.add(panel1, BorderLayout.WEST);
		this.add(topanel, BorderLayout.NORTH);
	}
}