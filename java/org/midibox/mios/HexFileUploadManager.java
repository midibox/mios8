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

public class HexFileUploadManager extends Observable implements Observer {

	private Vector hexFileUploads;

	private boolean mios32Mode;

	public HexFileUploadManager() {

		hexFileUploads = new Vector();

		mios32Mode = false;
	}

	public boolean isMIOS32Mode() {

		return mios32Mode;
	}

	public void setMIOS32Mode(boolean mode) {

		mios32Mode = mode;
	}

	public HexFileUpload newHexFileUpload() {

		HexFileUpload hexFileUpload = new HexFileUpload(null);

		hexFileUpload.setMIOS32Mode(mios32Mode);

		addHexFileUpload(hexFileUpload);

		return hexFileUpload;
	}

	public void addHexFileUpload(HexFileUpload hexFileUpload) {

		hexFileUpload.addObserver(this);

		hexFileUploads.add(hexFileUpload);

		setChanged();
		notifyObservers(hexFileUpload);
		clearChanged();
	}

	public void removeHexFileUpload(HexFileUpload hexFileUpload) {

		hexFileUploads.remove(hexFileUpload);
		hexFileUpload.deleteObserver(this);

		hexFileUploads.remove(hexFileUpload);

		setChanged();
		notifyObservers(hexFileUpload);
		clearChanged();
	}

	public Vector getHexFileUploads() {
		return hexFileUploads;
	}

	public void update(Observable observable, Object object) {

		if (object == HexFileUpload.MIOS32_MODE) {
			mios32Mode = ((HexFileUpload) observable).isMIOS32Mode();
		}
	}
}
