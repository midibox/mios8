package org.midibox.midi.xml;

import javax.sound.midi.MidiDevice;

import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Node;

public class MidiDeviceXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "midiDevice";

	public final static String ATTR_NAME = "name";

	public final static String ATTR_HASH_CODE = "hashCode";

	protected MidiDevice midiDevice;

	public MidiDeviceXML(MidiDevice midiDevice, String rootElementTag) {

		super(rootElementTag);

		this.midiDevice = midiDevice;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_NAME, midiDevice.getDeviceInfo()
				.getName());

		rootElement.setAttribute(ATTR_HASH_CODE, ""
				+ midiDevice.getDeviceInfo().toString().hashCode());
	}
}
