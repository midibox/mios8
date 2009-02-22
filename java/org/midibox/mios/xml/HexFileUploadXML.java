package org.midibox.mios.xml;

import org.midibox.mios.HexFileUpload;
import org.w3c.dom.Node;

public class HexFileUploadXML extends MIOSSysexSendReceiveXML {

	protected HexFileUpload hexFileUpload;

	protected String waitForUploadRequestAttr = "waitForUploadRequest";

	protected String uploadModeAttr = "uploadMode";

	protected String delayTimeAttr = "delayTime";

	public HexFileUploadXML(HexFileUpload hexFileUpload, String rootElementTag) {

		super(hexFileUpload, rootElementTag);

		this.hexFileUpload = hexFileUpload;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(waitForUploadRequestAttr, hexFileUpload
				.isWaitForUploadRequest() ? "true" : "false");

		rootElement.setAttribute(uploadModeAttr, ""
				+ hexFileUpload.getUploadMode());

		rootElement.setAttribute(delayTimeAttr, ""
				+ hexFileUpload.getDelayTime());
	}
}
