package org.midibox.midi.xml;

import org.midibox.midi.MidiKeyboardController;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiKeyboardControllerXML extends MidiParameterControlManagerXML {

	public final static String TAG_ROOT_ELEMENT = "midiKeyboardController";

	public final static String ATTR_VELOCITY_VAL = "velocityVal";

	protected MidiKeyboardController midiKeyboardController;

	public MidiKeyboardControllerXML(
			MidiKeyboardController midiKeyboardController, String rootElementTag) {

		super(midiKeyboardController, rootElementTag);

		this.midiKeyboardController = midiKeyboardController;
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			midiKeyboardController.setVelocityVal(stringToInt(element
					.getAttribute(ATTR_VELOCITY_VAL)));
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_VELOCITY_VAL,
				intToString(midiKeyboardController.getVelocityVal()));
	}
}
