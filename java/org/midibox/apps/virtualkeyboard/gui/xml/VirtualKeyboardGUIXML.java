package org.midibox.apps.virtualkeyboard.gui.xml;

import java.awt.Point;

import javax.swing.JFrame;
import javax.swing.UIManager;

import org.midibox.apps.virtualkeyboard.VirtualKeyboard;
import org.midibox.apps.virtualkeyboard.gui.VirtualKeyboardGUI;
import org.midibox.apps.virtualkeyboard.xml.VirtualKeyboardXML;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class VirtualKeyboardGUIXML extends XMLUtils {

	public static final String TAG_ROOT_ELEMENT = "virtualKeyboardGUI";

	public final static String TAG_LOOK_AND_FEEL = "lookAndFeel";

	public final static String TAG_MAIN_WINDOW = "mainWindow";

	public final static String TAG_CONNECTIONS_VISIBLE = "connectionsVisible";

	public final static String ATTR_POS_X = "posX";

	public final static String ATTR_POS_Y = "posY";

	public final static String ATTR_WIDTH = "width";

	public final static String ATTR_HEIGHT = "height";

	public final static String ATTR_ENABLE = "enable";

	protected VirtualKeyboardGUI virtualKeyboardGUI;

	protected VirtualKeyboard virtualKeyboard;

	public VirtualKeyboardGUIXML(VirtualKeyboard virtualKeyboard,
			String rootElementTag) {

		this(null, virtualKeyboard, rootElementTag);
	}

	public VirtualKeyboardGUIXML(VirtualKeyboardGUI virtualKeyboardGUI,
			String rootElementTag) {

		this(virtualKeyboardGUI, virtualKeyboardGUI.getVirtualKeyboard(),
				rootElementTag);
	}

	public VirtualKeyboardGUIXML(VirtualKeyboardGUI virtualKeyboardGUI,
			VirtualKeyboard virtualKeyboard, String rootElementTag) {

		super(rootElementTag);

		this.virtualKeyboard = virtualKeyboard;

		this.virtualKeyboardGUI = virtualKeyboardGUI;

		addTag(VirtualKeyboardXML.TAG_ROOT_ELEMENT);
		addTag(TAG_LOOK_AND_FEEL);
		addTag(TAG_MAIN_WINDOW);
		addTag(TAG_CONNECTIONS_VISIBLE);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == VirtualKeyboardXML.TAG_ROOT_ELEMENT) {

			VirtualKeyboardXML virtualKeyboardXML = new VirtualKeyboardXML(
					virtualKeyboard, VirtualKeyboardXML.TAG_ROOT_ELEMENT);

			virtualKeyboardXML.loadXML(element);

		} else if (name == TAG_MAIN_WINDOW) {

			JFrame mainWindow = DialogOwner.getFrame();

			if (mainWindow != null) {

				mainWindow.setLocation(new Point(stringToInt(element
						.getAttribute(ATTR_POS_X)), stringToInt(element
						.getAttribute(ATTR_POS_Y))));
			}

		} else if (name == TAG_LOOK_AND_FEEL) {

			String lookAndFeel = element.getTextContent();

			if (virtualKeyboardGUI == null) {

				try {

					UIManager.setLookAndFeel(lookAndFeel);

				} catch (Exception e) {

					e.printStackTrace();
				}

				virtualKeyboardGUI = new VirtualKeyboardGUI(virtualKeyboard);
			}

			virtualKeyboardGUI.setLookAndFeel(lookAndFeel);

		} else if (name == TAG_CONNECTIONS_VISIBLE) {

			virtualKeyboardGUI.setShowConnections(stringToBoolean(element
					.getAttribute(ATTR_ENABLE)));
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		VirtualKeyboardXML virtualKeyboardXML = new VirtualKeyboardXML(
				virtualKeyboard, VirtualKeyboardXML.TAG_ROOT_ELEMENT);

		virtualKeyboardXML.saveXML(rootElement);

		JFrame mainWindow = DialogOwner.getFrame();

		if (mainWindow != null) {

			Element mainWindowElement = document.createElement(TAG_MAIN_WINDOW);

			rootElement.appendChild(mainWindowElement);

			mainWindowElement.setAttribute(ATTR_POS_X, intToString(mainWindow
					.getX()));

			mainWindowElement.setAttribute(ATTR_POS_Y, intToString(mainWindow
					.getY()));
		}

		Element lookAndFeelElement = document.createElement(TAG_LOOK_AND_FEEL);

		rootElement.appendChild(lookAndFeelElement);

		lookAndFeelElement.setTextContent(virtualKeyboardGUI.getLookAndFeel());

		Element connectionsVisibleElement = document
				.createElement(TAG_CONNECTIONS_VISIBLE);

		rootElement.appendChild(connectionsVisibleElement);

		connectionsVisibleElement.setAttribute(ATTR_ENABLE,
				booleanToString(virtualKeyboardGUI.isShowConnections()));
	}

	public VirtualKeyboardGUI getVirtualKeyboardGUI() {
		return virtualKeyboardGUI;
	}
}
