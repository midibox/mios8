/*
 * @(#)MidiParameterControl.java	beta7	2006/04/23
 *
 * Copyright (C) 2006    Adam King (adamjking@optusnet.com.au)
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

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.ShortMessage;

public class MidiParameterControl extends MidiParameter {

	public final static Object RECEIVE = new Object();

	public final static Object SEND = new Object();

	public final static Object GLOBAL = new Object();

	public final static int RANGE = 0;

	public final static int BOOLEAN = 1;

	protected boolean receive;

	protected boolean send;

	protected boolean global;

	protected boolean highResolution;

	public MidiParameterControl(int type, Receiver receiver, int status,
			int channel, int number, int value, int defaultValue) {
		super(receiver, status, channel, number, value, defaultValue);
		setType(type);
		setGlobal(true);
		setReceive(true);
		setSend(true);
	}

	public boolean isReceive() {
		return receive;
	}

	public void setReceive(boolean respond) {
		this.receive = respond;

		setChanged();
		notifyObservers(RECEIVE);
		clearChanged();
	}

	public boolean isSend() {
		return send;
	}

	public void setSend(boolean send) {
		this.send = send;

		setChanged();
		notifyObservers(SEND);
		clearChanged();
	}

	public boolean isGlobal() {
		return global;
	}

	public void setGlobal(boolean global) {

		this.global = global;

		setChanged();
		notifyObservers(GLOBAL);
		clearChanged();
	}

	protected void midiLearn(MidiMessage message) {
		if (message instanceof ShortMessage) {
			setGlobal(false);
			super.midiLearn(message);
		}
	}

	public int getType() {
		return type;
	}

	public void setType(int type) {
		this.type = type;
	}

	public void setMidiValue(int value, boolean forward) {

		if (type != RANGE) {
			value = (value > 0) ? getMidiDefaultValue() : 0;
		}

		super.setMidiValue(value, forward);
	}

	public boolean isMidiValueOn() {
		return getMidiValue() > 0;
	}

	public void setMidiValueOn(boolean on, boolean forward) {
		setMidiValue(on ? 1 : 0, forward);
	}

	public void createMessage() {
		if (send) {
			super.createMessage();
		}
	}

	public void send(MidiMessage message, long lTimeStamp) {
		if (receive) {
			super.send(message, lTimeStamp);
		}
	}
}
