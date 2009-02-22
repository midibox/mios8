package org.midibox.midi.xml;

import java.util.Iterator;

import org.midibox.midi.MidiFilterDevice;
import org.midibox.midi.MidiFilterDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiFilterDeviceManagerXML extends XMLUtils {

	protected MidiFilterDeviceManager midiFilterDeviceManager;
	
	protected String midiFilterDevicesTag = "midiFilterDevices";

	protected String midiFilterDeviceTag = "midiFilterDevice";

	public MidiFilterDeviceManagerXML(MidiFilterDeviceManager midiFilterDeviceManager, String rootElementTag) {

		super(rootElementTag);
		
		this.midiFilterDeviceManager = midiFilterDeviceManager;

		tags.add(midiFilterDevicesTag);
		tags.add(midiFilterDeviceTag);
	}
	
	
	protected void parseElement(Element element) {
		
		super.parseElement(element);
		
		String name = element.getNodeName();
		
		if (name == rootElementTag) {
			 
		} else if (name == midiFilterDevicesTag) {
			
		} else if (name == midiFilterDeviceTag) {
			
			MidiFilterDevice midiFilterDevice = midiFilterDeviceManager.addMidiFilterDevice();
			
			MidiFilterDeviceXML midiFilterDeviceXML = new MidiFilterDeviceXML(midiFilterDevice, midiFilterDeviceTag);
		
			midiFilterDeviceXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element midiFilterDevicesElement = document
				.createElement(midiFilterDevicesTag);

		rootElement.appendChild(midiFilterDevicesElement);

		Iterator it = midiFilterDeviceManager.getMidiFilterDevices().iterator();

		while (it.hasNext()) {

			MidiFilterDevice midiFilterDevice = (MidiFilterDevice) it.next();
			
			MidiFilterDeviceXML midiFilterDeviceXML = new MidiFilterDeviceXML(midiFilterDevice, midiFilterDeviceTag);
			
			midiFilterDeviceXML.saveXML(midiFilterDevicesElement);
		}
	}
}
