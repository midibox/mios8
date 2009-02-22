package org.midibox.midi.xml;

import org.midibox.midi.VirtualMidiDevice;

public class VirtualMidiDeviceXML extends MidiDeviceXML {
	
	public VirtualMidiDeviceXML(VirtualMidiDevice virtualMidiDevice, String rootElementTag) {
			
		super(virtualMidiDevice, rootElementTag);
	}
}
