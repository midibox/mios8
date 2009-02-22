package org.midibox.midi.xml;

import org.midibox.midi.MidiParameter;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Node;

public class MidiParameterXML extends XMLUtils {

	protected MidiParameter midiParameter;
	
	protected String channelAttr = "channel";
	protected String statusAttr = "status";
	protected String valueAttr = "value";
	protected String numberAttr = "number";
	protected String highResolutionAttr = "highResolution";
	
	protected MidiParameterXML(MidiParameter midiParameter, String rootElementTag) {
		
		super(rootElementTag);
		
		this.midiParameter = midiParameter;
	}
	
	public void saveXML(Node node) {

		super.saveXML(node);
		
		rootElement.setAttribute(channelAttr, "" + midiParameter.getMidiChannel());
		rootElement.setAttribute(statusAttr, "" + midiParameter.getMidiStatus());
		rootElement.setAttribute(valueAttr, "" + midiParameter.getMidiValue());
		rootElement.setAttribute(numberAttr, "" + midiParameter.getMidiNumber());
		rootElement.setAttribute(highResolutionAttr, midiParameter.isHighResolution() ? "true" : "false");
	}
}
