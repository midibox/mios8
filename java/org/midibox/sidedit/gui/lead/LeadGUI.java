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
import javax.swing.JComboBox;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JToggleButton;
import org.midibox.sidedit.gui.MBSIDV2EditorGUI;
import org.midibox.sidedit.SIDSysexInfo;
import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.SIDSysexParameterControl;

import org.midibox.sidedit.gui.controls.*;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.Knob;
import org.midibox.utils.gui.MyButtonUI;
import org.midibox.sidedit.gui.GlobalGUI;
import org.midibox.sidedit.gui.FilterGUI;

public class LeadGUI extends JPanel {
	private boolean linked = false;
	private Vector GUIs, EXT_L, EXT_R, MP_L, MP_R;
	
	public LeadGUI(SIDEditController sidEditController) {		
		setLayout(new BorderLayout());
		
		GUIs = createGUIs(sidEditController);
		createLinkGroups();
		permanentLink();
		JTabbedPane tabbedPane = new JTabbedPane();		
		JPanel t1 = new GlobalGUI((Vector)GUIs.elementAt(0),(Vector)GUIs.elementAt(1));
		JPanel t2 = new OscillatorGUI((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(7),(Vector)GUIs.elementAt(8),(Vector)GUIs.elementAt(9),(Vector)GUIs.elementAt(10));
		JPanel t3 = new FilterGUI((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));
		JPanel t4 = new LfoGUI((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(13));
		JPanel t6 = new ModulationGUI((Vector)GUIs.elementAt(14));
		JPanel t7 = new TriggerGUI((Vector)GUIs.elementAt(15));
		JPanel t8 = new ArpGUI((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(7),(Vector)GUIs.elementAt(8),(Vector)GUIs.elementAt(9),(Vector)GUIs.elementAt(10));
		JPanel t9 =  new WTGUI((Vector)GUIs.elementAt(16));
		JPanel t11 = new SwinGUI((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(7),(Vector)GUIs.elementAt(8),(Vector)GUIs.elementAt(9),(Vector)GUIs.elementAt(10));
		
		tabbedPane.addTab("Global", t1);
		tabbedPane.addTab("Oscillator", t2);
		tabbedPane.addTab("Filter", t3);
		tabbedPane.addTab("LFO/ENV", t4);
		tabbedPane.addTab("Modulation", t6);
		tabbedPane.addTab("Trigger", t7);
		tabbedPane.addTab("Arpeggiator", t8);
		tabbedPane.addTab("Wavetable", t9);
		tabbedPane.addTab("SwinSID", t11);
			
		add(tabbedPane, BorderLayout.NORTH);
		setOpaque(false);
		setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));
		link();
	}
	
	protected Vector createGUIs(SIDEditController sidEditController) {
		Vector v_in = (Vector)sidEditController.getEngine();
		Vector v_out = new Vector();
		for (int b = 0; b < v_in.size(); b++) {
			v_out.addElement(createGuiElements((Vector)v_in.elementAt(b)));			
		}
		return v_out;
	}
		
	protected Vector createGuiElements(Vector v) {
		// Create controller GUI's
		Vector midiParameterGUIs = new Vector();
		for (int c = 0; c < v.size(); c++) {
			SIDSysexParameterControl midiParameter = (SIDSysexParameterControl) v.elementAt(c);
			switch (midiParameter.getType()) {
				case 0:
					// Create knob
					Knob knob = new Knob(ImageLoader.getImageIcon("plasticknob_strip.png"));		
					midiParameterGUIs.add(new SIDSysexParameterControlKnob(midiParameter, knob, true, BorderLayout.SOUTH, true, true));
					break;
				case 1:
					// Create toggle button
					JToggleButton button = new JToggleButton();
					button.setUI(new MyButtonUI(ImageLoader.getImageIcon("button_on.png"), ImageLoader.getImageIcon("button.png")));
					midiParameterGUIs.add(new SIDSysexParameterControlToggleButton(midiParameter, button, false, true, BorderLayout.SOUTH, true, false));
					break;
				case 2:
					// Create simple toggle button
					JToggleButton simpleButton = new JToggleButton();
					simpleButton.setUI(new MyButtonUI(ImageLoader.getImageIcon("button_on.png"), ImageLoader.getImageIcon("button.png")));
					midiParameterGUIs.add(new SIDSysexParameterControlToggleButton(midiParameter, simpleButton, false, false, BorderLayout.SOUTH, false, false));
					break;
				case 3:
					// Create slider
					JSlider slider = new JSlider();
					midiParameterGUIs.add(new SIDSysexParameterControlSlider(midiParameter, slider, true, BorderLayout.SOUTH,true,true));
					break;
				case 4:
					// Create waveform selector (Voice)
					midiParameterGUIs.add(new SIDSysexParameterControlWaveSelect(midiParameter, true, BorderLayout.NORTH,false,false));
					break;
				case 5:
					// Create waveform selector (LFO)
					midiParameterGUIs.add(new SIDSysexParameterControlLFOSelect(midiParameter, true, BorderLayout.NORTH,false,false));
					break;		
				case 6:
					// Normal combo
					JComboBox combo = new JComboBox(midiParameter.getSparseValAlias());
					midiParameterGUIs.add(new SIDSysexParameterControlCombo(midiParameter,combo,true,BorderLayout.NORTH,false,false));
					break;
				case 7:
					// Simple rotary
					Knob knob1 = new Knob(ImageLoader.getImageIcon("plasticknob_strip.png"));		
					midiParameterGUIs.add(new SIDSysexParameterControlKnob(midiParameter, knob1, false, BorderLayout.SOUTH, true, true));
					break;
				default: midiParameterGUIs.add(midiParameter);System.out.println("Unknown controller type!");
			}	
		}
		return midiParameterGUIs;
	}	
	
	public void createLinkGroups() {
		// Create vector of pointers to left and right parameters (for linking only)
		EXT_L = new Vector();
		EXT_R = new Vector();
		for (int c = 0; c < 8; c++) {
			EXT_L.add(((Vector)GUIs.elementAt(1)).elementAt(2*c));
			EXT_R.add(((Vector)GUIs.elementAt(1)).elementAt((2*c)+1));
		}
		
		// Create vector of pointers to left and right parameters (for linking only)
		MP_L = new Vector();
		MP_R = new Vector();		
		for (int nmp = 0; nmp < 8; nmp++) {
			for (int c = 8; c < 16; c++) {
				MP_L.add(((Vector)GUIs.elementAt(14)).elementAt((24*nmp)+ c));			
				MP_R.add(((Vector)GUIs.elementAt(14)).elementAt((24*nmp) + c + 8));
			}
		}
	}
	
	public void permanentLink() {
		// Link Key Sync button to Trigger: LFO Reset at Note On
		Vector LFO = new Vector();
		Vector TRIGGER = new Vector();
		for (int c = 0; c < 6; c++) {
			LFO.add(((Vector)GUIs.elementAt(11)).elementAt(8+(9*c)));
			TRIGGER.add(((Vector)GUIs.elementAt(15)).elementAt(10+c));
		}
		linkPar(LFO,TRIGGER);	
	}
	
	public void link() {
		linkPar(EXT_L,EXT_R);											// Link external outputs 2 by 2
		linkPar(MP_L,MP_R);												// Link MP direct assign left & right
		linkPar((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));	// Link filter left & right
		linkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(8));	// Link V1 & V4
		linkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(9));	// Link V2 & V5
		linkPar((Vector)GUIs.elementAt(7),(Vector)GUIs.elementAt(10));  // Link V3 & V6	
		linked = true;
	}
	
