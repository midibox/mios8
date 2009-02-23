/*
 * @(#)MidiParameter.java	beta8	2006/04/23
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

import java.util.Observable;

import javax.sound.midi.ShortMessage;

public class MidiParameter extends Observable {

	public final static int NOTE_OFF = ShortMessage.NOTE_OFF;

	public final static int NOTE_ON = ShortMessage.NOTE_ON;

	public final static int AFTERTOUCH = ShortMessage.POLY_PRESSURE;

	public final static int CONTROL_CHANGE = ShortMessage.CONTROL_CHANGE;

	public final static int PROGRAM_CHANGE = ShortMessage.PROGRAM_CHANGE;

	public final static int CHANNEL_PRESSURE = ShortMessage.CHANNEL_PRESSURE;

	public final static int PITCH_BEND = ShortMessage.PITCH_BEND;

	public final static Object STATUS = new Object();

	public final static Object CHANNEL = new Object();

	public final static Object NUMBER = new Object();

	public final static Object VALUE = new Object();

	public final static Object HIGH_RESOLUTION = new Object();

	protected int channel;

	protected int status;

	protected int value;

	protected int number;

	protected boolean highResolution;

	public MidiParameter(int status, int channel, int number, int value) {
		setMidiStatus(status);
		setMidiChannel(channel);
		setMidiNumber(number);
		setMidiValue(value);
		setHighResolution(false);
	}

	public int getMidiStatus() {
		return status;
	}

	public void setMidiStatus(int status) {

		this.status = status;

		setChanged();
		notifyObservers(STATUS);
		clearChanged();
	}

	public int getMidiChannel() {
		return channel;
	}

	public void setMidiChannel(int channel) {

		this.channel = channel;

		setChanged();
		notifyObservers(CHANNEL);
		clearChanged();
	}

	public int getMidiValue() {
		return value & getMidiMaxValue();
	}

	public void setMidiValue(int value) {

		this.value = value & getMidiMaxValue();

		setChanged();
		notifyObservers(VALUE);
		clearChanged();
	}

	public int getMidiMaxValue() {

		int currentStatus = getMidiStatus();

		if (currentStatus == PITCH_BEND
				|| (currentStatus == CONTROL_CHANGE && highResolution)) {
			return 16383;
		} else {
			return 127;
		}
	}

	public int getMidiMinValue() {

		return 0;
	}

	public String getMidiName() {

		if (status == NOTE_ON || status == NOTE_OFF) {
			return "Note " + MidiUtils.getKeyName(number);
		} else if (status == AFTERTOUCH) {
			return "Aftertouch " + MidiUtils.getKeyName(number);
		} else if (status == CONTROL_CHANGE) {
			if (highResolution) {
				return MidiUtils.cc14BitNames[number];
			} else {
				return MidiUtils.ccNames[number];
			}
		} else if (status == PROGRAM_CHANGE) {
			return "Program Change";
		} else if (status == CHANNEL_PRESSURE) {
			return "Channel Pressure";
		} else if (status == PITCH_BEND) {
			return "Pitch Bend";
		} else {
			return "undefined";
		}
	}

	public int getMidiNumber() {
		return number;
	}

	public void setMidiNumber(int number) {
		this.number = number;

		int currentStatus = getMidiStatus();

		setChanged();
		notifyObservers(NUMBER);
		clearChanged();
	}

	public boolean isHighResolution() {
		return highResolution;
	}

	public void setHighResolution(boolean highResolution) {
		this.highResolution = highResolution;

		setChanged();
		notifyObservers(HIGH_RESOLUTION);
		clearChanged();
	}
}
