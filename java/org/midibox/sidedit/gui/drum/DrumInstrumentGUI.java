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

import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.SIDSysexInfo;
import org.midibox.sidedit.SIDSysexParameterControl;
import  org.midibox.sidedit.gui.controls.*;
public class DrumInstrumentGUI extends JPanel{
	private int[] snapvals1 = {0,1,2,3,4,5,6,7,8};
	private int[] snapvals2 = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
	
	protected DrumInstrumentGUI(Vector V1GUIv, boolean second) {
		setLayout(new BorderLayout());
		this.setOpaque(false);
		
		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		panel.setOpaque(false);
		
		JPanel panel1 = new JPanel();
		panel1.setLayout(new GridLayout(4,2));
		panel1.setOpaque(false);
		
		if (!second) {
			panel1.add(createVoice(V1GUIv,"Drum instrument 1", 0));
			panel1.add(createVoice(V1GUIv,"Drum instrument 2", 11));
			panel1.add(createVoice(V1GUIv,"Drum instrument 3", 22));
			panel1.add(createVoice(V1GUIv,"Drum instrument 4", 33));
			panel1.add(createVoice(V1GUIv,"Drum instrument 5", 44));
			panel1.add(createVoice(V1GUIv,"Drum instrument 6", 55));
			panel1.add(createVoice(V1GUIv,"Drum instrument 7", 66));
			panel1.add(createVoice(V1GUIv,"Drum instrument 8", 77));
		} else {
			panel1.add(createVoice(V1GUIv,"Drum instrument 9", 88));
			panel1.add(createVoice(V1GUIv,"Drum instrument 10", 99));
			panel1.add(createVoice(V1GUIv,"Drum instrument 11", 110));
			panel1.add(createVoice(V1GUIv,"Drum instrument 12", 121));
			panel1.add(createVoice(V1GUIv,"Drum instrument 13", 132));
			panel1.add(createVoice(V1GUIv,"Drum instrument 14", 143));
			panel1.add(createVoice(V1GUIv,"Drum instrument 15", 154));
			panel1.add(createVoice(V1GUIv,"Drum instrument 16", 165));
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
		c.gridx = 0;c.gridy = 0;c.gridwidth = 1;c.gridheight = 1;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(0+offset),c);
		((SIDSysexParameterControlKnob) vGUI.elementAt(0+offset)).setSnapvals(snapvals1);
		((SIDSysexParameterControlKnob) vGUI.elementAt(0+offset)).setSnap(true);
		c.gridx = 1;c.gridy = 0;c.gridwidth = 1;c.gridheight = 1;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(1+offset),c);
		((SIDSysexParameterControlKnob) vGUI.elementAt(1+offset)).setSnapvals(snapvals2);
		((SIDSysexParameterControlKnob) vGUI.elementAt(1+offset)).setSnap(true);
		c.gridx = 2;c.gridy = 0;c.gridwidth = 2;c.gridheight = 1;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(10+offset),c);
					
		// Knobs
		c.gridx = 0;c.gridy = 1;c.gridwidth = 1;c.gridheight = 1;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(6+offset),c);
		c.gridx = 1;c.gridy = 1;c.gridwidth = 1;c.gridheight = 1;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(7+offset),c);
		c.gridx = 2;c.gridy = 1;c.gridwidth = 1;c.gridheight = 1;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(8+offset),c);
		c.gridx = 3;c.gridy = 1;c.gridwidth = 1;c.gridheight = 1;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(9+offset),c);
		
		// ADSR
		c.gridx = 4;c.gridy = 0;c.gridwidth = 1;c.gridheight = 2;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(2+offset),c);
		c.gridx = 5;c.gridy = 0;c.gridwidth = 1;c.gridheight = 2;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(3+offset),c);
		c.gridx = 6;c.gridy = 0;c.gridwidth = 1;c.gridheight = 2;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(4+offset),c);
		c.gridx = 7;c.gridy = 0;c.gridwidth = 1;c.gridheight = 2;
		voicePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(5+offset),c);
		
		return voicePanel;
	}	
}

