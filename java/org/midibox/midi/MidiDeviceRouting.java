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
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.Transmitter;
import java.util.prefs.*;
public class MidiDeviceRouting extends Observable implements Observer {

	public static int DISCONNECTED = 0;

	public static int LOGICALLY_CONNECTED = 1;

	public static int PHYSICALLY_CONNECTED = 2;

	private Vector midiReadDevices;

	private Vector midiWriteDevices;

	private Vector logicalConnections;

	private MidiDeviceManager midiDeviceManager;

	private MidiRouterDeviceManager midiRouterDeviceManager;

	private MidiFilterDeviceManager midiFilterManager;

	private MidiMapDeviceManager midiMapManager;

	private boolean portsReleased;

	public MidiDeviceRouting() {
		midiReadDevices = new Vector();
		midiWriteDevices = new Vector();

		logicalConnections = new Vector();
		
		midiDeviceManager = new MidiDeviceManager();
		midiDeviceManager.addObserver(this);

		midiRouterDeviceManager = new MidiRouterDeviceManager();
		midiRouterDeviceManager.addObserver(this);

		midiFilterManager = new MidiFilterDeviceManager();
		midiFilterManager.addObserver(this);

		midiMapManager = new MidiMapDeviceManager();
		midiMapManager.addObserver(this);		
	}

	public Vector getMidiReadDevices() {
		return midiReadDevices;
	}

	public Vector getMidiWriteDevices() {
		return midiWriteDevices;
	}

	public void addMidiReadDevice(MidiDevice midiDevice) {
		midiReadDevices.add(midiDevice);
		setChanged();
		notifyObservers(midiReadDevices);
		clearChanged();
	}

	public void addMidiWriteDevice(MidiDevice midiDevice) {
		midiWriteDevices.add(midiDevice);
		setChanged();
		notifyObservers(midiReadDevices);
		clearChanged();
	}
	
	public int hashCode() {
		int i = 0;
		Iterator it1 = midiWriteDevices.iterator();
		while (it1.hasNext()) {
			MidiDevice midiDevice = (MidiDevice) it1.next();
			i = i + midiDevice.getDeviceInfo().getName().hashCode();
		}
		Iterator it2 = midiReadDevices.iterator();
		while (it2.hasNext()) {
			MidiDevice midiDevice = (MidiDevice) it2.next();
			i = i + midiDevice.getDeviceInfo().getName().hashCode();
		}
		return i;
	}

	public void addMidiReadDevices(Collection midiDevices) {
		Iterator it = midiDevices.iterator();

		while (it.hasNext()) {
			MidiDevice midiDevice = (MidiDevice) it.next();
			addMidiReadDevice(midiDevice);
		}
	}

	public void addMidiWriteDevices(Collection midiDevices) {
		Iterator it = midiDevices.iterator();

		while (it.hasNext()) {
			MidiDevice midiDevice = (MidiDevice) it.next();
			addMidiWriteDevice(midiDevice);
		}
	}

	public void removeMidiReadDevice(MidiDevice midiDevice) {
		midiReadDevices.remove(midiDevice);
		setChanged();
		notifyObservers(midiReadDevices);
		clearChanged();
	}

	public void removeMidiWriteDevice(MidiDevice midiDevice) {
		midiWriteDevices.remove(midiDevice);
		setChanged();
		notifyObservers(midiWriteDevices);
		clearChanged();
	}

	public void removeMidiReadDevices(Collection midiDevices) {
		Iterator it = midiDevices.iterator();

		while (it.hasNext()) {
			MidiDevice midiDevice = (MidiDevice) it.next();
			removeMidiReadDevice(midiDevice);
		}
	}

	public void removeMidiWriteDevices(Collection midiDevices) {
		Iterator it = midiDevices.iterator();

		while (it.hasNext()) {
			MidiDevice midiDevice = (MidiDevice) it.next();
			removeMidiWriteDevice(midiDevice);
		}
	}

	public void removeAllMidiReadDevices() {
		midiReadDevices.removeAllElements();
		setChanged();
		notifyObservers(midiReadDevices);
		clearChanged();
	}

	public void removeAllMidiWriteDevices() {
		midiWriteDevices.removeAllElements();
		setChanged();
		notifyObservers(midiWriteDevices);
		clearChanged();
	}

	public MidiDeviceManager getMidiDeviceManager() {
		return midiDeviceManager;
	}

	public MidiRouterDeviceManager getMidiRouterDeviceManager() {
		return midiRouterDeviceManager;
	}

	public MidiFilterDeviceManager getMidiFilterManager() {
		return midiFilterManager;
	}

	public MidiMapDeviceManager getMidiMapManager() {
		return midiMapManager;
	}
	
	public void setConnectionMatrix(int[][] matrix) {
		disconnectAll();
		for(int r=0;r<matrix.length;r++) {
			MidiDevice transmittingDevice = (MidiDevice) midiReadDevices.elementAt(r);
			for(int w=0;w<matrix[r].length;w++) {
				MidiDevice receivingDevice = (MidiDevice) midiWriteDevices.elementAt(w);
				if ((matrix[r][w] == PHYSICALLY_CONNECTED) || (matrix[r][w] == LOGICALLY_CONNECTED)) {					
					connectDevices(transmittingDevice, receivingDevice);
				}				
			}
		}		
	}
	
