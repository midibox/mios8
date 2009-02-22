package org.midibox.mios.xml;

import org.midibox.mios.LCDMessage;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class LCDMessageXML extends MIOSSysexSendReceiveXML {

	protected LCDMessage lcdMessage;

	protected String lcdWidthAttr = "lcdWidth";

	protected String lcdHeightAttr = "lcdHeight";

	protected String modeAttr = "mode";

	protected String posXAttr = "posX";

	protected String posYAttr = "posY";

	protected String loopAttr = "loop";

	protected String messageTextTag = "messageText";

	public LCDMessageXML(LCDMessage lcdMessage, String rootElementTag) {

		super(lcdMessage, rootElementTag);

		this.lcdMessage = lcdMessage;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(lcdWidthAttr, "" + lcdMessage.getLcdWidth());

		rootElement.setAttribute(lcdHeightAttr, "" + lcdMessage.getLcdHeight());

		rootElement.setAttribute(modeAttr, "" + lcdMessage.getMode());

		rootElement.setAttribute(posXAttr, "" + lcdMessage.getPosX());

		rootElement.setAttribute(posYAttr, "" + lcdMessage.getPosY());

		rootElement.setAttribute(loopAttr, lcdMessage.isLoop() ? "true"
				: "false");

		Element messageTextElement = document.createElement(messageTextTag);

		rootElement.appendChild(messageTextElement);

		messageTextElement.setTextContent(lcdMessage.getMessageText());
	}
}