	public void unlink() {
		unlinkPar(EXT_L,EXT_R);											  // Unlink external outputs 2 by 2
		unlinkPar(MP_L,MP_R);											  // Unlink MP direct assign left & right
		unlinkPar((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));	  // Unlink filter left & right
		unlinkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(8));	  // Unlink V1 & V4
		unlinkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(9));	  // Unlink V2 & V5
		unlinkPar((Vector)GUIs.elementAt(7),(Vector)GUIs.elementAt(10));  // Unlink V3 & V6	
		linked = false;	
	}	
	
	protected void linkPar(Vector left, Vector right) {
		for (int c = 0; c < left.size(); c++) {
			SIDSysexParameterControlGUI g1 = (SIDSysexParameterControlGUI) left.elementAt(c);
			SIDSysexParameterControlGUI g2 = (SIDSysexParameterControlGUI) right.elementAt(c);
			g1.addMidiParameter(g2.getMidiParameter());
			g2.addMidiParameter(g1.getMidiParameter());
		}
	}
	
	protected void unlinkPar(Vector left, Vector right) {
		for (int c = 0; c < left.size(); c++) {
			SIDSysexParameterControlGUI g1 = (SIDSysexParameterControlGUI) left.elementAt(c);
			SIDSysexParameterControlGUI g2 = (SIDSysexParameterControlGUI) right.elementAt(c);
			g1.removeMidiParameter(g2.getMidiParameter());
			g2.removeMidiParameter(g1.getMidiParameter());
		}
	}
}
