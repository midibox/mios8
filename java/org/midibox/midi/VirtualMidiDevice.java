/*
 * @(#)VitualMidiDevice.java	beta8	2006/04/23
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
import java.util.LinkedList;
import java.util.List;
import java.util.Observable;

import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.Transmitter;

public class VirtualMidiDevice extends Observable implements MidiDevice {

	public static Object NAME = new Object();

	protected VirtualMidiDevice.Info info;

	protected int maxNoTransmitters;

	protected int maxNoReceivers;

	protected LinkedList transmitters;

	protected LinkedList receivers;

	protected MidiOutPort midiOutPort;

	protected long timeStart = System.currentTimeMillis();

	public VirtualMidiDevice(String name, int maxNoTransmitters,
			int maxNoReceivers) {

		setName(name);

		this.maxNoTransmitters = maxNoTransmitters;
		this.maxNoReceivers = maxNoReceivers;
		this.transmitters = new LinkedList();
		this.receivers = new LinkedList();
		this.midiOutPort = new MidiOutPort();
	}

	public void setName(String name) {
		this.info = new VirtualMidiDevice.MyInfo(name, "midibox.org",
				"Virtual MIDI Device", "v1.0");

		setChanged();

		notifyObservers(NAME);

		clearChanged();
	}

	public void close() {

	}

	public int getMaxReceivers() {
		return maxNoReceivers;
	}

	public int getMaxTransmitters() {
		return maxNoTransmitters;
	}

	public long getMicrosecondPosition() {
		return (System.currentTimeMillis() - timeStart) * 1000;
	}

	public Receiver getReceiver() throws MidiUnavailableException {
		MyReceiver receiver = new MyReceiver();
		receivers.add(receiver);
		return receiver;
	}

	public List getTransmitters() {
		return (LinkedList) transmitters.clone();
	}

	public boolean isOpen() {
		return true;
	}

	public void open() throws MidiUnavailableException {

	}

	public List getReceivers() {
		return (LinkedList) receivers.clone();
	}

	public MidiOutPort getMidiOutPort() {
		return midiOutPort;
	}

	public Transmitter getTransmitter() {
		MyTransmitter transmitter = new MyTransmitter();
		transmitters.add(transmitter);
		return transmitter;
	}

	public MidiDevice.Info getDeviceInfo() {
		return info;
	}

	protected void receiveFromReceivers(MidiMessage message, long timestamp) {

	}

	protected void sendToTransmitters(MidiMessage message, long timeStamp) {
		synchronized (transmitters) {
			Iterator it = transmitters.iterator();
			while (it.hasNext()) {
				Transmitter transmitter = (Transmitter) it.next();
				if (transmitter != null) {
					Receiver receiver = transmitter.getReceiver();
					if (receiver != null) {
						receiver.send(message, timeStamp);
					}
				}
			}
		}
	}

	public class MyReceiver implements Receiver {

		public void close() {
			synchronized (receivers) {
				receivers.remove(this);
			}
		}

		public void send(MidiMessage message, long timeStamp) {
			receiveFromReceivers(message, timeStamp);
		}
	}

	public class MyTransmitter implements Transmitter {

		protected Receiver receiver;

		public void close() {
			synchronized (transmitters) {
				transmitters.remove(this);
			}
		}

		public Receiver getReceiver() {
			return receiver;
		}

		public void setReceiver(Receiver receiver) {
			this.receiver = receiver;
		}
	}

	public class MyInfo extends MidiDevice.Info {

		protected MyInfo(String name, String vendor, String description,
				String version) {
			super(name, vendor, description, version);
		}
	}

	public class MidiOutPort implements Receiver {
		public void close() {

		}

		public void send(MidiMessage message, long timestamp) {
			sendToTransmitters(message, timestamp);
		}
	}
}
