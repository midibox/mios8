/*
 * @(#)MidiParameterControlManager.java	beta8	2006/04/23
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

import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;

public class MidiParameterControlManager extends Observable implements
		Observer, Receiver {

	protected Vector midiParametersControls;

	protected int globalChannel = 0;

	protected Receiver receiver;

	public MidiParameterControlManager(Receiver receiver, int globalChannel) {
		this.globalChannel = globalChannel;
		this.receiver = receiver;
		midiParametersControls = new Vector();
	}

	public Receiver getReceiver() {
		return receiver;
	}

	public void setReceiver(Receiver receiver) {
		this.receiver = receiver;

		Iterator it = midiParametersControls.iterator();

		while (it.hasNext()) {
			MidiParameterControl midiParameter = (MidiParameterControl) it
					.next();
			if (midiParameter.isGlobal()) {
				midiParameter.setReceiver(receiver);
			}
		}
	}

	public Vector getMidiParametersControls() {
		return midiParametersControls;
	}

	public void addMidiParameter(MidiParameter midiParameter) {
		midiParametersControls.remove(midiParameter);
		midiParametersControls.add(midiParameter);
		midiParameter.deleteObserver(this);
		midiParameter.addObserver(this);
	}

	public void removeMidiParameter(MidiParameter midiParameter) {
		midiParameter.deleteObserver(this);
		midiParameter.addObserver(this);
	}

	public int getGlobalChannel() {
		return globalChannel;
	}

	public void setGlobalChannel(int globalChannel) {
		this.globalChannel = globalChannel;

		Iterator it = midiParametersControls.iterator();

		while (it.hasNext()) {
			MidiParameterControl midiParameter = (MidiParameterControl) it
					.next();
			if (midiParameter.isGlobal()) {
				midiParameter.setMidiChannel(globalChannel);
			}
		}

		setChanged();
		notifyObservers();
		clearChanged();
	}

	public void close() {

	}

	public void send(MidiMessage message, long timestamp) {
		Object[] parametersArray = midiParametersControls.toArray();

		for (int p = 0; p < parametersArray.length; p++) {
			if (parametersArray[p] != null) {
				((MidiParameterControl) parametersArray[p]).send(message,
						timestamp);
			}
		}
	}

	public void update(Observable observable, Object object) {
		if (object == MidiParameterControl.GLOBAL) {
			if (((MidiParameterControl) observable).isGlobal()) {
				((MidiParameterControl) observable)
						.setMidiChannel(globalChannel);
			}
		}
	}
}
