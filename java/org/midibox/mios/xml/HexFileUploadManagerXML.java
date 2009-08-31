package org.midibox.mios.xml;

import java.util.Iterator;
import java.util.Vector;

import org.midibox.mios.HexFileUpload;
import org.midibox.mios.HexFileUploadManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class HexFileUploadManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "hexFileUploadManager";

	public final static String TAG_HEX_FILE_UPLOADS = "hexFileUploads";

	public final static String ATTR_MIOS_32_MODE = "mios32Mode";

	protected HexFileUploadManager hexFileUploadManager;

	public HexFileUploadManagerXML(
			HexFileUploadManager hexFileUploadDeviceManager,
			String rootElementTag) {

		super(rootElementTag);

		this.hexFileUploadManager = hexFileUploadDeviceManager;

		addTag(TAG_HEX_FILE_UPLOADS);
		addTag(HexFileUploadXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			hexFileUploadManager.setMIOS32Mode(stringToBoolean(element
					.getAttribute(ATTR_MIOS_32_MODE)));

		} else if (name == TAG_HEX_FILE_UPLOADS) {

			Iterator it = ((Vector) hexFileUploadManager.getHexFileUploads()
					.clone()).iterator();

			while (it.hasNext()) {

				HexFileUpload hexFileUpload = (HexFileUpload) it.next();

				hexFileUploadManager.removeHexFileUpload(hexFileUpload);
			}

		} else if (name == HexFileUploadXML.TAG_ROOT_ELEMENT) {

			HexFileUpload hexFileUpload = new HexFileUpload(null);

			HexFileUploadXML hexFileUploadXML = new HexFileUploadXML(
					hexFileUpload, HexFileUploadXML.TAG_ROOT_ELEMENT);

			hexFileUploadManager.addHexFileUpload(hexFileUpload);

			hexFileUploadXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element hexFileUploadDevicesElement = document
				.createElement(TAG_HEX_FILE_UPLOADS);

		rootElement.setAttribute(ATTR_MIOS_32_MODE,
				booleanToString(hexFileUploadManager.isMIOS32Mode()));

		rootElement.appendChild(hexFileUploadDevicesElement);

		Iterator it = hexFileUploadManager.getHexFileUploads().iterator();

		while (it.hasNext()) {

			HexFileUpload hexFileUpload = (HexFileUpload) it.next();

			HexFileUploadXML hexFileUploadXML = new HexFileUploadXML(
					hexFileUpload, HexFileUploadXML.TAG_ROOT_ELEMENT);

			hexFileUploadXML.saveXML(hexFileUploadDevicesElement);
		}
	}
}
