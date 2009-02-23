package org.midibox.midi.xml;

import org.midibox.midi.MidiFilterDevice;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiFilterDeviceXML extends VirtualMidiDeviceXML {

	public final static String TAG_ROOT_ELEMENT = "midiFilterDevice";

	protected MidiFilterDevice midiFilterDevice;

	public MidiFilterDeviceXML(MidiFilterDevice midiFilterDevice,
			String rootElementTag) {

		super(midiFilterDevice, rootElementTag);

		this.midiFilterDevice = midiFilterDevice;

		tags.add(MidiFilterXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == MidiFilterXML.TAG_ROOT_ELEMENT) {

			MidiFilterXML midiFilterXML = new MidiFilterXML(midiFilterDevice
					.getMidiFilter(), MidiFilterXML.TAG_ROOT_ELEMENT);

			midiFilterXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		MidiFilterXML midiFilterXML = new MidiFilterXML(midiFilterDevice
				.getMidiFilter(), MidiFilterXML.TAG_ROOT_ELEMENT);

		midiFilterXML.saveXML(rootElement);
	}
}
