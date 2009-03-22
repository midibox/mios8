package org.midibox.midi.xml;

import org.midibox.midi.MidiKeyboardControllerDevice;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiKeyboardControllerDeviceXML extends VirtualMidiDeviceXML {

	public final static String TAG_ROOT_ELEMENT = "midiKeyboardControllerDevice";

	protected MidiKeyboardControllerDevice midiKeyboardControllerDevice;

	public MidiKeyboardControllerDeviceXML(
			MidiKeyboardControllerDevice midiKeyboardControllerDevice,
			String rootElementTag) {

		super(midiKeyboardControllerDevice, rootElementTag);

		addTag(MidiKeyboardControllerXML.TAG_ROOT_ELEMENT);

		this.midiKeyboardControllerDevice = midiKeyboardControllerDevice;
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			MidiKeyboardControllerXML midiKeyboardControllerXML = new MidiKeyboardControllerXML(
					midiKeyboardControllerDevice.getMidiKeyboardController(),
					MidiKeyboardControllerXML.TAG_ROOT_ELEMENT);

			midiKeyboardControllerXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		MidiKeyboardControllerXML midiKeyboardControllerXML = new MidiKeyboardControllerXML(
				midiKeyboardControllerDevice.getMidiKeyboardController(),
				MidiKeyboardControllerXML.TAG_ROOT_ELEMENT);

		midiKeyboardControllerXML.saveXML(rootElement);
	}
}
