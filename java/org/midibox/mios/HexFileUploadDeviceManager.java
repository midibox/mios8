/*
 * @(#)HexFileUploadDeviceManager.java	beta8	2006/04/23
 *
 * Copyright (C) 2008   Adam King (adamjking@optusnet.com.au)
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

import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import org.midibox.midi.MidiFilterDevice;

public class HexFileUploadDeviceManager extends Observable implements Observer {

	private Vector hexFileUploadDevices;

	private boolean mios32Mode;

	public HexFileUploadDeviceManager() {
		hexFileUploadDevices = new Vector();

		mios32Mode = false;
	}

	public boolean isMIOS32Mode() {
		return mios32Mode;
	}

	public void setMIOS32Mode(boolean mode) {

		mios32Mode = mode;
	}

	public HexFileUploadDevice newHexFileUploadDevice() {

		HexFileUploadDevice hexFileUploadDevice = new HexFileUploadDevice(
				"MIOS Hex File Upload " + (hexFileUploadDevices.size() + 1));

		hexFileUploadDevice.getHexFileUpload().setMIOS32Mode(mios32Mode);
		hexFileUploadDevice.getHexFileUpload().addObserver(this);

		hexFileUploadDevices.add(hexFileUploadDevice);

		setChanged();
		notifyObservers(hexFileUploadDevice);
		clearChanged();

		return hexFileUploadDevice;
	}

	public void removeHexFileUploadDevice(
			HexFileUploadDevice hexFileUploadDevice) {
		hexFileUploadDevices.remove(hexFileUploadDevice);
		hexFileUploadDevice.getHexFileUpload().deleteObserver(this);
		
		hexFileUploadDevices.remove(hexFileUploadDevice);

		for (int i = 0; i < hexFileUploadDevices.size(); i++) {

			HexFileUploadDevice currentHexFileUploadDevice = (HexFileUploadDevice) hexFileUploadDevices.elementAt(i);

			currentHexFileUploadDevice.setName("MIOS Hex File Upload " + (i + 1));
		}

		setChanged();
		notifyObservers(hexFileUploadDevice);
		clearChanged();
	}

	public Vector getHexFileUploadDevices() {
		return hexFileUploadDevices;
	}

	public void update(Observable observable, Object object) {
		if (object == HexFileUpload.MIOS32_MODE) {
			mios32Mode = ((HexFileUpload) observable).isMIOS32Mode();
		}
	}
}
