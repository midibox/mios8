package org.midibox.mios.xml;

import org.midibox.mios.MIOSSysexSendReceive;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Node;

public class MIOSSysexSendReceiveXML extends XMLUtils {

	protected MIOSSysexSendReceive miosSysexSendReceive;

	public MIOSSysexSendReceiveXML(MIOSSysexSendReceive miosSysexSendReceive,
			String rootElementTag) {

		super(rootElementTag);

		this.miosSysexSendReceive = miosSysexSendReceive;
	}

	protected String deviceIDTag = "deviceID";

	protected String mios32ModeTag = "mios32Mode";

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(deviceIDTag, ""
				+ miosSysexSendReceive.getDeviceID());

		rootElement.setAttribute(mios32ModeTag, miosSysexSendReceive
				.isMIOS32Mode() ? "true" : "false");
	}
}
