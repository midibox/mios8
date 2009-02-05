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

package org.midibox.sidedit.gui.ensemble;

import java.awt.BorderLayout;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.JComboBox;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTabbedPane;
import javax.swing.JToggleButton;

import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.sidedit.gui.FilterGUI;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlCombo;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlGUI;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlKnob;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlLFOSelect;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlSlider;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlToggleButton;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlWaveSelect;
import org.midibox.sidlibr.Patch;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.Knob;
import org.midibox.utils.gui.MyButtonUI;

public class EnsembleGUI extends JPanel {
	private Vector GUIs, EXT_L, EXT_R, MP_L, MP_R;
	private SIDEditController sidEditController;
	private boolean stereoLinked = true;
	private boolean oscLinked = false;

	public EnsembleGUI(SIDEditController sidEditController) {
		this.sidEditController = sidEditController;
		setLayout(new BorderLayout());
		
		GUIs = createGUIs(sidEditController);
		
		JTabbedPane tabbedPane = new JTabbedPane();
		JPanel t1 = new SIDGUI((Vector) GUIs.elementAt(0));
		JPanel t2 = new SIDGUI((Vector) GUIs.elementAt(1));
		JPanel t3 = new SIDGUI((Vector) GUIs.elementAt(2));
		JPanel t4 = new SIDGUI((Vector) GUIs.elementAt(3));
		
		tabbedPane.addTab("SID1", t1);
		tabbedPane.addTab("SID2", t2);
		tabbedPane.addTab("SID3", t3);
		tabbedPane.addTab("SID4", t4);
		
		add(tabbedPane, BorderLayout.NORTH);
		setOpaque(false);
		setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));
	}

	protected Vector createGUIs(SIDEditController sidEditController) {
		Vector v_in = (Vector) sidEditController.getEngine();
		Vector v_out = new Vector();
		for (int b = 0; b < v_in.size(); b++) {
			v_out.addElement(createGuiElements((Vector) v_in.elementAt(b)));
		}
		return v_out;
	}

	protected Vector createGuiElements(Vector v) {
		// Create controller GUI's
		Vector midiParameterGUIs = new Vector();
		for (int c = 0; c < v.size(); c++) {
			SIDSysexParameterControl midiParameter = (SIDSysexParameterControl) v
					.elementAt(c);
			switch (midiParameter.getType()) {
			case 0:
				// Create knob
				Knob knob = new Knob(ImageLoader
						.getImageIcon("plasticknob_strip.png"));
				midiParameterGUIs.add(new SIDSysexParameterControlKnob(
						midiParameter, knob, true, BorderLayout.SOUTH, true,
						true));
				break;
			case 1:
				// Create toggle button
				JToggleButton button = new JToggleButton();
				button.setUI(new MyButtonUI(ImageLoader
						.getImageIcon("button_on.png"), ImageLoader
						.getImageIcon("button.png")));
				midiParameterGUIs.add(new SIDSysexParameterControlToggleButton(
						midiParameter, button, false, true, BorderLayout.SOUTH,
						true, false));
				break;
			case 2:
				// Create simple toggle button
				JToggleButton simpleButton = new JToggleButton();
				simpleButton.setUI(new MyButtonUI(ImageLoader
						.getImageIcon("button_on.png"), ImageLoader
						.getImageIcon("button.png")));
				midiParameterGUIs.add(new SIDSysexParameterControlToggleButton(
						midiParameter, simpleButton, false, false,
						BorderLayout.SOUTH, false, false));
				break;
			case 3:
				// Create slider
				JSlider slider = new JSlider();
				midiParameterGUIs.add(new SIDSysexParameterControlSlider(
						midiParameter, slider, true, BorderLayout.SOUTH, true,
						true));
				break;
			case 4:
				// Create waveform selector (Voice)
				midiParameterGUIs.add(new SIDSysexParameterControlWaveSelect(
						midiParameter, true, BorderLayout.NORTH, false, false));
				break;
			case 5:
				// Create waveform selector (LFO)
				midiParameterGUIs.add(new SIDSysexParameterControlLFOSelect(
						midiParameter, true, BorderLayout.NORTH, false, false));
				break;
			case 6:
				// Normal combo
				JComboBox combo = new JComboBox(midiParameter
						.getSparseValAlias());
				midiParameterGUIs.add(new SIDSysexParameterControlCombo(
						midiParameter, combo, true, BorderLayout.NORTH, false,
						false));
				break;
			case 7:
				// Simple rotary
				Knob knob1 = new Knob(ImageLoader
						.getImageIcon("plasticknob_strip.png"));
				midiParameterGUIs.add(new SIDSysexParameterControlKnob(
						midiParameter, knob1, false, BorderLayout.SOUTH, true,
						true));
				break;
			default:
				midiParameterGUIs.add(midiParameter);
				System.out.println("Unknown controller type!");
			}
		}
		return midiParameterGUIs;
	}	
}
