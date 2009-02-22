package org.midibox.mios.xml;

import org.midibox.midi.xml.VirtualMidiDeviceXML;
import org.midibox.mios.HexFileUploadDevice;
import org.w3c.dom.Node;

public class HexFileUploadDeviceXML extends VirtualMidiDeviceXML {

	protected HexFileUploadDevice hexFileUploadDevice;

	protected String hexFileUploadTag = "hexFileUpload";

	public HexFileUploadDeviceXML(HexFileUploadDevice hexFileUploadDevice,
			String rootElementTag) {

		super(hexFileUploadDevice, rootElementTag);

		this.hexFileUploadDevice = hexFileUploadDevice;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		HexFileUploadXML hexFileUploadXML = new HexFileUploadXML(
				hexFileUploadDevice.getHexFileUpload(), hexFileUploadTag);

		hexFileUploadXML.saveXML(rootElement);
	}
}
