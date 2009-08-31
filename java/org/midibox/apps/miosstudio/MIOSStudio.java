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

import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Observable;
import java.util.Observer;

import javax.sound.midi.MidiDevice;

import org.midibox.midi.MidiDeviceManager;
import org.midibox.midi.MidiDeviceRouting;
import org.midibox.midi.MidiFilter;
import org.midibox.midi.MidiFilterManager;
import org.midibox.midi.MidiKeyboardController;
import org.midibox.midi.MidiMonitorFiltered;
import org.midibox.midi.SysexSendReceive;
import org.midibox.midi.SysexSendReceiveManager;
import org.midibox.midi.VirtualMidiDevice;
import org.midibox.mios.DebugFunction;
import org.midibox.mios.HexFileUpload;
import org.midibox.mios.HexFileUploadManager;
import org.midibox.mios.LCDMessage;
import org.midibox.mios.MIOSTerminal;

public class MIOSStudio extends Observable implements Observer {

	public static Object MIDI_THRU_OUT_PORT = new Object();

	public static Object ROUTE_INDIVIDUAL_DEVICES = new Object();

	protected MidiDeviceRouting midiDeviceRouting;

	protected VirtualMidiDevice miosStudioInPort;

	protected MidiFilter midiThruFilter;

	protected VirtualMidiDevice midiThruFilterDevice;

	protected VirtualMidiDevice miosStudioThruPort;

	protected VirtualMidiDevice miosStudioOutPort;

	private MidiDeviceManager midiDeviceManager;

	private MidiFilterManager midiFilterManager;

	private MidiMonitorFiltered midiInPortMonitor;

	private VirtualMidiDevice midiInPortMonitorDevice;

	private MidiMonitorFiltered midiOutPortMonitor;

	private VirtualMidiDevice midiOutPortMonitorDevice;

	private MidiKeyboardController midiKeyboardController;

	private VirtualMidiDevice midiKeyboardControllerDevice;

	private SysexSendReceiveManager sysexSendReceiveManager;

	private HexFileUploadManager hexFileUploadManager;

	private LCDMessage lcdMessage;

	private VirtualMidiDevice lcdMessageDevice;

	private DebugFunction debugFunction;

	private VirtualMidiDevice debugFunctionDevice;

	private MidiMonitorFiltered miosTerminal;

	private VirtualMidiDevice miosTerminalDevice;

	private boolean routeIndividualDevices;

	private boolean midiThruOutPort = false;

	private LinkedHashMap midiFilterDevices;

	private LinkedHashMap sysexSendReceiveDevices;

	private LinkedHashMap hexFileUploadDevices;

	public MIOSStudio() {

		createDevices();

		setRouteIndividualDevices(false);

		connectDevices();
	}

