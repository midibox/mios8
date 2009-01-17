/*
 * @(#)MIOSTerminalFilteredDevice.java
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

import org.midibox.midi.VirtualMidiDevice;

public class MIOSTerminalFilteredDevice extends VirtualMidiDevice {

	protected MIOSTerminalFiltered miosTerminalFiltered;

	public MIOSTerminalFilteredDevice(String name) {
		super(name, 0, -1);
		miosTerminalFiltered = new MIOSTerminalFiltered();
	}

	public MIOSTerminalFiltered getMIOSTerminalFiltered() {
		return miosTerminalFiltered;
	}

	protected void receiveFromReceivers(MidiMessage message, long timestamp) {
		miosTerminalFiltered.send(message, timestamp);
	}
}
