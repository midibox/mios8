package org.midibox.mios.xml;

import org.midibox.mios.DebugFunctionParameters;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class DebugFunctionParametersXML extends XMLUtils {

	protected final static String TAG_ROOT_ELEMENT = "debugFunctionParameters";

	protected final static String ATTR_ADDRESS = "address";

	protected final static String ATTR_WREG = "wreg";

	protected final static String ATTR_PARAM_1 = "param1";

	protected final static String ATTR_PARAM_2 = "param2";

	protected final static String ATTR_PARAM_3 = "param3";

	protected DebugFunctionParameters debugFunctionParameters;

	public DebugFunctionParametersXML(
			DebugFunctionParameters debugFunctionParameters,
			String rootElementTag) {

		super(rootElementTag);

		this.debugFunctionParameters = debugFunctionParameters;
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			debugFunctionParameters.setAddress(stringToInt(element
					.getAttribute(ATTR_ADDRESS)));

			debugFunctionParameters.setWreg(stringToInt(element
					.getAttribute(ATTR_WREG)));

			debugFunctionParameters.setParam1(stringToInt(element
					.getAttribute(ATTR_PARAM_1)));

			debugFunctionParameters.setParam2(stringToInt(element
					.getAttribute(ATTR_PARAM_2)));

			debugFunctionParameters.setParam3(stringToInt(element
					.getAttribute(ATTR_PARAM_3)));
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_ADDRESS,
				intToString(debugFunctionParameters.getAddress()));

		rootElement.setAttribute(ATTR_WREG, intToString(debugFunctionParameters
				.getWreg()));

		rootElement.setAttribute(ATTR_PARAM_1,
				intToString(debugFunctionParameters.getParam1()));

		rootElement.setAttribute(ATTR_PARAM_2,
				intToString(debugFunctionParameters.getParam2()));

		rootElement.setAttribute(ATTR_PARAM_3,
				intToString(debugFunctionParameters.getParam3()));
	}
}
