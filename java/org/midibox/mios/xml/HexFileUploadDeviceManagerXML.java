package org.midibox.mios.xml;

import java.util.Iterator;

import org.midibox.mios.HexFileUploadDevice;
import org.midibox.mios.HexFileUploadDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class HexFileUploadDeviceManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "hexFileUploadDeviceManager";

	public final static String TAG_HEX_FILE_UPLOAD_DEVICES = "hexFileUploadDevices";

	protected HexFileUploadDeviceManager hexFileUploadDeviceManager;

	public HexFileUploadDeviceManagerXML(
			HexFileUploadDeviceManager hexFileUploadDeviceManager,
			String rootElementTag) {

		super(rootElementTag);

		this.hexFileUploadDeviceManager = hexFileUploadDeviceManager;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element hexFileUploadDevicesElement = document
				.createElement(TAG_HEX_FILE_UPLOAD_DEVICES);

		rootElement.appendChild(hexFileUploadDevicesElement);

		Iterator it = hexFileUploadDeviceManager.getHexFileUploadDevices()
				.iterator();

		while (it.hasNext()) {

			HexFileUploadDevice hexFileUploadDevice = (HexFileUploadDevice) it
					.next();

			HexFileUploadDeviceXML hexFileUploadDeviceXML = new HexFileUploadDeviceXML(
					hexFileUploadDevice,
					HexFileUploadDeviceXML.TAG_ROOT_ELEMENT);

			hexFileUploadDeviceXML.saveXML(hexFileUploadDevicesElement);
		}
	}
}
