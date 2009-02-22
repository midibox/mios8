package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import javax.sound.midi.MidiDevice;

import org.midibox.midi.MidiDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiDeviceManagerXML extends XMLUtils {
	
	protected MidiDeviceManager midiDeviceManager;

	protected String midiDeviceManagerTag = "midiDeviceManager";

	protected String selectedMidiReadDevicesTag = "selectedMidiReadDevices";

	protected String selectedMidiWriteDevicesTag = "selectedMidiWriteDevices";

	protected String selectedMidiReadDeviceTag = "selectedMidiReadDevice";

	protected String selectedMidiWriteDeviceTag = "selectedMidiWriteDevice";
	
	protected String hashAttr = "hashCode";
	
	public MidiDeviceManagerXML(MidiDeviceManager midiDeviceManager, String rootElementTag)  {

		super(rootElementTag);
		
		this.midiDeviceManager = midiDeviceManager;
		
		tags.add(selectedMidiReadDevicesTag);
		tags.add(selectedMidiWriteDevicesTag);
		tags.add(selectedMidiReadDeviceTag);
		tags.add(selectedMidiWriteDeviceTag);
	}
	
	
	protected void parseElement(Element element) {
		
		super.parseElement(element);
				
		String name = element.getNodeName();
		
		if (name == rootElementTag) {
			
			midiDeviceManager.rescanDevices();
					
		} else if (name == selectedMidiReadDevicesTag){			

			midiDeviceManager.getSelectedMidiReadDevices().removeAllElements();	
						
		} else if (name == selectedMidiReadDeviceTag){
			
			int hash = Integer.parseInt(element.getAttribute(hashAttr));
			
			MidiDevice midiDevice  = findDeviceHash(midiDeviceManager.getMidiReadDevices(), hash);
			
			if (midiDevice != null) {
				
				midiDeviceManager.selectMidiReadDevice(midiDevice);
			}
						
		} else if (name == selectedMidiWriteDevicesTag){			

			midiDeviceManager.getSelectedMidiWriteDevices().removeAllElements();	
						
		} else if (name == selectedMidiWriteDeviceTag){
			
			int hash = Integer.parseInt(element.getAttribute(hashAttr));
			
			MidiDevice midiDevice  = findDeviceHash(midiDeviceManager.getMidiWriteDevices(), hash);
			
			if (midiDevice != null) {
				
				midiDeviceManager.selectMidiWriteDevice(midiDevice);
			}
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element selectedMidiReadDevicesElement = document
				.createElement(selectedMidiReadDevicesTag);

		rootElement.appendChild(selectedMidiReadDevicesElement);

		Iterator it = midiDeviceManager.getSelectedMidiReadDevices().iterator();

		while (it.hasNext()) {

			MidiDevice midiDevice = (MidiDevice) it.next();

			MidiDeviceXML midiDeviceXML = new MidiDeviceXML(midiDevice, selectedMidiReadDeviceTag);

			midiDeviceXML.saveXML(selectedMidiReadDevicesElement);
		}
		
		Element selectedMidiWriteDevicesElement = document
				.createElement(selectedMidiWriteDevicesTag);

		rootElement.appendChild(selectedMidiWriteDevicesElement);

		it = midiDeviceManager.getSelectedMidiWriteDevices().iterator();

		while (it.hasNext()) {

			MidiDevice midiDevice = (MidiDevice) it.next();

			MidiDeviceXML midiDeviceXML = new MidiDeviceXML(midiDevice, selectedMidiWriteDeviceTag);

			midiDeviceXML.saveXML(selectedMidiWriteDevicesElement);
		}
	}

	
	protected MidiDevice findDeviceHash(Vector midiDevices, int hash) {

		Iterator it = midiDevices.iterator();
		
		while (it.hasNext()) {
			
			MidiDevice midiDevice = (MidiDevice) it.next();
			
			if (midiDevice.getDeviceInfo().toString().hashCode() == hash) {
								
				return midiDevice;
			}						
		}
		
		return null;
	}
}
