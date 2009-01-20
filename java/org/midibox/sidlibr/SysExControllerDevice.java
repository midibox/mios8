/*
 * @(#)SIDV2librarian.java	beta1	2008/01/21
 *
 * Copyright (C) 2008    Rutger Vlek (rutgervlek@hotmail.com)
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

package org.midibox.sidlibr;

import javax.sound.midi.MidiMessage;

import org.midibox.midi.VirtualMidiDevice;

public class SysExControllerDevice extends VirtualMidiDevice {

	protected SysExController sysexController;

	public SysExControllerDevice(String name) {
		super(name, -1, -1);
		this.sysexController = new SysExController(midiOutPort);
	}

	public SysExController getSysExController() {
		return sysexController;
	}

	protected void receiveFromReceivers(MidiMessage message, long timestamp) {
		sysexController.send(message, timestamp);
	}
}
