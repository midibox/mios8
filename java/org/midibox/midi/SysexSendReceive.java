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

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

public class SysexSendReceive extends Observable implements Receiver {

	protected Receiver receiver;

	protected File file;

	protected long fileLastModified;

	protected SysexFile sysexFile;

	protected SysexSendReceiveWorkerTask sysexSendReceiveWorkerTask;

	protected int sendBufferSize;

	protected LinkedList receivedBytes;

	public SysexSendReceive(Receiver receiver) {
		super();
		this.receiver = receiver;
		sendBufferSize = 256;
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

	public void setSendBufferSize(int sendBufferSize) {
		this.sendBufferSize = sendBufferSize;
	}

	public LinkedList getReceivedBytes() {
		return receivedBytes;
	}

	public void send(MidiMessage message, long timestamp) {
		if (message instanceof SysexMessage) {
			receivedBytes.add(((SysexMessage) message).getData());

			setChanged();
			notifyObservers(receivedBytes);
			clearChanged();
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
}
