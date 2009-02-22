package org.midibox.midi.xml;

import javax.sound.midi.MidiDevice;

import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Node;

public class MidiDeviceXML extends XMLUtils {

	protected String nameAttr = "name";

	protected String hashCodeAttr = "hashCode";
	
	protected MidiDevice midiDevice;

	public MidiDeviceXML(MidiDevice midiDevice, String rootElementTag) {

		super(rootElementTag);

		this.midiDevice = midiDevice;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement
				.setAttribute(nameAttr, midiDevice.getDeviceInfo().getName());

		rootElement.setAttribute(hashCodeAttr, ""
				+ midiDevice.getDeviceInfo().toString().hashCode());
	}
}
