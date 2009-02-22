package org.midibox.midi.xml;

import org.midibox.midi.MidiFilterDevice;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiFilterDeviceXML extends VirtualMidiDeviceXML {
	
	MidiFilterDevice midiFilterDevice;

	protected String midiFilterDeviceAttrName = "name";

	protected String midiFilterDeviceAttrHashCode = "hashCode";

	protected String midiFilterTag = "midiFilter";

	public MidiFilterDeviceXML(MidiFilterDevice midiFilterDevice, String rootElementTag) {

		super(midiFilterDevice, rootElementTag);
		
		this.midiFilterDevice = midiFilterDevice;
		
		tags.add(midiFilterTag);
	}
	
	protected void parseElement(Element element) {
		
		super.parseElement(element);
		
		String name = element.getNodeName();
		
		if (name == midiFilterTag) {
		
			MidiFilterXML midiFilterXML = new MidiFilterXML(midiFilterDevice.getMidiFilter(), midiFilterTag);
			
			midiFilterXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		MidiFilterXML midiFilterXML = new MidiFilterXML(midiFilterDevice.getMidiFilter(), midiFilterTag);

		midiFilterXML.saveXML(rootElement);
	}
}
