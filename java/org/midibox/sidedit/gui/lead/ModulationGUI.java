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

import org.midibox.sidedit.SIDSysexInfo;
import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.SIDSysexParameterControl;
import  org.midibox.sidedit.gui.controls.*;
public class ModulationGUI extends JPanel{
	protected ModulationGUI(Vector modGUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);
		
		JPanel panel1 = new JPanel();
		panel1.setLayout(new GridLayout(1,8));
		panel1.add(createMod(modGUIv,"Modulation path 1", 0));
		panel1.add(createMod(modGUIv,"Modulation path 2", 24));
		panel1.add(createMod(modGUIv,"Modulation path 3", 48));
		panel1.add(createMod(modGUIv,"Modulation path 4", 72));
		panel1.add(createMod(modGUIv,"Modulation path 5", 96));
		panel1.add(createMod(modGUIv,"Modulation path 6", 120));
		panel1.add(createMod(modGUIv,"Modulation path 7", 144));
		panel1.add(createMod(modGUIv,"Modulation path 8", 168));
		
		panel1.setOpaque(false);
	    this.add(panel1, BorderLayout.NORTH);
	}
	
	protected JPanel createMod(Vector midiParameterGUI, String s, int offset) {
		JPanel topPanel = new JPanel();
		topPanel.setLayout(new BoxLayout(topPanel, BoxLayout.Y_AXIS));
		topPanel.setBorder(BorderFactory.createEtchedBorder());
		topPanel.setBorder(BorderFactory.createTitledBorder(s));
		topPanel.setOpaque(false);
		
		JPanel modPanel = new JPanel();
		modPanel.setLayout(new BoxLayout(modPanel, BoxLayout.Y_AXIS));
		modPanel.setOpaque(false);	
		
		modPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(0+offset));		
		modPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(1+offset));	
		modPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(2+offset));
		modPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(6+offset));
		modPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(7+offset));
		modPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(5+offset));
		
		JPanel sub1Panel = new JPanel();
		sub1Panel.setLayout(new GridLayout(1,2));
		sub1Panel.setOpaque(false);
		sub1Panel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(3+offset));
		sub1Panel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(4+offset));
		
		
		JPanel subPanel = new JPanel();
		subPanel.setLayout(new GridLayout(8,2));
		subPanel.setBorder(BorderFactory.createEtchedBorder());
		subPanel.setBorder(BorderFactory.createTitledBorder("Direct assign to:"));
		subPanel.setOpaque(false);
		
		for (int c = 8; c < 16; c++) {
			subPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(c+offset));
			subPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(c+offset+8));
		}
		
		topPanel.add(modPanel);
		topPanel.add(sub1Panel);
		topPanel.add(subPanel);
		return topPanel;
	}
}