package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import org.midibox.midi.SysexSendReceive;
import org.midibox.midi.SysexSendReceiveManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class SysexSendReceiveManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "sysexSendReceiveManager";

	public final static String TAG_SYSEX_SEND_RECEIVES = "sysexSendReceives";

	protected SysexSendReceiveManager sysexSendReceiveManager;

	public SysexSendReceiveManagerXML(
			SysexSendReceiveManager sysexSendReceiveManager,
			String rootElementTag) {

		super(rootElementTag);

		this.sysexSendReceiveManager = sysexSendReceiveManager;

		addTag(TAG_SYSEX_SEND_RECEIVES);
		addTag(SysexSendReceiveXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == TAG_SYSEX_SEND_RECEIVES) {

			Iterator it = ((Vector) sysexSendReceiveManager
					.getSysexSendReceives().clone()).iterator();

			while (it.hasNext()) {

				SysexSendReceive sysexSendReceive = (SysexSendReceive) it
						.next();

				sysexSendReceiveManager
						.removeSysexSendReceive(sysexSendReceive);
			}

		} else if (name == SysexSendReceiveXML.TAG_ROOT_ELEMENT) {

			SysexSendReceive sysexSendReceive = new SysexSendReceive(null);

			SysexSendReceiveXML sysexSendReceiveXML = new SysexSendReceiveXML(
					sysexSendReceive, SysexSendReceiveXML.TAG_ROOT_ELEMENT);

			sysexSendReceiveManager.addSysexSendReceive(sysexSendReceive);

			sysexSendReceiveXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element hexFileUploadDevicesElement = document
				.createElement(TAG_SYSEX_SEND_RECEIVES);

		rootElement.appendChild(hexFileUploadDevicesElement);

		Iterator it = sysexSendReceiveManager.getSysexSendReceives().iterator();

		while (it.hasNext()) {

			SysexSendReceive sysexSendReceive = (SysexSendReceive) it.next();

			SysexSendReceiveXML sysexSendReceiveXML = new SysexSendReceiveXML(
					sysexSendReceive, SysexSendReceiveXML.TAG_ROOT_ELEMENT);

			sysexSendReceiveXML.saveXML(hexFileUploadDevicesElement);
		}
	}
}
