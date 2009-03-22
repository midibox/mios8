package org.midibox.midi.xml;

import org.midibox.midi.MidiMonitorFilteredDevice;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiMonitorFilteredDeviceXML extends VirtualMidiDeviceXML {

	protected MidiMonitorFilteredDevice midiMonitorFilteredDevice;

	public final static String TAG_ROOT_ELEMENT = "midiMonitorFilteredDevice";

	public MidiMonitorFilteredDeviceXML(
			MidiMonitorFilteredDevice midiMonitorFilteredDevice,
			String rootElementTag) {

		super(midiMonitorFilteredDevice, rootElementTag);

		this.midiMonitorFilteredDevice = midiMonitorFilteredDevice;

		addTag(MidiMonitorFilteredXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == MidiMonitorFilteredXML.TAG_ROOT_ELEMENT) {

			MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
					midiMonitorFilteredDevice.getMidiMonitorFiltered(),
					MidiMonitorFilteredXML.TAG_ROOT_ELEMENT);

			midiMonitorFilteredXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		MidiMonitorFilteredXML midiMonitorFilteredXML = new MidiMonitorFilteredXML(
				midiMonitorFilteredDevice.getMidiMonitorFiltered(),
				MidiMonitorFilteredXML.TAG_ROOT_ELEMENT);

		midiMonitorFilteredXML.saveXML(rootElement);
	}
}
