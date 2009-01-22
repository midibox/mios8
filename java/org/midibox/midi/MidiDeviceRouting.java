/*
 * @(#)MidiDeviceRouting.java	beta8	2006/04/23
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

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.Transmitter;

public class MidiDeviceRouting extends Observable {

	public static int DISCONNECTED = 0;

	public static int LOGICALLY_CONNECTED = 1;

	public static int PHYSICALLY_CONNECTED = 2;

	private Vector midiReadDevices;

	private Vector midiWriteDevices;

	private Vector logicalConnections;

	private boolean portsReleased;

	public MidiDeviceRouting() {
		midiReadDevices = new Vector();
		midiWriteDevices = new Vector();

		logicalConnections = new Vector();
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

		// TK: to avoid a "java.util.ConcurrentModificationException" under
		// MacOS (using mmj)
		// when a transmitter or receiver is closed, we put them into a list and
		// execute close()
		// after iteration
		List closeTransmitters = new ArrayList();
		List closeReceivers = new ArrayList();

		Iterator it = transmittingDevice.getTransmitters().iterator();
		while (it.hasNext()) {

			Transmitter transmitter = (Transmitter) it.next();

			Iterator it2 = receivingDevice.getReceivers().iterator();
			while (it2.hasNext()) {

				Receiver receiver = (Receiver) it2.next();

				if (transmitter.getReceiver() == receiver) {

					// transmitter.close();
					closeTransmitters.add(transmitter);

					// receiver.close();
					closeReceivers.add(receiver);
				}
			}
		}

		int i;
		for (i = 0; i < closeTransmitters.size(); ++i) {
			Transmitter transmitter = (Transmitter) closeTransmitters.get(i);
			transmitter.close();
		}

		for (i = 0; i < closeReceivers.size(); ++i) {
			Receiver receiver = (Receiver) closeReceivers.get(i);
			receiver.close();
		}

		int num_transmitters = (transmittingDevice.getTransmitters() != null) ? transmittingDevice
				.getTransmitters().size()
				: 0;
		int num_receivers = (transmittingDevice.getReceivers() != null) ? transmittingDevice
				.getReceivers().size()
				: 0;

		if (num_transmitters == 0 && num_receivers == 0) {
			transmittingDevice.close();
		}

		num_transmitters = (receivingDevice.getTransmitters() != null) ? receivingDevice
				.getTransmitters().size()
				: 0;
		num_receivers = (receivingDevice.getReceivers() != null) ? receivingDevice
				.getReceivers().size()
				: 0;

		if (num_transmitters == 0 && num_receivers == 0) {
			receivingDevice.close();
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
