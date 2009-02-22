package org.midibox.midi.xml;

import org.midibox.midi.MidiParameterControl;
import org.w3c.dom.Node;

public class MidiParameterControlXML extends MidiParameterXML {

	protected MidiParameterControl midiParameterControl;
	
	protected String receiveAttr = "receive"; 
	
	protected String sendAttr = "send";
	
	protected String globalAttr = "global";
	
	protected String learnAttr = "learn";
	
	protected String typeAttr = "type";
	
	protected String defaultValueAttr = "defaultValue";
	
	public MidiParameterControlXML(MidiParameterControl midiParameterControl, String rootElementTag) {
		
		super(midiParameterControl, rootElementTag);
		
		this.midiParameterControl = midiParameterControl;
	}
	
	public void saveXML(Node node) {

		super.saveXML(node);
		
		rootElement.setAttribute(typeAttr, "" + midiParameterControl.getType());
		
		rootElement.setAttribute(defaultValueAttr, "" + midiParameterControl.getMidiDefaultValue());
		
		rootElement.setAttribute(sendAttr, midiParameterControl.isSend() ? "true" : "false");
		
		rootElement.setAttribute(receiveAttr, midiParameterControl.isReceive() ? "true" : "false");
		
		rootElement.setAttribute(globalAttr, midiParameterControl.isGlobal() ? "true" : "false");
		
		rootElement.setAttribute(learnAttr, midiParameterControl.isLearn() ? "true" : "false");		
	}
}
