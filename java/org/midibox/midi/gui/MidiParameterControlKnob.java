/*
 * @(#)MidiParameterControlKnob.java	beta8	2006/04/23
 *
 * Copyright (C) 2008    Adam King (adamjking@optusnet.com.au)
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

package org.midibox.midi.gui;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.util.Observable;

import javax.swing.JPanel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.midibox.midi.MidiParameterControl;
import org.midibox.utils.gui.Knob;

public class MidiParameterControlKnob extends MidiParameterControlGUI implements
		ChangeListener, MouseWheelListener {

	public static float mouseWheelResolution = 0.1f;

	private Knob knob;

	public MidiParameterControlKnob(MidiParameterControl midiParameter,
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
		knob.addMouseListener(this);
		knob.addMouseWheelListener(this);
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

			midiParameterControl.setMidiValue(knob.getValue(), true);

			update = true;
		}
	}

	public void updateGraphics() {

		super.updateGraphics();

		if (update) {

			update = false;

			knob.setValue(midiParameterControl.getMidiValue());

			update = true;
		}
	}

	public void update(Observable observable, Object object) {
		super.update(observable, object);
		if (object == midiParameterControl.HIGH_RESOLUTION) {

			knob.setMinimum(midiParameterControl.getMidiMinValue());
			knob.setMaximum(midiParameterControl.getMidiMaxValue());
		}
	}
}
