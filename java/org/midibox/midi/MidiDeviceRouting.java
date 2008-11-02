/*
 * @(#)MidiDeviceRouting.java	beta7	2006/04/23
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

import java.util.Collection;
import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.Sequencer;
import javax.sound.midi.Synthesizer;
import javax.sound.midi.Transmitter;
import javax.swing.JOptionPane;

import org.midibox.sidlibr.SysExControllerDevice;

import java.util.prefs.*;
public class MidiDeviceRouting extends Observable {
	private Vector midiReadDevices;
	private Vector midiWriteDevices;
	
	private MidiDevice localMidiDevice;
	private MidiDevice inputMidiDevice;
	private MidiDevice outputMidiDevice;
	
	public MidiDeviceRouting(MidiDevice localMidiDevice) {
		this.localMidiDevice = localMidiDevice;	
		
		midiReadDevices = new Vector();
		midiWriteDevices = new Vector();	
		
		rescanDevices();
		if (getMidiReadDevices().size() < 1) {
			JOptionPane.showMessageDialog(null,"Your system does not appear to have any MIDI-devices connected (please close program)!.","Error",JOptionPane.OK_OPTION);
		} else {
			inputMidiDevice = (MidiDevice)getMidiReadDevices().elementAt(0);
			outputMidiDevice = (MidiDevice)getMidiWriteDevices().elementAt(0);		
			connectDevices(inputMidiDevice, localMidiDevice);
			connectDevices(localMidiDevice, outputMidiDevice);	
		}
	}

	public Vector getMidiReadDevices() {
		return midiReadDevices;
	}

	public Vector getMidiWriteDevices() {
		return midiWriteDevices;
	}
	
	public int getInputDeviceIndex() {
		return midiReadDevices.indexOf(inputMidiDevice);
	}
	
	public int getOutputDeviceIndex() {
		return midiWriteDevices.indexOf(outputMidiDevice);		
	}

	public void reconnectAllDevices() {		// This function is a workaround for the SysEx (string length doesn't reset) bug in the javax.sound.midi class
		disconnectDevices(inputMidiDevice, localMidiDevice);
		connectDevices(inputMidiDevice, localMidiDevice);
		disconnectDevices(localMidiDevice, outputMidiDevice);
		connectDevices(localMidiDevice, outputMidiDevice);
	}
	
	public int findInputDeviceHash(int hash) {
		int temp = -1;
		for(int i=0;i<midiReadDevices.size();i++) {			
			if (((MidiDevice)midiReadDevices.elementAt(i)).hashCode()==hash) {
				temp = i;
				break;
			}
		}
		return temp;
	}
	
	public int findOuputDeviceHash(int hash) {
		int temp = -1;
		for(int i=0;i<midiWriteDevices.size();i++) {			
			if (((MidiDevice)midiWriteDevices.elementAt(i)).hashCode()==hash) {
				temp = i;
				break;
			}
		}
		return temp;
	}
	
	public int getInputDeviceHash() {
		return inputMidiDevice.hashCode();
	}
	
	public int getOutputDeviceHash() {
		return outputMidiDevice.hashCode();
	}
	
	public void closeMidi() {
		disconnectDevices(inputMidiDevice, localMidiDevice);
		disconnectDevices(localMidiDevice, outputMidiDevice);
	}
		
	public void setInputDevice(int index) {		
		disconnectDevices(inputMidiDevice, localMidiDevice);
		inputMidiDevice = (MidiDevice)midiReadDevices.elementAt(index);		
		connectDevices(inputMidiDevice, localMidiDevice);
		setChanged();
		notifyObservers();
		clearChanged();
	}
	
	public void setOutputDevice(int index) {
		disconnectDevices(localMidiDevice, outputMidiDevice);
		outputMidiDevice = (MidiDevice)midiWriteDevices.elementAt(index);
		connectDevices(localMidiDevice, outputMidiDevice);
		setChanged();
		notifyObservers();
		clearChanged();
	}
	
	private void connectDevices(MidiDevice transmittingDevice,
			MidiDevice receivingDevice) {
		try {
			if (!receivingDevice.isOpen()) {
				receivingDevice.open();
			}

			Receiver receiver = receivingDevice.getReceiver();

			if (!transmittingDevice.isOpen()) {
				transmittingDevice.open();
			}

			transmittingDevice.getTransmitter().setReceiver(receiver);
			
		} catch (MidiUnavailableException e) {
			
		} catch (NullPointerException e) {
			
		}
	}
	
	private void disconnectDevices(MidiDevice transmittingDevice,
			MidiDevice receivingDevice) {

		Iterator it = transmittingDevice.getTransmitters().iterator();
		while (it.hasNext()) {

			Transmitter transmitter = (Transmitter) it.next();

			Iterator it2 = receivingDevice.getReceivers().iterator();
			while (it2.hasNext()) {

				Receiver receiver = (Receiver) it2.next();

				if (transmitter.getReceiver() == receiver) {

					transmitter.close();
					if (transmittingDevice.getTransmitters().size() == 0
							&& transmittingDevice.getReceivers().size() == 0) {
						transmittingDevice.close();
					}

					receiver.close();
					if (receivingDevice.getTransmitters().size() == 0
							&& receivingDevice.getReceivers().size() == 0) {
						receivingDevice.close();
					}
				}
			}
		}
	}
	
	public void rescanDevices() {
		MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();

		Vector tempWriteDevices = new Vector();
		Vector tempReadDevices = new Vector();
		
		for (int i = 0; i < infos.length; i++) {
			try {
				MidiDevice device = MidiSystem.getMidiDevice(infos[i]);
				
				if (!(device instanceof Sequencer)
						&& !(device instanceof Synthesizer)) {

					int noReceivers = device.getMaxReceivers();
					int noTransmitters = device.getMaxTransmitters();

					if (noReceivers != 0) {
						tempWriteDevices.add(device);
						setChanged();
						notifyObservers(device);
						clearChanged();
					}

					if (noTransmitters != 0) {
						tempReadDevices.add(device);
						setChanged();
						notifyObservers(device);
						clearChanged();
					}
				}
			} catch (MidiUnavailableException e) {

			}
		}
		
		midiReadDevices = tempReadDevices; 		
		setChanged();
		notifyObservers(midiReadDevices);
		clearChanged();
		
		midiWriteDevices = tempWriteDevices; 
		setChanged();
		notifyObservers(midiWriteDevices);
		clearChanged();
	}	
}
