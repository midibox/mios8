package org.midibox.mios.xml;

import org.midibox.midi.xml.VirtualMidiDeviceXML;
import org.midibox.mios.LCDMessageDevice;
import org.w3c.dom.Node;

public class LCDMessageDeviceXML extends VirtualMidiDeviceXML {

	protected LCDMessageDevice lcdMessageDevice;

	protected String lcdMessageTag = "lcdMessage";

	public LCDMessageDeviceXML(LCDMessageDevice lcdMessageDevice,
			String rootElementTag) {

		super(lcdMessageDevice, rootElementTag);

		this.lcdMessageDevice = lcdMessageDevice;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		LCDMessageXML lcdMessageXML = new LCDMessageXML(lcdMessageDevice
				.getLCDMessage(), lcdMessageTag);

		lcdMessageXML.saveXML(rootElement);
	}
}
