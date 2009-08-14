package org.midibox.midi.xml;

import org.midibox.midi.SysexSendReceiveDevice;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class SysexSendReceiveDeviceXML extends VirtualMidiDeviceXML {

	protected SysexSendReceiveDevice sysexSendReceiveDevice;

	public static final String TAG_ROOT_ELEMENT = "sysexSendReceiveDevice";

	public SysexSendReceiveDeviceXML(SysexSendReceiveDevice sysexSendReceiveDevice,
			String rootElementTag) {

		super(sysexSendReceiveDevice, rootElementTag);

		this.sysexSendReceiveDevice = sysexSendReceiveDevice;

		addTag(SysexSendReceiveXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			SysexSendReceiveXML hexFileUploadXML = new SysexSendReceiveXML(
					sysexSendReceiveDevice.getSysexSendReceive(),
					SysexSendReceiveXML.TAG_ROOT_ELEMENT);

			hexFileUploadXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		SysexSendReceiveXML hexFileUploadXML = new SysexSendReceiveXML(
				sysexSendReceiveDevice.getSysexSendReceive(),
				SysexSendReceiveXML.TAG_ROOT_ELEMENT);

		hexFileUploadXML.saveXML(rootElement);
	}
}
