/*
 * @(#)MidiKeyboardController.java	beta8	2006/04/23
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

package org.midibox.midi;

import java.util.Vector;

import javax.sound.midi.Receiver;

public class MidiKeyboardController extends MidiParameterControlManager {

	private Vector controllers;

	private Vector buttons;

	private Vector keys;

	private int velocityVal = 100;

	private int noOctaves = 6;

	private int transpose = 24;

	private MidiParameterControl bankSelect;

	private MidiParameterControl programChange;

	private MidiParameterControl modWheel;

	private MidiParameterControl pitchWheel;

	public MidiKeyboardController(Receiver receiver, int globalChannel) {

		super(receiver, globalChannel);

		controllers = new Vector();

		controllers.add(new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 7, 127,
				64));
		controllers.add(new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 8, 64,
				64));
		controllers.add(new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 10, 64,
				64));
		controllers.add(new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 11, 127,
				64));
		controllers.add(new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 12, 0,
				64));
		controllers.add(new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 13, 0,
				64));

		for (int c = 0; c < controllers.size(); c++) {
			MidiParameterControl midiParameter = (MidiParameterControl) controllers
					.elementAt(c);
			addMidiParameter(midiParameter);
		}

		buttons = new Vector();

		buttons.add(new MidiParameterControl(MidiParameterControl.BOOLEAN,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 64, 0,
				64));
		buttons.add(new MidiParameterControl(MidiParameterControl.BOOLEAN,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 69, 0,
				64));
		buttons.add(new MidiParameterControl(MidiParameterControl.BOOLEAN,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 124, 0,
				64));
		buttons.add(new MidiParameterControl(MidiParameterControl.BOOLEAN,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 125, 0,
				64));
		buttons.add(new MidiParameterControl(MidiParameterControl.BOOLEAN,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 126, 0,
				64));
		buttons.add(new MidiParameterControl(MidiParameterControl.BOOLEAN,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 127, 0,
				64));

		for (int b = 0; b < buttons.size(); b++) {
			MidiParameterControl midiParameter = (MidiParameterControl) buttons
					.elementAt(b);
			addMidiParameter(midiParameter);
		}

		keys = new Vector();

		for (int o = 0; o < noOctaves; o++) {
			for (int note = 0; note < 12; note++) {
				MidiParameterControl midiParameter = new MidiParameterControl(
						MidiParameterControl.BOOLEAN, receiver,
						MidiParameter.NOTE_ON, globalChannel, (o * 12) + note
								+ transpose, 0, velocityVal);
				keys.add(midiParameter);
				addMidiParameter(midiParameter);
			}
		}

		bankSelect = new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 0, 0, 0);
		bankSelect.addObserver(this);
		bankSelect.setReceive(false);
		bankSelect.setHighResolution(true);
		addMidiParameter(bankSelect);

		programChange = new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.PROGRAM_CHANGE, globalChannel, 0, 0, 64);
		programChange.addObserver(this);
		programChange.setReceive(false);
		addMidiParameter(programChange);

		modWheel = new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.CONTROL_CHANGE, globalChannel, 1, 0, 64);
		modWheel.addObserver(this);
		addMidiParameter(modWheel);

		pitchWheel = new MidiParameterControl(MidiParameterControl.RANGE,
				receiver, MidiParameter.PITCH_BEND, globalChannel, 0, 8191,
				8192);
		pitchWheel.addObserver(this);
		addMidiParameter(pitchWheel);
	}

	public Vector getControllers() {
		return controllers;
	}

	public Vector getButtons() {
		return buttons;
	}

	public Vector getKeys() {
		return keys;
	}

	public int getVelocityVal() {
		return velocityVal;
	}

	public void setVelocityVal(int velocityVal) {
		this.velocityVal = velocityVal;
		for (int k = 0; k < keys.size(); k++) {
			MidiParameterControl midiParameter = (MidiParameterControl) keys
					.elementAt(k);
			midiParameter.setMidiDefaultValue(velocityVal);
		}
	}

	public MidiParameterControl getBankSelect() {
		return bankSelect;
	}

	public MidiParameterControl getProgramChange() {
		return programChange;
	}

	public MidiParameterControl getModWheel() {
		return modWheel;
	}

	public MidiParameterControl getPitchWheel() {
		return pitchWheel;
	}

	public int getNoOctaves() {
		return noOctaves;
	}

	public int getTranspose() {
		return transpose;
	}
}
