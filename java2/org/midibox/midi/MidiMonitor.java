/*
 * @(#)MidiMonitor.java	beta8	2006/04/23
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
 *
 ****************************************************************************
 * Based on DumpReceiver.java. Original copyright below:
 ****************************************************************************
 * DumpReceiver.java
 *
 * This file is part of jsresources.org
 * Copyright (c) 1999 - 2001 by Matthias Pfisterer
 * Copyright (c) 2003 by Florian Bomers
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.midibox.midi;

import java.util.LinkedList;
import java.util.Observable;

import javax.sound.midi.MetaMessage;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.SysexMessage;

public class MidiMonitor extends Observable implements Receiver {

	public static long seByteCount = 0;

	public static long seCount = 0;

	public static long smCount = 0;

	private boolean _bPrintTimeStampAsTicks;

	private LinkedList receivedMessages;

	public MidiMonitor() {
		super();
		_bPrintTimeStampAsTicks = false;
		receivedMessages = new LinkedList();
	}

	public void close() {

	}

	public LinkedList getReceivedMessages() {
		return receivedMessages;
	}

	public void send(MidiMessage message, long lTimeStamp) {
		receivedMessages.add(new MidiMessageReceived(message, lTimeStamp));

		setChanged();
		notifyObservers(receivedMessages);
		clearChanged();
	}

	// TK: cheap version to create the timestamp string from a long (time base
	// uS)
	private String long2mS(long l) {
		Long l_ms = new Long(l / 1000);
		String l_str_ms = l_ms.toString();
		return ("00000000000000" + l_str_ms).substring(l_str_ms.length());
	}

	public String decodeMessage(MidiMessage message, long lTimeStamp) {
		String strMessage = null;

		if (message instanceof ShortMessage) {
			strMessage = decodeMessage((ShortMessage) message);
		} else if (message instanceof SysexMessage) {
			strMessage = decodeMessage((SysexMessage) message);
		} else if (message instanceof MetaMessage) {
			strMessage = decodeMessage((MetaMessage) message);
		} else {
			strMessage = "unknown message type";
		}
		String strTimeStamp = null;
		if (_bPrintTimeStampAsTicks) {
			strTimeStamp = "tick " + lTimeStamp + ": ";
		} else {
			if (lTimeStamp == -1L) {
				strTimeStamp = "timestamp [unknown] | ";
			} else {
				strTimeStamp = long2mS(lTimeStamp) + " ms | ";
			}
		}

		// return null if message is empty (required for filtered SysEx messages)
		return (strMessage == null) ? null : strTimeStamp + strMessage;
	}

	public String decodeMessage(ShortMessage message) {
		String strMessage = null;
		switch (message.getCommand()) {
		case 0x80:
			strMessage = "Note Off " + MidiUtils.getKeyName(message.getData1())
					+ " velocity: " + message.getData2();
			break;

		case 0x90:
			strMessage = "Note On " + MidiUtils.getKeyName(message.getData1())
					+ " velocity: " + message.getData2();
			break;

		case 0xa0:
			strMessage = "Aftertouch "
					+ MidiUtils.getKeyName(message.getData1()) + " pressure: "
					+ message.getData2();
			break;

		case 0xb0:
			strMessage = "CC " + MidiUtils.ccNames[message.getData1()]
					+ " value: " + message.getData2();
			break;

		case 0xc0:
			strMessage = "Program Change no: " + message.getData1();
			break;

		case 0xd0:
			strMessage = "Channel Pressure value: " + message.getData1();
			break;

		case 0xe0:
			strMessage = "Pitch Bend value: "
					+ MidiUtils.get14bitValue(message.getData1(), message
							.getData2());
			break;

		case 0xF0:
			strMessage = MidiUtils.SYSTEM_MESSAGE_TEXT[message.getChannel()];
			switch (message.getChannel()) {
			case 0x1:
				int nQType = (message.getData1() & 0x70) >> 4;
				int nQData = message.getData1() & 0x0F;
				if (nQType == 7) {
					nQData = nQData & 0x1;
				}
				strMessage += MidiUtils.QUARTER_FRAME_MESSAGE_TEXT[nQType]
						+ nQData;
				if (nQType == 7) {
					int nFrameType = (message.getData1() & 0x06) >> 1;
					strMessage += ", frame type: "
							+ MidiUtils.FRAME_TYPE_TEXT[nFrameType];
				}
				break;

			case 0x2:
				strMessage += MidiUtils.get14bitValue(message.getData1(),
						message.getData2());
				break;

			case 0x3:
				strMessage += message.getData1();
				break;
			}
			break;

		default:
			strMessage = "unknown message: status = " + message.getStatus()
					+ ", byte1 = " + message.getData1() + ", byte2 = "
					+ message.getData2();
			break;
		}
		if (message.getCommand() != 0xF0) {
			int nChannel = message.getChannel() + 1;
			String strChannel = "Channel " + nChannel + ": ";
			strMessage = strChannel + strMessage;
		}
		smCount++;
		return "[" + MidiUtils.getHexString(message) + "] " + strMessage;
	}

	public String decodeMessage(SysexMessage message) {
		byte[] abData = message.getData();
		String strMessage = null;
		// System.out.println("sysex status: " + message.getStatus());
		if (message.getStatus() == SysexMessage.SYSTEM_EXCLUSIVE) {
		    strMessage = "SysEx: F0 " + MidiUtils.getHexString(abData);
		} else if (message.getStatus() == SysexMessage.SPECIAL_SYSTEM_EXCLUSIVE) {
			strMessage = "Continued SysEx: F7 "
					+ MidiUtils.getHexString(abData);
			seByteCount--; // do not count the F7
		}
		seByteCount += abData.length + 1;
		seCount++; // for the status byte
		return strMessage;
	}

	public String decodeMessage(MetaMessage message) {
		byte[] abData = message.getData();
		String strMessage = null;
		// System.out.println("data array length: " + abData.length);
		switch (message.getType()) {
		case 0:
			int nSequenceNumber = ((abData[0] & 0xFF) << 8)
					| (abData[1] & 0xFF);
			strMessage = "Sequence Number: " + nSequenceNumber;
			break;

		case 1:
			String strText = new String(abData);
			strMessage = "Text Event: " + strText;
			break;

		case 2:
			String strCopyrightText = new String(abData);
			strMessage = "Copyright Notice: " + strCopyrightText;
			break;

		case 3:
			String strTrackName = new String(abData);
			strMessage = "Sequence/Track Name: " + strTrackName;
			break;

		case 4:
			String strInstrumentName = new String(abData);
			strMessage = "Instrument Name: " + strInstrumentName;
			break;

		case 5:
			String strLyrics = new String(abData);
			strMessage = "Lyric: " + strLyrics;
			break;

		case 6:
			String strMarkerText = new String(abData);
			strMessage = "Marker: " + strMarkerText;
			break;

		case 7:
			String strCuePointText = new String(abData);
			strMessage = "Cue Point: " + strCuePointText;
			break;

		case 0x20:
			int nChannelPrefix = abData[0] & 0xFF;
			strMessage = "MIDI Channel Prefix: " + nChannelPrefix;
			break;

		case 0x2F:
			strMessage = "End of Track";
			break;

		case 0x51:
			int nTempo = ((abData[0] & 0xFF) << 16) | ((abData[1] & 0xFF) << 8)
					| (abData[2] & 0xFF); // tempo in microseconds per beat
			float bpm = MidiUtils.convertTempo(nTempo);
			// truncate it to 2 digits after dot
			bpm = (float) (Math.round(bpm * 100.0f) / 100.0f);
			strMessage = "Set Tempo: " + bpm + " bpm";
			break;

		case 0x54:
			// System.out.println("data array length: " + abData.length);
			strMessage = "SMTPE Offset: " + (abData[0] & 0xFF) + ":"
					+ (abData[1] & 0xFF) + ":" + (abData[2] & 0xFF) + "."
					+ (abData[3] & 0xFF) + "." + (abData[4] & 0xFF);
			break;

		case 0x58:
			strMessage = "Time Signature: " + (abData[0] & 0xFF) + "/"
					+ (1 << (abData[1] & 0xFF))
					+ ", MIDI clocks per metronome tick: " + (abData[2] & 0xFF)
					+ ", 1/32 per 24 MIDI clocks: " + (abData[3] & 0xFF);
			break;

		case 0x59:
			String strGender = (abData[1] == 1) ? "minor" : "major";
			strMessage = "Key Signature: "
					+ MidiUtils.s_astrKeySignatures[abData[0] + 7] + " "
					+ strGender;
			break;

		case 0x7F:
			// TODO: decode vendor code, dump data in rows
			String strDataDump = MidiUtils.getHexString(abData);
			strMessage = "Sequencer-Specific Meta event: " + strDataDump;
			break;

		default:
			String strUnknownDump = MidiUtils.getHexString(abData);
			strMessage = "unknown Meta event: " + strUnknownDump;
			break;

		}
		return strMessage;
	}
}
