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

import javax.sound.midi.MidiDevice;
import javax.sound.midi.ShortMessage;

import org.midibox.midi.MidiDeviceManager;
import org.midibox.midi.MidiDeviceRouting;
import org.midibox.midi.MidiFilterDevice;
import org.midibox.midi.MidiFilterDeviceManager;
import org.midibox.midi.MidiKeyboardControllerDevice;
import org.midibox.midi.MidiMonitorFiltered;
import org.midibox.midi.MidiMonitorFilteredDevice;
import org.midibox.midi.MidiRouterDevice;
import org.midibox.mios.DebugFunctionDevice;
import org.midibox.mios.HexFileUploadDevice;
import org.midibox.mios.HexFileUploadDeviceManager;
import org.midibox.mios.LCDMessageDevice;
import org.midibox.mios.MIOSTerminal;

public class MIOSStudio extends Observable implements Observer {

	public static Object MIDI_THRU_OUT_PORT = new Object();

	public static Object ROUTE_INDIVIDUAL_DEVICES = new Object();

	protected MidiDeviceRouting midiDeviceRouting;

	protected MidiRouterDevice miosStudioInPort;

	protected MidiFilterDevice midiThruFilterDevice;

	// TODO: implement a map for MIDI thru

	/*
	 * private MidiMapDevice midiThruMapDevice;
	 */

	protected MidiRouterDevice miosStudioThruPort;

	protected MidiRouterDevice miosStudioOutPort;

	private MidiDeviceManager midiDeviceManager;

	private MidiFilterDeviceManager midiFilterManager;

	/*
	 * private MidiMapDeviceManager midiMapManager;
	 */

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

	private boolean midiThruOutPort = false;

	public MIOSStudio() {

		createDevices();

		setRouteIndividualDevices(false);

		connectDevices();
	}

	protected void createDevices() {

		midiDeviceRouting = new MidiDeviceRouting();

		miosStudioInPort = new MidiRouterDevice("MIOS Studio In Port");

		midiThruFilterDevice = new MidiFilterDevice("MIOS Studio Thru Filter");

		/*
		 * midiThruMapDevice = new MidiMapDevice("MIOS Studio Thru Map");
		 */

		miosStudioThruPort = new MidiRouterDevice("MIOS Studio Thru Port");

		miosStudioOutPort = new MidiRouterDevice("MIOS Studio Out Port");

		midiDeviceManager = new MidiDeviceManager();
		midiDeviceManager.addObserver(this);

		midiFilterManager = new MidiFilterDeviceManager();
		midiFilterManager.addObserver(this);

		/*
		 * midiMapManager = new MidiMapDeviceManager();
		 * midiMapManager.addObserver(this);
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

		miosTerminalFiltered.getMidiFilter().setVoiceMessage(
				ShortMessage.NOTE_OFF, false);
		miosTerminalFiltered.getMidiFilter().setVoiceMessage(
				ShortMessage.NOTE_ON, false);
		miosTerminalFiltered.getMidiFilter().setVoiceMessage(
				ShortMessage.POLY_PRESSURE, false);
		miosTerminalFiltered.getMidiFilter().setVoiceMessage(
				ShortMessage.CONTROL_CHANGE, false);
		miosTerminalFiltered.getMidiFilter().setVoiceMessage(
				ShortMessage.PROGRAM_CHANGE, false);
		miosTerminalFiltered.getMidiFilter().setVoiceMessage(
				ShortMessage.CHANNEL_PRESSURE, false);
		miosTerminalFiltered.getMidiFilter().setVoiceMessage(
				ShortMessage.PITCH_BEND, false);

		miosTerminalFiltered.getMidiFilter().setSystemCommonMessage(
				ShortMessage.MIDI_TIME_CODE, false);
		miosTerminalFiltered.getMidiFilter().setSystemCommonMessage(
				ShortMessage.SONG_POSITION_POINTER, false);
		miosTerminalFiltered.getMidiFilter().setSystemCommonMessage(
				ShortMessage.SONG_SELECT, false);
		miosTerminalFiltered.getMidiFilter().setSystemCommonMessage(
				ShortMessage.TUNE_REQUEST, false);

		miosTerminalFiltered.getMidiFilter().setSystemRealtimeMessage(
				ShortMessage.TIMING_CLOCK, false);
		miosTerminalFiltered.getMidiFilter().setSystemRealtimeMessage(
				ShortMessage.START, false);
		miosTerminalFiltered.getMidiFilter().setSystemRealtimeMessage(
				ShortMessage.CONTINUE, false);
		miosTerminalFiltered.getMidiFilter().setSystemRealtimeMessage(
				ShortMessage.STOP, false);
		miosTerminalFiltered.getMidiFilter().setSystemRealtimeMessage(
				ShortMessage.ACTIVE_SENSING, false);
		miosTerminalFiltered.getMidiFilter().setSystemRealtimeMessage(
				ShortMessage.SYSTEM_RESET, false);

		/*
		 * miosTerminalFiltered.getMidiFilter().setVoiceMessages(false);
		 * 
		 * miosTerminalFiltered.getMidiFilter().setSystemCommonMessages(false);
		 * 
		 * 
		 * 
		 * miosTerminalFiltered.getMidiFilter().setSystemRealtimeMessages(false);
		 */

		miosTerminalFiltered.getMidiFilter().setSysexMessages(true);

		miosTerminalFiltered.getMidiFilter().setMetaMessages(false);
	}

