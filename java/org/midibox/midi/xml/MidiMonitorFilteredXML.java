package org.midibox.midi.xml;

import org.midibox.midi.MidiMonitorFiltered;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiMonitorFilteredXML extends XMLUtils {

	protected MidiMonitorFiltered midiMonitorFiltered;

	protected String midiMonitorFilteredTag = "midiMonitorFiltered";

	protected String midiFilterTag = "midiFilter";

	public MidiMonitorFilteredXML(MidiMonitorFiltered midiMonitorFiltered,
			String rootElementTag) {

		super(rootElementTag);

		this.midiMonitorFiltered = midiMonitorFiltered;

		tags.add(midiFilterTag);
	}


	protected void parseElement(Element element) {

		super.parseElement(element);
		
		String name = element.getNodeName();
		
		if (name == midiFilterTag) {
			
			MidiFilterXML midiFilterXML = new MidiFilterXML(midiMonitorFiltered.getMidiFilter(), midiFilterTag);
			
			midiFilterXML.loadXML(element);
		}
	}
	
	public void saveXML(Node node) {

		super.saveXML(node);

		MidiFilterXML midiFilterXML = new MidiFilterXML(midiMonitorFiltered
				.getMidiFilter(), midiFilterTag);

		midiFilterXML.saveXML(rootElement);
	}
}