	protected void createDevices() {

		midiDeviceRouting = new MidiDeviceRouting();

		miosStudioInPort = new VirtualMidiDevice("MIOS Studio In Port", -1, -1);
		miosStudioInPort.setMidiInReceiver(miosStudioInPort
				.getMidiOutReceiver());

		midiThruFilterDevice = new VirtualMidiDevice("MIOS Studio Thru Filter",
				-1, -1);
		midiThruFilter = new MidiFilter(midiThruFilterDevice
				.getMidiOutReceiver());
		midiThruFilterDevice.setMidiInReceiver(midiThruFilter);

		miosStudioThruPort = new VirtualMidiDevice("MIOS Studio Thru Port", -1,
				-1);
		miosStudioThruPort.setMidiInReceiver(miosStudioThruPort
				.getMidiOutReceiver());

		miosStudioOutPort = new VirtualMidiDevice("MIOS Studio Out Port", -1,
				-1);
		miosStudioOutPort.setMidiInReceiver(miosStudioOutPort
				.getMidiOutReceiver());

		midiDeviceManager = new MidiDeviceManager();
		midiDeviceManager.addObserver(this);

		midiFilterManager = new MidiFilterManager();
		midiFilterManager.addObserver(this);

		midiFilterDevices = new LinkedHashMap();

		midiInPortMonitorDevice = new VirtualMidiDevice("MIDI Monitor: IN", 0,
				-1);
		midiInPortMonitor = new MidiMonitorFiltered();
		midiInPortMonitorDevice.setMidiInReceiver(midiInPortMonitor);

		midiOutPortMonitorDevice = new VirtualMidiDevice("MIDI Monitor: OUT",
				0, -1);
		midiOutPortMonitor = new MidiMonitorFiltered();
		midiOutPortMonitorDevice.setMidiInReceiver(midiOutPortMonitor);

		midiKeyboardControllerDevice = new VirtualMidiDevice(
				"MIDI Keyboard Controller", -1, -1);
		midiKeyboardController = new MidiKeyboardController(
				midiKeyboardControllerDevice.getMidiOutReceiver(), 0);
		midiKeyboardControllerDevice.setMidiInReceiver(midiKeyboardController);

		sysexSendReceiveManager = new SysexSendReceiveManager();

		sysexSendReceiveManager.addObserver(this);

		sysexSendReceiveDevices = new LinkedHashMap();

		hexFileUploadManager = new HexFileUploadManager();

		hexFileUploadManager.addObserver(this);

		hexFileUploadDevices = new LinkedHashMap();

		lcdMessageDevice = new VirtualMidiDevice("MIOS LCD Message", -1, -1);
		lcdMessage = new LCDMessage(lcdMessageDevice.getMidiOutReceiver());
		lcdMessageDevice.setMidiInReceiver(lcdMessage);

		debugFunctionDevice = new VirtualMidiDevice("MIOS Debug Function", -1,
				-1);
		debugFunction = new DebugFunction(debugFunctionDevice
				.getMidiOutReceiver());
		debugFunctionDevice.setMidiInReceiver(debugFunction);

		miosTerminalDevice = new VirtualMidiDevice("MIOS Terminal", 0, -1);
		miosTerminal = new MidiMonitorFiltered(new MIOSTerminal());
		miosTerminalDevice.setMidiInReceiver(miosTerminal);

		// special for MIOS Terminal:
		// disable all messages by default, only allow pass SysEx
		// user can enable other MIDI events again if required

		miosTerminal.getMidiFilter().setVoiceMessages(false);

		miosTerminal.getMidiFilter().setSystemCommonMessages(false);

		miosTerminal.getMidiFilter().setSystemRealtimeMessages(false);

		miosTerminal.getMidiFilter().setSysexMessages(true);

		miosTerminal.getMidiFilter().setMetaMessages(false);
	}

	public MidiDeviceManager getMidiDeviceManager() {
		return midiDeviceManager;
	}

	public MidiFilterManager getMidiFilterManager() {
		return midiFilterManager;
	}

	public MidiDeviceRouting getMidiDeviceRouting() {
		return midiDeviceRouting;
	}

	public MidiMonitorFiltered getMidiOutPortMonitor() {
		return midiOutPortMonitor;
	}

	public MidiMonitorFiltered getMidiInPortMonitor() {
		return midiInPortMonitor;
	}

	public MidiKeyboardController getMidiKeyboardController() {
		return midiKeyboardController;
	}

	public SysexSendReceiveManager getSysexSendReceiveManager() {
		return sysexSendReceiveManager;
	}

	public HexFileUploadManager getHexFileUploadManager() {
		return hexFileUploadManager;
	}

	public DebugFunction getDebugFunction() {
		return debugFunction;
	}

	public LCDMessage getLcdMessage() {
		return lcdMessage;
	}

	public MidiMonitorFiltered getMIOSTerminal() {
		return miosTerminal;
	}

	public boolean isMidiThruOutPort() {
		return midiThruOutPort;
	}

