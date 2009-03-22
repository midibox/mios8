package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import javax.sound.midi.MidiDevice;

import org.midibox.midi.MidiDeviceRouting;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiDeviceRoutingXML extends XMLUtils {

	public static final String TAG_ROOT_ELEMENT = "midiDeviceRouting";

	public static final String TAG_LOGICAL_CONNECTIONS = "logicalConnections";

	public static final String TAG_LOGICAL_CONNECTION = "logicalConnection";

	public static final String TAG_SOURCE_MIDI_DEVICE = "sourceMidiDevice";

	public static final String TAG_TARGET_MIDI_DEVICE = "targetMidiDevice";

	public static final String ATTR_PORTS_RELEASED = "portsReleased";

	protected MidiDeviceRouting midiDeviceRouting;

	public MidiDeviceRoutingXML(MidiDeviceRouting midiDeviceRouting,
			String rootElementTag) {

		super(rootElementTag);

		this.midiDeviceRouting = midiDeviceRouting;

		addTag(TAG_LOGICAL_CONNECTIONS);
		addTag(TAG_LOGICAL_CONNECTION);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			midiDeviceRouting.disconnectAll();

			midiDeviceRouting.setPortsReleased(stringToBoolean(element
					.getAttribute(ATTR_PORTS_RELEASED)));

		} else if (name == TAG_LOGICAL_CONNECTION) {

			Element transmittingDeviceElement = (Element) element
					.getFirstChild();

			MidiDevice transmittingDevice = findDeviceHash(midiDeviceRouting
					.getMidiReadDevices(), Integer
					.parseInt(transmittingDeviceElement
							.getAttribute(MidiDeviceXML.ATTR_HASH_CODE)));

			Element receivingDeviceElement = (Element) element.getLastChild();

			MidiDevice receivingDevice = findDeviceHash(midiDeviceRouting
					.getMidiWriteDevices(), Integer
					.parseInt(receivingDeviceElement
							.getAttribute(MidiDeviceXML.ATTR_HASH_CODE)));
			;

			if (transmittingDevice != null && receivingDevice != null) {

				midiDeviceRouting.connectDevices(transmittingDevice,
						receivingDevice);
			}
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_PORTS_RELEASED,
				booleanToString(midiDeviceRouting.isPortsReleased()));

		Element logicalConnectionsElement = document
				.createElement(TAG_LOGICAL_CONNECTIONS);

		rootElement.appendChild(logicalConnectionsElement);

		Iterator it = midiDeviceRouting.getLogicalConnections().iterator();

		while (it.hasNext()) {

			MidiDeviceRouting.LogicalConnection logicalConnection = (MidiDeviceRouting.LogicalConnection) it
					.next();

			Element logicalConnectionElement = document
					.createElement(TAG_LOGICAL_CONNECTION);

			logicalConnectionsElement.appendChild(logicalConnectionElement);

			MidiDeviceXML sourceMidiDeviceXML = new MidiDeviceXML(
					logicalConnection.getSourceDevice(), TAG_SOURCE_MIDI_DEVICE);

			sourceMidiDeviceXML.saveXML(logicalConnectionElement);

			MidiDeviceXML targetMidiDeviceXML = new MidiDeviceXML(
					logicalConnection.getTargetDevice(), TAG_TARGET_MIDI_DEVICE);

			targetMidiDeviceXML.saveXML(logicalConnectionElement);
		}
	}

	protected MidiDevice findDeviceHash(Vector midiDevices, int hash) {

		Iterator it = midiDevices.iterator();

		while (it.hasNext()) {

			MidiDevice midiDevice = (MidiDevice) it.next();

			if (midiDevice.getDeviceInfo().toString().hashCode() == hash) {

				return midiDevice;
			}
		}

		return null;
	}
}
