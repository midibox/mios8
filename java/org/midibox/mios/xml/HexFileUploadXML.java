package org.midibox.mios.xml;

import org.midibox.mios.HexFileUpload;
import org.w3c.dom.Node;

public class HexFileUploadXML extends MIOSSysexSendReceiveXML {

	protected HexFileUpload hexFileUpload;

	public final static String TAG_ROOT_ELEMENT = "hexFileUpload";

	public final static String ATTR_WAIT_FOR_UPLOAD_REQUEST = "waitForUploadRequest";

	public final static String ATTR_UPLOAD_MODE = "uploadMode";

	public final static String ATTR_DELAY_TIME = "delayTime";

	public HexFileUploadXML(HexFileUpload hexFileUpload, String rootElementTag) {

		super(hexFileUpload, rootElementTag);

		this.hexFileUpload = hexFileUpload;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_WAIT_FOR_UPLOAD_REQUEST,
				booleanToString(hexFileUpload.isWaitForUploadRequest()));

		rootElement.setAttribute(ATTR_UPLOAD_MODE, intToString(hexFileUpload
				.getUploadMode()));

		rootElement.setAttribute(ATTR_DELAY_TIME, intToString(hexFileUpload
				.getDelayTime()));
	}
}
