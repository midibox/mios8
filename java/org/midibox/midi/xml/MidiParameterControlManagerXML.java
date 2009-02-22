package org.midibox.midi.xml;

import java.util.Iterator;

import org.midibox.midi.MidiParameterControl;
import org.midibox.midi.MidiParameterControlManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiParameterControlManagerXML extends XMLUtils {

	protected MidiParameterControlManager midiParameterControlManager;
	
	protected String globalChannelAttr = "globalChannel";

	protected String midiParameterControlsTag = "midiParameterControls"; 
	
	protected String midiParameterControlTag = "midiParameterControl"; 
	
	public MidiParameterControlManagerXML(MidiParameterControlManager midiParameterControlManager, String rootElementTag) {
		
		super(rootElementTag);
		
		this.midiParameterControlManager = midiParameterControlManager;
	}
	
	public void saveXML(Node node) {
		
		super.saveXML(node);
				
		rootElement.setAttribute(globalChannelAttr, "" + midiParameterControlManager.getGlobalChannel());
		
		Element midiParameterControlsElement = document.createElement(midiParameterControlsTag);
		
		rootElement.appendChild(midiParameterControlsElement);
			
		Iterator it = midiParameterControlManager.getMidiParameters().iterator();
		
		while (it.hasNext()) {
					
			MidiParameterControl midiParameterControl = (MidiParameterControl) it.next();
			
			MidiParameterControlXML midiParameterControlXML = new MidiParameterControlXML(midiParameterControl, midiParameterControlTag);
						
			midiParameterControlXML.saveXML(midiParameterControlsElement);
		}
	}
}
