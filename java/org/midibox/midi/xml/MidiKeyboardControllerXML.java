package org.midibox.midi.xml;

import org.midibox.midi.MidiKeyboardController;
import org.w3c.dom.Node;

public class MidiKeyboardControllerXML extends MidiParameterControlManagerXML {
	
	protected MidiKeyboardController midiKeyboardController;
	
	protected String velocityValAttr = "velocityVal";
	
	public MidiKeyboardControllerXML(MidiKeyboardController midiKeyboardController, String rootElementTag) {
		
		super(midiKeyboardController, rootElementTag);
		
		this.midiKeyboardController = midiKeyboardController;
	}
	
	public void saveXML(Node node) {
		
		super.saveXML(node);
		
		rootElement.setAttribute(velocityValAttr, "" + midiKeyboardController.getVelocityVal());
	}
}
