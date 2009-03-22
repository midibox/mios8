package org.midibox.mios.xml;

import java.util.Iterator;

import org.midibox.mios.DebugFunction;
import org.midibox.mios.DebugFunctionParameters;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class DebugFunctionXML extends MIOSSysexSendReceiveXML {

	protected DebugFunction debugFunction;

	public final static String TAG_ROOT_ELEMENT = "debugFunction";

	public static final String TAG_DEBUG_FUNCTION_PARAMETERS_LIST = "debugFunctionParametersList";

	public static final String ATTR_MODE = "mode";

	public static final String ATTR_DELAY_TIME = "delayTime";

	public static final String ATTR_SRAM_READ_ADDRESS = "sramReadAddress";

	public static final String ATTR_SRAM_READ_COUNTER = "sramReadCounter";

	public static final String ATTR_SRAM_WRITE_ADDRESS = "sramWriteAddress";

	public static final String ATTR_SRAM_WRITE_DATA = "sramWriteData";

	public DebugFunctionXML(DebugFunction debugFunction, String rootElementTag) {

		super(debugFunction, rootElementTag);

		this.debugFunction = debugFunction;

		addTag(TAG_DEBUG_FUNCTION_PARAMETERS_LIST);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			debugFunction.setDeviceID(stringToInt(element
					.getAttribute(ATTR_DEVICE_ID)));

			debugFunction.setMIOS32Mode(stringToBoolean(element
					.getAttribute(ATTR_MIOS32_MODE)));

			debugFunction.setMode(stringToInt(element.getAttribute(ATTR_MODE)));

			debugFunction.setDelayTime(stringToInt(element
					.getAttribute(ATTR_DELAY_TIME)));

			debugFunction.setSramReadAddress(stringToInt(element
					.getAttribute(ATTR_SRAM_READ_ADDRESS)));

			debugFunction.setSramReadCounter(stringToInt(element
					.getAttribute(ATTR_SRAM_READ_COUNTER)));

			debugFunction.setSramWriteAddress(stringToInt(element
					.getAttribute(ATTR_SRAM_WRITE_ADDRESS)));

			debugFunction.setSramWriteData(stringToInt(element
					.getAttribute(ATTR_SRAM_WRITE_DATA)));

		} else if (name == TAG_DEBUG_FUNCTION_PARAMETERS_LIST) {

			debugFunction.getDebugFunctionParameters().removeAllElements();

			NodeList children = element.getChildNodes();

			DebugFunctionParameters[] params = new DebugFunctionParameters[children
					.getLength()];

			for (int p = 0; p < children.getLength(); p++) {

				DebugFunctionParameters debugFunctionParameters = new DebugFunctionParameters();

				DebugFunctionParametersXML debugFunctionParametersXML = new DebugFunctionParametersXML(
						debugFunctionParameters,
						DebugFunctionParametersXML.TAG_ROOT_ELEMENT);

				debugFunctionParametersXML.loadXML(children.item(p));

				params[p] = debugFunctionParameters;
			}

			debugFunction.insertDebugFunctionParameters(params, 0);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_MODE,
				intToString(debugFunction.getMode()));

		rootElement.setAttribute(ATTR_DELAY_TIME, intToString(debugFunction
				.getDelayTime()));

		rootElement.setAttribute(ATTR_SRAM_READ_ADDRESS,
				intToString(debugFunction.getSramReadAddress()));

		rootElement.setAttribute(ATTR_SRAM_READ_COUNTER,
				intToString(debugFunction.getSramReadCounter()));

		rootElement.setAttribute(ATTR_SRAM_WRITE_ADDRESS,
				intToString(debugFunction.getSramWriteAddress()));

		rootElement.setAttribute(ATTR_SRAM_WRITE_DATA,
				intToString(debugFunction.getSramWriteData()));

		Element debugFunctionParametersListElement = document
				.createElement(TAG_DEBUG_FUNCTION_PARAMETERS_LIST);

		rootElement.appendChild(debugFunctionParametersListElement);

		Iterator it = debugFunction.getDebugFunctionParameters().iterator();

		while (it.hasNext()) {

			DebugFunctionParameters debugFunctionParameters = (DebugFunctionParameters) it
					.next();

			DebugFunctionParametersXML debugFunctionParametersXML = new DebugFunctionParametersXML(
					debugFunctionParameters,
					DebugFunctionParametersXML.TAG_ROOT_ELEMENT);

			debugFunctionParametersXML
					.saveXML(debugFunctionParametersListElement);
		}
	}
}
