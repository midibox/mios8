package org.midibox.mios.xml;

import org.midibox.midi.xml.VirtualMidiDeviceXML;
import org.midibox.mios.DebugFunctionDevice;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class DebugFunctionDeviceXML extends VirtualMidiDeviceXML {

	public final static String TAG_ROOT_ELEMENT = "debugFunctionDevice";

	protected DebugFunctionDevice debugFunctionDevice;

	public DebugFunctionDeviceXML(DebugFunctionDevice debugFunctionDevice,
			String rootElementTag) {

		super(debugFunctionDevice, rootElementTag);

		this.debugFunctionDevice = debugFunctionDevice;

		tags.add(DebugFunctionXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == DebugFunctionXML.TAG_ROOT_ELEMENT) {

			DebugFunctionXML debugFunctionXML = new DebugFunctionXML(
					debugFunctionDevice.getDebugFunction(),
					DebugFunctionXML.TAG_ROOT_ELEMENT);
			
			debugFunctionXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		DebugFunctionXML debugFunctionXML = new DebugFunctionXML(
				debugFunctionDevice.getDebugFunction(),
				DebugFunctionXML.TAG_ROOT_ELEMENT);

		debugFunctionXML.saveXML(rootElement);
	}
}
