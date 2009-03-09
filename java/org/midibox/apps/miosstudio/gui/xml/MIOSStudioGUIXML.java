package org.midibox.apps.miosstudio.gui.xml;

import java.util.Iterator;
import java.util.Vector;

import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.UIManager;

import org.midibox.apps.miosstudio.MIOSStudio;
import org.midibox.apps.miosstudio.gui.MIOSStudioGUI;
import org.midibox.apps.miosstudio.gui.MIOSStudioGUI.ExternalCommandButton;
import org.midibox.apps.miosstudio.xml.MIOSStudioXML;
import org.midibox.midi.gui.MidiFilterGUI;
import org.midibox.mios.gui.HexFileUploadGUI;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MIOSStudioGUIXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "miosStudioGUI";

	public final static String TAG_LOOK_AND_FEEL = "lookAndFeel";

	public final static String TAG_MAIN_WINDOW = "mainWindow";

	public final static String TAG_INTERNAL_FRAMES = "internalFrames";

	public final static String TAG_INTERNAL_FRAME = "internalFrame";

	public final static String TAG_EXTERNAL_COMMANDS = "externalCommands";

	public final static String TAG_EXTERNAL_COMMAND = "externalCommand";

	public final static String TAG_WORKSPACE_CURRENT_DIRECTORY = "workspaceCurrentDirectory";

	public final static String TAG_MIDI_FILTER_DEVICE_MANAGER_GUI = "midiFilterDeviceManagerGUI";

	public final static String TAG_MIDI_FILTER_GUI_CURRENT_DIRECTORY = "midiFilterGUICurrentDirectory";

	public final static String TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER_GUI = "hexFileUploadDeviceManagerGUI";

	public final static String TAG_HEX_FILE_GUI_CURRENT_DIRECTORY = "hexFileGUICurrentDirectory";

	public final static String ATTR_TITLE = "title";

	public final static String ATTR_POS_X = "posX";

	public final static String ATTR_POS_Y = "posY";

	public final static String ATTR_WIDTH = "width";

	public final static String ATTR_HEIGHT = "height";

	public final static String ATTR_VISIBLE = "visible";

	public final static String ATTR_ICONIFIED = "iconified";

	public final static String ATTR_MAXIMIZED = "maximized";

	public final static String ATTR_COMMAND_NAME = "commandName";

	public final static String ATTR_EXTERNAL_COMMAND = "externalCommand";

	public final static String ATTR_SELECTED_TAB = "selectedTab";

	protected MIOSStudioGUI miosStudioGUI;

	protected MIOSStudio miosStudio;

	protected boolean includeModel;

	protected boolean includeGUI;

	public MIOSStudioGUIXML(MIOSStudio miosStudio, String rootElementTag,
			boolean includeModel, boolean includeGUI) {

		this(null, miosStudio, rootElementTag, includeModel, includeGUI);
	}

	public MIOSStudioGUIXML(MIOSStudioGUI miosStudioGUI, String rootElementTag,
			boolean includeModel, boolean includeGUI) {

		this(miosStudioGUI, miosStudioGUI.getMiosStudio(), rootElementTag,
				includeModel, includeGUI);
	}

	public MIOSStudioGUIXML(MIOSStudioGUI miosStudioGUI, MIOSStudio miosStudio,
			String rootElementTag, boolean includeModel, boolean includeGUI) {

		super(rootElementTag);

		this.miosStudioGUI = miosStudioGUI;

		this.miosStudio = miosStudio;

		this.includeModel = includeModel;

		this.includeGUI = includeGUI;

		if (includeModel) {

			tags.add(MIOSStudioXML.TAG_ROOT_ELEMENT);
		}

		if (includeGUI) {

			tags.add(TAG_LOOK_AND_FEEL);
			tags.add(TAG_MAIN_WINDOW);
			tags.add(TAG_INTERNAL_FRAMES);
			tags.add(TAG_EXTERNAL_COMMANDS);
			tags.add(TAG_EXTERNAL_COMMAND);
			tags.add(TAG_WORKSPACE_CURRENT_DIRECTORY);
			tags.add(TAG_MIDI_FILTER_DEVICE_MANAGER_GUI);
			tags.add(TAG_MIDI_FILTER_GUI_CURRENT_DIRECTORY);
			tags.add(TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER_GUI);
			tags.add(TAG_HEX_FILE_GUI_CURRENT_DIRECTORY);
		}
	}

	protected void createGUI() {

		miosStudioGUI = new MIOSStudioGUI(miosStudio);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == MIOSStudioXML.TAG_ROOT_ELEMENT) {

			MIOSStudioXML miosStudioXML = new MIOSStudioXML(miosStudio,
					MIOSStudioXML.TAG_ROOT_ELEMENT);

			miosStudioXML.loadXML(element);

		} else if (name == TAG_MAIN_WINDOW) {

			JFrame mainWindow = DialogOwner.getFrame();

			if (mainWindow != null) {

				mainWindow.setBounds(stringToInt(element
						.getAttribute(ATTR_POS_X)), stringToInt(element
						.getAttribute(ATTR_POS_Y)), stringToInt(element
						.getAttribute(ATTR_WIDTH)), stringToInt(element
						.getAttribute(ATTR_HEIGHT)));
			}

		} else if (name == TAG_LOOK_AND_FEEL) {

			String lookAndFeel = element.getTextContent();

			if (miosStudioGUI == null) {

				try {

					UIManager.setLookAndFeel(lookAndFeel);

				} catch (Exception e) {

					e.printStackTrace();
				}

				createGUI();
			}

			miosStudioGUI.setLookAndFeel(lookAndFeel);

		} else if (name == TAG_INTERNAL_FRAMES) {

			Iterator it = miosStudioGUI.getInternalFrames().iterator();

			while (it.hasNext()) {

				JInternalFrame internalFrame = (JInternalFrame) it.next();

				internalFrame.setVisible(false);

				miosStudioGUI.getDesktop().remove(internalFrame);
			}

			Node childNode = element.getLastChild();

			while (childNode != null) {

				element = (Element) childNode;

				if (element.getNodeName() == TAG_INTERNAL_FRAME) {

					JInternalFrame internalFrame = findInternalFrame(element
							.getAttribute(ATTR_TITLE));

					if (internalFrame != null) {

						internalFrame.setBounds(stringToInt(element
								.getAttribute(ATTR_POS_X)), stringToInt(element
								.getAttribute(ATTR_POS_Y)), stringToInt(element
								.getAttribute(ATTR_WIDTH)), stringToInt(element
								.getAttribute(ATTR_HEIGHT)));

						try {

							internalFrame.setMaximum(stringToBoolean(element
									.getAttribute(ATTR_MAXIMIZED)));

							internalFrame.setIcon(stringToBoolean(element
									.getAttribute(ATTR_ICONIFIED)));

						} catch (Exception e) {

							e.printStackTrace();
						}

						if (stringToBoolean(element.getAttribute(ATTR_VISIBLE))) {

							miosStudioGUI.showFrame(internalFrame);
						}
					}

					childNode = childNode.getPreviousSibling();
				}
			}

		} else if (name == TAG_EXTERNAL_COMMANDS) {

			Iterator it = ((Vector) miosStudioGUI.getExternalCommands().clone())
					.iterator();

			while (it.hasNext()) {

				ExternalCommandButton externalCommandButton = (ExternalCommandButton) it
						.next();

				miosStudioGUI
						.removeExternalCommandButton(externalCommandButton);
			}

		} else if (name == TAG_EXTERNAL_COMMAND) {

			miosStudioGUI.addExternalCommandButton(element
					.getAttribute(ATTR_COMMAND_NAME), element
					.getAttribute(ATTR_EXTERNAL_COMMAND));

		} else if (name == TAG_WORKSPACE_CURRENT_DIRECTORY) {

			MIOSStudioGUI.setCurrentDirectory(element.getTextContent());

		} else if (name == TAG_MIDI_FILTER_DEVICE_MANAGER_GUI) {

		} else if (name == TAG_MIDI_FILTER_GUI_CURRENT_DIRECTORY) {

			MidiFilterGUI.setCurrentDirectory(element.getTextContent());

		} else if (name == TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER_GUI) {

			int tabIndex = stringToInt(element.getAttribute(ATTR_SELECTED_TAB));

			if (tabIndex < miosStudioGUI.getHexFileUploadDeviceManagerGUI()
					.getTabbedPane().getTabCount()) {

				miosStudioGUI.getHexFileUploadDeviceManagerGUI()
						.getTabbedPane().setSelectedIndex(tabIndex);

			}

		} else if (name == TAG_HEX_FILE_GUI_CURRENT_DIRECTORY) {

			HexFileUploadGUI.setCurrentDirectory(element.getTextContent());
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		if (includeModel) {

			MIOSStudioXML miosStudioXML = new MIOSStudioXML(miosStudio,
					MIOSStudioXML.TAG_ROOT_ELEMENT);

			miosStudioXML.saveXML(rootElement);
		}

		if (includeGUI) {

			JFrame mainWindow = DialogOwner.getFrame();

			if (mainWindow != null) {

				Element mainWindowElement = document
						.createElement(TAG_MAIN_WINDOW);

				rootElement.appendChild(mainWindowElement);

				mainWindowElement.setAttribute(ATTR_POS_X,
						intToString(mainWindow.getX()));

				mainWindowElement.setAttribute(ATTR_POS_Y,
						intToString(mainWindow.getY()));

				mainWindowElement.setAttribute(ATTR_WIDTH,
						intToString(mainWindow.getWidth()));

				mainWindowElement.setAttribute(ATTR_HEIGHT,
						intToString(mainWindow.getHeight()));
			}

			Element lookAndFeelElement = document
					.createElement(TAG_LOOK_AND_FEEL);

			rootElement.appendChild(lookAndFeelElement);

			lookAndFeelElement.setTextContent(miosStudioGUI.getLookAndFeel());

			Element internalFramesElement = document
					.createElement(TAG_INTERNAL_FRAMES);

			rootElement.appendChild(internalFramesElement);

			Vector internalFrames = (Vector) miosStudioGUI.getInternalFrames()
					.clone();

			JInternalFrame[] visibleInternalFrames = miosStudioGUI.getDesktop()
					.getAllFrames();

			for (int i = 0; i < visibleInternalFrames.length; i++) {

				JInternalFrame internalFrame = visibleInternalFrames[i];

				Element internalFrameElement = document
						.createElement(TAG_INTERNAL_FRAME);

				internalFramesElement.appendChild(internalFrameElement);

				saveInternalFrame(internalFrame, internalFrameElement);

				internalFrames.remove(internalFrame);
			}

			Iterator it = internalFrames.iterator();

			while (it.hasNext()) {

				JInternalFrame internalFrame = (JInternalFrame) it.next();

				Element internalFrameElement = document
						.createElement(TAG_INTERNAL_FRAME);

				internalFramesElement.appendChild(internalFrameElement);

				saveInternalFrame(internalFrame, internalFrameElement);
			}

			Element externalCommandsElement = document
					.createElement(TAG_EXTERNAL_COMMANDS);

			rootElement.appendChild(externalCommandsElement);

			it = miosStudioGUI.getExternalCommands().iterator();

			while (it.hasNext()) {

				ExternalCommandButton externalCommandButton = (ExternalCommandButton) it
						.next();

				Element externalCommandElement = document
						.createElement(TAG_EXTERNAL_COMMAND);

				externalCommandsElement.appendChild(externalCommandElement);

				externalCommandElement.setAttribute(ATTR_COMMAND_NAME,
						externalCommandButton.commandName);

				externalCommandElement.setAttribute(ATTR_EXTERNAL_COMMAND,
						externalCommandButton.externalCommand);
			}

			Element workSpaceCurrentDirectory = document
					.createElement(TAG_WORKSPACE_CURRENT_DIRECTORY);

			rootElement.appendChild(workSpaceCurrentDirectory);

			workSpaceCurrentDirectory.setTextContent(MIOSStudioGUI
					.getCurrentDirectory());

			Element midiFilterDeviceManagerGUIelement = document
					.createElement(TAG_MIDI_FILTER_DEVICE_MANAGER_GUI);

			rootElement.appendChild(midiFilterDeviceManagerGUIelement);

			Element midiFilterCurrentDirectory = document
					.createElement(TAG_MIDI_FILTER_GUI_CURRENT_DIRECTORY);

			midiFilterDeviceManagerGUIelement
					.appendChild(midiFilterCurrentDirectory);

			midiFilterCurrentDirectory.setTextContent(MidiFilterGUI
					.getCurrentDirectory());

			Element hexFileUploadDeviceManagerGUIelement = document
					.createElement(TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER_GUI);

			rootElement.appendChild(hexFileUploadDeviceManagerGUIelement);

			hexFileUploadDeviceManagerGUIelement.setAttribute(
					ATTR_SELECTED_TAB, intToString(miosStudioGUI
							.getHexFileUploadDeviceManagerGUI().getTabbedPane()
							.getSelectedIndex()));

			Element hexFileCurrentDirectoryElement = document
					.createElement(TAG_HEX_FILE_GUI_CURRENT_DIRECTORY);

			hexFileUploadDeviceManagerGUIelement
					.appendChild(hexFileCurrentDirectoryElement);

			hexFileCurrentDirectoryElement.setTextContent(HexFileUploadGUI
					.getCurrentDirectory());
		}
	}

	public MIOSStudioGUI getMiosStudioGUI() {
		return miosStudioGUI;
	}

	protected void saveInternalFrame(JInternalFrame internalFrame,
			Element internalFrameElement) {

		internalFrameElement.setAttribute(ATTR_TITLE, internalFrame.getTitle());

		internalFrameElement.setAttribute(ATTR_POS_X, intToString(internalFrame
				.getX()));

		internalFrameElement.setAttribute(ATTR_POS_Y, intToString(internalFrame
				.getY()));

		internalFrameElement.setAttribute(ATTR_WIDTH, intToString(internalFrame
				.getWidth()));

		internalFrameElement.setAttribute(ATTR_HEIGHT,
				intToString(internalFrame.getHeight()));

		internalFrameElement.setAttribute(ATTR_VISIBLE,
				booleanToString(internalFrame.isVisible()));

		internalFrameElement.setAttribute(ATTR_ICONIFIED,
				booleanToString(internalFrame.isIcon()));

		internalFrameElement.setAttribute(ATTR_MAXIMIZED,
				booleanToString(internalFrame.isMaximum()));
	}

	protected JInternalFrame findInternalFrame(String name) {

		Iterator it = miosStudioGUI.getInternalFrames().iterator();

		while (it.hasNext()) {

			JInternalFrame internalFrame = (JInternalFrame) it.next();

			if (internalFrame.getTitle().equals(name)) {

				return internalFrame;
			}
		}

		return null;
	}
}
