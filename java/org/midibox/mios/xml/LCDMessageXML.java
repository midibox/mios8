package org.midibox.mios.xml;

import org.midibox.mios.LCDMessage;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class LCDMessageXML extends MIOSSysexSendReceiveXML {

	protected LCDMessage lcdMessage;

	public static final String TAG_ROOT_ELEMENT = "lcdMessage";

	public static final String TAG_MESSAGE_TEXT = "messageText";

	public static final String ATTR_LCD_WIDTH = "lcdWidth";

	public static final String ATTR_LCD_HEIGHT = "lcdHeight";

	public static final String ATTR_MODE = "mode";

	public static final String ATTR_POS_X = "posX";

	public static final String ATTR_POS_Y = "posY";

	public static final String ATTR_LOOP = "loop";

	public LCDMessageXML(LCDMessage lcdMessage, String rootElementTag) {

		super(lcdMessage, rootElementTag);

		this.lcdMessage = lcdMessage;

		addTag(TAG_MESSAGE_TEXT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			lcdMessage.setDeviceID(stringToInt(element
					.getAttribute(ATTR_DEVICE_ID)));

			lcdMessage.setMIOS32Mode(stringToBoolean(element
					.getAttribute(ATTR_MIOS32_MODE)));

			lcdMessage.setLcdWidth(stringToInt(element
					.getAttribute(ATTR_LCD_WIDTH)));

			lcdMessage.setLcdHeight(stringToInt(element
					.getAttribute(ATTR_LCD_HEIGHT)));

			lcdMessage.setMode(stringToInt(element.getAttribute(ATTR_MODE)));

			lcdMessage.setPosX(stringToInt(element.getAttribute(ATTR_POS_X)));

			lcdMessage.setPosY(stringToInt(element.getAttribute(ATTR_POS_Y)));

			lcdMessage
					.setLoop(stringToBoolean(element.getAttribute(ATTR_LOOP)));

		} else if (name == TAG_MESSAGE_TEXT) {

			lcdMessage.setMessageText(element.getTextContent());
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_LCD_WIDTH, intToString(lcdMessage
				.getLcdWidth()));

		rootElement.setAttribute(ATTR_LCD_HEIGHT, intToString(lcdMessage
				.getLcdHeight()));

		rootElement.setAttribute(ATTR_MODE, intToString(lcdMessage.getMode()));

		rootElement.setAttribute(ATTR_POS_X, intToString(lcdMessage.getPosX()));

		rootElement.setAttribute(ATTR_POS_Y, intToString(lcdMessage.getPosY()));

		rootElement.setAttribute(ATTR_LOOP,
				booleanToString(lcdMessage.isLoop()));

		Element messageTextElement = document.createElement(TAG_MESSAGE_TEXT);

		rootElement.appendChild(messageTextElement);

		messageTextElement.setTextContent(lcdMessage.getMessageText());
	}
}
