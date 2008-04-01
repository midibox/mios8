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
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;

import javax.swing.AbstractButton;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JToggleButton;

import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.Knob;
import org.midibox.utils.gui.MyButtonUI;
import  org.midibox.sidedit.gui.controls.*;
import  org.midibox.sidedit.gui.WTTable;

public class WTGUI extends JPanel{
	protected WTGUI(Vector WTGUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);
		
		JPanel panel1 = new JPanel();
		panel1.setLayout(new BoxLayout(panel1, BoxLayout.X_AXIS));
		panel1.setOpaque(false);
		
		JPanel panel2 = new JPanel();
		panel2.setLayout(new GridLayout(4,1));
		panel2.setOpaque(false);
		
		panel2.add(createWTE(WTGUIv,"Wavetable 1",0));
		panel2.add(createWTE(WTGUIv,"Wavetable 2",9));
		panel2.add(createWTE(WTGUIv,"Wavetable 3",18));
		panel2.add(createWTE(WTGUIv,"Wavetable 4",27));		
		
		panel1.add(panel2);
		//panel1.add(createWTC(WTGUIv,"Wavetable"));
		panel1.add(createWTC2(WTGUIv,"Wavetable data"));
	    this.add(panel1, BorderLayout.NORTH);
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

	protected JPanel createWTC(Vector vGUI, String s) {		
		JPanel wtcPanel = new JPanel();
		wtcPanel.setLayout(new GridLayout(3,43));
		wtcPanel.setBorder(BorderFactory.createEtchedBorder());
		wtcPanel.setBorder(BorderFactory.createTitledBorder(s));
		wtcPanel.setOpaque(false);
		for (int c = 0; c <128; c++) {		
			wtcPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(36+c));			
		}
		return wtcPanel;
	}
	
	protected JPanel createWTC2(Vector vGUI, String s) {		
		JPanel wtcPanel = new JPanel();
		wtcPanel.setBorder(BorderFactory.createEtchedBorder());
		wtcPanel.setBorder(BorderFactory.createTitledBorder(s));
		wtcPanel.setOpaque(false);	
		
		Vector v = new Vector();
		for (int c = 0; c <128; c++) {		
			v.add(((SIDSysexParameterControlGUI)vGUI.elementAt(36+c)).getMidiParameter());			
		}
		
		// Setup the parameters that control begin, end and loop
		SIDSysexParameterControl[][] config = new SIDSysexParameterControl[4][3];
		config[0][0] = (((SIDSysexParameterControlGUI)vGUI.elementAt(4)).getMidiParameter());			
		config[0][1] = (((SIDSysexParameterControlGUI)vGUI.elementAt(6)).getMidiParameter());
		config[0][2] = (((SIDSysexParameterControlGUI)vGUI.elementAt(7)).getMidiParameter());
		config[1][0] = (((SIDSysexParameterControlGUI)vGUI.elementAt(4+9)).getMidiParameter());			
		config[1][1] = (((SIDSysexParameterControlGUI)vGUI.elementAt(6+9)).getMidiParameter());
		config[1][2] = (((SIDSysexParameterControlGUI)vGUI.elementAt(7+9)).getMidiParameter());
		config[2][0] = (((SIDSysexParameterControlGUI)vGUI.elementAt(4+18)).getMidiParameter());			
		config[2][1] = (((SIDSysexParameterControlGUI)vGUI.elementAt(6+18)).getMidiParameter());
		config[2][2] = (((SIDSysexParameterControlGUI)vGUI.elementAt(7+18)).getMidiParameter());
		config[3][0] = (((SIDSysexParameterControlGUI)vGUI.elementAt(4+27)).getMidiParameter());			
		config[3][1] = (((SIDSysexParameterControlGUI)vGUI.elementAt(6+27)).getMidiParameter());
		config[3][2] = (((SIDSysexParameterControlGUI)vGUI.elementAt(7+27)).getMidiParameter());
		
		wtcPanel.add(new WTTable(4, v, config));
		return wtcPanel;
	}
}
