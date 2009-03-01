/*
 * @(#)MidiDeviceManager.java	beta8	2006/04/23
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

import java.util.LinkedHashMap;
import java.util.Observable;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Sequencer;
import javax.sound.midi.Synthesizer;

public class MidiDeviceManager extends Observable {

	private LinkedHashMap midiReadDevices;

	private LinkedHashMap midiWriteDevices;

	private LinkedHashMap selectedMidiReadDevices;

	private LinkedHashMap selectedMidiWriteDevices;

	public MidiDeviceManager() {
		midiReadDevices = new LinkedHashMap();
		midiWriteDevices = new LinkedHashMap();
		selectedMidiReadDevices = new LinkedHashMap();
		selectedMidiWriteDevices = new LinkedHashMap();
		rescanDevices();
	}

	public LinkedHashMap getMidiReadDevices() {
		return midiReadDevices;
	}

	public LinkedHashMap getMidiWriteDevices() {
		return midiWriteDevices;
	}

	public LinkedHashMap getSelectedMidiReadDevices() {
		return selectedMidiReadDevices;
	}

	public LinkedHashMap getSelectedMidiWriteDevices() {
		return selectedMidiWriteDevices;
	}

	public void selectMidiReadDevice(MidiDevice midiDevice) {
		selectedMidiReadDevices.remove(midiDevice);
		selectedMidiReadDevices.put(""
				+ midiDevice.getDeviceInfo().toString().hashCode(), midiDevice);

		setChanged();
		notifyObservers(midiDevice);
		clearChanged();
	}

	public void selectMidiWriteDevice(MidiDevice midiDevice) {
		selectedMidiWriteDevices.remove(midiDevice);
		selectedMidiWriteDevices.put(""
				+ midiDevice.getDeviceInfo().toString().hashCode(), midiDevice);

		setChanged();
		notifyObservers(midiDevice);
		clearChanged();
	}

	public void deselectMidiReadDevice(MidiDevice midiDevice) {
		selectedMidiReadDevices.remove(""
				+ midiDevice.getDeviceInfo().toString().hashCode());

		setChanged();
		notifyObservers(midiDevice);
		clearChanged();
	}

	public void deselectMidiWriteDevice(MidiDevice midiDevice) {
		selectedMidiWriteDevices.remove(""
				+ midiDevice.getDeviceInfo().toString().hashCode());

		setChanged();
		notifyObservers(midiDevice);
		clearChanged();
	}

	public void rescanDevices() {

		MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();

		MidiDevice.Info mInfo;

		for (int i = 0; i < infos.length; i++) {

			try {

				MidiDevice device = MidiSystem.getMidiDevice(infos[i]);

				if (!(device instanceof Sequencer)
						&& !(device instanceof Synthesizer)) {

					int noReceivers = device.getMaxReceivers();
					int noTransmitters = device.getMaxTransmitters();

					if (noReceivers != 0) {
						if (!midiWriteDevices.containsKey(""
								+ device.getDeviceInfo().toString().hashCode())) {
							midiWriteDevices.put(""
									+ device.getDeviceInfo().toString()
											.hashCode(), device);
							selectMidiWriteDevice(device);
						}
					}

					if (noTransmitters != 0) {
						if (!midiReadDevices.containsKey(""
								+ device.getDeviceInfo().toString().hashCode())) {
							midiReadDevices.put(""
									+ device.getDeviceInfo().toString()
											.hashCode(), device);
							selectMidiReadDevice(device);
						}
					}
				}
			} catch (MidiUnavailableException e) {

			}

		}
	}
}
