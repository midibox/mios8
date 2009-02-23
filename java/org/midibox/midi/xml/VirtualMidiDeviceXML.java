package org.midibox.midi.xml;

import org.midibox.midi.VirtualMidiDevice;
import org.w3c.dom.Element;

public class VirtualMidiDeviceXML extends MidiDeviceXML {

	public final static String TAG_ROOT_ELEMENT = "virtualMidiDevice";

	protected VirtualMidiDevice virtualMidiDevice;

	public VirtualMidiDeviceXML(VirtualMidiDevice virtualMidiDevice,
			String rootElementTag) {

		super(virtualMidiDevice, rootElementTag);

		this.virtualMidiDevice = virtualMidiDevice;
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == TAG_ROOT_ELEMENT) {

			virtualMidiDevice.setName(element.getAttribute(ATTR_NAME));
		}
	}
}
