/*
 * @(#)MidiFilter.java	beta8	2006/04/23
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

import java.util.Observable;

import javax.sound.midi.MetaMessage;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.SysexMessage;

public class MidiFilter extends Observable implements Receiver {

	protected Receiver receiver;
	
	// voice messages
	public boolean noteOff = true;

	public boolean noteOn = true;

	public boolean pollyPressure = true;

	public boolean controlChange = true;

	public boolean programChange = true;

	public boolean channelPressure = true;

	public boolean pitchBend = true;

	// sysex messages
	public boolean sysexMessage = true;

	// system common
	public boolean midiTimeCode = true;

	public boolean songPositionPointer = true;

	public boolean songSelect = true;

	public boolean tuneRequest = true;

	// system realtime
	public boolean timingClock = true;

	public boolean start = true;

	public boolean continueMessage = true;

	public boolean stop = true;

	public boolean activeSensing = true;

	public boolean systemReset = true;

	// meta messages
	public boolean metaMessage = true;

	// control change

	public boolean[] cc = new boolean[128];

	// channels
	public boolean[] channel = new boolean[16];

	public MidiFilter(Receiver receiver) {

		this.receiver = receiver;

		for (int i = 0; i < cc.length; i++) {
			cc[i] = true;
		}

		for (int i = 0; i < channel.length; i++) {
			channel[i] = true;
		}
	}

	public void close() {

	}

	public void send(MidiMessage message, long timestamp) {

		// meta messages
		if (message instanceof MetaMessage) {
			if (!metaMessage)
				return;
		}

		// sysex messages
		else if (message instanceof SysexMessage) {
			if (!sysexMessage)
				return;
		}

		else {

			int status = ((ShortMessage) message).getCommand();

			switch (status) {

			// voice messages
			case ShortMessage.NOTE_OFF:
				if (!noteOff)
					return;
				break;

			case ShortMessage.NOTE_ON:
				if (!noteOn)
					return;
				break;

			case ShortMessage.POLY_PRESSURE:
				if (!pollyPressure)
					return;
				break;

			case ShortMessage.CONTROL_CHANGE:
				if (controlChange) {
					if (!cc[((ShortMessage) message).getData1()]) {
						return;
					}
				} else {
					return;
				}
				break;

			case ShortMessage.PROGRAM_CHANGE:
				if (!programChange)
					return;
				break;

			case ShortMessage.CHANNEL_PRESSURE:
				if (!channelPressure)
					return;
				break;

			case ShortMessage.PITCH_BEND:
				if (!pitchBend)
					return;
				break;

			// system common
			case ShortMessage.MIDI_TIME_CODE:
				if (!midiTimeCode)
					return;
				break;

			case ShortMessage.SONG_POSITION_POINTER:
				if (!songPositionPointer)
					return;
				break;

			case ShortMessage.SONG_SELECT:
				if (!songSelect)
					return;
				break;

			case ShortMessage.TUNE_REQUEST:
				if (!tuneRequest)
					return;
				break;

			// system realtime
			case ShortMessage.TIMING_CLOCK:
				if (!timingClock)
					return;
				break;

			case ShortMessage.START:
				if (!start)
					return;
				break;

			case ShortMessage.CONTINUE:
				if (!continueMessage)
					return;
				break;

			case ShortMessage.STOP:
				if (!stop)
					return;
				break;

			case ShortMessage.ACTIVE_SENSING:
				if (!activeSensing)
					return;
				break;

			case ShortMessage.SYSTEM_RESET:
				if (!systemReset)
					return;
				break;

			}

			// channels
			if (!channel[((ShortMessage) message).getChannel()]) {
				return;
			}
		}

		if (receiver != null) {
			receiver.send(message, timestamp);
		}
	}
}
