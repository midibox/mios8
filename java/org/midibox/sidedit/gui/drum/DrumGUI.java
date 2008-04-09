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

public class DrumGUI extends JPanel {
	private Vector GUIs, EXT_L, EXT_R;
	private SIDEditController sidEditController;
	public DrumGUI(SIDEditController sidEditController) {	
		this.sidEditController = sidEditController;
		setLayout(new BorderLayout());
		
		GUIs = createGUIs(sidEditController);
		createLinkGroups();
		JTabbedPane tabbedPane = new JTabbedPane();
		
		JPanel t1 = new GlobalGUI((Vector)GUIs.elementAt(0),(Vector)GUIs.elementAt(1));		
		JPanel t2 = new DrumInstrumentGUI((Vector)GUIs.elementAt(4),false);
		JPanel t3 = new DrumInstrumentGUI((Vector)GUIs.elementAt(4),true);
		JPanel t4 = new FilterGUI((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));
		JPanel t5 = new SequencerGUI((Vector)GUIs.elementAt(5),(Vector)GUIs.elementAt(6));		
		
		tabbedPane.addTab("Global", t1);
		tabbedPane.addTab("Drum Instr.1-8", t2);
		tabbedPane.addTab("Drum Instr.9-16", t3);
		tabbedPane.addTab("Filter", t4);				
		tabbedPane.addTab("Sequencer", t5);
			
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
	
	public void stereoLink(boolean b) {
		if (b) {
			sidEditController.getPatch().setStereoLink(true);
			linkPar(EXT_L,EXT_R);												// Link external outputs 2 by 2
			linkPar((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));		// Link filter left & right		
		} else {
			sidEditController.getPatch().setStereoLink(false);
			unlinkPar(EXT_L,EXT_R);											  	// Unlink external outputs 2 by 2
			unlinkPar((Vector)GUIs.elementAt(2),(Vector)GUIs.elementAt(3));		// Link filter left & right		
		}
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
