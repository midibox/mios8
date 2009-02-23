package org.midibox.midi.xml;

import org.midibox.midi.MidiKeyboardController;
import org.w3c.dom.Node;

public class MidiKeyboardControllerXML extends MidiParameterControlManagerXML {

	public final static String TAG_ROOT_ELEMENT = "midiKeyboarController";

	public final static String ATTR_VELOCITY_VAL = "velocityVal";

	protected MidiKeyboardController midiKeyboardController;

	public MidiKeyboardControllerXML(
			MidiKeyboardController midiKeyboardController, String rootElementTag) {

		super(midiKeyboardController, rootElementTag);

		this.midiKeyboardController = midiKeyboardController;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_VELOCITY_VAL,
				intToString(midiKeyboardController.getVelocityVal()));
	}
}