	public MidiDeviceManager getMidiDeviceManager() {
		return midiDeviceManager;
	}

	public MidiFilterDeviceManager getMidiFilterManager() {
		return midiFilterManager;
	}

	/*
	 * public MidiMapDeviceManager getMidiMapManager() { return midiMapManager;
	 * }
	 */

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

	public boolean isMidiThruOutPort() {
		return midiThruOutPort;
	}

	public void setMidiThruOutPort(boolean midiThru) {
		this.midiThruOutPort = midiThru;

		midiDeviceRouting
				.connectDevices(miosStudioInPort, midiThruFilterDevice);

		if (midiThru) {

			/*
			 * midiDeviceRouting.connectDevices(midiThruFilterDevice,
			 * midiThruMapDevice);
			 */

			midiDeviceRouting.disconnectDevice(miosStudioThruPort);

			midiDeviceRouting.disconnectDevices(midiThruFilterDevice,
					miosStudioThruPort);

			midiDeviceRouting.connectDevices(midiThruFilterDevice,
					miosStudioOutPort);
		} else {

			/*
			 * midiDeviceRouting.disconnectDevices(midiThruFilterDevice,
			 * midiThruMapDevice);
			 */

			midiDeviceRouting.disconnectDevices(midiThruFilterDevice,
					miosStudioOutPort);

			midiDeviceRouting.connectDevices(midiThruFilterDevice,
					miosStudioThruPort);
		}

		setRouteIndividualDevices(routeIndividualDevices);

		setChanged();

		notifyObservers(MIDI_THRU_OUT_PORT);

		clearChanged();
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

			if (!midiThruOutPort) {
				midiDeviceRouting.addMidiReadDevice(miosStudioThruPort);
			}
		}

		reorder();

		setChanged();

		notifyObservers(ROUTE_INDIVIDUAL_DEVICES);

		clearChanged();
	}

	public void reorder() {

		midiDeviceRouting.getMidiReadDevices().removeAll(
				midiFilterManager.getMidiFilterDevices());
		midiDeviceRouting.getMidiWriteDevices().removeAll(
				midiFilterManager.getMidiFilterDevices());

		midiDeviceRouting.getMidiReadDevices().removeAll(
				midiDeviceManager.getSelectedMidiReadDevices());
		midiDeviceRouting.getMidiWriteDevices().removeAll(
				midiDeviceManager.getSelectedMidiWriteDevices());

		/*
		 * midiReadDevices.removeAll(midiMapManager.getMidiMapDevices());
		 * midiWriteDevices.removeAll(midiMapManager.getMidiMapDevices());
		 */

		midiDeviceRouting.addMidiReadDevices(midiFilterManager
				.getMidiFilterDevices());
		midiDeviceRouting.addMidiWriteDevices(midiFilterManager
				.getMidiFilterDevices());

		Iterator it = midiDeviceManager.getMidiReadDevices().iterator();

		while (it.hasNext()) {

			Object object = it.next();

			if (midiDeviceManager.getSelectedMidiReadDevices().contains(object)) {
				midiDeviceRouting.addMidiReadDevice((MidiDevice) object);
			}
		}

		it = midiDeviceManager.getMidiWriteDevices().iterator();
		while (it.hasNext()) {

			Object object = it.next();

			if (midiDeviceManager.getSelectedMidiWriteDevices()
					.contains(object)) {
				midiDeviceRouting.addMidiWriteDevice((MidiDevice) object);
			}
		}

		/*
		 * midiReadDevices.addAll(midiMapManager.getMidiMapDevices());
		 * midiWriteDevices.addAll(midiMapManager.getMidiMapDevices());
		 */
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

		if (!midiThruOutPort) {

			midiDeviceRouting.addMidiWriteDevice(miosStudioThruPort);
			midiDeviceRouting.addMidiReadDevice(miosStudioThruPort);
		}
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

		setMidiThruOutPort(midiThruOutPort);

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

		if (observable == hexFileUploadDeviceManager) {

			HexFileUploadDevice hexFileUploadDevice = (HexFileUploadDevice) object;

			if (hexFileUploadDeviceManager.getHexFileUploadDevices().contains(
					hexFileUploadDevice)) {

				midiDeviceRouting.connectDevices(miosStudioInPort,
						hexFileUploadDevice);
				midiDeviceRouting.connectDevices(hexFileUploadDevice,
						miosStudioOutPort);
			} else {
				midiDeviceRouting.disconnectDevice(hexFileUploadDevice);
			}

			setRouteIndividualDevices(routeIndividualDevices);
		}

		if (observable == midiDeviceManager) {

			MidiDevice midiDevice = (MidiDevice) object;

			if (!midiDeviceManager.getSelectedMidiReadDevices().contains(
					midiDevice)
					&& !midiDeviceManager.getSelectedMidiWriteDevices()
							.contains(midiDevice)) {
				midiDeviceRouting.disconnectDevice(midiDevice);
			}

			setRouteIndividualDevices(routeIndividualDevices);
		}

		if (observable == midiFilterManager) {

			MidiDevice midiFilter = (MidiDevice) object;

			if (!midiFilterManager.getMidiFilterDevices().contains(midiFilter)) {
				midiDeviceRouting.disconnectDevice(midiFilter);
			}

			setRouteIndividualDevices(routeIndividualDevices);
		}
	}
}