	public int[][] getConnectionMatrix() {
		int[][] matrix = new int[midiReadDevices.size()][midiWriteDevices.size()];		
		for(int r=0;r<midiReadDevices.size();r++) {
			MidiDevice transmittingDevice = (MidiDevice) midiReadDevices.elementAt(r);
			for(int w=0;w<midiWriteDevices.size();w++) {
				MidiDevice receivingDevice = (MidiDevice) midiWriteDevices.elementAt(w);
				matrix[r][w] = devicesConnected(transmittingDevice, receivingDevice);
			}
		}		
		return matrix;
	}
	
	public void reconnectAllDevices() {		// This function is a workaround for the SysEx (string length doesn't reset) bug in the javax.sound.midi class
		Iterator it = midiReadDevices.iterator();
		while (it.hasNext()) {
			MidiDevice transmittingDevice = (MidiDevice) it.next();
			Iterator it2 = midiWriteDevices.iterator();
			while (it2.hasNext()) {
				MidiDevice receivingDevice = (MidiDevice) it2.next();				
				if ((devicesConnected(transmittingDevice, receivingDevice) == PHYSICALLY_CONNECTED) || 
					(devicesConnected(transmittingDevice, receivingDevice) == LOGICALLY_CONNECTED)) {
					disconnectDevices(transmittingDevice, receivingDevice);
					connectDevices(transmittingDevice, receivingDevice);
				}				
			}
		}		
	}

	public void connectDevices(MidiDevice transmittingDevice,
			MidiDevice receivingDevice) {

		if (devicesConnected(transmittingDevice, receivingDevice) == PHYSICALLY_CONNECTED) {
			return;
		}

		logicallyConnectDevices(transmittingDevice, receivingDevice);

		if (!portsReleased
				|| ((transmittingDevice instanceof VirtualMidiDevice) && (receivingDevice instanceof VirtualMidiDevice))) {

			physicallyConnectDevices(transmittingDevice, receivingDevice);
		}

		setChanged();
		notifyObservers();
		clearChanged();
	}

