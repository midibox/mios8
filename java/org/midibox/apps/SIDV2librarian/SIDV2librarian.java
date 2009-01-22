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

package org.midibox.apps.SIDV2librarian;

import javax.sound.midi.MidiDevice;

import org.midibox.midi.MidiDeviceManager;
import org.midibox.midi.MidiDeviceRouting;
import org.midibox.sidlibr.SIDLibController;
import org.midibox.sidlibr.SysExControllerDevice;

public class SIDV2librarian {

	private MidiDeviceRouting midiDeviceRouting;
	private SysExControllerDevice sysexControllerDevice;
	private SIDLibController sidLibController;

	private MidiDevice localMidiDevice;
	private MidiDevice inputMidiDevice;
	private MidiDevice outputMidiDevice;

	private MidiDeviceManager midiDeviceManager;

	public SIDV2librarian() {

		this(new MidiDeviceManager(), new MidiDeviceRouting());
	}

	public SIDV2librarian(MidiDeviceManager midiDeviceManager,
			MidiDeviceRouting midiDeviceRouting) {

		this.midiDeviceManager = midiDeviceManager;

		this.midiDeviceRouting = midiDeviceRouting;

		sysexControllerDevice = new SysExControllerDevice(
				"MidiBox SID V2 Editor");

		this.localMidiDevice = sysexControllerDevice;

		sidLibController = new SIDLibController(sysexControllerDevice
				.getSysExController());

		if (midiDeviceRouting != null) {
			midiDeviceManager.rescanDevices();
			midiDeviceRouting.addMidiReadDevices(midiDeviceManager
					.getSelectedMidiReadDevices());
			midiDeviceRouting.addMidiWriteDevices(midiDeviceManager
					.getSelectedMidiWriteDevices());
		}
	}

	public MidiDeviceManager getMidiDeviceManager() {
		return midiDeviceManager;
	}

	public MidiDeviceRouting getMidiDeviceRouting() {
		return midiDeviceRouting;
	}

	public SysExControllerDevice getSysExControllerDevice() {
		return sysexControllerDevice;
	}

	public SIDLibController getSIDLibController() {
		return sidLibController;
	}

	public int getInputDeviceIndex() {
		return midiDeviceRouting.getMidiReadDevices().indexOf(inputMidiDevice);
	}

	public int getOutputDeviceIndex() {
		return midiDeviceRouting.getMidiWriteDevices()
				.indexOf(outputMidiDevice);
	}

	public MidiDevice getLocalMidiDevice() {
		return localMidiDevice;
	}

	public MidiDevice getInputMidiDevice() {
		return inputMidiDevice;
	}

	public MidiDevice getOutputMidiDevice() {
		return outputMidiDevice;
	}

	public void reconnectAllDevices() { // This function is a workaround for the
		// SysEx (string length doesn't reset)
		// bug in the javax.sound.midi class

		if (midiDeviceRouting != null) {

			System.out.println("Now reconnecting!");
			midiDeviceRouting.disconnectDevices(inputMidiDevice,
					localMidiDevice);
			System.out.println("Disconnection of "
					+ inputMidiDevice.getDeviceInfo() + " succesfull");
			midiDeviceRouting.disconnectDevices(localMidiDevice,
					outputMidiDevice);
			System.out.println("Disconnection of "
					+ outputMidiDevice.getDeviceInfo() + " succesfull");

			midiDeviceRouting.connectDevices(inputMidiDevice, localMidiDevice);
			System.out.println("Reconnection of "
					+ inputMidiDevice.getDeviceInfo() + " succesfull");
			midiDeviceRouting.connectDevices(localMidiDevice, outputMidiDevice);
			System.out.println("Reconnection of "
					+ outputMidiDevice.getDeviceInfo() + " succesfull");
		}

	}

	public int findInputDeviceHash(int hash) {
		int temp = -1;
		for (int i = 0; i < midiDeviceRouting.getMidiReadDevices().size(); i++) {
			if (((MidiDevice) midiDeviceRouting.getMidiReadDevices().elementAt(
					i)).getDeviceInfo().toString().hashCode() == hash) {
				temp = i;
				break;
			}
		}
		return temp;
	}

	public int findOuputDeviceHash(int hash) {
		int temp = -1;
		for (int i = 0; i < midiDeviceRouting.getMidiWriteDevices().size(); i++) {
			if (((MidiDevice) midiDeviceRouting.getMidiWriteDevices()
					.elementAt(i)).getDeviceInfo().toString().hashCode() == hash) {
				temp = i;
				break;
			}
		}
		return temp;
	}

	public int getInputDeviceHash() {
		return inputMidiDevice.getDeviceInfo().toString().hashCode();
	}

	public int getOutputDeviceHash() {
		return outputMidiDevice.getDeviceInfo().toString().hashCode();
	}

	public void closeMidi() {
		midiDeviceRouting.disconnectDevices(inputMidiDevice, localMidiDevice);
		midiDeviceRouting.disconnectDevices(localMidiDevice, outputMidiDevice);
	}

	public void setInputDeviceIndex(int index) {
		if (index != getInputDeviceIndex()) {
			midiDeviceRouting.disconnectDevices(inputMidiDevice,
					localMidiDevice);
			inputMidiDevice = (MidiDevice) midiDeviceRouting
					.getMidiReadDevices().elementAt(index);
			midiDeviceRouting.connectDevices(inputMidiDevice, localMidiDevice);
			System.out.println("Connection of "
					+ inputMidiDevice.getDeviceInfo() + " succesfull");
		}
	}

	public void setOutputDeviceIndex(int index) {
		if (index != getOutputDeviceIndex()) {
			midiDeviceRouting.disconnectDevices(localMidiDevice,
					outputMidiDevice);
			outputMidiDevice = (MidiDevice) midiDeviceRouting
					.getMidiWriteDevices().elementAt(index);
			midiDeviceRouting.connectDevices(localMidiDevice, outputMidiDevice);
			System.out.println("Connection of "
					+ outputMidiDevice.getDeviceInfo() + " succesfull");
		}
	}
}
