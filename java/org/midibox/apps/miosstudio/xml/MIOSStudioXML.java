package org.midibox.apps.miosstudio.xml;

import org.midibox.apps.miosstudio.MIOSStudio;
import org.midibox.midi.xml.MidiDeviceManagerXML;
import org.midibox.midi.xml.MidiDeviceRoutingXML;
import org.midibox.midi.xml.MidiFilterManagerXML;
import org.midibox.midi.xml.MidiFilterXML;
import org.midibox.midi.xml.MidiKeyboardControllerXML;
import org.midibox.midi.xml.MidiMonitorFilteredXML;
import org.midibox.midi.xml.SysexSendReceiveManagerXML;
import org.midibox.mios.xml.DebugFunctionXML;
import org.midibox.mios.xml.HexFileUploadManagerXML;
import org.midibox.mios.xml.LCDMessageXML;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MIOSStudioXML extends XMLUtils {

	public static final String TAG_ROOT_ELEMENT = "miosStudio";

	public static final String TAG_MIDI_THRU_FILTER = "thruMidiFilter";

	public static final String TAG_MIDI_DEVICE_MANAGER = "midiDeviceManager";

	public static final String TAG_MIDI_FILTER_MANAGER = "midiFilterManager";

	public static final String TAG_IN_PORT_MIDI_MONITOR_FILTERED = "inPortMidiMonitorFiltered";

	public static final String TAG_OUT_PORT_MIDI_MONITOR_FILTERED = "outPortMidiMonitorFiltered";

	public static final String TAG_MIDI_KEYBOARD_CONTROLLER = "midiKeyboardController";

	public static final String TAG_SYSEX_SEND_RECEIVE_MANAGER = "sysexSendReciveManager";

	public static final String TAG_HEX_FILE_UPLOAD_MANAGER = "hexFileUploadManager";

	public static final String TAG_LCD_MESSAGE = "lcdMessage";

	public static final String TAG_DEBUG_FUNCTION = "debugFunction";

	public static final String TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED = "miosTeminalMidiMonitorFiltered";

	public static final String TAG_MIDI_DEVICE_ROUTING = "midiDeviceRouting";

	public static final String ATTR_MIDI_THRU_OUT_PORT = "midiThruOutPort";

	public static final String ATTR_ROUTE_INDIVIDUAL_DEVICES = "routeIndividualDevices";

	protected MIOSStudio miosStudio;

	public MIOSStudioXML(MIOSStudio miosStudio, String rootElementTag) {

		super(rootElementTag);

		this.miosStudio = miosStudio;

		addTag(TAG_MIDI_THRU_FILTER);
		addTag(TAG_MIDI_DEVICE_MANAGER);
		addTag(TAG_MIDI_FILTER_MANAGER);
		addTag(TAG_OUT_PORT_MIDI_MONITOR_FILTERED);
		addTag(TAG_IN_PORT_MIDI_MONITOR_FILTERED);
		addTag(TAG_MIDI_KEYBOARD_CONTROLLER);
		addTag(TAG_SYSEX_SEND_RECEIVE_MANAGER);
		addTag(TAG_HEX_FILE_UPLOAD_MANAGER);
		addTag(TAG_LCD_MESSAGE);
		addTag(TAG_DEBUG_FUNCTION);
		addTag(TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED);
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

		} else if (name == TAG_MIDI_THRU_FILTER) {

			MidiFilterXML midiFilterXML = new MidiFilterXML(miosStudio
					.getMidiThruFilter(), TAG_MIDI_THRU_FILTER);

			midiFilterXML.loadXML(element);

		} else if (name == TAG_MIDI_DEVICE_MANAGER) {

			MidiDeviceManagerXML midiDeviceManagerXML = new MidiDeviceManagerXML(
					miosStudio.getMidiDeviceManager(), TAG_MIDI_DEVICE_MANAGER);

			midiDeviceManagerXML.loadXML(element);

		} else if (name == TAG_MIDI_FILTER_MANAGER) {

			MidiFilterManagerXML midiFilterDeviceManagerXML = new MidiFilterManagerXML(
					miosStudio.getMidiFilterManager(), TAG_MIDI_FILTER_MANAGER);

			midiFilterDeviceManagerXML.loadXML(element);

		} else if (name == TAG_OUT_PORT_MIDI_MONITOR_FILTERED) {

			MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
					miosStudio.getMidiOutPortMonitor(),
					TAG_OUT_PORT_MIDI_MONITOR_FILTERED);

			midiMonitorFilteredXML.loadXML(element);

		} else if (name == TAG_IN_PORT_MIDI_MONITOR_FILTERED) {

			MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
					miosStudio.getMidiInPortMonitor(),
					TAG_IN_PORT_MIDI_MONITOR_FILTERED);

			midiMonitorFilteredXML.loadXML(element);

		} else if (name == TAG_MIDI_KEYBOARD_CONTROLLER) {

			MidiKeyboardControllerXML midiKeyboardControllerXML = new MidiKeyboardControllerXML(
					miosStudio.getMidiKeyboardController(),
					TAG_MIDI_KEYBOARD_CONTROLLER);

			midiKeyboardControllerXML.loadXML(element);

		} else if (name == TAG_SYSEX_SEND_RECEIVE_MANAGER) {

			SysexSendReceiveManagerXML sysexSendReceiveManagerXML = new SysexSendReceiveManagerXML(
					miosStudio.getSysexSendReceiveManager(),
					TAG_SYSEX_SEND_RECEIVE_MANAGER);

			sysexSendReceiveManagerXML.loadXML(element);

		} else if (name == TAG_HEX_FILE_UPLOAD_MANAGER) {

			HexFileUploadManagerXML hexFileUploadManagerXML = new HexFileUploadManagerXML(
					miosStudio.getHexFileUploadManager(),
					TAG_HEX_FILE_UPLOAD_MANAGER);

			hexFileUploadManagerXML.loadXML(element);

		} else if (name == TAG_LCD_MESSAGE) {

			LCDMessageXML lcdMessageXML = new LCDMessageXML(miosStudio
					.getLcdMessage(), TAG_LCD_MESSAGE);

			lcdMessageXML.loadXML(element);

		} else if (name == TAG_DEBUG_FUNCTION) {

			DebugFunctionXML debugFunctionXML = new DebugFunctionXML(miosStudio
					.getDebugFunction(), TAG_DEBUG_FUNCTION);

			debugFunctionXML.loadXML(element);

		} else if (name == TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED) {

			MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
					miosStudio.getMIOSTerminal(),
					TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED);

			midiMonitorFilteredXML.loadXML(element);

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

		MidiFilterXML midiFilterXML = new MidiFilterXML(miosStudio
				.getMidiThruFilter(), TAG_MIDI_THRU_FILTER);

		midiFilterXML.saveXML(rootElement);

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

		MidiFilterManagerXML midiFilterDeviceManagerXML = new MidiFilterManagerXML(
				miosStudio.getMidiFilterManager(), TAG_MIDI_FILTER_MANAGER);

		midiFilterDeviceManagerXML.saveXML(rootElement);

		MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
				miosStudio.getMidiOutPortMonitor(),
				TAG_OUT_PORT_MIDI_MONITOR_FILTERED);

		midiMonitorFilteredXML.saveXML(rootElement);

		midiMonitorFilteredXML = new MidiMonitorFilteredXML(miosStudio
				.getMidiInPortMonitor(), TAG_IN_PORT_MIDI_MONITOR_FILTERED);

		midiMonitorFilteredXML.saveXML(rootElement);

		MidiKeyboardControllerXML midiControllerXML = new MidiKeyboardControllerXML(
				miosStudio.getMidiKeyboardController(),
				TAG_MIDI_KEYBOARD_CONTROLLER);

		midiControllerXML.saveXML(rootElement);

		SysexSendReceiveManagerXML sysexSendReceiveManagerXML = new SysexSendReceiveManagerXML(
				miosStudio.getSysexSendReceiveManager(),
				TAG_SYSEX_SEND_RECEIVE_MANAGER);

		sysexSendReceiveManagerXML.saveXML(rootElement);
	}

	protected void saveMIOSXML(MIOSStudio miosStudio) {

		HexFileUploadManagerXML hexFileUploadManagerXML = new HexFileUploadManagerXML(
				miosStudio.getHexFileUploadManager(),
				TAG_HEX_FILE_UPLOAD_MANAGER);

		hexFileUploadManagerXML.saveXML(rootElement);

		LCDMessageXML lcdMessageXML = new LCDMessageXML(miosStudio
				.getLcdMessage(), TAG_LCD_MESSAGE);

		lcdMessageXML.saveXML(rootElement);

		DebugFunctionXML debugFunctionXML = new DebugFunctionXML(miosStudio
				.getDebugFunction(), TAG_DEBUG_FUNCTION);

		debugFunctionXML.saveXML(rootElement);

		MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
				miosStudio.getMIOSTerminal(),
				TAG_MIOS_TERMINAL_MIDI_MONITOR_FILTERED);

		midiMonitorFilteredXML.saveXML(rootElement);
	}
}
