package org.midibox.mios.xml;

import org.midibox.midi.xml.VirtualMidiDeviceXML;
import org.midibox.mios.DebugFunctionDevice;
import org.w3c.dom.Node;

public class DebugFunctionDeviceXML extends VirtualMidiDeviceXML {

	protected DebugFunctionDevice debugFunctionDevice;

	protected String debugFunctionTag = "debugFunction";

	public DebugFunctionDeviceXML(DebugFunctionDevice debugFunctionDevice,
			String rootElementTag) {

		super(debugFunctionDevice, rootElementTag);

		this.debugFunctionDevice = debugFunctionDevice;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		DebugFunctionXML debugFunctionXML = new DebugFunctionXML(
				debugFunctionDevice.getDebugFunction(), debugFunctionTag);

		debugFunctionXML.saveXML(rootElement);
	}
}
