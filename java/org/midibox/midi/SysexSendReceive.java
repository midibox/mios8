/*
 * @(#)SysexSendReceive.java	beta8	2006/04/23
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

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.util.LinkedList;
import java.util.Observable;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

public class SysexSendReceive extends Observable implements Receiver {

	public final static Object WORKER = new Object();

	public final static Object SEND_SYSEX_FILE = new Object();

	public final static Object RECEIVE_SYSEX_FILE = new Object();

	public final static Object SEND_DELAY_TIME = new Object();

	public final static Object RECEIVED_BYTES = new Object();

	protected Receiver receiver;

	protected File sendFile;
	protected long sendFileLastModified;

	protected File receiveFile;

	protected SysexFile sysexFile;

	protected int sendBufferSize;

	protected int sendDelayTime;

	protected LinkedList receivedBytes;

	protected boolean sendDone = true;
	protected boolean sendCancelled = true;

	protected boolean receiveStopped = true;

	public SysexSendReceive(Receiver receiver) {
		super();
		this.receiver = receiver;
		sendBufferSize = 100000; // TK: shouldn't be changed by user since Java
									// doesn't allow to split SysEx streams
									// properly
		sendDelayTime = 750; // delay between F0
		receivedBytes = new LinkedList();
	}

	public File getSendFile() {
		return sendFile;
	}

	public boolean sendFileChanged() {
		return sendFile.lastModified() != sendFileLastModified;
	}

	public void setSendFile(File file) {

		this.sendFile = file;

		sendFileLastModified = sendFile.lastModified();

		readSysexFile();

		setChanged();
		notifyObservers(SEND_SYSEX_FILE);
		clearChanged();
	}

	public void readSysexFile() {
		sysexFile = new SysexFile();
		if (!sysexFile.read(sendFile.getPath())) {
			// TODO
		}
	}

	public File getReceiveFile() {
		return receiveFile;
	}

	public void setReceiveFile(File file) {

		this.receiveFile = file;

		setChanged();
		notifyObservers(RECEIVE_SYSEX_FILE);
		clearChanged();
	}

	public SysexFile getSysexFile() {
		return sysexFile;
	}

	public int getSendBufferSize() {
		return sendBufferSize;
	}

	public int getSendDelayTime() {
		return sendDelayTime;
	}

	public void setSendDelayTime(int sendDelayTime) {

		this.sendDelayTime = sendDelayTime;

		setChanged();
		notifyObservers(SEND_DELAY_TIME);
		clearChanged();
	}

	public LinkedList getReceivedBytes() {
		return receivedBytes;
	}

	public void clearReceivedBytes() {
		receivedBytes.clear();
	}

	public boolean writeSysexFile(File file, byte[] data, int dataLen) {
		// could also be located in SysexFile.java?

		try {
			OutputStream os = new FileOutputStream(file);
			os.write(data, 0, dataLen);
			os.close();
		} catch (Exception e) {
			return false;
		}

		return true;
	}

	public boolean isSendDone() {
		return sendDone;
	}

	public boolean isSendCancelled() {
		return sendCancelled;
	}

	public void sendCancel() {
		synchronized (this) {
			sendCancelled = true;
			// addMessage("Process stopped by user");
			notify(); // TK: finally the reason why threads where not informed
			// about a cancel request and mixed the upload blocks!
		}
	}

	public boolean isReceiveStopped() {
		return receiveStopped;
	}

	public void setReceiveStopped(boolean receiveStopped) {
		this.receiveStopped = receiveStopped;
	}

	public void close() {

	}

	public void setReceiver(Receiver receiver) {
		this.receiver = receiver;
	}

	public void send(MidiMessage message, long timestamp) {
		if (!isReceiveStopped()) {
			if (message instanceof SysexMessage) {
				receivedBytes.add(((SysexMessage) message).getData());

				setChanged();
				notifyObservers(RECEIVED_BYTES);
				clearChanged();
			}
		}
	}

	// ///////////////////////////////////////////////////////////////////////////////
	// SysEx File Sender Functions
	// ///////////////////////////////////////////////////////////////////////////////

	public void startSend(byte[] sysexData, int sysexDataLen) {
		int offset = 0;

		sendCancelled = false;
		sendDone = false;

		synchronized (this) {

			while (!sendCancelled && !sendDone && offset < sysexDataLen) {
				int sendLen = 1;
				int i;

				// update GUI
				setChanged();
				notifyObservers(WORKER);
				clearChanged();

				for (i = 1; i < sendBufferSize && (i + offset) < sysexDataLen
						&& sysexData[i + offset] != (byte) 0xf0; ++i) {
					++sendLen;
				}

				if (sendLen == 0) {
					sendDone = true;
					break;
				}

				byte[] axSysExData = new byte[sendLen];
				System.arraycopy(sysexData, offset, axSysExData, 0, sendLen);

				try {
					SysexMessage sysExMessage = new SysexMessage();
					sysExMessage.setMessage(axSysExData, axSysExData.length);
					receiver.send(sysExMessage, -1);
				} catch (InvalidMidiDataException ex) {
					sendCancelled = true;
					System.out.println("Error: " + ex.getMessage());
					break;
				}

				offset += sendLen;

				if (offset < sysexDataLen) {
					try {
						wait(sendDelayTime);
					} catch (InterruptedException e) {
						sendCancelled = true;
						System.out.println("Error: SysEx task interrupted");
						break;
					}
				} else {
					sendDone = true;
				}
			}
		}

		// update GUI
		setChanged();
		notifyObservers(WORKER);
		clearChanged();
	}
}