	private void physicallyConnectDevices(MidiDevice transmittingDevice,
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

	private void logicallyConnectDevices(MidiDevice transmittingDevice,
			MidiDevice receivingDevice) {
		logicalConnections.add(new LogicalConnection(transmittingDevice,
				receivingDevice));
	}

	public void disconnectDevices(MidiDevice transmittingDevice,
			MidiDevice receivingDevice) {

		if (devicesConnected(transmittingDevice, receivingDevice) == DISCONNECTED) {
			return;
		}

		logicallyDisconnectDevices(transmittingDevice, receivingDevice);
		if (!portsReleased
				|| ((transmittingDevice instanceof VirtualMidiDevice) && (receivingDevice instanceof VirtualMidiDevice))) {

			physicallyDisconnectDevices(transmittingDevice, receivingDevice);
		}
		setChanged();
		notifyObservers();
		clearChanged();
	}

	private void physicallyDisconnectDevices(MidiDevice transmittingDevice,
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

	private void logicallyDisconnectDevices(MidiDevice transmittingDevice,
			MidiDevice receivingDevice) {
		logicalConnections.remove(new LogicalConnection(transmittingDevice,
				receivingDevice));
	}

	public void disconnectDevice(MidiDevice midiDevice) {

		Vector matches = new Vector();

		Iterator it = logicalConnections.iterator();

		while (it.hasNext()) {
			LogicalConnection logicalConnection = (LogicalConnection) it.next();
			if (logicalConnection.getSourceDevice() == midiDevice
					|| logicalConnection.getTargetDevice() == midiDevice) {
				matches.add(logicalConnection);
			}
		}

		it = matches.iterator();

		while (it.hasNext()) {
			LogicalConnection logicalConnection = (LogicalConnection) it.next();
			disconnectDevices(logicalConnection.getSourceDevice(),
					logicalConnection.getTargetDevice());
		}
	}

	public void disconnectAll() {

		Iterator it = midiReadDevices.iterator();

		while (it.hasNext()) {

			MidiDevice transmittingDevice = (MidiDevice) it.next();

			Iterator it2 = midiWriteDevices.iterator();
			while (it2.hasNext()) {

				MidiDevice receivingDevice = (MidiDevice) it2.next();

				disconnectDevices(transmittingDevice, receivingDevice);
			}
		}
	}

	public void setPortsReleased(boolean portsReleased) {

		this.portsReleased = portsReleased;

		if (portsReleased) {

			Iterator it = logicalConnections.iterator();

			while (it.hasNext()) {
				LogicalConnection logicalConnection = (LogicalConnection) it
						.next();
				MidiDevice sourceDevice = logicalConnection.getSourceDevice();
				MidiDevice targetDevice = logicalConnection.getTargetDevice();

				if (!(sourceDevice instanceof VirtualMidiDevice)
						|| !(targetDevice instanceof VirtualMidiDevice)) {

					physicallyDisconnectDevices(sourceDevice, targetDevice);
				}
			}
		} else {

			Iterator it = logicalConnections.iterator();

			while (it.hasNext()) {
				LogicalConnection logicalConnection = (LogicalConnection) it
						.next();
				MidiDevice sourceDevice = logicalConnection.getSourceDevice();
				MidiDevice targetDevice = logicalConnection.getTargetDevice();

				if (!(sourceDevice instanceof VirtualMidiDevice)
						|| !(targetDevice instanceof VirtualMidiDevice)) {
					physicallyConnectDevices(sourceDevice, targetDevice);
				}
			}
		}

		setChanged();
		notifyObservers();
		clearChanged();
	}

	public boolean getPortsReleased() {
		return portsReleased;
	}

	public int devicesConnected(MidiDevice transmittingDevice,
			MidiDevice receivingDevice) {

		try {

			Iterator it = transmittingDevice.getTransmitters().iterator();

			while (it.hasNext()) {
				Transmitter transmitter = (Transmitter) it.next();

				Iterator it2 = receivingDevice.getReceivers().iterator();

				while (it2.hasNext()) {

					Receiver receiver = (Receiver) it2.next();

					if (transmitter.getReceiver() == receiver) {
						return PHYSICALLY_CONNECTED;
					}
				}
			}
		} catch (Exception e) {
			return DISCONNECTED;
		}

		if (logicalConnections.contains(new LogicalConnection(
				transmittingDevice, receivingDevice))) {
			return LOGICALLY_CONNECTED;
		}

		return DISCONNECTED;
	}

	public void reorder() {
		midiReadDevices.removeAll(midiDeviceManager
				.getSelectedMidiReadDevices());
		midiWriteDevices.removeAll(midiDeviceManager
				.getSelectedMidiWriteDevices());
		midiReadDevices.removeAll(midiRouterDeviceManager
				.getMidiRouterDevices());
		midiWriteDevices.removeAll(midiRouterDeviceManager
				.getMidiRouterDevices());
		midiReadDevices.removeAll(midiFilterManager.getMidiFilterDevices());
		midiWriteDevices.removeAll(midiFilterManager.getMidiFilterDevices());
		midiReadDevices.removeAll(midiMapManager.getMidiMapDevices());
		midiWriteDevices.removeAll(midiMapManager.getMidiMapDevices());

		Iterator it = midiDeviceManager.getMidiReadDevices().iterator();
		while (it.hasNext()) {

			Object object = it.next();

			if (midiDeviceManager.getSelectedMidiReadDevices().contains(object)) {
				midiReadDevices.add(object);
			}
		}

		it = midiDeviceManager.getMidiWriteDevices().iterator();
		while (it.hasNext()) {

			Object object = it.next();

			if (midiDeviceManager.getSelectedMidiWriteDevices()
					.contains(object)) {
				midiWriteDevices.add(object);
			}
		}

		midiReadDevices.addAll(midiRouterDeviceManager.getMidiRouterDevices());
		midiWriteDevices.addAll(midiRouterDeviceManager.getMidiRouterDevices());
		midiReadDevices.addAll(midiFilterManager.getMidiFilterDevices());
		midiWriteDevices.addAll(midiFilterManager.getMidiFilterDevices());
		midiReadDevices.addAll(midiMapManager.getMidiMapDevices());
		midiWriteDevices.addAll(midiMapManager.getMidiMapDevices());

		setChanged();
		notifyObservers(midiReadDevices);
		clearChanged();

		setChanged();
		notifyObservers(midiWriteDevices);
		clearChanged();
	}

	public void update(Observable observable, Object object) {
		if (observable == midiDeviceManager
				|| observable == midiRouterDeviceManager
				|| observable == midiFilterManager
				|| observable == midiMapManager) {

			MidiDevice midiDevice = (MidiDevice) object;

			if (midiReadDevices.contains(midiDevice)
					|| midiWriteDevices.contains(midiDevice)) {
				disconnectDevice(midiDevice);
				removeMidiReadDevice(midiDevice);
				removeMidiWriteDevice(midiDevice);
			} else {
				reorder();
			}
		}
	}

	public static class LogicalConnection {

		private MidiDevice sourceDevice;

		private MidiDevice targetDevice;

		public LogicalConnection(MidiDevice sourceDevice,
				MidiDevice targetDevice) {
			this.sourceDevice = sourceDevice;
			this.targetDevice = targetDevice;
		}

		public MidiDevice getSourceDevice() {
			return sourceDevice;
		}

		public MidiDevice getTargetDevice() {
			return targetDevice;
		}

		public boolean equals(Object object) {
			if (object instanceof LogicalConnection) {
				if (((LogicalConnection) object).getSourceDevice() == sourceDevice
						&& ((LogicalConnection) object).getTargetDevice() == targetDevice) {
					return true;
				}
			}
			return false;
		}
	}
}
