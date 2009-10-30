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
import java.util.LinkedList;
import java.util.Observable;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

public class SysexSendReceive extends Observable implements Receiver {

	public final static Object WORKER = new Object();

	protected Receiver receiver;

	protected File file;

	protected long fileLastModified;

	protected SysexFile sysexFile;

	protected SysexSendReceiveWorkerTask sysexSendReceiveWorkerTask;

	protected int sendBufferSize;

	protected int sendDelayTime;

	protected LinkedList receivedBytes;

	protected boolean done = true;

	protected boolean cancelled = true;

	public SysexSendReceive(Receiver receiver) {
		super();
		this.receiver = receiver;
		sendBufferSize = 100000; // TK: shouldn't be changed by user since Java doesn't allow to split SysEx streams properly
		sendDelayTime = 750; // delay between F0
		receivedBytes = new LinkedList();
	}

	public File getFile() {
		return file;
	}

	public boolean fileChanged() {
		return file.lastModified() != fileLastModified;
	}

	public void setFile(File file) {
		this.file = file;
		fileLastModified = file.lastModified();

		readSysexFile();

		setChanged();
		notifyObservers(file);
		clearChanged();
	}

	public void readSysexFile() {
		sysexFile = new SysexFile();
		if (!sysexFile.read(file.getPath())) {
			// TODO
		}
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
	}

	public LinkedList getReceivedBytes() {
		return receivedBytes;
	}



	public boolean isDone() {
		return done;
	}

	public boolean isCancelled() {
		return cancelled;
	}

	public void cancel() {
		synchronized (this) {
			cancelled = true;
			//addMessage("Process stopped by user");
			notify(); // TK: finally the reason why threads where not informed
			// about a cancel request and mixed the upload blocks!
		}
	}


	public void close() {

	}

	public void setReceiver(Receiver receiver) {
		this.receiver = receiver;
	}

	public class SysexSendReceiveWorkerTask extends Thread {

		public void run() {

		}
	}


	public void send(MidiMessage message, long timestamp) {
		if (!isCancelled() && !isDone()) {
			if (message instanceof SysexMessage) {
				receivedBytes.add(((SysexMessage) message).getData());

				setChanged();
				notifyObservers(receivedBytes);
				clearChanged();
			}
		}
	}


	/////////////////////////////////////////////////////////////////////////////////
	// SysEx File Sender Functions
	/////////////////////////////////////////////////////////////////////////////////


	public void startSend(byte[] sysexData, int sysexDataLen) {
		int offset = 0;

		cancelled = false;
		done = false;

		synchronized (this) {

			while (!cancelled && !done && offset < sysexDataLen ) {
				int sendLen = 1;
				int i;

				// update GUI
				setChanged();
				notifyObservers(WORKER);
				clearChanged();

				for(i=1; i<sendBufferSize && (i+offset)<sysexDataLen && sysexData[i+offset] != (byte)0xf0; ++i) {
					++sendLen;
				}

				if( sendLen == 0 ) {
					done = true;
					break;
				}

				byte[] axSysExData = new byte[sendLen];
				System.arraycopy(sysexData, offset, axSysExData, 0, sendLen);

				try {
					SysexMessage sysExMessage = new SysexMessage();
					sysExMessage.setMessage(axSysExData, axSysExData.length);
					receiver.send(sysExMessage, -1);
				} catch (InvalidMidiDataException ex) {
					cancelled = true;
					System.out.println("Error: " + ex.getMessage());
					break;
				}

				offset += sendLen;

				if( offset < sysexDataLen ) {
					try {
						wait(sendDelayTime);
					} catch (InterruptedException e) {
						cancelled = true;
						System.out.println("Error: SysEx task interrupted");
						break;
					}
				} else {
					done = true;
				}
			}
		}

		// update GUI
		setChanged();
		notifyObservers(WORKER);
		clearChanged();
	}
}
