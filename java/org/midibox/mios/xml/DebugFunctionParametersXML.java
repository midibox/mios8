package org.midibox.mios.xml;

import org.midibox.mios.DebugFunctionParameters;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Node;

public class DebugFunctionParametersXML extends XMLUtils {

	protected DebugFunctionParameters debugFunctionParameters;

	public DebugFunctionParametersXML(
			DebugFunctionParameters debugFunctionParameters,
			String rootElementTag) {

		super(rootElementTag);

		this.debugFunctionParameters = debugFunctionParameters;
	}

	String addressAttr = "address";

	String wregAttr = "wreg";

	String param1Attr = "param1";

	String param2Attr = "param2";

	String param3Attr = "param3";

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(addressAttr, ""
				+ debugFunctionParameters.getAddress());

		rootElement.setAttribute(wregAttr, ""
				+ debugFunctionParameters.getWreg());

		rootElement.setAttribute(param1Attr, ""
				+ debugFunctionParameters.getParam1());

		rootElement.setAttribute(param2Attr, ""
				+ debugFunctionParameters.getParam2());

		rootElement.setAttribute(param3Attr, ""
				+ debugFunctionParameters.getParam3());
	}
}
