package org.midibox.midi.xml;

import org.midibox.midi.MidiKeyboardControllerDevice;
import org.w3c.dom.Node;

public class MidiKeyboardControllerDeviceXML extends VirtualMidiDeviceXML {
	
	protected MidiKeyboardControllerDevice midiKeyboardControllerDevice;
	
	protected String midiKeyboardControllerTag = "midiKeyboardController";
	
	public MidiKeyboardControllerDeviceXML(MidiKeyboardControllerDevice midiKeyboardControllerDevice, String rootElementTag) {
		
		super(midiKeyboardControllerDevice, rootElementTag);
		
		this.midiKeyboardControllerDevice = midiKeyboardControllerDevice;
	}	
	
	public void saveXML(Node node) {

		super.saveXML(node);
				
		MidiKeyboardControllerXML midiKeyboardControllerXML = new MidiKeyboardControllerXML(midiKeyboardControllerDevice.getMidiKeyboardController(), midiKeyboardControllerTag);
		
		midiKeyboardControllerXML.saveXML(rootElement);
	}
}
