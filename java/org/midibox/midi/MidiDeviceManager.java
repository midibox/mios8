/*
 * @(#)MidiDeviceManager.java	beta7	2006/04/23
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

import java.util.Observable;
import java.util.Vector;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Sequencer;
import javax.sound.midi.Synthesizer;

public class MidiDeviceManager extends Observable {

	private Vector midiReadDevices;

	private Vector midiWriteDevices;

	private Vector selectedMidiReadDevices;

	private Vector selectedMidiWriteDevices;

	public MidiDeviceManager() {
		midiReadDevices = new Vector();
		midiWriteDevices = new Vector();
		selectedMidiReadDevices = new Vector();
		selectedMidiWriteDevices = new Vector();
		rescanDevices();
	}

	public Vector getMidiReadDevices() {
		return midiReadDevices;
	}

	public Vector getMidiWriteDevices() {
		return midiWriteDevices;
	}

	public Vector getSelectedMidiReadDevices() {
		return selectedMidiReadDevices;
	}

	public Vector getSelectedMidiWriteDevices() {
		return selectedMidiWriteDevices;
	}

	public void selectMidiReadDevice(MidiDevice midiDevice) {
		selectedMidiReadDevices.remove(midiDevice);
		selectedMidiReadDevices.add(midiDevice);

		setChanged();
		notifyObservers(midiDevice);
		clearChanged();
	}

	public void selectMidiWriteDevice(MidiDevice midiDevice) {
		selectedMidiWriteDevices.remove(midiDevice);
		selectedMidiWriteDevices.add(midiDevice);

		setChanged();
		notifyObservers(midiDevice);
		clearChanged();
	}

	public void deselectMidiReadDevice(MidiDevice midiDevice) {
		selectedMidiReadDevices.remove(midiDevice);

		setChanged();
		notifyObservers(midiDevice);
		clearChanged();
	}

	public void deselectMidiWriteDevice(MidiDevice midiDevice) {
		selectedMidiWriteDevices.remove(midiDevice);

		setChanged();
		notifyObservers(midiDevice);
		clearChanged();
	}
	
	public void rescanDevices() {
		String hashStr = "";
		MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();

		for (int i = 0; i < infos.length; i++) {
			try {
				MidiDevice device = MidiSystem.getMidiDevice(infos[i]);
				
				if (!(device instanceof Sequencer)
						&& !(device instanceof Synthesizer)) {

					int noReceivers = device.getMaxReceivers();
					int noTransmitters = device.getMaxTransmitters();

					if (noReceivers != 0) {
						if (!midiWriteDevices.contains(device)) {
							midiWriteDevices.add(device);
							selectMidiWriteDevice(device);
						}
					}

					if (noTransmitters != 0) {
						if (!midiReadDevices.contains(device)) {
							midiReadDevices.add(device);
							selectMidiReadDevice(device);
						}
					}
				}
			} catch (MidiUnavailableException e) {

			}
		}
	}
}
