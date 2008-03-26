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
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.AbstractButton;

import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.Knob;
import org.midibox.utils.gui.MyButtonUI;
import  org.midibox.sidedit.gui.controls.*;
public class LfoGUI extends JPanel{
	private int[] snapvals = {245,246,247,248,249,250,251,252,253,254,255};
	
	protected LfoGUI(Vector lfoGUIv, Vector env1GUIv, Vector env2GUIv) {		
		setLayout(new BorderLayout());
		this.setOpaque(false);
		
		JPanel topPanel = new JPanel();
		topPanel.setLayout(new BoxLayout(topPanel, BoxLayout.Y_AXIS));
		topPanel.setOpaque(false);
		
		JPanel panel1 = new JPanel();
		panel1.setLayout(new GridLayout(3,2));
		panel1.add(createLFO(lfoGUIv,"LFO 1",0));
		panel1.add(createLFO(lfoGUIv,"LFO 4",27));
		panel1.add(createLFO(lfoGUIv,"LFO 2",9));
		panel1.add(createLFO(lfoGUIv,"LFO 5",36));
		panel1.add(createLFO(lfoGUIv,"LFO 3",18));
		panel1.add(createLFO(lfoGUIv,"LFO 6",45));		
		panel1.setOpaque(false);
		
		JPanel panel2 = new JPanel();
		panel2.setLayout(new GridLayout(1,2));
		panel2.add(createENV(env1GUIv,"Envelope 1"));
		panel2.add(createENV(env2GUIv,"Envelope 2"));
		panel2.setOpaque(false);
		
		topPanel.add(panel1);
		topPanel.add(panel2);
	    this.add(topPanel, BorderLayout.NORTH);
	}
	
	protected JPanel createLFO(Vector midiParameterGUI, String s, int offset) {
		JPanel lfoPanel = new JPanel(new GridBagLayout());
		lfoPanel.setBorder(BorderFactory.createEtchedBorder());
		lfoPanel.setBorder(BorderFactory.createTitledBorder(s));
		lfoPanel.setOpaque(false);	
		GridBagConstraints c = new GridBagConstraints();		
		c.fill = GridBagConstraints.HORIZONTAL;
		c.ipadx = 10;
		
		// Buttons
		c.gridx = 0;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(0+offset), c);
		c.gridx = 0;c.gridy = 1;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(1+offset), c);		
		c.gridx = 1;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(2+offset), c);
		c.gridx = 1;c.gridy = 1;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(8+offset),c);
		
		// Waveform selector
		c.gridx = 2;c.gridy = 0;c.gridwidth=5;c.gridheight=2;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(3+offset), c);
		
		// Knobs
		c.gridx = 7;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(4+offset),c);
		c.gridx = 8;c.gridy = 0;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(5+offset),c);
		c.gridx = 7;c.gridy = 1;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(6+offset),c);
		c.gridx = 8;c.gridy = 1;c.gridwidth=1;c.gridheight=1;
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(7+offset),c);
		
		// Setup the 'rate'-knob behavior to be depending on the 'master sync' switch
		((SIDSysexParameterControlKnob) midiParameterGUI.elementAt(5+offset)).setSnapvals(snapvals);
		((SIDSysexParameterControlToggleButton)midiParameterGUI.elementAt(1+offset)).setSnapParameter((SIDSysexParameterControlGUI)midiParameterGUI.elementAt(5+offset));
	
		return lfoPanel;
	}
	/*
	protected JPanel createLFO(Vector midiParameterGUI, String s, int offset) {
		JPanel lfoPanel = new JPanel();
		lfoPanel.setLayout(new BoxLayout(lfoPanel, BoxLayout.X_AXIS));
		lfoPanel.setBorder(BorderFactory.createEtchedBorder());
		lfoPanel.setBorder(BorderFactory.createTitledBorder(s));
		lfoPanel.setOpaque(false);	
		
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(0+offset));
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(3+offset));
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(4+offset));
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(5+offset));
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(6+offset));
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(7+offset));
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(1+offset));
		
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(2+offset));
		lfoPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(8+offset));
		
		// Setup the 'rate'-knob behavior to be depending on the 'master sync' switch
		((SIDSysexParameterControlKnob) midiParameterGUI.elementAt(5+offset)).setSnapvals(snapvals);
		((SIDSysexParameterControlToggleButton)midiParameterGUI.elementAt(1+offset)).setSnapParameter((SIDSysexParameterControlGUI)midiParameterGUI.elementAt(5+offset));
		
		return lfoPanel;
	}
	*/
	protected JPanel createENV(Vector midiParameterGUI, String s) {
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
		subPanel1.setLayout(new GridLayout(3,2));
		subPanel1.setOpaque(false);	
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(3));
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(5));
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(4));
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(7));
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(2));
		subPanel1.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(15));
		envPanel.add(subPanel1);
		
		envPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(6));
		((SIDSysexParameterControlSlider)midiParameterGUI.elementAt(6)).setHeight(120);
		
		// SubPanel2
		JPanel subPanel2 = new JPanel();
		subPanel2.setLayout(new GridLayout(3,1));
		subPanel2.setOpaque(false);	
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(8));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(10));
		subPanel2.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(16));
		envPanel.add(subPanel2);
		
		envPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(9));
		((SIDSysexParameterControlSlider)midiParameterGUI.elementAt(9)).setHeight(120);
		
		envPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(11));
		((SIDSysexParameterControlSlider)midiParameterGUI.elementAt(11)).setHeight(120);
		
		// SubPanel4
		JPanel subPanel4 = new JPanel();
		subPanel4.setLayout(new GridLayout(3,1));
		subPanel4.setOpaque(false);	
		subPanel4.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(12));
		subPanel4.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(14));
		subPanel4.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(17));
		envPanel.add(subPanel4);
		
		envPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(13));
		((SIDSysexParameterControlSlider)midiParameterGUI.elementAt(13)).setHeight(120);
		
		envTopPanel.add(envPanel);
		
		// SubPanel5
		JPanel subPanel5 = new JPanel();
		subPanel5.setLayout(new BoxLayout(subPanel5, BoxLayout.X_AXIS));
		subPanel5.setOpaque(false);	
		
		subPanel5.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(0));
		subPanel5.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(1));
		
		envTopPanel.add(subPanel5);
		
		return envTopPanel;
	}
}	