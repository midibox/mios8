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
import org.midibox.midi.MidiMonitorFiltered;
import org.midibox.midi.MidiMonitorFilteredDevice;
import org.midibox.midi.MidiRouterDevice;
import org.midibox.mios.DebugFunctionDevice;
import org.midibox.mios.HexFileUploadDevice;
import org.midibox.mios.HexFileUploadDeviceManager;
import org.midibox.mios.LCDMessageDevice;
import org.midibox.mios.MIOSTerminal;

public class MIOSStudio implements Observer {

	protected MidiDeviceRouting midiDeviceRouting;

	protected MidiRouterDevice miosStudioInPort;

	protected MidiFilterDevice midiThruFilterDevice;

	// TODO: implement a map for MIDI thru

	/*
	 * private MidiMapDevice midiThruMapDevice;
	 */

	protected MidiRouterDevice miosStudioOutPort;

	private MidiMonitorFilteredDevice midiInPortMonitorDevice;

	private MidiMonitorFilteredDevice midiOutPortMonitorDevice;

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

	private MidiMonitorFilteredDevice miosTerminalDevice;

	private boolean routeIndividualDevices;

	private boolean midiThru;

	public MIOSStudio() {

		createDevices();

		midiThru = false;

		setRouteIndividualDevices(false);

		connectDevices();
	}

	protected void createDevices() {

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

		MidiMonitorFiltered miosTerminalFiltered = new MidiMonitorFiltered(
				new MIOSTerminal());

		miosTerminalDevice = new MidiMonitorFilteredDevice("MIOS Terminal",
				miosTerminalFiltered);

		// special for MIOS Terminal:
		// disable all messages by default, only allow pass SysEx
		// user can enable other MIDI events again if required
		miosTerminalFiltered.getMidiFilter().sysexMessage = true;

		miosTerminalFiltered.getMidiFilter().metaMessage = false;
		miosTerminalFiltered.getMidiFilter().activeSensing = false;
		miosTerminalFiltered.getMidiFilter().channelPressure = false;
		miosTerminalFiltered.getMidiFilter().continueMessage = false;
		miosTerminalFiltered.getMidiFilter().controlChange = false;
		miosTerminalFiltered.getMidiFilter().midiTimeCode = false;
		miosTerminalFiltered.getMidiFilter().noteOff = false;
		miosTerminalFiltered.getMidiFilter().noteOn = false;
		miosTerminalFiltered.getMidiFilter().pitchBend = false;
		miosTerminalFiltered.getMidiFilter().pollyPressure = false;
		miosTerminalFiltered.getMidiFilter().programChange = false;
		miosTerminalFiltered.getMidiFilter().songPositionPointer = false;
		miosTerminalFiltered.getMidiFilter().songSelect = false;
		miosTerminalFiltered.getMidiFilter().start = false;
		miosTerminalFiltered.getMidiFilter().stop = false;
		miosTerminalFiltered.getMidiFilter().systemReset = false;
		miosTerminalFiltered.getMidiFilter().timingClock = false;
		miosTerminalFiltered.getMidiFilter().tuneRequest = false;
	}

	public MidiDeviceRouting getMidiDeviceRouting() {
		return midiDeviceRouting;
	}

	public MidiMonitorFilteredDevice getMidiOutPortMonitorDevice() {
		return midiOutPortMonitorDevice;
	}

	public MidiMonitorFilteredDevice getMidiInPortMonitorDevice() {
		return midiInPortMonitorDevice;
	}

	public MidiKeyboardControllerDevice getMidiKeyboardControllerDevice() {
		return midiKeyboardControllerDevice;
	}

	/*
	 * public SysexSendReceiveDeviceManager getSysexSendReceiveDeviceManager() {
	 * return sysexSendReceiveDeviceManager; }
	 */

	public HexFileUploadDeviceManager getHexFileUploadDeviceManager() {
		return hexFileUploadDeviceManager;
	}

	public DebugFunctionDevice getDebugFunctionDevice() {
		return debugFunctionDevice;
	}

	/*
	 * public MemoryReadWriteDevice getMemoryReadWriteDevice() { return
	 * memoryReadWriteDevice; }
	 */

	public LCDMessageDevice getLcdMessageDevice() {
		return lcdMessageDevice;
	}

	public MidiMonitorFilteredDevice getMIOSTerminalDevice() {
		return miosTerminalDevice;
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

			routeIndividualDevices();

		} else {
			midiDeviceRouting.addMidiWriteDevice(miosStudioInPort);
			midiDeviceRouting.addMidiReadDevice(miosStudioOutPort);
		}

		// get midiDeviceRouting to add physical, virtual, filters and maps
		midiDeviceRouting.reorder();
	}

	protected void routeIndividualDevices() {

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

		/*
		 * midiDeviceRouting.addMidiWriteDevice(inDumpReceiverDevice);
		 * 
		 * midiDeviceRouting.addMidiWriteDevice(outDumpReceiverDevice);
		 */

		midiDeviceRouting.addMidiWriteDevice(midiKeyboardControllerDevice);
		midiDeviceRouting.addMidiReadDevice(midiKeyboardControllerDevice);

		/*
		 * midiDeviceRouting.addMidiReadDevices(sysexSendReceiveDeviceManager
		 * .getSysexSendReceiveDevices()); midiDeviceRouting.addMidiWriteDevices
		 * (sysexSendReceiveDeviceManager .getSysexSendReceiveDevices());
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

		midiDeviceRouting.addMidiWriteDevice(miosTerminalDevice);
	}

	public void connectDevices() {

		midiDeviceRouting.connectDevices(miosStudioInPort,
				midiInPortMonitorDevice);

		midiDeviceRouting.connectDevices(miosStudioOutPort,
				midiOutPortMonitorDevice);

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

		midiDeviceRouting.connectDevices(miosStudioInPort, miosTerminalDevice);
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

		if (object == "Edit") {

			boolean portsReleased = midiDeviceRouting.getPortsReleased();

			if (!portsReleased) {
				midiDeviceRouting.setPortsReleased(true);
				midiDeviceRouting.setPortsReleased(false);
			}
		}
	}
}
