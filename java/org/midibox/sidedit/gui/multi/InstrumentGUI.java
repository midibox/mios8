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

package org.midibox.sidedit.gui.multi;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JToggleButton;

import org.midibox.sidedit.SIDEditController;
import  org.midibox.sidedit.gui.controls.*;
public class InstrumentGUI extends JPanel{
	private int[] snapvals1 = {0,1,2,3,4,5,6,7,8};
	private int[] snapvals = {245,246,247,248,249,250,251,252,253,254,255};
	
	protected InstrumentGUI(Vector V1GUIv, Vector V2GUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);
		
		JPanel panel = new JPanel();
		panel.setOpaque(false);
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		
		JPanel panel1 = new JPanel(new GridLayout(1,2));
		panel1.setOpaque(false);
		
		JPanel panel2 = new JPanel(new GridLayout(1,2));
		panel2.setOpaque(false);
		
		JPanel panel3 = new JPanel(new GridLayout(1,2));
		panel3.setOpaque(false);		
		
		panel.add(createGlob(V2GUIv,"Master settings"));	
		
		panel1.add(createVoice(V1GUIv,"Voice"));
		panel1.add(createENV(V2GUIv,"Envelope",27));	
		panel2.add(createLFO(V2GUIv,"LFO 1",7));	
		panel2.add(createLFO(V2GUIv,"LFO 2",17));
		panel3.add(createArp(V1GUIv,"Arpeggiator"));
		panel3.add(createWTE(V2GUIv,"Wavetable setup",39));
		panel.add(panel1);
		panel.add(panel2);
		panel.add(panel3);
		
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
		((SIDSysexParameterControlKnob) vGUI.elementAt(4)).setSnapvals(snapvals1);
		((SIDSysexParameterControlKnob) vGUI.elementAt(4)).setSnap(true);
		globPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(5));
		globPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(6));
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
		tunePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(8));
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
	
	protected JPanel createLFO(Vector midiParameterGUI, String s, int offset) {
		JPanel lfoPanel = new JPanel(new GridBagLayout());
		lfoPanel.setBorder(BorderFactory.createEtchedBorder());
		lfoPanel.setBorder(BorderFactory.createTitledBorder(s));
		lfoPanel.setOpaque(false);	
		GridBagConstraints c = new GridBagConstraints();		
		c.fill = GridBagConstraints.HORIZONTAL;
		c.ipadx = 5;
		
		// Buttons
		c.gridx = 0;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(0+offset), c);
		c.gridx = 0;c.gridy = 1;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(1+offset), c);
		c.gridx = 1;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(2+offset), c);
			
		// Waveform selector
		c.gridx = 2;c.gridy = 0;c.gridwidth=5;c.gridheight=2;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(3+offset), c);
		
		// Knobs
		c.gridx = 7;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(4+offset),c);
		c.gridx = 8;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(8+offset),c);
		c.gridx = 9;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(9+offset),c);		
		c.gridx = 7;c.gridy = 1;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(5+offset),c);
		c.gridx = 8;c.gridy = 1;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(6+offset),c);
		c.gridx = 9;c.gridy = 1;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(7+offset),c);
		
		// Setup the 'rate'-knob behavior to be depending on the 'master sync' switch
		((SIDSysexParameterControlKnob) midiParameterGUI.elementAt(5+offset)).setSnapvals(snapvals);
		((SIDSysexParameterControlToggleButton)midiParameterGUI.elementAt(1+offset)).setSnapParameter((SIDSysexParameterControlGUI)midiParameterGUI.elementAt(5+offset));
		
		return lfoPanel;
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
		c.gridx = 0;c.gridy = 0;c.gridwidth=1;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(14), c);
		c.gridx = 1;c.gridy = 0;c.gridwidth=2;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(15), c);
			
		// Second row
		c.gridx = 0;c.gridy = 1;c.gridwidth=1;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(20), c);
		c.gridx = 1;c.gridy = 1;c.gridwidth=1;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(23), c);
		c.gridx = 2;c.gridy = 1;c.gridwidth=1;
		subPanel1.add((SIDSysexParameterControlGUI) vGUI.elementAt(22), c);
		
		JPanel subPanel2 = new JPanel();
		subPanel2.setLayout(new GridLayout(2,3));
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
	
	protected JPanel createENV(Vector midiParameterGUI, String s, int offset) {
		JPanel envTopPanel = new JPanel();
		envTopPanel.setLayout(new BoxLayout(envTopPanel, BoxLayout.Y_AXIS));
		envTopPanel.setBorder(BorderFactory.createEtchedBorder());
		envTopPanel.setBorder(BorderFactory.createTitledBorder(s));
		envTopPanel.setOpaque(false);
		
		JPanel envPanel = new JPanel();
		envPanel.setLayout(new BoxLayout(envPanel, BoxLayout.X_AXIS));
		envPanel.setOpaque(false);	
		
		// SubPanel1
		JPanel subPanel1 = new JPanel();
		subPanel1.setLayout(new GridLayout(1,4));
		subPanel1.setOpaque(false);	
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(7+offset));
		((SIDSysexParameterControlSlider)midiParameterGUI.elementAt(7+offset)).setHeight(120);
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(8+offset));
		((SIDSysexParameterControlSlider)midiParameterGUI.elementAt(8+offset)).setHeight(120);
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(9+offset));
		((SIDSysexParameterControlSlider)midiParameterGUI.elementAt(9+offset)).setHeight(120);
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(10+offset));
		((SIDSysexParameterControlSlider)midiParameterGUI.elementAt(10+offset)).setHeight(120);
		
		envPanel.add(subPanel1);
		
		JPanel fill = new JPanel();
		fill.setOpaque(false);
		
		// SubPanel2
		JPanel subPanel2 = new JPanel();
		subPanel2.setLayout(new GridLayout(3,5));
		subPanel2.setOpaque(false);	
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(4+offset));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(5+offset));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(6+offset));
		subPanel2.add(fill);		
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(11+offset));		
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(3+offset));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(0+offset));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(1+offset));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(2+offset));
		envPanel.add(subPanel2);
		
		envTopPanel.add(envPanel);
		
		return envTopPanel;
	}	
	
	protected JPanel createWTE(Vector vGUI, String s, int offset) {		
		JPanel wtePanel = new JPanel();
		wtePanel.setLayout(new BoxLayout(wtePanel, BoxLayout.X_AXIS));
		wtePanel.setBorder(BorderFactory.createEtchedBorder());
		wtePanel.setBorder(BorderFactory.createTitledBorder(s));
		wtePanel.setOpaque(false);
		
		wtePanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(3+offset));
		
		JPanel subPanel = new JPanel(new GridLayout(2,4));
		subPanel.setOpaque(false);
		subPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(4+offset));
		subPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(6+offset));
		subPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(7+offset));
		subPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(0+offset));
		subPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(1+offset));
		subPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(2+offset));
		subPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(5+offset));
		subPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(8+offset));
		wtePanel.add(subPanel);
		
		return wtePanel;
	}
}
