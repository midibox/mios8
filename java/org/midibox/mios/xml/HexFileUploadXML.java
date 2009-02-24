package org.midibox.mios.xml;

import java.io.File;

import org.midibox.mios.HexFileUpload;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class HexFileUploadXML extends MIOSSysexSendReceiveXML {

	protected HexFileUpload hexFileUpload;

	public final static String TAG_ROOT_ELEMENT = "hexFileUpload";
	
	public final static String ATTR_FILE = "file";

	public final static String ATTR_WAIT_FOR_UPLOAD_REQUEST = "waitForUploadRequest";

	public final static String ATTR_UPLOAD_MODE = "uploadMode";

	public final static String ATTR_DELAY_TIME = "delayTime";

	public HexFileUploadXML(HexFileUpload hexFileUpload, String rootElementTag) {

		super(hexFileUpload, rootElementTag);

		this.hexFileUpload = hexFileUpload;
	}
	
	protected void parseElement(Element element) {

		super.parseElement(element);
		
		String name = element.getNodeName();
		
		if (name == rootElementTag) {
			
			File file = new File(element.getAttribute(ATTR_FILE));
			
			if (file.exists()) {
			
				hexFileUpload.setFile(file);
			}
			
			hexFileUpload.setMIOS32Mode(stringToBoolean(element.getAttribute(ATTR_MIOS32_MODE)));
			
			hexFileUpload.setDeviceID(stringToInt(element.getAttribute(ATTR_DEVICE_ID)));
			
			hexFileUpload.setDelayTime(stringToInt(element.getAttribute(ATTR_DELAY_TIME)));
			
			hexFileUpload.setUploadMode(stringToInt(element.getAttribute(ATTR_UPLOAD_MODE)));
			
			hexFileUpload.setWaitForUploadRequest(stringToBoolean(element.getAttribute(ATTR_WAIT_FOR_UPLOAD_REQUEST)));
		}
	}



	public void saveXML(Node node) {

		super.saveXML(node);

		File file = hexFileUpload.getFile();
		
		String fileAttr = (file != null) ? file.getAbsoluteFile().toString() : "";
		
		rootElement.setAttribute(ATTR_FILE, fileAttr);		

		rootElement.setAttribute(ATTR_WAIT_FOR_UPLOAD_REQUEST,
				booleanToString(hexFileUpload.isWaitForUploadRequest()));

		rootElement.setAttribute(ATTR_UPLOAD_MODE, intToString(hexFileUpload
				.getUploadMode()));

		rootElement.setAttribute(ATTR_DELAY_TIME, intToString(hexFileUpload
				.getDelayTime()));
	}
}
