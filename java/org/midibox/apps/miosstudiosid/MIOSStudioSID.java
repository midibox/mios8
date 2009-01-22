package org.midibox.apps.miosstudiosid;

import java.util.Observable;

import org.midibox.apps.SIDV2librarian.SIDV2librarian;

public class MIOSStudioSID extends org.midibox.apps.miosstudio.MIOSStudio {

	private SIDV2librarian sidv2librarian;

	public SIDV2librarian getSidv2librarian() {

		return sidv2librarian;
	}

	protected void createDevices() {

		super.createDevices();

		sidv2librarian = new SIDV2librarian(null, null);

		sidv2librarian.getSIDLibController().addObserver(this);
	}

	protected void routeIndividualDevices() {

		super.routeIndividualDevices();

		midiDeviceRouting.addMidiWriteDevice(sidv2librarian
				.getSysExControllerDevice());
		midiDeviceRouting.addMidiReadDevice(sidv2librarian
				.getSysExControllerDevice());
	}

	public void connectDevices() {

		super.connectDevices();

		midiDeviceRouting.connectDevices(miosStudioInPort, sidv2librarian
				.getSysExControllerDevice());
		midiDeviceRouting.connectDevices(sidv2librarian
				.getSysExControllerDevice(), miosStudioOutPort);
	}

	public void update(Observable observable, Object object) {

		super.update(observable, object);

		if (object == "Edit") {

			boolean portsReleased = midiDeviceRouting.getPortsReleased();

			if (!portsReleased) {
				midiDeviceRouting.setPortsReleased(true);
				midiDeviceRouting.setPortsReleased(false);
			}
		}
	}
}
