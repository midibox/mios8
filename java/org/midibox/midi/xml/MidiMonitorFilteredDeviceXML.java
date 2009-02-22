package org.midibox.midi.xml;

import org.midibox.midi.MidiMonitorFilteredDevice;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiMonitorFilteredDeviceXML extends VirtualMidiDeviceXML {

	protected MidiMonitorFilteredDevice midiMonitorFilteredDevice;

	protected String midiMonitorFilteredTag = "midiMonitorFiltered";

	public MidiMonitorFilteredDeviceXML(
			MidiMonitorFilteredDevice midiMonitorFilteredDevice,
			String rootElementTag) {

		super(midiMonitorFilteredDevice, rootElementTag);

		this.midiMonitorFilteredDevice = midiMonitorFilteredDevice;

		tags.add(midiMonitorFilteredTag);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == midiMonitorFilteredTag) {

			MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
					midiMonitorFilteredDevice.getMidiMonitorFiltered(),
					midiMonitorFilteredTag);

			midiMonitorFilteredXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
				midiMonitorFilteredDevice.getMidiMonitorFiltered(),
				midiMonitorFilteredTag);

		midiMonitorFilteredXML.saveXML(rootElement);
	}
}
