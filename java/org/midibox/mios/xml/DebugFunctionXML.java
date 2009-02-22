package org.midibox.mios.xml;

import java.util.Iterator;

import org.midibox.mios.DebugFunction;
import org.midibox.mios.DebugFunctionParameters;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class DebugFunctionXML extends MIOSSysexSendReceiveXML {

	protected DebugFunction debugFunction;

	protected String modeAttr = "mode";

	protected String delayTimeAttr = "delayTime";

	protected String sramReadAddressAttr = "sramReadAddress";

	protected String sramReadCounterAttr = "sramReadCounter";

	protected String sramWriteAddressAttr = "sramWriteAddress";

	protected String sramWriteDataAttr = "sramWriteData";

	protected String debugFunctionParametersTag = "debugFunctionParameters";

	protected String debugFunctionParameterTag = "debugFunctionParameter";

	public DebugFunctionXML(DebugFunction debugFunction, String rootElementTag) {

		super(debugFunction, rootElementTag);

		this.debugFunction = debugFunction;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(modeAttr, "" + debugFunction.getMode());

		rootElement.setAttribute(delayTimeAttr, ""
				+ debugFunction.getDelayTime());

		rootElement.setAttribute(sramReadAddressAttr, ""
				+ debugFunction.getSramReadAddress());

		rootElement.setAttribute(sramReadCounterAttr, ""
				+ debugFunction.getSramReadCounter());

		rootElement.setAttribute(sramWriteAddressAttr, ""
				+ debugFunction.getSramWriteAddress());

		rootElement.setAttribute(sramWriteDataAttr, ""
				+ debugFunction.getSramWriteData());

		Element debugFunctionParametersElement = document
				.createElement(debugFunctionParametersTag);

		rootElement.appendChild(debugFunctionParametersElement);

		Iterator it = debugFunction.getDebugFunctionParameters().iterator();

		while (it.hasNext()) {

			DebugFunctionParameters debugFunctionParameters = (DebugFunctionParameters) it
					.next();

			DebugFunctionParametersXML debugFunctionParametersXML = new DebugFunctionParametersXML(
					debugFunctionParameters, debugFunctionParametersTag);

			debugFunctionParametersXML.saveXML(debugFunctionParametersElement);
		}
	}
}
