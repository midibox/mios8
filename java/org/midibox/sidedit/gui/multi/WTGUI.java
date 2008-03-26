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
public class WTGUI extends JPanel{
	protected WTGUI(Vector WTGUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);
		
		JPanel panel1 = new JPanel();
		panel1.setLayout(new BoxLayout(panel1, BoxLayout.Y_AXIS));
		panel1.setOpaque(false);
		
		panel1.add(createWTC(WTGUIv,"Wavetable"));
		
	    this.add(panel1, BorderLayout.NORTH);
	}
	
	
	protected JPanel createWTC(Vector vGUI, String s) {		
		JPanel wtcPanel = new JPanel();
		wtcPanel.setLayout(new GridLayout(3,43));
		wtcPanel.setBorder(BorderFactory.createEtchedBorder());
		wtcPanel.setBorder(BorderFactory.createTitledBorder(s));
		wtcPanel.setOpaque(false);
		for (int c = 0; c <128; c++) {		
			wtcPanel.add((SIDSysexParameterControlGUI) vGUI.elementAt(c));		
		}
		return wtcPanel;
	}
}
