package org.midibox.mios.xml;

import java.io.File;

import org.midibox.mios.HexFileUpload;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class HexFileUploadXML extends MIOSSysexSendReceiveXML {

	protected HexFileUpload hexFileUpload;

	public final static String TAG_ROOT_ELEMENT = "hexFileUpload";

	public final static String TAG_FILE = "hexFile";

	public final static String ATTR_WAIT_FOR_UPLOAD_REQUEST = "waitForUploadRequest";

	public final static String ATTR_UPLOAD_MODE = "uploadMode";

	public final static String ATTR_DELAY_TIME = "delayTime";

	public HexFileUploadXML(HexFileUpload hexFileUpload, String rootElementTag) {

		super(hexFileUpload, rootElementTag);

		this.hexFileUpload = hexFileUpload;

		addTag(TAG_FILE);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			hexFileUpload.setMIOS32Mode(stringToBoolean(element
					.getAttribute(ATTR_MIOS32_MODE)));

			hexFileUpload.setDeviceID(stringToInt(element
					.getAttribute(ATTR_DEVICE_ID)));

			hexFileUpload.setDelayTime(stringToInt(element
					.getAttribute(ATTR_DELAY_TIME)));

			hexFileUpload.setUploadMode(stringToInt(element
					.getAttribute(ATTR_UPLOAD_MODE)));

			hexFileUpload.setWaitForUploadRequest(stringToBoolean(element
					.getAttribute(ATTR_WAIT_FOR_UPLOAD_REQUEST)));

		}
		if (name == TAG_FILE) {

			File file = new File(element.getTextContent());

			if (file.exists()) {

				hexFileUpload.setFile(file);
			}
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_WAIT_FOR_UPLOAD_REQUEST,
				booleanToString(hexFileUpload.isWaitForUploadRequest()));

		rootElement.setAttribute(ATTR_UPLOAD_MODE, intToString(hexFileUpload
				.getUploadMode()));

		rootElement.setAttribute(ATTR_DELAY_TIME, intToString(hexFileUpload
				.getDelayTime()));

		File file = hexFileUpload.getFile();

		if (file != null) {

			Element fileElement = document.createElement(TAG_FILE);

			rootElement.appendChild(fileElement);

			fileElement.setTextContent(file.getAbsolutePath());
		}
	}
}
