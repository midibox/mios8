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

import javax.swing.JPanel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.utils.gui.Knob;

public class SIDSysexParameterControlKnob extends SIDSysexParameterControlGUI
		implements ChangeListener, MouseWheelListener {

	public static float mouseWheelResolution = 10.0f;

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
		knob
				.setValue((int) (knob.getValue() - (mwe.getWheelRotation() * ((mouseWheelResolution / 100) * (knob
						.getMaximum() - knob.getMinimum())))));
	}

	public void stateChanged(ChangeEvent ce) {

		if (update) {

			update = false;

			int newval;

			if (midiParameter.snap) {

				int index = (int) (((float) knob.getValue() / (float) knob
						.getMaximum()) * (midiParameter.snapvals.length - 1));
				newval = midiParameter.snapvals[index];

			} else {

				newval = knob.getValue();
			}

			midiParameter.setMidiValue(newval, true);

			for (int c = 0; c < midiParameters.size(); c++) {
				SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
						.elementAt(c);
				mp.setMidiValue(newval, false);
			}

			update = true;
		}

	}

	private int findSnap(int val) {
		int outval = 0;
		for (int i = 0; i < midiParameter.snapvals.length; i++) {
			if (midiParameter.snapvals[i] == val) {
				outval = i;
				break;
			}
		}
		return outval;
	}

	public void setSnapvals(int[] vals) {
		midiParameter.useAlias = false;
		midiParameter.snapvals = vals;
		for (int c = 0; c < midiParameters.size(); c++) {
			SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
					.elementAt(c);
			mp.snapvals = vals;
		}
	}

	public void updateGraphics() {

		super.updateGraphics();

		if (update) {

			update = false;

			int newval;

			if (midiParameter.snap) {

				int index = findSnap(midiParameter.getMidiValue());
				newval = index / (midiParameter.snapvals.length - 1);

			} else {

				newval = midiParameter.getMidiValue();
			}

			knob.setValue(newval);

			update = true;
		}
	}
}