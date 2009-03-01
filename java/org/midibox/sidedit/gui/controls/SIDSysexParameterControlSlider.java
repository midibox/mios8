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
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;

import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.utils.gui.FaderUI;
import org.midibox.utils.gui.ImageLoader;

public class SIDSysexParameterControlSlider extends SIDSysexParameterControlGUI
		implements ChangeListener, MouseWheelListener {

	public static float mouseWheelResolution = 0.1f;

	private JSlider slider;

	public SIDSysexParameterControlSlider(
			SIDSysexParameterControl midiParameter, JSlider slider,
			boolean showLabel, String labelLocation, boolean valueBelow,
			boolean showValue) {
		super(midiParameter, showLabel, labelLocation, valueBelow, showValue);
		this.slider = slider;

		slider.setMinimum(midiParameter.getMidiMinValue());
		slider.setMaximum(midiParameter.getMidiMaxValue());

		FaderUI faderUI = new FaderUI(ImageLoader
				.getImageIcon("plasticslider_horiz.png"), ImageLoader
				.getImageIcon("plasticslider_vert.png"), false);

		faderUI.setTrackWidth(3);

		slider.setUI(faderUI);

		slider.setMaximum(midiParameter.getMidiMaxValue()
				- midiParameter.getMidiMinValue());
		slider.setOpaque(false);
		slider.setOrientation(JSlider.VERTICAL);
		slider.setPreferredSize(new Dimension(20, 80));
		JPanel panel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
		panel.setOpaque(false);
		panel.add(slider);
		add(panel, BorderLayout.CENTER);
		slider.addChangeListener(this);
		slider.addMouseWheelListener(this);
		slider.addMouseListener(this);
		updateGraphics();
	}

	public void setHeight(int height) {
		slider.setPreferredSize(new Dimension(20, height));
	}

	public void mouseWheelMoved(MouseWheelEvent mwe) {
		slider
				.setValue((int) (slider.getValue() - ((mwe.getWheelRotation() * Math
						.max(mouseWheelResolution
								* (slider.getMaximum() - slider.getMinimum()),
								1)))));
	}

	public void stateChanged(ChangeEvent ce) {
		if (update) {
			update = false;
			int newval;
			if (midiParameter.snap) {
				int index = (int) (((float) slider.getValue() / (float) slider
						.getMaximum()) * (midiParameter.snapVals.length - 1));
				newval = midiParameter.snapVals[index];
			} else {
				newval = slider.getValue();
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

	public void updateGraphics() {
		super.updateGraphics();

		if (update) {
			update = false;
			int newval;
			if (midiParameter.snap) {
				float ratio = (float) midiParameter.getMidiValSnapIndex()
						/ (float) (midiParameter.snapVals.length - 1);
				newval = (int) ((float) (slider.getMaximum() - slider
						.getMinimum()) * ratio);
			} else {
				newval = midiParameter.getMidiValue();
			}
			slider.setValue(newval);
			update = true;
		}
	}
}