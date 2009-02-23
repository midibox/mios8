package org.midibox.mios.xml;

import java.util.Iterator;

import org.midibox.mios.DebugFunction;
import org.midibox.mios.DebugFunctionParameters;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

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