	public void setMidiThruOutPort(boolean midiThru) {
		this.midiThruOutPort = midiThru;

		midiDeviceRouting
				.connectDevices(miosStudioInPort, midiThruFilterDevice);

		if (midiThru) {

			midiDeviceRouting.disconnectDevice(miosStudioThruPort);

			midiDeviceRouting.disconnectDevices(midiThruFilterDevice,
					miosStudioThruPort);

			midiDeviceRouting.connectDevices(midiThruFilterDevice,
					miosStudioOutPort);
		} else {

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

	public VirtualMidiDevice getMiosStudioInPort() {
		return miosStudioInPort;
	}

	public VirtualMidiDevice getMiosStudioOutPort() {
		return miosStudioOutPort;
	}

	public MidiFilter getMidiThruFilter() {
		return midiThruFilter;
	}

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

		midiDeviceRouting.addMidiReadDevices(midiFilterDevices.values());

		midiDeviceRouting.addMidiWriteDevices(midiFilterDevices.values());

		Iterator it = midiDeviceManager.getMidiReadDevices().values()
				.iterator();

		while (it.hasNext()) {

			Object object = it.next();

			if (midiDeviceManager.getSelectedMidiReadDevices().values()
					.contains(object)) {
				midiDeviceRouting.addMidiReadDevice((MidiDevice) object);
			}
		}

		it = midiDeviceManager.getMidiWriteDevices().values().iterator();

		while (it.hasNext()) {

			Object object = it.next();

			if (midiDeviceManager.getSelectedMidiWriteDevices().values()
					.contains(object)) {
				midiDeviceRouting.addMidiWriteDevice((MidiDevice) object);
			}
		}

		setChanged();

		notifyObservers(ROUTE_INDIVIDUAL_DEVICES);

		clearChanged();
	}

	protected void routeIndividualDevices() {

		midiDeviceRouting.addMidiWriteDevice(miosStudioInPort);
		midiDeviceRouting.addMidiReadDevice(miosStudioInPort);

		midiDeviceRouting.addMidiWriteDevice(midiThruFilterDevice);
		midiDeviceRouting.addMidiReadDevice(midiThruFilterDevice);

		if (!midiThruOutPort) {

			midiDeviceRouting.addMidiWriteDevice(miosStudioThruPort);
			midiDeviceRouting.addMidiReadDevice(miosStudioThruPort);
		}

		midiDeviceRouting.addMidiWriteDevice(miosStudioOutPort);
		midiDeviceRouting.addMidiReadDevice(miosStudioOutPort);

		midiDeviceRouting.addMidiWriteDevice(midiOutPortMonitorDevice);

		midiDeviceRouting.addMidiWriteDevice(midiInPortMonitorDevice);

		midiDeviceRouting.addMidiWriteDevice(midiKeyboardControllerDevice);
		midiDeviceRouting.addMidiReadDevice(midiKeyboardControllerDevice);

		Collection virtualMidiDevices = sysexSendReceiveDevices.values();

		midiDeviceRouting.addMidiReadDevices(virtualMidiDevices);

		midiDeviceRouting.addMidiWriteDevices(virtualMidiDevices);

		virtualMidiDevices = hexFileUploadDevices.values();

		midiDeviceRouting.addMidiReadDevices(virtualMidiDevices);

		midiDeviceRouting.addMidiWriteDevices(virtualMidiDevices);

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

		midiDeviceRouting.connectDevices(miosStudioInPort,
				midiKeyboardControllerDevice);
		midiDeviceRouting.connectDevices(midiKeyboardControllerDevice,
				miosStudioOutPort);

		Iterator it = sysexSendReceiveDevices.values().iterator();

		while (it.hasNext()) {

			VirtualMidiDevice virtualMidiDevice = (VirtualMidiDevice) it.next();

			midiDeviceRouting.connectDevices(miosStudioInPort,
					virtualMidiDevice);

			midiDeviceRouting.connectDevices(virtualMidiDevice,
					miosStudioOutPort);
		}

		it = hexFileUploadDevices.values().iterator();

		while (it.hasNext()) {

			VirtualMidiDevice virtualMidiDevice = (VirtualMidiDevice) it.next();

			midiDeviceRouting.connectDevices(miosStudioInPort,
					virtualMidiDevice);
			midiDeviceRouting.connectDevices(virtualMidiDevice,
					miosStudioOutPort);
		}

		midiDeviceRouting.connectDevices(lcdMessageDevice, miosStudioOutPort);
		midiDeviceRouting.connectDevices(miosStudioInPort, debugFunctionDevice);
		midiDeviceRouting
				.connectDevices(debugFunctionDevice, miosStudioOutPort);

		midiDeviceRouting.connectDevices(miosStudioInPort, miosTerminalDevice);
	}

	public void update(Observable observable, Object object) {

		if (observable == hexFileUploadManager) {

			HexFileUpload hexFileUpload = (HexFileUpload) object;

			if (hexFileUploadDevices.containsKey(hexFileUpload)) {

				VirtualMidiDevice virtualMidiDevice = (VirtualMidiDevice) hexFileUploadDevices
						.get(hexFileUpload);

				midiDeviceRouting.disconnectDevice(virtualMidiDevice);

				hexFileUploadDevices.remove(hexFileUpload);

			} else {

				VirtualMidiDevice virtualMidiDevice = new VirtualMidiDevice("",
						-1, -1);

				virtualMidiDevice.setMidiInReceiver(hexFileUpload);

				hexFileUpload.setReceiver(virtualMidiDevice
						.getMidiOutReceiver());

				midiDeviceRouting.connectDevices(miosStudioInPort,
						virtualMidiDevice);
				midiDeviceRouting.connectDevices(virtualMidiDevice,
						miosStudioOutPort);

				hexFileUploadDevices.put(hexFileUpload, virtualMidiDevice);
			}

			Collection virtualMidiDevices = hexFileUploadDevices.values();

			for (int i = 0; i < virtualMidiDevices.size(); i++) {

				VirtualMidiDevice virtualMidiDevice = (VirtualMidiDevice) virtualMidiDevices
						.toArray()[i];

				virtualMidiDevice.setName("Hex File Upload " + (i + 1));
			}

			setRouteIndividualDevices(routeIndividualDevices);
		}

		if (observable == sysexSendReceiveManager) {

			SysexSendReceive sysexSendReceive = (SysexSendReceive) object;

			if (sysexSendReceiveDevices.containsKey(sysexSendReceive)) {

				VirtualMidiDevice virtualMidiDevice = (VirtualMidiDevice) sysexSendReceiveDevices
						.get(sysexSendReceive);

				midiDeviceRouting.disconnectDevice(virtualMidiDevice);

				sysexSendReceiveDevices.remove(sysexSendReceive);

			} else {

				VirtualMidiDevice virtualMidiDevice = new VirtualMidiDevice("",
						-1, -1);

				virtualMidiDevice.setMidiInReceiver(sysexSendReceive);

				sysexSendReceive.setReceiver(virtualMidiDevice
						.getMidiOutReceiver());

				midiDeviceRouting.connectDevices(miosStudioInPort,
						virtualMidiDevice);
				midiDeviceRouting.connectDevices(virtualMidiDevice,
						miosStudioOutPort);

				sysexSendReceiveDevices
						.put(sysexSendReceive, virtualMidiDevice);
			}

			Collection virtualMidiDevices = sysexSendReceiveDevices.values();

			for (int i = 0; i < virtualMidiDevices.size(); i++) {

				VirtualMidiDevice virtualMidiDevice = (VirtualMidiDevice) virtualMidiDevices
						.toArray()[i];

				virtualMidiDevice.setName("Sysex Send/Receive " + (i + 1));
			}

			setRouteIndividualDevices(routeIndividualDevices);
		}

		if (observable == midiDeviceManager) {

			MidiDevice midiDevice = (MidiDevice) object;

			if (!midiDeviceManager.getSelectedMidiReadDevices().values()
					.contains(midiDevice)
					&& !midiDeviceManager.getSelectedMidiWriteDevices()
							.values().contains(midiDevice)) {
				midiDeviceRouting.disconnectDevice(midiDevice);
			}

			setRouteIndividualDevices(routeIndividualDevices);
		}

		if (observable == midiFilterManager) {

			MidiFilter midiFilter = (MidiFilter) object;

			if (midiFilterDevices.containsKey(midiFilter)) {

				midiDeviceRouting
						.disconnectDevice((VirtualMidiDevice) midiFilterDevices
								.get(midiFilter));

				midiFilterDevices.remove(midiFilter);

			} else {

				VirtualMidiDevice virtualMidiDevice = new VirtualMidiDevice("",
						-1, -1);

				virtualMidiDevice.setMidiInReceiver(midiFilter);

				midiFilter.setReceiver(virtualMidiDevice.getMidiOutReceiver());

				midiFilterDevices.put(midiFilter, virtualMidiDevice);
			}

			Collection virtualMidiDevices = midiFilterDevices.values();

			for (int i = 0; i < virtualMidiDevices.size(); i++) {

				VirtualMidiDevice virtualMidiDevice = (VirtualMidiDevice) virtualMidiDevices
						.toArray()[i];

				virtualMidiDevice.setName("MIDI Filter " + (i + 1));
			}

			setRouteIndividualDevices(routeIndividualDevices);
		}
	}
}
