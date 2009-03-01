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
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.sidedit.gui.SequencerButton;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlGUI;

public class SequencerGUI extends JPanel implements ActionListener {
	private JPanel seqPanel, currentPanel, seq1, seq2, seq3, seq4, seq5, seq6,
			seq7, seq8;
	private JRadioButton radioButtons[];

	protected SequencerGUI(Vector V1GUIv, Vector V2GUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);

		JPanel mainPanel = new JPanel();
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
		mainPanel.setOpaque(false);

		seqPanel = new JPanel();
		seqPanel.setLayout(new BoxLayout(seqPanel, BoxLayout.X_AXIS));
		seqPanel.setBorder(BorderFactory.createEtchedBorder());
		seqPanel.setBorder(BorderFactory.createTitledBorder("Sequencer data"));
		seqPanel.setOpaque(false);

		JPanel comboPanel = new JPanel();
		comboPanel.setLayout(new BoxLayout(comboPanel, BoxLayout.Y_AXIS));
		comboPanel.setOpaque(false);
		String[] s = { "Sequence #1", "Sequence #2", "Sequence #3",
				"Sequence #4", "Sequence #5", "Sequence #6", "Sequence #7",
				"Sequence #8" };
		ButtonGroup bg = new ButtonGroup();
		radioButtons = new JRadioButton[s.length];
		for (int i = 0; i < s.length; i++) {
			radioButtons[i] = new JRadioButton(s[i], true);
			radioButtons[i].setOpaque(false);
			radioButtons[i].addActionListener(this);
			radioButtons[i].setFont(new Font("sansserif", Font.BOLD, 10));
			bg.add(radioButtons[i]);
			comboPanel.add(radioButtons[i]);
		}

		seqPanel.add(comboPanel);

		seq1 = createSeqData(V2GUIv, 0);
		seq2 = createSeqData(V2GUIv, 1);
		seq3 = createSeqData(V2GUIv, 2);
		seq4 = createSeqData(V2GUIv, 3);
		seq5 = createSeqData(V2GUIv, 4);
		seq6 = createSeqData(V2GUIv, 5);
		seq7 = createSeqData(V2GUIv, 6);
		seq8 = createSeqData(V2GUIv, 7);

		currentPanel = seq1;
		seqPanel.add(currentPanel);

		mainPanel.add(createConfig(V1GUIv, "Sequencer setup"));
		mainPanel.add(seqPanel);
		this.add(mainPanel, BorderLayout.NORTH);
	}

	protected JPanel createConfig(Vector vGUI, String s) {
		JPanel configPanel = new JPanel();
		configPanel.setLayout(new BoxLayout(configPanel, BoxLayout.X_AXIS));
		configPanel.setBorder(BorderFactory.createEtchedBorder());
		configPanel.setBorder(BorderFactory.createTitledBorder(s));
		configPanel.setOpaque(false);

		configPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(3));
		configPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(4));
		configPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(0));
		configPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(1));
		configPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(2));
		return configPanel;
	}

	protected JPanel createSeqData(Vector vGUI, int seqNr) {
		JPanel chanPanel = new JPanel();
		chanPanel.setLayout(new GridBagLayout());
		chanPanel.setOpaque(false);

		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.anchor = GridBagConstraints.CENTER;

		// Loop over all channels
		for (int chan = 0; chan < 8; chan++) {
			// Dummy panel to fill
			c.gridx = 0;
			c.gridy = (2 * chan) + 2;
			c.gridheight = 1;
			JPanel transp = new JPanel();
			transp.setOpaque(false);
			chanPanel.add(transp, c);

			// Row labels
			c.gridwidth = 2;
			if (chan == 0) {
				c.gridx = 0;
				c.gridy = (2 * chan);
				chanPanel.add(createSeqLabel("Step"), c);
			}
			c.gridx = 0;
			c.gridy = (2 * chan) + 1;
			chanPanel.add(createSeqLabel("Chan." + Integer.toString(chan + 1)),
					c);
			c.gridwidth = 1;

			int offset = (32 * chan) + (seqNr * 256);
			for (int col = 0; col < 16; col++) {
				// Column labels
				if (chan == 0) {
					JLabel label = new JLabel(Integer.toString(col + 1));
					label.setHorizontalAlignment(JLabel.CENTER);
					label.setFont(new Font("serif", Font.BOLD, 10));
					label.setOpaque(false);
					c.gridx = col + 2;
					c.gridy = (2 * chan);
					chanPanel.add(label, c);
				}

				// Buttons
				c.gridx = col + 2;
				c.gridy = 1 + (2 * chan);
				SIDSysexParameterControl mp1 = ((SIDSysexParameterControlGUI) vGUI
						.elementAt(0 + offset + col)).getMidiParameter();
				SIDSysexParameterControl mp2 = ((SIDSysexParameterControlGUI) vGUI
						.elementAt(0 + offset + col + 16)).getMidiParameter();
				chanPanel.add(new SequencerButton(mp1, mp2), c);
			}
		}
		return chanPanel;
	}

	public JLabel createSeqLabel(String s) {
		JLabel label = new JLabel(s);
		label.setVerticalAlignment(JLabel.CENTER);
		label.setFont(new Font("serif", Font.BOLD, 10));
		label.setOpaque(false);
		label.setPreferredSize(new Dimension(35, 10));
		return label;
	}

	public void actionPerformed(ActionEvent ae) {
		for (int i = 1; i < radioButtons.length; i++) {
			if ((ae.getSource() == radioButtons[i])
					&& (radioButtons[i].isSelected())) {
				seqPanel.remove(currentPanel);
				switch (i) {
				case 0:
					currentPanel = seq1;
					break;
				case 1:
					currentPanel = seq2;
					break;
				case 2:
					currentPanel = seq3;
					break;
				case 3:
					currentPanel = seq4;
					break;
				case 4:
					currentPanel = seq5;
					break;
				case 5:
					currentPanel = seq6;
					break;
				case 6:
					currentPanel = seq7;
					break;
				case 7:
					currentPanel = seq8;
					break;
				}
				seqPanel.add(currentPanel);
				this.repaint();
				break;
			}
		}
	}
}
