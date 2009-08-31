/*
 * @(#)MIOSSysexSendReceive.java	beta8	2006/04/23
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

package org.midibox.mios;

import java.util.LinkedList;
import java.util.Observable;

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

import org.midibox.utils.Utils;

public class MIOSSysexSendReceive extends Observable implements Receiver {

	public final static Object DEVICE_ID = new Object();

	public final static Object MIOS32_MODE = new Object();

	public final static Object MESSAGES = new Object();

	public final static Object WORKER = new Object();

	protected Receiver receiver;

	protected int deviceID;

	protected boolean mios32Mode;

	protected LinkedList messages;

	protected boolean done = true;

	protected boolean cancelled = true;

	protected static String[] _errorCodes = {
			"Less bytes than expected have been received", // 1
			"More bytes than expected have been received", // 2
			"Checksum mismatch", // 3
			"Write failed (verify error or invalid address)", // 4
			"Write access failed (invalid address range)", // 5
			"MIDI Time Out", // 6
			"Wrong Debug Command", // 7
			"Read/Write command tried to access an invalid address range", // 8
			"Read/Write address not correctly aligned", // 9
			"BankStick not available", // A
			"MIDI IN Overrun Error", // B
			"MIDI IN Frame Error", // C
			"Unknown Query", // D
			"Invalid SysEx command" // E
	};

	public MIOSSysexSendReceive(Receiver receiver) {
		super();
		this.receiver = receiver;
		messages = new LinkedList();
	}

	public int getDeviceID() {
		return deviceID;
	}

	public void setDeviceID(int deviceID) {
		this.deviceID = deviceID;

		setChanged();
		notifyObservers(DEVICE_ID);
		clearChanged();
	}

	public boolean isMIOS32Mode() {
		return this.mios32Mode;
	}

	public void setMIOS32Mode(boolean enabled) {
		this.mios32Mode = enabled;

		setChanged();
		notifyObservers(MIOS32_MODE);
		clearChanged();
	}

	public byte getMIOS_SysExId() {
		return (byte) (mios32Mode ? 0x32 : 0x40); // MIOS32/MIOS8
	}

	public Receiver getReceiver() {
		return receiver;
	}

	public void setReceiver(Receiver receiver) {
		this.receiver = receiver;
	}

	public void addMessage(String message) {

		messages.addLast(message);

		setChanged();
		notifyObservers(MESSAGES);
		clearChanged();
	}

	public LinkedList getMessages() {
		return messages;
	}

	public void send(MidiMessage message, long timestamp) {
		if (!isCancelled() && !isDone()) {
			decodeMessage(message, timestamp);
		}
	}

	public void close() {

	}

	protected byte axExpectedHeader[] = new byte[] { (byte) 0x00, (byte) 0x00,
			(byte) 0x7E, (byte) 0x40, (byte) (deviceID & 0x7F) }; // Note:

	// 0x40 will
	// be
	// changed
	// to 0x32
	// if
	// MIOS32_Mode

	public boolean isDone() {
		return done;
	}

	public boolean isCancelled() {
		return cancelled;
	}

	public void cancel() {
		synchronized (this) {
			cancelled = true;
			addMessage("Process stopped by user");
			notify(); // TK: finally the reason why threads where not informed
			// about a cancel request and mixed the upload blocks!
		}
	}

	public void decodeMessage(MidiMessage message, long timestamp) {
		if (message instanceof SysexMessage) {

			SysexMessage sysExMessage = (SysexMessage) message;

			byte[] axSysExData = sysExMessage.getData();

			axExpectedHeader[3] = getMIOS_SysExId();
			axExpectedHeader[4] = (byte) (deviceID & 0x7F);

			if ((axSysExData.length + 1) >= 8) {
				boolean bValid = true;
				if (sysExMessage.getStatus() == SysexMessage.SYSTEM_EXCLUSIVE) {
					for (int n = 0; n < axExpectedHeader.length; n++) {
						if (axSysExData[n] != axExpectedHeader[n]) {
							bValid = false;
							break;
						}
					}

					// bValid is true if message header matches expected.
					if (bValid) {
						if (axSysExData[5] == 0x01) {
							// Received "read" command
							receivedRead(axSysExData);
						} else if (axSysExData[5] == 0x02) {
							// Received "write" command, probably from MIDI
							// loopback cable or MIDI Yoke
							receivedWrite(axSysExData);
						} else if (axSysExData[5] == 0x03) {
							// Received "select bankstick" command, probably
							// from MIDI loopback cable or MIDI Yoke
							receivedSelectBankstick(axSysExData);
						} else if (axSysExData[5] == 0x08) {
							// Received "lcd" command, probably from MIDI
							// loopback cable or MIDI Yoke
							receivedLCDCommand(axSysExData);
						} else if (axSysExData[5] == 0x0D) {
							// Received "debug" command, probably from MIDI
							// loopback cable or MIDI Yoke
							receivedDebugCommand(axSysExData);
						} else if (axSysExData[5] == 0x0E) {
							receivedErrorCode(axSysExData);
						} else if (axSysExData[5] == 0x0F) {
							receivedReturnValues(axSysExData);
						} else {
							receivedUnknownMIOSType(axSysExData);
						}
					} else {
						receivedUnknownSysex(axSysExData);
					}
				} else if (sysExMessage.getStatus() == SysexMessage.SPECIAL_SYSTEM_EXCLUSIVE) {
					// "Continued" SysEx message, currently unhandled.
					receivedContinuedSysex(axSysExData);
				}
			} else {
				receivedSysexMessageLessThan8Bytes(axSysExData);
			}
		} else {
			// _progressMessages.addLast( "Received non-SysEx message" );
		}
	}

	protected void receivedRead(byte[] data) {

	}

	protected void receivedWrite(byte[] data) {

	}

	protected void receivedSelectBankstick(byte[] data) {

	}

	protected void receivedLCDCommand(byte[] data) {

	}

	protected void receivedDebugCommand(byte[] data) {

	}

	protected void receivedContinuedSysex(byte[] data) {
		addMessage("Received 'continued' SysEx message");
	}

	protected void receivedErrorCode(byte[] data) {
		int errorCode = data[6] - 1;
		if (errorCode >= _errorCodes.length) {
			addMessage("Received error code " + Utils.byteToHex(data[6])
					+ " (unknown)");
		} else {
			addMessage("Received error code " + Utils.byteToHex(data[6]) + ": "
					+ _errorCodes[errorCode]);
		}
	}

	protected void receivedReturnValues(byte[] data) {
		addMessage("Received acknowledgement response");
	}

	protected void receivedUnknownMIOSType(byte[] data) {
		addMessage("Received MIOS SysEx message of unknown type '"
				+ Utils.bytesToString(data) + "'");
	}

	protected void receivedUnknownSysex(byte[] data) {
		addMessage("Received unexpected MIOS SysEx message = "
				+ Utils.bytesToString(data) + "  expected = "
				+ Utils.bytesToString(axExpectedHeader));
	}

	protected void receivedSysexMessageLessThan8Bytes(byte[] data) {
		addMessage("Received SysEx message of less than 8 bytes");
	}

}
