/*
 * @(#)MidiMonitorFiltered.java	beta8	2006/04/23
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

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;

public class MidiMonitorFiltered implements Receiver {
	protected MidiFilter midiFilter;

	protected MidiMonitor midiPortMonitor;

	public MidiMonitorFiltered() {
		midiPortMonitor = new MidiMonitor();
		midiFilter = new MidiFilter(midiPortMonitor);
	}

	public MidiFilter getMidiFilter() {
		return midiFilter;
	}

	public MidiMonitor getMidiPortMonitor() {
		return midiPortMonitor;
	}

	public void close() {

	}

	public void send(MidiMessage midiMessage, long timestamp) {
		midiFilter.send(midiMessage, timestamp);
	}
}
