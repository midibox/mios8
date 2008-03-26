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

import org.midibox.sidedit.SIDEditController;
import  org.midibox.sidedit.gui.controls.*;
public class OscillatorGUI extends JPanel{
	protected OscillatorGUI(Vector masterGUIv, Vector V1GUIv, Vector V2GUIv, Vector V3GUIv, Vector V4GUIv, Vector V5GUIv, Vector V6GUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);
		
		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		panel.setOpaque(false);
		
		JPanel panel1 = new JPanel();
		panel1.setLayout(new BoxLayout(panel1, BoxLayout.X_AXIS));
		panel1.setOpaque(false);
		
		JPanel panel2 = new JPanel();
		panel2.setLayout(new BoxLayout(panel2, BoxLayout.Y_AXIS));
		panel2.setOpaque(false);
		
		panel2.add(createVoice(V1GUIv,"Voice 1 (left)"));
		panel2.add(createVoice(V2GUIv,"Voice 2 (left)"));
		panel2.add(createVoice(V3GUIv,"Voice 3 (left)"));
		
		JPanel panel3 = new JPanel();
		panel3.setLayout(new BoxLayout(panel3, BoxLayout.Y_AXIS));
		panel3.setOpaque(false);
		
		panel3.add(createVoice(V4GUIv,"Voice 4 (right)"));
		panel3.add(createVoice(V5GUIv,"Voice 5 (right)"));
		panel3.add(createVoice(V6GUIv,"Voice 6 (right)"));
		
		panel1.add(panel2);
		panel1.add(panel3);
		
		JPanel globPanel = new JPanel();
		globPanel.setLayout(new BoxLayout(globPanel, BoxLayout.Y_AXIS));
		globPanel.setOpaque(false);
		
		panel.add(createGlob(masterGUIv,"Master settings"));		
		panel.add(panel1);
	    this.add(panel, BorderLayout.NORTH);
	}

	protected JPanel createGlob(Vector vGUI, String s) {			
		JPanel globPanel = new JPanel();
		globPanel.setLayout(new BoxLayout(globPanel, BoxLayout.X_AXIS));
		globPanel.setBorder(BorderFactory.createEtchedBorder());
		globPanel.setBorder(BorderFactory.createTitledBorder(s));
		globPanel.setOpaque(false);
		
		globPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(0));
		globPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(1));
		globPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(2));
		globPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(3));
		globPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(4));
		
		return globPanel;
	}
	
	protected JPanel createVoice(Vector vGUI, String s) {
		
		JPanel voicePanel = new JPanel();
		voicePanel.setLayout(new BoxLayout(voicePanel, BoxLayout.X_AXIS));
		voicePanel.setBorder(BorderFactory.createEtchedBorder());
		voicePanel.setBorder(BorderFactory.createTitledBorder(s));
		voicePanel.setOpaque(false);
		
		JPanel subPanel1 = new JPanel();
		subPanel1.setLayout(new BoxLayout(subPanel1, BoxLayout.Y_AXIS));
		subPanel1.setOpaque(false);
		
		JPanel subPanel2 = new JPanel();
		subPanel2.setLayout(new BoxLayout(subPanel2, BoxLayout.Y_AXIS));
		subPanel2.setOpaque(false);
		
		// Waveform selection
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(2));
		
		// Tuning Panel
		JPanel tunePanel = new JPanel();
		tunePanel.setLayout(new GridLayout(2,3));
		tunePanel.setOpaque(false);
		tunePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(10));
		tunePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(11));
		tunePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(12));
		tunePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(7));
		//tunePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(8));
		tunePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(13));
		subPanel2.add(tunePanel);
		
		// Portamento Panel
		JPanel portaPanel = new JPanel();
		portaPanel.setLayout(new BoxLayout(portaPanel, BoxLayout.X_AXIS));
		portaPanel.setOpaque(false);
		
		portaPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(1));
		portaPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(0));
		
		subPanel2.add(portaPanel);
		
		// Envelope Panel
		JPanel envPanel = new JPanel();
		envPanel.setLayout(new BoxLayout(envPanel, BoxLayout.X_AXIS));
		envPanel.setOpaque(false);
		envPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(9));
		envPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(3));
		envPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(4));
		envPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(5));
		envPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(6));
		subPanel1.add(envPanel);
				
		voicePanel.add(subPanel1);
		voicePanel.add(subPanel2);
		return voicePanel;
	}	
}
