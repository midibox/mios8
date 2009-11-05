package org.midibox.midi.xml;

import java.io.File;

import org.midibox.midi.SysexSendReceive;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class SysexSendReceiveXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "sysexSendReceive";

	public final static String TAG_SEND_FILE = "sendSysexFile";

	public final static String ATTR_SEND_DELAY_TIME = "sendDelayTime";

	protected SysexSendReceive sysexSendReceive;

	public SysexSendReceiveXML(SysexSendReceive sysexSendReceive,
			String rootElementTag) {

		super(rootElementTag);

		this.sysexSendReceive = sysexSendReceive;

		addTag(TAG_SEND_FILE);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			if (element.getAttribute(ATTR_SEND_DELAY_TIME) != "") {

				sysexSendReceive.setSendDelayTime(stringToInt(element
						.getAttribute(ATTR_SEND_DELAY_TIME)));
			}
		}

		if (name == TAG_SEND_FILE) {

			File file = new File(element.getTextContent());

			if (file.exists()) {

				sysexSendReceive.setSendFile(file);
			}
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_SEND_DELAY_TIME,
				intToString(sysexSendReceive.getSendDelayTime()));

		File file = sysexSendReceive.getSendFile();

		if (file != null) {

			Element fileElement = document.createElement(TAG_SEND_FILE);

			rootElement.appendChild(fileElement);

			fileElement.setTextContent(file.getAbsolutePath());
		}
	}
}
