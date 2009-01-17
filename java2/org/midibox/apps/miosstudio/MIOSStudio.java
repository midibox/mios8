/*
 * @(#)MIOSStudio.java	beta8	2006/04/23
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

package org.midibox.apps.miosstudio;

import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;

import org.midibox.midi.MidiDeviceRouting;
import org.midibox.midi.MidiFilterDevice;
import org.midibox.midi.MidiKeyboardControllerDevice;
import org.midibox.midi.MidiMonitorFilteredDevice;
import org.midibox.midi.MidiRouterDevice;
import org.midibox.mios.DebugFunctionDevice;
import org.midibox.mios.HexFileUploadDevice;
import org.midibox.mios.HexFileUploadDeviceManager;
import org.midibox.mios.LCDMessageDevice;
import org.midibox.mios.MIOSTerminalFilteredDevice;

public class MIOSStudio implements Observer {

	private MidiDeviceRouting midiDeviceRouting;

	private MidiRouterDevice miosStudioInPort;

	private MidiFilterDevice midiThruFilterDevice;

	// TODO: implement a map for MIDI thru

	/*
	 * private MidiMapDevice midiThruMapDevice;
	 */

	private MidiRouterDevice miosStudioOutPort;

	private MidiMonitorFilteredDevice midiInPortMonitorDevice;

	private MidiMonitorFilteredDevice midiOutPortMonitorDevice;

	private MIOSTerminalFilteredDevice miosTerminalDevice;

	// Debuging for command line

	/*
	 * private DumpReceiverDevice inDumpReceiverDevice;
	 * 
	 * private DumpReceiverDevice outDumpReceiverDevice;
	 */

	private MidiKeyboardControllerDevice midiKeyboardControllerDevice;

	// TODO: implement a manager for sysex sending and receing tasks

	/*
	 * private SysexSendReceiveDeviceManager sysexSendReceiveDeviceManager;
	 */

	private HexFileUploadDeviceManager hexFileUploadDeviceManager;

	// TODO: implement a device for reading and writing to MIOS

	/*
	 * private MemoryReadWriteDevice memoryReadWriteDevice;
	 */

	private LCDMessageDevice lcdMessageDevice;

	private DebugFunctionDevice debugFunctionDevice;

	private boolean routeIndividualDevices;

	private boolean midiThru;
	
	public MIOSStudio() {

		midiDeviceRouting = new MidiDeviceRouting();

		miosStudioInPort = new MidiRouterDevice("MIOS Studio In Port");

		miosStudioOutPort = new MidiRouterDevice("MIOS Studio Out Port");

		midiThruFilterDevice = new MidiFilterDevice("MIOS Studio Thru Filter");

		/*
		 * midiThruMapDevice = new MidiMapDevice("MIOS Studio Thru Map");
		 */

		midiInPortMonitorDevice = new MidiMonitorFilteredDevice(
				"MIDI Monitor: IN");

		midiOutPortMonitorDevice = new MidiMonitorFilteredDevice(
				"MIDI Monitor: OUT");

		miosTerminalDevice = new MIOSTerminalFilteredDevice(
				"MIOS Terminal");

		/*
		 * inDumpReceiverDevice = new DumpReceiverDevice("Dump Receiver: IN");
		 * 
		 * outDumpReceiverDevice = new DumpReceiverDevice("Dump Receiver: OUT");
		 */
		midiKeyboardControllerDevice = new MidiKeyboardControllerDevice(
				"MIDI Keyboard Controller", 0);

		/*
		 * sysexSendReceiveDeviceManager = new SysexSendReceiveDeviceManager();
		 * sysexSendReceiveDeviceManager.newSysexSendReceive();
		 * sysexSendReceiveDeviceManager.addObserver(this);
		 */

		hexFileUploadDeviceManager = new HexFileUploadDeviceManager();
		hexFileUploadDeviceManager.newHexFileUploadDevice();
		hexFileUploadDeviceManager.addObserver(this);

		/*
		 * memoryReadWriteDevice = new MemoryReadWriteDevice( "MIOS Memory
		 * Read/Write");
		 */

		lcdMessageDevice = new LCDMessageDevice("MIOS LCD Message");

		debugFunctionDevice = new DebugFunctionDevice("MIOS Debug Function");

		midiThru = false;
		
		setRouteIndividualDevices(false);

		connectDevices();
	}

	public MidiDeviceRouting getMidiDeviceRouting() {
		return midiDeviceRouting;
	}

	public DebugFunctionDevice getDebugFunctionDevice() {
		return debugFunctionDevice;
	}

	public LCDMessageDevice getLcdMessageDevice() {
		return lcdMessageDevice;
	}

	public MidiMonitorFilteredDevice getMidiInPortMonitorDevice() {
		return midiInPortMonitorDevice;
	}

	public MidiKeyboardControllerDevice getMidiKeyboardControllerDevice() {
		return midiKeyboardControllerDevice;
	}

	public MidiMonitorFilteredDevice getMidiOutPortMonitorDevice() {
		return midiOutPortMonitorDevice;
	}

	public MIOSTerminalFilteredDevice getMIOSTerminalDevice() {
		return miosTerminalDevice;
	}

	/*
	 * public MemoryReadWriteDevice getMemoryReadWriteDevice() { return
	 * memoryReadWriteDevice; }
	 */

	/*
	 * public SysexSendReceiveDeviceManager getSysexSendReceiveDeviceManager() {
	 * return sysexSendReceiveDeviceManager; }
	 */

	public HexFileUploadDeviceManager getHexFileUploadDeviceManager() {
		return hexFileUploadDeviceManager;
	}

	public boolean isMidiThru() {
		return midiThru;
	}

	public void setMidiThru(boolean midiThru) {
		this.midiThru = midiThru;

		if (midiThru) {
			midiDeviceRouting.connectDevices(miosStudioInPort,
					midiThruFilterDevice);
			/*
			 * midiDeviceRouting.connectDevices(midiThruFilterDevice,
			 * midiThruMapDevice);
			 */

			midiDeviceRouting.connectDevices(midiThruFilterDevice,
					miosStudioOutPort);
		} else {
			midiDeviceRouting.disconnectDevices(miosStudioInPort,
					midiThruFilterDevice);
			/*
			 * midiDeviceRouting.disconnectDevices(midiThruFilterDevice,
			 * midiThruMapDevice);
			 */

			midiDeviceRouting.disconnectDevices(midiThruFilterDevice,
					miosStudioOutPort);
		}
	}

	public MidiRouterDevice getMiosStudioInPort() {
		return miosStudioInPort;
	}

	public MidiRouterDevice getMiosStudioOutPort() {
		return miosStudioOutPort;
	}

	public MidiFilterDevice getMidiThruFilterDevice() {
		return midiThruFilterDevice;
	}
	
	/*
	 * public MidiMapDevice getMidiThruMapDevice() { return midiThruMapDevice; }
	 */

	public boolean isRouteIndividualDevices() {
		return routeIndividualDevices;
	}

	public void setRouteIndividualDevices(boolean routeIndividualDevices) {
		this.routeIndividualDevices = routeIndividualDevices;

		// remove all devices
		midiDeviceRouting.removeAllMidiReadDevices();
		midiDeviceRouting.removeAllMidiWriteDevices();

		// add internal MIOSStudio devices if selected
		if (routeIndividualDevices) {

			midiDeviceRouting.addMidiWriteDevice(miosStudioInPort);
			midiDeviceRouting.addMidiReadDevice(miosStudioInPort);

			midiDeviceRouting.addMidiWriteDevice(midiThruFilterDevice);
			midiDeviceRouting.addMidiReadDevice(midiThruFilterDevice);

			/*
			 * midiDeviceRouting.addMidiWriteDevice(midiThruMapDevice);
			 * midiDeviceRouting.addMidiReadDevice(midiThruMapDevice);
			 */

			midiDeviceRouting.addMidiWriteDevice(miosStudioOutPort);
			midiDeviceRouting.addMidiReadDevice(miosStudioOutPort);

			midiDeviceRouting.addMidiWriteDevice(midiOutPortMonitorDevice);

			midiDeviceRouting.addMidiWriteDevice(midiInPortMonitorDevice);

			midiDeviceRouting.addMidiWriteDevice(miosTerminalDevice);

			/*
			 * midiDeviceRouting.addMidiWriteDevice(inDumpReceiverDevice);
			 * 
			 * midiDeviceRouting.addMidiWriteDevice(outDumpReceiverDevice);
			 */

			midiDeviceRouting.addMidiWriteDevice(midiKeyboardControllerDevice);
			midiDeviceRouting.addMidiReadDevice(midiKeyboardControllerDevice);

			/*
			 * midiDeviceRouting.addMidiReadDevices(sysexSendReceiveDeviceManager
			 * .getSysexSendReceiveDevices());
			 * midiDeviceRouting.addMidiWriteDevices(sysexSendReceiveDeviceManager
			 * .getSysexSendReceiveDevices());
			 */

			midiDeviceRouting.addMidiReadDevices(hexFileUploadDeviceManager
					.getHexFileUploadDevices());
			midiDeviceRouting.addMidiWriteDevices(hexFileUploadDeviceManager
					.getHexFileUploadDevices());
			/*
			 * midiDeviceRouting.addMidiWriteDevice(memoryReadWriteDevice);
			 * midiDeviceRouting.addMidiReadDevice(memoryReadWriteDevice);
			 */

			midiDeviceRouting.addMidiReadDevice(lcdMessageDevice);

			midiDeviceRouting.addMidiWriteDevice(debugFunctionDevice);
			midiDeviceRouting.addMidiReadDevice(debugFunctionDevice);

		} else {
			midiDeviceRouting.addMidiWriteDevice(miosStudioInPort);
			midiDeviceRouting.addMidiReadDevice(miosStudioOutPort);
		}

		// get midiDeviceRouting to add physical, virtual, filters and maps
		midiDeviceRouting.reorder();
	}

	public void connectDevices() {

		midiDeviceRouting.connectDevices(miosStudioInPort,
				midiInPortMonitorDevice);

		midiDeviceRouting.connectDevices(miosStudioOutPort,
				midiOutPortMonitorDevice);

		midiDeviceRouting.connectDevices(miosStudioInPort,
				miosTerminalDevice);

		/*
		 * midiDeviceRouting.connectDevices(inVirtualMidiPortDevice,
		 * inDumpReceiverDevice);
		 * midiDeviceRouting.connectDevices(outVirtualMidiPortDevice,
		 * outDumpReceiverDevice);
		 */

		setMidiThru(midiThru);

		midiDeviceRouting.connectDevices(miosStudioInPort,
				midiKeyboardControllerDevice);
		midiDeviceRouting.connectDevices(midiKeyboardControllerDevice,
				miosStudioOutPort);
		/*
		 * Iterator it = sysexSendReceiveDeviceManager
		 * .getSysexSendReceiveDevices().iterator(); while (it.hasNext()) {
		 * SysexSendReceiveDevice ssrt = (SysexSendReceiveDevice) it.next();
		 * midiDeviceRouting.connectDevices(miosStudioInPort, ssrt);
		 * midiDeviceRouting.connectDevices(ssrt, miosStudioOutPort); }
		 */
		Iterator it = hexFileUploadDeviceManager.getHexFileUploadDevices()
				.iterator();
		while (it.hasNext()) {
			HexFileUploadDevice hutd = (HexFileUploadDevice) it.next();
			midiDeviceRouting.connectDevices(miosStudioInPort, hutd);
			midiDeviceRouting.connectDevices(hutd, miosStudioOutPort);
		}
		/*
		 * midiDeviceRouting.connectDevices(miosStudioInPort,
		 * memoryReadWriteDevice);
		 * midiDeviceRouting.connectDevices(memoryReadWriteDevice,
		 * miosStudioOutPort);
		 */

		midiDeviceRouting.connectDevices(lcdMessageDevice, miosStudioOutPort);
		midiDeviceRouting.connectDevices(miosStudioInPort, debugFunctionDevice);
		midiDeviceRouting
				.connectDevices(debugFunctionDevice, miosStudioOutPort);
	}

	public void update(Observable observable, Object object) {
		/*
		 * if (observable == sysexSendReceiveDeviceManager) {
		 * 
		 * SysexSendReceiveDevice ssrt = (SysexSendReceiveDevice) object;
		 * 
		 * if (sysexSendReceiveDeviceManager.getSysexSendReceiveDevices()
		 * .contains(ssrt)) {
		 * 
		 * midiDeviceRouting.connectDevices(miosStudioInPort, ssrt);
		 * midiDeviceRouting.connectDevices(ssrt, miosStudioOutPort);
		 * 
		 * if (routeIndividualDevices) {
		 * midiDeviceRouting.addMidiReadDevice(ssrt);
		 * midiDeviceRouting.addMidiWriteDevice(ssrt);
		 * setRouteIndividualDevices(true); } } else {
		 * midiDeviceRouting.disconnectDevice(ssrt);
		 * 
		 * if (routeIndividualDevices) {
		 * midiDeviceRouting.removeMidiReadDevice(ssrt);
		 * midiDeviceRouting.removeMidiWriteDevice(ssrt); } } }
		 */

		if (observable == hexFileUploadDeviceManager) {

			HexFileUploadDevice hutd = (HexFileUploadDevice) object;

			if (hexFileUploadDeviceManager.getHexFileUploadDevices().contains(
					hutd)) {

				midiDeviceRouting.connectDevices(miosStudioInPort, hutd);
				midiDeviceRouting.connectDevices(hutd, miosStudioOutPort);

				if (routeIndividualDevices) {
					midiDeviceRouting.addMidiReadDevice(hutd);
					midiDeviceRouting.addMidiWriteDevice(hutd);
					setRouteIndividualDevices(true);
				}

			} else {
				midiDeviceRouting.disconnectDevice(hutd);

				if (routeIndividualDevices) {
					midiDeviceRouting.removeMidiReadDevice(hutd);
					midiDeviceRouting.removeMidiWriteDevice(hutd);
				}
			}
		}
	}
}
