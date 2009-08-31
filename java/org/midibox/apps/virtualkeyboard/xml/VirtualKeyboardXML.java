package org.midibox.apps.virtualkeyboard.xml;

import org.midibox.apps.virtualkeyboard.VirtualKeyboard;
import org.midibox.midi.xml.MidiDeviceRoutingXML;
import org.midibox.midi.xml.MidiKeyboardControllerXML;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class VirtualKeyboardXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "virtualKeyboard";

	protected VirtualKeyboard virtualKeyboard;

	public VirtualKeyboardXML(VirtualKeyboard virtualKeyboard,
			String rootElementTag) {

		super(rootElementTag);

		this.virtualKeyboard = virtualKeyboard;

		addTag(MidiDeviceRoutingXML.TAG_ROOT_ELEMENT);
		addTag(MidiKeyboardControllerXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == MidiDeviceRoutingXML.TAG_ROOT_ELEMENT) {

			MidiDeviceRoutingXML midiDeviceRoutingXML = new MidiDeviceRoutingXML(
					virtualKeyboard.getMidiDeviceRouting(),
					MidiDeviceRoutingXML.TAG_ROOT_ELEMENT);

			midiDeviceRoutingXML.loadXML(element);

		} else if (name == MidiKeyboardControllerXML.TAG_ROOT_ELEMENT) {

			MidiKeyboardControllerXML midiKeyboardControllerXML = new MidiKeyboardControllerXML(
					virtualKeyboard.getMidiKeyboardController(),
					MidiKeyboardControllerXML.TAG_ROOT_ELEMENT);

			midiKeyboardControllerXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		MidiDeviceRoutingXML midiDeviceRoutingXML = new MidiDeviceRoutingXML(
				virtualKeyboard.getMidiDeviceRouting(),
				MidiDeviceRoutingXML.TAG_ROOT_ELEMENT);

		midiDeviceRoutingXML.saveXML(rootElement);

		MidiKeyboardControllerXML midiKeyboardControllerDeviceXML = new MidiKeyboardControllerXML(
				virtualKeyboard.getMidiKeyboardController(),
				MidiKeyboardControllerXML.TAG_ROOT_ELEMENT);

		midiKeyboardControllerDeviceXML.saveXML(rootElement);
	}
}
