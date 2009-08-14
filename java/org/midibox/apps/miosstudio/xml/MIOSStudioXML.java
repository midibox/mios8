package org.midibox.apps.miosstudio.xml;

import org.midibox.apps.miosstudio.MIOSStudio;
import org.midibox.midi.xml.MidiDeviceManagerXML;
import org.midibox.midi.xml.MidiDeviceRoutingXML;
import org.midibox.midi.xml.MidiFilterDeviceManagerXML;
import org.midibox.midi.xml.MidiFilterDeviceXML;
import org.midibox.midi.xml.MidiKeyboardControllerDeviceXML;
import org.midibox.midi.xml.MidiMonitorFilteredDeviceXML;
import org.midibox.midi.xml.SysexSendReceiveDeviceManagerXML;
import org.midibox.mios.xml.DebugFunctionDeviceXML;
import org.midibox.mios.xml.HexFileUploadDeviceManagerXML;
import org.midibox.mios.xml.LCDMessageDeviceXML;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MIOSStudioXML extends XMLUtils {

	public static final String TAG_ROOT_ELEMENT = "miosStudio";

	public static final String TAG_MIDI_THRU_FILTER_DEVICE = "thruMidiFilterDevice";

	public static final String TAG_MIDI_DEVICE_MANAGER = "midiDeviceManager";

	public static final String TAG_MIDI_FILTER_DEVICE_MANAGER = "midiFilterDeviceManager";

	public static final String TAG_IN_PORT_MIDI_MONITOR_FILTERED_DEVICE = "inPortMidiMonitorFilteredDevice";

	public static final String TAG_OUT_PORT_MIDI_MONITOR_FILTERED_DEVICE = "outPortMidiMonitorFilteredDevice";

	public static final String TAG_MIDI_KEYBOARD_CONTROLLER_DEVICE = "midiKeyboardControllerDevice";
	
	public static final String TAG_SYSEX_SEND_RECEIVE_DEVICE_MANAGER = "sysexSendReciveDeviceManager";

	public static final String TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER = "hexFileUploadDeviceManager";

	public static final String TAG_LCD_MESSAGE_DEVICE = "lcdMessageDevice";

	public static final String TAG_DEBUG_FUNCTION_DEVICE = "debugFunctionDevice";

	public static final String TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED_DEVICE = "miosTeminalMidiMonitorFilteredDevice";

	public static final String TAG_MIDI_DEVICE_ROUTING = "midiDeviceRouting";

	public static final String ATTR_MIDI_THRU_OUT_PORT = "midiThruOutPort";

	public static final String ATTR_ROUTE_INDIVIDUAL_DEVICES = "routeIndividualDevices";

	protected MIOSStudio miosStudio;

	public MIOSStudioXML(MIOSStudio miosStudio, String rootElementTag) {

		super(rootElementTag);

		this.miosStudio = miosStudio;

		addTag(TAG_MIDI_THRU_FILTER_DEVICE);
		addTag(TAG_MIDI_DEVICE_MANAGER);
		addTag(TAG_MIDI_FILTER_DEVICE_MANAGER);
		addTag(TAG_OUT_PORT_MIDI_MONITOR_FILTERED_DEVICE);
		addTag(TAG_IN_PORT_MIDI_MONITOR_FILTERED_DEVICE);
		addTag(TAG_MIDI_KEYBOARD_CONTROLLER_DEVICE);
		addTag(TAG_SYSEX_SEND_RECEIVE_DEVICE_MANAGER);
		addTag(TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER);
		addTag(TAG_LCD_MESSAGE_DEVICE);
		addTag(TAG_DEBUG_FUNCTION_DEVICE);
		addTag(TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED_DEVICE);
		addTag(TAG_MIDI_DEVICE_ROUTING);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			miosStudio.setMidiThruOutPort(stringToBoolean(element
					.getAttribute(ATTR_MIDI_THRU_OUT_PORT)));

			miosStudio.setRouteIndividualDevices(stringToBoolean(element
					.getAttribute(ATTR_ROUTE_INDIVIDUAL_DEVICES)));

		} else if (name == TAG_MIDI_THRU_FILTER_DEVICE) {

			MidiFilterDeviceXML midiFilterDeviceXML = new MidiFilterDeviceXML(
					miosStudio.getMidiThruFilterDevice(),
					TAG_MIDI_THRU_FILTER_DEVICE);

			midiFilterDeviceXML.loadXML(element);

		} else if (name == TAG_MIDI_DEVICE_MANAGER) {

			MidiDeviceManagerXML midiDeviceManagerXML = new MidiDeviceManagerXML(
					miosStudio.getMidiDeviceManager(), TAG_MIDI_DEVICE_MANAGER);

			midiDeviceManagerXML.loadXML(element);

		} else if (name == TAG_MIDI_FILTER_DEVICE_MANAGER) {

			MidiFilterDeviceManagerXML midiFilterDeviceManagerXML = new MidiFilterDeviceManagerXML(
					miosStudio.getMidiFilterManager(),
					TAG_MIDI_FILTER_DEVICE_MANAGER);

			midiFilterDeviceManagerXML.loadXML(element);

		} else if (name == TAG_OUT_PORT_MIDI_MONITOR_FILTERED_DEVICE) {

			MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
					miosStudio.getMidiOutPortMonitorDevice(),
					TAG_OUT_PORT_MIDI_MONITOR_FILTERED_DEVICE);

			midiMonitorFilteredDeviceXML.loadXML(element);

		} else if (name == TAG_IN_PORT_MIDI_MONITOR_FILTERED_DEVICE) {

			MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
					miosStudio.getMidiInPortMonitorDevice(),
					TAG_IN_PORT_MIDI_MONITOR_FILTERED_DEVICE);

			midiMonitorFilteredDeviceXML.loadXML(element);

		} else if (name == TAG_MIDI_KEYBOARD_CONTROLLER_DEVICE) {

			MidiKeyboardControllerDeviceXML midiKeyboardControllerDeviceXML = new MidiKeyboardControllerDeviceXML(
					miosStudio.getMidiKeyboardControllerDevice(),
					TAG_MIDI_KEYBOARD_CONTROLLER_DEVICE);

			midiKeyboardControllerDeviceXML.loadXML(element);

		} else if (name == TAG_SYSEX_SEND_RECEIVE_DEVICE_MANAGER) {

			SysexSendReceiveDeviceManagerXML sysexSendReceiveDeviceManagerXML = new SysexSendReceiveDeviceManagerXML(
					miosStudio.getSysexSendReceiveDeviceManager(),
					TAG_SYSEX_SEND_RECEIVE_DEVICE_MANAGER);

			sysexSendReceiveDeviceManagerXML.loadXML(element);

		} else if (name == TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER) {

			HexFileUploadDeviceManagerXML hexFileUploadDeviceManagerXML = new HexFileUploadDeviceManagerXML(
					miosStudio.getHexFileUploadDeviceManager(),
					TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER);

			hexFileUploadDeviceManagerXML.loadXML(element);

		} else if (name == TAG_LCD_MESSAGE_DEVICE) {

			LCDMessageDeviceXML lcdMessageDeviceXML = new LCDMessageDeviceXML(
					miosStudio.getLcdMessageDevice(), TAG_LCD_MESSAGE_DEVICE);

			lcdMessageDeviceXML.loadXML(element);

		} else if (name == TAG_DEBUG_FUNCTION_DEVICE) {

			DebugFunctionDeviceXML debugFunctionDeviceXML = new DebugFunctionDeviceXML(
					miosStudio.getDebugFunctionDevice(),
					TAG_DEBUG_FUNCTION_DEVICE);

			debugFunctionDeviceXML.loadXML(element);

		} else if (name == TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED_DEVICE) {

			MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
					miosStudio.getMIOSTerminalDevice(),
					TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED_DEVICE);

			midiMonitorFilteredDeviceXML.loadXML(element);

		} else if (name == TAG_MIDI_DEVICE_ROUTING) {

			boolean routeIndividualDevices = miosStudio
					.isRouteIndividualDevices();

			miosStudio.setRouteIndividualDevices(true);

			MidiDeviceRoutingXML midiDeviceRoutingXML = new MidiDeviceRoutingXML(
					miosStudio.getMidiDeviceRouting(), TAG_MIDI_DEVICE_ROUTING);

			midiDeviceRoutingXML.loadXML(element);

			miosStudio.setRouteIndividualDevices(routeIndividualDevices);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_ROUTE_INDIVIDUAL_DEVICES, miosStudio
				.isRouteIndividualDevices() ? "true" : "false");

		rootElement.setAttribute(ATTR_MIDI_THRU_OUT_PORT, miosStudio
				.isMidiThruOutPort() ? "true" : "false");

		MidiFilterDeviceXML midiFilterDeviceXML = new MidiFilterDeviceXML(
				miosStudio.getMidiThruFilterDevice(),
				TAG_MIDI_THRU_FILTER_DEVICE);

		midiFilterDeviceXML.saveXML(rootElement);

		saveMIDIXML(miosStudio);

		saveMIOSXML(miosStudio);

		MidiDeviceRoutingXML midiDeviceRoutingXML = new MidiDeviceRoutingXML(
				miosStudio.getMidiDeviceRouting(), TAG_MIDI_DEVICE_ROUTING);

		midiDeviceRoutingXML.saveXML(rootElement);
	}

	protected void saveMIDIXML(MIOSStudio miosStudio) {

		MidiDeviceManagerXML midiDeviceManagerXML = new MidiDeviceManagerXML(
				miosStudio.getMidiDeviceManager(), TAG_MIDI_DEVICE_MANAGER);

		midiDeviceManagerXML.saveXML(rootElement);

		MidiFilterDeviceManagerXML midiFilterDeviceManagerXML = new MidiFilterDeviceManagerXML(
				miosStudio.getMidiFilterManager(),
				TAG_MIDI_FILTER_DEVICE_MANAGER);

		midiFilterDeviceManagerXML.saveXML(rootElement);

		MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
				miosStudio.getMidiOutPortMonitorDevice(),
				TAG_OUT_PORT_MIDI_MONITOR_FILTERED_DEVICE);

		midiMonitorFilteredDeviceXML.saveXML(rootElement);

		midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
				miosStudio.getMidiInPortMonitorDevice(),
				TAG_IN_PORT_MIDI_MONITOR_FILTERED_DEVICE);

		midiMonitorFilteredDeviceXML.saveXML(rootElement);

		MidiKeyboardControllerDeviceXML midiControllerDeviceXML = new MidiKeyboardControllerDeviceXML(
				miosStudio.getMidiKeyboardControllerDevice(),
				TAG_MIDI_KEYBOARD_CONTROLLER_DEVICE);

		midiControllerDeviceXML.saveXML(rootElement);
		
		SysexSendReceiveDeviceManagerXML sysexSendReceiveDeviceManagerXML = new SysexSendReceiveDeviceManagerXML(
				miosStudio.getSysexSendReceiveDeviceManager(),
				TAG_SYSEX_SEND_RECEIVE_DEVICE_MANAGER);

		sysexSendReceiveDeviceManagerXML.saveXML(rootElement);
	}

	protected void saveMIOSXML(MIOSStudio miosStudio) {

		HexFileUploadDeviceManagerXML hexFileUploadDeviceManagerXML = new HexFileUploadDeviceManagerXML(
				miosStudio.getHexFileUploadDeviceManager(),
				TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER);

		hexFileUploadDeviceManagerXML.saveXML(rootElement);

		LCDMessageDeviceXML lcdMessageDeviceXML = new LCDMessageDeviceXML(
				miosStudio.getLcdMessageDevice(), TAG_LCD_MESSAGE_DEVICE);

		lcdMessageDeviceXML.saveXML(rootElement);

		DebugFunctionDeviceXML debugFunctionDeviceXML = new DebugFunctionDeviceXML(
				miosStudio.getDebugFunctionDevice(), TAG_DEBUG_FUNCTION_DEVICE);

		debugFunctionDeviceXML.saveXML(rootElement);

		MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
				miosStudio.getMIOSTerminalDevice(),
				TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED_DEVICE);

		midiMonitorFilteredDeviceXML.saveXML(rootElement);
	}
}
