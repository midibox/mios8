package org.midibox.apps.miosstudiosid;

import org.midibox.apps.sidv2editor.SIDV2Editor;

public class MIOSStudioSID extends org.midibox.apps.miosstudio.MIOSStudio {

	private SIDV2Editor sidv2librarian;

	public SIDV2Editor getSidv2librarian() {

		return sidv2librarian;
	}

	protected void createDevices() {

		super.createDevices();

		sidv2librarian = new SIDV2Editor(null, null);

	}

	protected void routeIndividualDevices() {

		super.routeIndividualDevices();

		midiDeviceRouting.addMidiWriteDevice(sidv2librarian
				.getSysexControllerDevice());

		midiDeviceRouting.addMidiReadDevice(sidv2librarian
				.getSysexControllerDevice());
	}

	public void connectDevices() {

		super.connectDevices();

		midiDeviceRouting.connectDevices(miosStudioInPort, sidv2librarian
				.getSysexControllerDevice());
		midiDeviceRouting.connectDevices(sidv2librarian
				.getSysexControllerDevice(), miosStudioOutPort);
	}
}
