package org.midibox.mios.xml;

import org.midibox.midi.xml.VirtualMidiDeviceXML;
import org.midibox.mios.LCDMessageDevice;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class LCDMessageDeviceXML extends VirtualMidiDeviceXML {

	protected LCDMessageDevice lcdMessageDevice;

	public static final String TAG_ROOT_ELEMENT = "lcdMessageDevice";

	public LCDMessageDeviceXML(LCDMessageDevice lcdMessageDevice,
			String rootElementTag) {

		super(lcdMessageDevice, rootElementTag);

		this.lcdMessageDevice = lcdMessageDevice;

		addTag(LCDMessageXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == LCDMessageXML.TAG_ROOT_ELEMENT) {

			LCDMessageXML lcdMessageXML = new LCDMessageXML(lcdMessageDevice
					.getLCDMessage(), LCDMessageXML.TAG_ROOT_ELEMENT);

			lcdMessageXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		LCDMessageXML lcdMessageXML = new LCDMessageXML(lcdMessageDevice
				.getLCDMessage(), LCDMessageXML.TAG_ROOT_ELEMENT);

		lcdMessageXML.saveXML(rootElement);
	}
}
