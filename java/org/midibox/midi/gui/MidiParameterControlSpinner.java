/*
 * @(#)MidiParameterControlSpinner.java	beta8	2006/04/23
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
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.midibox.midi.MidiParameterControl;
import org.midibox.utils.gui.FontLoader;

public class MidiParameterControlSpinner extends MidiParameterControlGUI
		implements ChangeListener {

	private JSpinner spinner;

	public MidiParameterControlSpinner(MidiParameterControl midiParameter,
			JSpinner spinner, String name) {
		super(midiParameter, true, BorderLayout.NORTH, false, false);
		this.spinner = spinner;
		midiParameterLabel.setText(name);
		JPanel panel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
		panel.setOpaque(false);
		panel.add(spinner);
		add(panel, BorderLayout.CENTER);
		spinner.addChangeListener(this);
		spinner.setFont(FontLoader.getFont("uni05_53.ttf", 8f));
		spinner.addMouseListener(this);
		updateGraphics();
	}

	public void stateChanged(ChangeEvent ce) {
		if (update) {
			update = false;
			midiParameter
					.setMidiValue(
							(int) (((float) ((Integer) spinner.getModel()
									.getValue()).intValue() / (float) ((Integer) ((SpinnerNumberModel) spinner
									.getModel()).getMaximum()).intValue()) * (float) midiParameter
									.getMidiMaxValue()), true);
			update = true;
		}
	}

	protected JPopupMenu createPopMenu() {
		JPopupMenu popup = new JPopupMenu();

		final JCheckBoxMenuItem channelMenuItem = new JCheckBoxMenuItem(
				"Global Channel", (midiParameter.isGlobal()));
		channelMenuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				midiParameter.setGlobal(channelMenuItem.isSelected());
			}
		});

		popup.add(channelMenuItem);

		for (int i = 0; i < 16; i++) {
			JCheckBoxMenuItem channelItem = new JCheckBoxMenuItem("Channel "
					+ (i + 1), (midiParameter.getMidiChannel() == i));
			channelItem.setEnabled(!midiParameter.isGlobal());
			popup.add(channelItem);

			final int channelNo = i;

			channelItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ae) {
					midiParameter.setMidiChannel(channelNo);
				}
			});
		}
		return popup;
	}

	public void updateGraphics() {
		super.updateGraphics();
		if (update) {
			update = false;
			spinner
					.setValue(new Integer(
							(int) ((((float) midiParameter.getMidiValue() / (float) midiParameter
									.getMidiMaxValue())) * (float) (((Integer) ((SpinnerNumberModel) spinner
									.getModel()).getMaximum()).intValue()))));
			update = true;
		}
	}

	public void update(Observable observable, Object object) {

	}
}
