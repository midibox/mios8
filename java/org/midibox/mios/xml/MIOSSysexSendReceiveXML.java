package org.midibox.mios.xml;

import org.midibox.mios.MIOSSysexSendReceive;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Node;

public class MIOSSysexSendReceiveXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "miosSysexSendReceive";

	public final static String ATTR_DEVICE_ID = "deviceID";

	public final static String ATTR_MIOS32_MODE = "mios32Mode";

	protected MIOSSysexSendReceive miosSysexSendReceive;

	public MIOSSysexSendReceiveXML(MIOSSysexSendReceive miosSysexSendReceive,
			String rootElementTag) {

		super(rootElementTag);

		this.miosSysexSendReceive = miosSysexSendReceive;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_DEVICE_ID,
				intToString(miosSysexSendReceive.getDeviceID()));

		rootElement.setAttribute(ATTR_MIOS32_MODE,
				booleanToString(miosSysexSendReceive.isMIOS32Mode()));
	}
}
