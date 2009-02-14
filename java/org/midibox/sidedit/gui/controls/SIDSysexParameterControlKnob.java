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

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.util.Arrays;

import javax.swing.JPanel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.utils.gui.Knob;

public class SIDSysexParameterControlKnob extends SIDSysexParameterControlGUI
		implements ChangeListener, MouseWheelListener {

	public static float mouseWheelResolution = 0.1f;

	private Knob knob;

	public SIDSysexParameterControlKnob(SIDSysexParameterControl midiParameter,
			Knob knob, boolean showLabel, String labelLocation,
			boolean valueBelow, boolean showValue) {
		super(midiParameter, showLabel, labelLocation, valueBelow, showValue);
		this.knob = knob;

		JPanel panel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
		panel.setOpaque(false);
		panel.add(knob);

		knob.setMinimum(midiParameter.getMidiMinValue());
		knob.setMaximum(midiParameter.getMidiMaxValue());

		add(panel, BorderLayout.CENTER);
		knob.addChangeListener(this);
		knob.addMouseWheelListener(this);
		knob.addMouseListener(this);
		updateGraphics();
	}

	public void mouseWheelMoved(MouseWheelEvent mwe) {
		knob.setValue((int) (knob.getValue() - ((mwe.getWheelRotation() * Math
				.max(mouseWheelResolution
						* (knob.getMaximum() - knob.getMinimum()), 1)))));
	}

	public void stateChanged(ChangeEvent ce) {
		if (update) {
			update = false;
			int newval;
			if (midiParameter.snap) {int index = (int) (((float) knob.getValue() / (float) knob.getMaximum()) * (midiParameter.snapVals.length - 1));
				newval = midiParameter.snapVals[index];
			} else {
				newval = knob.getValue();
			}

			midiParameter.setMidiValue(newval, true);

			for (int c = 0; c < midiParameters.size(); c++) {
				SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters.elementAt(c);
				mp.setMidiValue(newval, false);
			}
			update = true;
		}
	}

	public void updateGraphics() {
		super.updateGraphics();
		if (update) {
			update = false;
			int newval;
			if (midiParameter.snap) {
				float ratio = (float) midiParameter.getMidiValSnapIndex() / (float) (midiParameter.snapVals.length - 1);
				newval = (int)((float)(knob.getMaximum()-knob.getMinimum())* ratio);
			} else {
				newval = midiParameter.getMidiValue();
			}
			knob.setValue(newval);
			update = true;
		}
	}
}