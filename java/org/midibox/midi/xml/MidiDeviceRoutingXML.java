package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import javax.sound.midi.MidiDevice;

import org.midibox.midi.MidiDeviceRouting;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiDeviceRoutingXML extends XMLUtils {

	protected MidiDeviceRouting midiDeviceRouting;

	protected String logicalConnectionsTag = "logicalConnections";

	protected String logicalConnectionTag = "logicalConnection";

	protected String sourceMidiDeviceTag = "sourceMidiDevice";

	protected String targetMidiDeviceTag = "targetMidiDevice";

	protected String portsReleasedAttr = "portsReleased";

	public MidiDeviceRoutingXML(MidiDeviceRouting midiDeviceRouting,
			String rootElementTag) {

		super(rootElementTag);

		this.midiDeviceRouting = midiDeviceRouting;

		tags.add(logicalConnectionsTag);
		tags.add(logicalConnectionTag);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {
			
			midiDeviceRouting.disconnectAll();

		} else if (name == logicalConnectionTag) {

			Element transmittingDeviceElement = (Element) element
					.getFirstChild();

			MidiDevice transmittingDevice = findDeviceHash(midiDeviceRouting
					.getMidiReadDevices(), Integer
					.parseInt(transmittingDeviceElement
							.getAttribute("hashCode")));

			Element receivingDeviceElement = (Element) element.getLastChild();

			MidiDevice receivingDevice = findDeviceHash(midiDeviceRouting
					.getMidiWriteDevices(), Integer
					.parseInt(receivingDeviceElement.getAttribute("hashCode")));
			;
			
			if (transmittingDevice != null && receivingDevice != null) {

				midiDeviceRouting.connectDevices(transmittingDevice,
					receivingDevice);
			}
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(portsReleasedAttr, midiDeviceRouting
				.isPortsReleased() ? "true" : "false");

		Element logicalConnectionsElement = document
				.createElement(logicalConnectionsTag);

		rootElement.appendChild(logicalConnectionsElement);

		Iterator it = midiDeviceRouting.getLogicalConnections().iterator();

		while (it.hasNext()) {

			MidiDeviceRouting.LogicalConnection logicalConnection = (MidiDeviceRouting.LogicalConnection) it
					.next();

			Element logicalConnectionElement = document
					.createElement(logicalConnectionTag);

			logicalConnectionsElement.appendChild(logicalConnectionElement);


			MidiDeviceXML sourceMidiDeviceXML = new MidiDeviceXML(
					logicalConnection.getSourceDevice(), sourceMidiDeviceTag);

			sourceMidiDeviceXML.saveXML(logicalConnectionElement);

		
			MidiDeviceXML targetMidiDeviceXML = new MidiDeviceXML(
					logicalConnection.getTargetDevice(), targetMidiDeviceTag);

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
