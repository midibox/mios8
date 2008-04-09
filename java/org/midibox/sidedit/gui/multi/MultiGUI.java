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
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JComboBox;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JToggleButton;

import org.midibox.sidedit.BasslineSpecs;
import org.midibox.sidedit.DrumSpecs;
import org.midibox.sidedit.SIDSysexInfo;
import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.SIDSysexParameterControl;

import org.midibox.sidedit.gui.controls.*;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.Knob;
import org.midibox.utils.gui.MyButtonUI;
import org.midibox.sidedit.gui.GlobalGUI;
import org.midibox.sidedit.gui.FilterGUI;

public class MultiGUI extends JPanel {
	private boolean linked = false;
	private Vector GUIs, EXT_L, EXT_R;
	private boolean oscLinked = false;
	private boolean stereoLinked = false;
	
	private SIDEditController sidEditController;
	
	public MultiGUI(SIDEditController sidEditController) {		
		this.sidEditController = sidEditController;
		setLayout(new BorderLayout());
		
		GUIs = createGUIs(sidEditController);
		createLinkGroups();
		JTabbedPane tabbedPane = new JTabbedPane();
		
		JPanel t1 = new GlobalGUI((Vector)GUIs.elementAt(0),(Vector)GUIs.elementAt(1));
		JPanel t2 = new FilterGUI((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));
		JPanel t3 = new InstrumentGUI((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(10));
		JPanel t4 = new InstrumentGUI((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(11));
		JPanel t5 = new InstrumentGUI((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(12));
		JPanel t6 = new InstrumentGUI((Vector)GUIs.elementAt(7),(Vector)GUIs.elementAt(13));
		JPanel t7 = new InstrumentGUI((Vector)GUIs.elementAt(8),(Vector)GUIs.elementAt(14));
		JPanel t8 = new InstrumentGUI((Vector)GUIs.elementAt(9),(Vector)GUIs.elementAt(15));		
		JPanel t9 = new WTGUI((Vector)GUIs.elementAt(16), (Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(13),(Vector)GUIs.elementAt(14),(Vector)GUIs.elementAt(15));
		
		tabbedPane.addTab("Global", t1);
		tabbedPane.addTab("Filter", t2);
		tabbedPane.addTab("Instr.1", t3);
		tabbedPane.addTab("Instr.2", t4);
		tabbedPane.addTab("Instr.3", t5);
		tabbedPane.addTab("Instr.4", t6);
		tabbedPane.addTab("Instr.5", t7);
		tabbedPane.addTab("Instr.6", t8);
		tabbedPane.addTab("Wavetable", t9);
			
		add(tabbedPane, BorderLayout.NORTH);
		setOpaque(false);
		setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));
		stereoLink(true);		
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
					midiParameterGUIs.add(new SIDSysexParameterControlToggleButton(midiParameter, simpleButton, false, false, BorderLayout.SOUTH, true, false));
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
	}
		
	public void restLink(boolean b) {		
		if (b) {
			linkPar((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(8));	// Link V1 & V5
			linkPar((Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(14));	// Link V1 & V5 rest
			linkPar((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(9));	// Link V1 & V6
			linkPar((Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(15));	// Link V1 & V6 rest
			linkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(7));	// Link V2 & V4			
			linkPar((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(13));	// Link V2 & V4 rest
			linkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(9));	// Link V2 & V6			
			linkPar((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(15));	// Link V2 & V6 rest
			linkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(7));	// Link V3 & V4
			linkPar((Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(13));	// Link V3 & V4 rest
			linkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(8));	// Link V3 & V5			
			linkPar((Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(14));	// Link V3 & V5 rest
		} else {
			unlinkPar((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(8));		// Unlink V1 & V5
			unlinkPar((Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(14));	// Unlink V1 & V5 rest
			unlinkPar((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(9));		// Unlink V1 & V6
			unlinkPar((Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(15));	// Unlink V1 & V6 rest
			unlinkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(7));		// Unlink V2 & V4			
			unlinkPar((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(13));	// Unlink V2 & V4 rest
			unlinkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(9));		// Unlink V2 & V6			
			unlinkPar((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(15));	// Unlink V2 & V6 rest
			unlinkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(7));		// Unlink V3 & V4
			unlinkPar((Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(13));	// Unlink V3 & V4 rest
			unlinkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(8));		// Unlink V3 & V5			
			unlinkPar((Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(14));	// Unlink V3 & V5 rest		
		}
	}
	
	public void oscLink(boolean b) {
		if (b) {
			sidEditController.getPatch().setOscillatorLink(true);
			linkPar((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(5));	// Link V1 & V2
			linkPar((Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(11));	// Link V1 & V2 rest			
			linkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(6));	// Link V2 & V3
			linkPar((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(12));	// Link V2 & V3 rest
			linkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(4));	// Link V3 & V1				
			linkPar((Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(10));	// Link V3 & V1 rest
			linkPar((Vector)GUIs.elementAt(7),(Vector)GUIs.elementAt(8));	// Link V4 & V5
			linkPar((Vector)GUIs.elementAt(13),(Vector)GUIs.elementAt(14));	// Link V4 & V5 rest
			linkPar((Vector)GUIs.elementAt(8),(Vector)GUIs.elementAt(9));	// Link V5 & V6	
			linkPar((Vector)GUIs.elementAt(14),(Vector)GUIs.elementAt(15));	// Link V5 & V6 rest
			linkPar((Vector)GUIs.elementAt(9),(Vector)GUIs.elementAt(7));	// Link V6 & V4			
			linkPar((Vector)GUIs.elementAt(15),(Vector)GUIs.elementAt(13));	// Link V6 & V4 rest
		} else {
			sidEditController.getPatch().setOscillatorLink(false);
			unlinkPar((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(5));		// Unlink V1 & V2
			unlinkPar((Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(11));	// Unlink V1 & V2 rest			
			unlinkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(6));		// Unlink V2 & V3
			unlinkPar((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(12));	// Unlink V2 & V3 rest
			unlinkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(4));		// Unlink V3 & V1				
			unlinkPar((Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(10));	// Unlink V3 & V1 rest
			unlinkPar((Vector)GUIs.elementAt(7),(Vector)GUIs.elementAt(8));		// Unlink V4 & V5
			unlinkPar((Vector)GUIs.elementAt(13),(Vector)GUIs.elementAt(14));	// Unlink V4 & V5 rest
			unlinkPar((Vector)GUIs.elementAt(8),(Vector)GUIs.elementAt(9));		// Unlink V5 & V6	
			unlinkPar((Vector)GUIs.elementAt(14),(Vector)GUIs.elementAt(15));	// Unlink V5 & V6 rest
			unlinkPar((Vector)GUIs.elementAt(9),(Vector)GUIs.elementAt(7));		// Unlink V6 & V4			
			unlinkPar((Vector)GUIs.elementAt(15),(Vector)GUIs.elementAt(13));	// Unlink V6 & V4 rest				
		}
		if (stereoLinked) {
			restLink(b);
		}		
		oscLinked = b;
	}
	
	
	public void stereoLink(boolean b) {
		if (b) {
			sidEditController.getPatch().setStereoLink(true);
			linkPar(EXT_L,EXT_R);												// Link external outputs 2 by 2
			linkPar((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));		// Link filter left & right		
			linkPar((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(7));		// Link V1 & V4
			linkPar((Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(13));		// Link V1 & V4 rest
			linkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(8));		// Link V2 & V5
			linkPar((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(14));		// Link V2 & V5 rest
			linkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(9));  		// Link V3 & V6	
			linkPar((Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(15));		// Link V3 & V6 rest
		} else {
			sidEditController.getPatch().setStereoLink(false);
			unlinkPar(EXT_L,EXT_R);											  	// Unlink external outputs 2 by 2
			unlinkPar((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));		// Unlink filter left & right	
			unlinkPar((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));		// Unlink filter left & right		
			unlinkPar((Vector)GUIs.elementAt(4),(Vector)GUIs.elementAt(7));		// Unlink V1 & V4
			unlinkPar((Vector)GUIs.elementAt(10),(Vector)GUIs.elementAt(13));		// Unlink V1 & V4 rest
			unlinkPar((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(8));		// Unlink V2 & V5
			unlinkPar((Vector)GUIs.elementAt(11),(Vector)GUIs.elementAt(14));		// Unlink V2 & V5 rest
			unlinkPar((Vector)GUIs.elementAt(6),(Vector)GUIs.elementAt(9));  		// Unlink V3 & V6	
			unlinkPar((Vector)GUIs.elementAt(12),(Vector)GUIs.elementAt(15));		// Unlink V3 & V6 rest
		}	
		if (oscLinked) {
			restLink(b);
		}
		stereoLinked = b;		
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
