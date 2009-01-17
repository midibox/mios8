/*
 * @(#)MIOSTerminalFiltered.java
 *
 * Copyright (C) 2008    Thorsten Klose (tk@midibox.org)
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

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;

import org.midibox.midi.MidiFilter;

public class MIOSTerminalFiltered implements Receiver {
	protected MidiFilter midiFilter;

	protected MIOSTerminal miosTerminal;

	public MIOSTerminalFiltered() {
		miosTerminal = new MIOSTerminal();
		midiFilter = new MidiFilter(miosTerminal);

		// special for MIOS Terminal:
		// disable all messages by default, only allow pass SysEx
		// user can enable other MIDI events again if required
		midiFilter.sysexMessage = true;

		midiFilter.metaMessage = false;
		midiFilter.activeSensing = false;
		midiFilter.channelPressure = false;
		midiFilter.continueMessage = false;
		midiFilter.controlChange = false;
		midiFilter.midiTimeCode = false;
		midiFilter.noteOff = false;
		midiFilter.noteOn = false;
		midiFilter.pitchBend = false;
		midiFilter.pollyPressure = false;
		midiFilter.programChange = false;
		midiFilter.songPositionPointer = false;
		midiFilter.songSelect = false;
		midiFilter.start = false;
		midiFilter.stop = false;
		midiFilter.systemReset = false;
		midiFilter.timingClock = false;
		midiFilter.tuneRequest = false;
	}

	public MidiFilter getMidiFilter() {
		return midiFilter;
	}

	public MIOSTerminal getMIOSTerminal() {
		return miosTerminal;
	}

	public void close() {

	}

	public void send(MidiMessage midiMessage, long timestamp) {
		midiFilter.send(midiMessage, timestamp);
	}
}
