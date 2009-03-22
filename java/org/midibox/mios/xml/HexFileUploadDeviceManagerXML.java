package org.midibox.mios.xml;

import java.util.Iterator;
import java.util.Vector;

import org.midibox.mios.HexFileUploadDevice;
import org.midibox.mios.HexFileUploadDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class HexFileUploadDeviceManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "hexFileUploadDeviceManager";

	public final static String TAG_HEX_FILE_UPLOAD_DEVICES = "hexFileUploadDevices";

	public final static String ATTR_MIOS_32_MODE = "mios32Mode";

	protected HexFileUploadDeviceManager hexFileUploadDeviceManager;

	public HexFileUploadDeviceManagerXML(
			HexFileUploadDeviceManager hexFileUploadDeviceManager,
			String rootElementTag) {

		super(rootElementTag);

		this.hexFileUploadDeviceManager = hexFileUploadDeviceManager;

		addTag(TAG_HEX_FILE_UPLOAD_DEVICES);
		addTag(HexFileUploadDeviceXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			hexFileUploadDeviceManager.setMIOS32Mode(stringToBoolean(element
					.getAttribute(ATTR_MIOS_32_MODE)));

		} else if (name == TAG_HEX_FILE_UPLOAD_DEVICES) {

			Iterator it = ((Vector) hexFileUploadDeviceManager
					.getHexFileUploadDevices().clone()).iterator();

			while (it.hasNext()) {

				HexFileUploadDevice hexFileUploadDevice = (HexFileUploadDevice) it
						.next();

				hexFileUploadDeviceManager
						.removeHexFileUploadDevice(hexFileUploadDevice);
			}

		} else if (name == HexFileUploadDeviceXML.TAG_ROOT_ELEMENT) {

			HexFileUploadDevice hexFileUploadDevice = new HexFileUploadDevice(
					element.getAttribute(HexFileUploadDeviceXML.ATTR_NAME));

			HexFileUploadDeviceXML hexFileUploadDeviceXML = new HexFileUploadDeviceXML(
					hexFileUploadDevice,
					HexFileUploadDeviceXML.TAG_ROOT_ELEMENT);

			hexFileUploadDeviceManager
					.addHexFileUploadDevice(hexFileUploadDevice);

			hexFileUploadDeviceXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element hexFileUploadDevicesElement = document
				.createElement(TAG_HEX_FILE_UPLOAD_DEVICES);

		rootElement.setAttribute(ATTR_MIOS_32_MODE,
				booleanToString(hexFileUploadDeviceManager.isMIOS32Mode()));

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
