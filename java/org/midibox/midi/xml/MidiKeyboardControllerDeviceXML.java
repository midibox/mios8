package org.midibox.midi.xml;

import org.midibox.midi.MidiKeyboardControllerDevice;
import org.w3c.dom.Node;

public class MidiKeyboardControllerDeviceXML extends VirtualMidiDeviceXML {

	public final static String TAG_ROOT_ELEMENT = "midiKeyboarControllerDevice";

	protected MidiKeyboardControllerDevice midiKeyboardControllerDevice;

	public MidiKeyboardControllerDeviceXML(
			MidiKeyboardControllerDevice midiKeyboardControllerDevice,
			String rootElementTag) {

		super(midiKeyboardControllerDevice, rootElementTag);

		this.midiKeyboardControllerDevice = midiKeyboardControllerDevice;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		MidiKeyboardControllerXML midiKeyboardControllerXML = new MidiKeyboardControllerXML(
				midiKeyboardControllerDevice.getMidiKeyboardController(),
				MidiKeyboardControllerXML.TAG_ROOT_ELEMENT);

		midiKeyboardControllerXML.saveXML(rootElement);
	}
}
