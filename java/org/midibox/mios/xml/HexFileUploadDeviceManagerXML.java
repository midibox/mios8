package org.midibox.mios.xml;

import java.util.Iterator;

import org.midibox.mios.HexFileUploadDevice;
import org.midibox.mios.HexFileUploadDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class HexFileUploadDeviceManagerXML extends XMLUtils {

	protected HexFileUploadDeviceManager hexFileUploadDeviceManager;

	protected String hexFileUploadDevicesTag = "hexFileUploadDevices";

	protected String hexFileUploadDeviceTag = "hexFileUploadDevice";

	public HexFileUploadDeviceManagerXML(
			HexFileUploadDeviceManager hexFileUploadDeviceManager,
			String rootElementTag) {

		super(rootElementTag);

		this.hexFileUploadDeviceManager = hexFileUploadDeviceManager;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element hexFileUploadDevicesElement = document
				.createElement(hexFileUploadDevicesTag);

		rootElement.appendChild(hexFileUploadDevicesElement);

		Iterator it = hexFileUploadDeviceManager.getHexFileUploadDevices()
				.iterator();

		while (it.hasNext()) {

			HexFileUploadDevice hexFileUploadDevice = (HexFileUploadDevice) it
					.next();

			HexFileUploadDeviceXML hexFileUploadDeviceXML = new HexFileUploadDeviceXML(
					hexFileUploadDevice, hexFileUploadDeviceTag);

			hexFileUploadDeviceXML.saveXML(hexFileUploadDevicesElement);
		}
	}
}
