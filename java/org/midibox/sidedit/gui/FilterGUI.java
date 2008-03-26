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
package org.midibox.sidedit.gui;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.AbstractButton;
import  org.midibox.sidedit.gui.controls.*;
import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.Knob;
import org.midibox.utils.gui.MyButtonUI;

public class FilterGUI extends JPanel{
	
	public FilterGUI(Vector filterGUIvL, Vector filterGUIvR) {		
		setLayout(new BorderLayout());
		this.setOpaque(false);
			
		JPanel panel1 = new JPanel();
		panel1.setLayout(new GridLayout(1,5));
		panel1.add(createFilter(filterGUIvL,"Filter left"));
		panel1.add(createFilter(filterGUIvR,"Filter right"));
		JPanel panel2 = new JPanel();
		panel2.setOpaque(false);
		JPanel panel3 = new JPanel();
		panel3.setOpaque(false);
		JPanel panel4 = new JPanel();
		panel4.setOpaque(false);
		panel1.add(panel2);
		panel1.add(panel3);
		panel1.add(panel4);
		panel1.setOpaque(false);
	    this.add(panel1, BorderLayout.NORTH);
	}
	
	protected JPanel createFilter(Vector midiParameterGUI, String s) {
		JPanel filterPanel = new JPanel();
		filterPanel.setLayout(new GridLayout(4,3));
		filterPanel.setBorder(BorderFactory.createEtchedBorder());
		filterPanel.setBorder(BorderFactory.createTitledBorder(s));
		filterPanel.setOpaque(false);	
		
		JToggleButton b1 = ((JToggleButton)((SIDSysexParameterControlToggleButton)midiParameterGUI.elementAt(7)).getToggleButton());
		JToggleButton b2 = ((JToggleButton)((SIDSysexParameterControlToggleButton)midiParameterGUI.elementAt(8)).getToggleButton());
		JToggleButton b3 = ((JToggleButton)((SIDSysexParameterControlToggleButton)midiParameterGUI.elementAt(9)).getToggleButton());
		b1.setUI(new MyButtonUI(ImageLoader.getImageIcon("lpf_on.png"), ImageLoader.getImageIcon("lpf.png")));
		b2.setUI(new MyButtonUI(ImageLoader.getImageIcon("bpf_on.png"), ImageLoader.getImageIcon("bpf.png")));
		b3.setUI(new MyButtonUI(ImageLoader.getImageIcon("hpf_on.png"), ImageLoader.getImageIcon("hpf.png")));
		
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(7));
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(8));
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(9));	
		
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(0));
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(1));
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(2));
		
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(3));
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(4));
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(5));
		
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(6));	
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(10));
		filterPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(11));
		
		return filterPanel;
	}
}