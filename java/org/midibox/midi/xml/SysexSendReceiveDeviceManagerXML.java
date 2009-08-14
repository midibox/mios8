package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import org.midibox.midi.SysexSendReceiveDevice;
import org.midibox.midi.SysexSendReceiveDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class SysexSendReceiveDeviceManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "hexFileUploadDeviceManager";

	public final static String TAG_HEX_FILE_UPLOAD_DEVICES = "hexFileUploadDevices";

	protected SysexSendReceiveDeviceManager sysexSendReceiveDeviceManager;

	public SysexSendReceiveDeviceManagerXML(
			SysexSendReceiveDeviceManager sysexSendReceiveDeviceManager,
			String rootElementTag) {

		super(rootElementTag);

		this.sysexSendReceiveDeviceManager = sysexSendReceiveDeviceManager;

		addTag(TAG_HEX_FILE_UPLOAD_DEVICES);
		addTag(SysexSendReceiveDeviceXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == TAG_HEX_FILE_UPLOAD_DEVICES) {

			Iterator it = ((Vector) sysexSendReceiveDeviceManager
					.getSysexSendReceiveDevices().clone()).iterator();

			while (it.hasNext()) {

				SysexSendReceiveDevice sysexSendReceiveDevice = (SysexSendReceiveDevice) it
						.next();

				sysexSendReceiveDeviceManager
						.removeSysexSendReceiveDevice(sysexSendReceiveDevice);
			}

		} else if (name == SysexSendReceiveDeviceXML.TAG_ROOT_ELEMENT) {

			SysexSendReceiveDevice sysexSendReceiveDevice = new SysexSendReceiveDevice(
					element.getAttribute(SysexSendReceiveDeviceXML.ATTR_NAME));

			SysexSendReceiveDeviceXML sysexSendReceiveDeviceXML = new SysexSendReceiveDeviceXML(
					sysexSendReceiveDevice,
					SysexSendReceiveDeviceXML.TAG_ROOT_ELEMENT);

			sysexSendReceiveDeviceManager
					.newSysexSendReceive();

			sysexSendReceiveDeviceXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element hexFileUploadDevicesElement = document
				.createElement(TAG_HEX_FILE_UPLOAD_DEVICES);

		rootElement.appendChild(hexFileUploadDevicesElement);

		Iterator it = sysexSendReceiveDeviceManager.getSysexSendReceiveDevices()
				.iterator();

		while (it.hasNext()) {

			SysexSendReceiveDevice sysexSendReceiveDevice = (SysexSendReceiveDevice) it
					.next();

			SysexSendReceiveDeviceXML hexFileUploadDeviceXML = new SysexSendReceiveDeviceXML(
					sysexSendReceiveDevice,
					SysexSendReceiveDeviceXML.TAG_ROOT_ELEMENT);

			hexFileUploadDeviceXML.saveXML(hexFileUploadDevicesElement);
		}
	}
}
