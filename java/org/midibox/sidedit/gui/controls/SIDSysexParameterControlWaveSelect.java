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

package org.midibox.sidedit.gui.controls;

import java.awt.event.ActionEvent;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JToggleButton;

import org.midibox.sidedit.*;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.MyButtonUI;

public class SIDSysexParameterControlWaveSelect extends SIDSysexParameterControlGUI {

	private JToggleButton b0, b1, b2, b3, b4, b5, b6;
	private int bitmask0 = (int) Math.pow(2,4); // Off
	private int bitmask1 = (int) Math.pow(2,0);	// Triangle
	private int bitmask2 = (int) Math.pow(2,1); // Saw
	private int bitmask3 = (int) Math.pow(2,2); // Pulse
	private int bitmask4 = (int) Math.pow(2,3); // Noise	
	private int bitmask5 = (int) Math.pow(2,5); // Sync
	private int bitmask6 = (int) Math.pow(2,6); // Ringmod
	
		public SIDSysexParameterControlWaveSelect(SIDSysexParameterControl midiParameter, boolean showLabel,
			String labelLocation, boolean valueBelow, boolean showValue) {
		super(midiParameter, showLabel, labelLocation, valueBelow, showValue);
		
		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
		panel.setOpaque(false);
		b0 = new JToggleButton();
		b1 = new JToggleButton();
		b2 = new JToggleButton();
		b3 = new JToggleButton();
		b4 = new JToggleButton();
		b5 = new JToggleButton();
		b6 = new JToggleButton();
		b0.setUI(new MyButtonUI(ImageLoader.getImageIcon("off_on.png"), ImageLoader.getImageIcon("off.png")));
		b1.setUI(new MyButtonUI(ImageLoader.getImageIcon("tri_on.png"), ImageLoader.getImageIcon("tri.png")));
		b2.setUI(new MyButtonUI(ImageLoader.getImageIcon("saw_on.png"), ImageLoader.getImageIcon("saw.png")));
		b3.setUI(new MyButtonUI(ImageLoader.getImageIcon("pulse_on.png"), ImageLoader.getImageIcon("pulse.png")));
		b4.setUI(new MyButtonUI(ImageLoader.getImageIcon("noise_on.png"), ImageLoader.getImageIcon("noise.png")));
		b5.setUI(new MyButtonUI(ImageLoader.getImageIcon("sync_on.png"), ImageLoader.getImageIcon("sync.png")));
		b6.setUI(new MyButtonUI(ImageLoader.getImageIcon("ringmod_on.png"), ImageLoader.getImageIcon("ringmod.png")));
		//panel.add(b0);	// This is the 'Off' button, which should be invisible because it's pressed automatically in the background
		panel.add(b1);
		panel.add(b2);
		panel.add(b3);
		panel.add(b4);
		panel.add(b5);
		panel.add(b6);
		add(panel);
		b0.addActionListener(this);
		b1.addActionListener(this);		
		b2.addActionListener(this);		
		b3.addActionListener(this);		
		b4.addActionListener(this);		
		b5.addActionListener(this);		
		b6.addActionListener(this);
		b0.addMouseListener(this);
		b1.addMouseListener(this);	
		b2.addMouseListener(this);		
		b3.addMouseListener(this);	
		b4.addMouseListener(this);	
		b5.addMouseListener(this);	
		b6.addMouseListener(this);
		updateGraphics();
	}

	public void actionPerformed(ActionEvent ae) {
		super.actionPerformed(ae);
		if (ae.getSource().getClass() == JToggleButton.class) {		
			if ((ae.getSource() == b4) && b4.isSelected()){				// Noise button disables all others			
				b0.setSelected(false);
				b1.setSelected(false);
				b2.setSelected(false);
				b3.setSelected(false);
			} else if ((ae.getSource() == b0) && b0.isSelected()) {		// Off button disables all others	
				b1.setSelected(false);
				b2.setSelected(false);
				b3.setSelected(false);
				b4.setSelected(false);
			} else if (((ae.getSource() == b1)||(ae.getSource() == b2)||(ae.getSource() == b3)) && (b1.isSelected()||b2.isSelected()||b3.isSelected())) {
				b0.setSelected(false);
				b4.setSelected(false);
			}	
			
			if (!(b0.isSelected()||b1.isSelected()||b2.isSelected()||b3.isSelected()||b4.isSelected())) {
				b0.setSelected(true);
			}
		
			if (update) {
				update = false;								
				int newval = 0;
				newval = newval | (b0.isSelected()?bitmask0:0);
				newval = newval | (b1.isSelected()?bitmask1:0);
				newval = newval | (b2.isSelected()?bitmask2:0);
				newval = newval | (b3.isSelected()?bitmask3:0);
				newval = newval | (b4.isSelected()?bitmask4:0);
				newval = newval | (b5.isSelected()?bitmask5:0);
				newval = newval | (b6.isSelected()?bitmask6:0);
				midiParameter.setMidiValue(newval, true);
				for (int c = 0; c < midiParameters.size(); c++) {
					SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters.elementAt(c);
					mp.setMidiValue(newval, true);
				}
				update = true;
			}
		}		
	}	
	
	public void updateGraphics() {
		super.updateGraphics();
		if (update) {
			update = false;
			b0.setSelected((midiParameter.getMidiValue() & bitmask0) != 0);
			b1.setSelected((midiParameter.getMidiValue() & bitmask1) != 0);
			b2.setSelected((midiParameter.getMidiValue() & bitmask2) != 0);
			b3.setSelected((midiParameter.getMidiValue() & bitmask3) != 0);
			b4.setSelected((midiParameter.getMidiValue() & bitmask4) != 0);
			b5.setSelected((midiParameter.getMidiValue() & bitmask5) != 0);
			b6.setSelected((midiParameter.getMidiValue() & bitmask6) != 0);
			update = true;
		}
	}
}