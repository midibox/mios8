/*
 * @(#)MidiDeviceTransferable	beta8	2006/04/03
 *
 * Copyright (C) 2008  Adam King (adamjking@optusnet.com.au)
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

package org.midibox.midi.gui;

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;

import javax.sound.midi.MidiDevice;

public class MidiDeviceTransferable implements Transferable {

	public static final DataFlavor LOCAL_JVM_MIDI_DEVICE_FLAVOR = new DataFlavor(
			DataFlavor.javaJVMLocalObjectMimeType + "; class=\""
					+ MidiDevice.class.getName() + "\"", "Local Audio Files");

	private MidiDevice[] midiDevices;

	private DataFlavor[] dataFlavors;

	public MidiDeviceTransferable(MidiDevice[] midiDevices) {
		this.midiDevices = midiDevices;
		dataFlavors = new DataFlavor[] { MidiDeviceTransferable.LOCAL_JVM_MIDI_DEVICE_FLAVOR };
	}

	public Object getTransferData(DataFlavor flavor) {

		if (flavor.equals(MidiDeviceTransferable.LOCAL_JVM_MIDI_DEVICE_FLAVOR)) {
			// System.out.println("JVM");
			return midiDevices;

		}
		return null;
	}

	public DataFlavor[] getTransferDataFlavors() {
		return dataFlavors;
	}

	public boolean isDataFlavorSupported(DataFlavor flavor) {
		for (int f = 0; f < dataFlavors.length; f++) {
			if (flavor.equals(dataFlavors[f])) {
				return true;
			}
		}
		return false;
	}
}
