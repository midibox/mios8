package org.midibox.apps.miosstudio.gui.xml;

import java.awt.Dimension;
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

	public final static String TAG_HEX_FILE_UPLOAD_GUI_CURRENT_DIRECTORY = "hexFileUploadGUICurrentDirectory";

	public final static String TAG_WORKSPACE_MRU_LIST = "workspaceMRUList";

	public final static String TAG_WORKSPACE_MRU = "workspaceMRU";

	public final static String TAG_MIDI_FILTER_GUI_MRU_LIST = "midiFilterGUIMRUList";

	public final static String TAG_MIDI_FILTER_GUI_MRU = "midiFilterGUIMRU";

	public final static String TAG_HEX_FILE_UPLOAD_GUI_MRU_LIST = "hexFileUploadGUIMRUList";

	public final static String TAG_HEX_FILE_UPLOAD_GUI_MRU = "hexFileUploadGUIMRU";

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

	protected boolean includeMRU;

	public MIOSStudioGUIXML(MIOSStudio miosStudio, String rootElementTag,
			boolean includeModel, boolean includeGUI, boolean includeMRU) {

		this(null, miosStudio, rootElementTag, includeModel, includeGUI,
				includeMRU);
	}

	public MIOSStudioGUIXML(MIOSStudioGUI miosStudioGUI, String rootElementTag,
			boolean includeModel, boolean includeGUI, boolean includeMRU) {

		this(miosStudioGUI, miosStudioGUI.getMiosStudio(), rootElementTag,
				includeModel, includeGUI, includeMRU);
	}

	public MIOSStudioGUIXML(MIOSStudioGUI miosStudioGUI, MIOSStudio miosStudio,
			String rootElementTag, boolean includeModel, boolean includeGUI,
			boolean includeMRU) {

		super(rootElementTag);

		this.miosStudioGUI = miosStudioGUI;

		this.miosStudio = miosStudio;

		this.includeModel = includeModel;

		this.includeGUI = includeGUI;

		this.includeMRU = includeMRU;

		if (includeModel) {

			addTag(MIOSStudioXML.TAG_ROOT_ELEMENT);
		}

		if (includeGUI) {

			addTag(TAG_LOOK_AND_FEEL);
			addTag(TAG_MAIN_WINDOW);
			addTag(TAG_INTERNAL_FRAMES);
			addTag(TAG_EXTERNAL_COMMANDS);
			addTag(TAG_EXTERNAL_COMMAND);
			addTag(TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER_GUI);
		}

		if (includeMRU) {

			addTag(TAG_WORKSPACE_CURRENT_DIRECTORY);
			addTag(TAG_WORKSPACE_MRU_LIST);
			addTag(TAG_WORKSPACE_MRU);
			addTag(TAG_MIDI_FILTER_DEVICE_MANAGER_GUI);
			addTag(TAG_MIDI_FILTER_GUI_CURRENT_DIRECTORY);
			addTag(TAG_MIDI_FILTER_GUI_MRU_LIST);
			addTag(TAG_MIDI_FILTER_GUI_MRU);
			addTag(TAG_HEX_FILE_UPLOAD_GUI_CURRENT_DIRECTORY);
			addTag(TAG_HEX_FILE_UPLOAD_GUI_MRU_LIST);
			addTag(TAG_HEX_FILE_UPLOAD_GUI_MRU);
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

						Dimension d = internalFrame.getPreferredSize();

						if (internalFrame.isResizable()) {

							internalFrame.setBounds(stringToInt(element
									.getAttribute(ATTR_POS_X)),
									stringToInt(element
											.getAttribute(ATTR_POS_Y)), Math
											.max(stringToInt(element
													.getAttribute(ATTR_WIDTH)),
													d.width),
									Math.max(stringToInt(element
											.getAttribute(ATTR_HEIGHT)),
											d.height));

						} else {

							internalFrame.setBounds(stringToInt(element
									.getAttribute(ATTR_POS_X)),
									stringToInt(element
											.getAttribute(ATTR_POS_Y)),
									d.width, d.height);
						}

						if (stringToBoolean(element.getAttribute(ATTR_VISIBLE))) {

							miosStudioGUI.showFrame(internalFrame);
						}

						try {

							internalFrame.setMaximum(stringToBoolean(element
									.getAttribute(ATTR_MAXIMIZED)));

							internalFrame.setIcon(stringToBoolean(element
									.getAttribute(ATTR_ICONIFIED)));

						} catch (Exception e) {

							e.printStackTrace();
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

		} else if (name == TAG_HEX_FILE_UPLOAD_GUI_CURRENT_DIRECTORY) {

			HexFileUploadGUI.setCurrentDirectory(element.getTextContent());

		} else if (name == TAG_WORKSPACE_MRU_LIST) {

			miosStudioGUI.getMRU().removeAllElements();

		} else if (name == TAG_WORKSPACE_MRU) {

			miosStudioGUI.saveMRU(element.getTextContent());

		} else if (name == TAG_MIDI_FILTER_GUI_MRU_LIST) {

			MidiFilterGUI.getMRU().removeAllElements();

		} else if (name == TAG_MIDI_FILTER_GUI_MRU) {

			MidiFilterGUI.saveMRU(element.getTextContent());

		} else if (name == TAG_HEX_FILE_UPLOAD_GUI_MRU_LIST) {

			HexFileUploadGUI.getMRU().removeAllElements();

		} else if (name == TAG_HEX_FILE_UPLOAD_GUI_MRU) {

			HexFileUploadGUI.saveMRU(element.getTextContent());
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

			if (includeMRU) {

				Element workSpaceCurrentDirectory = document
						.createElement(TAG_WORKSPACE_CURRENT_DIRECTORY);

				rootElement.appendChild(workSpaceCurrentDirectory);

				workSpaceCurrentDirectory.setTextContent(MIOSStudioGUI
						.getCurrentDirectory());

				Element mruListElement = document
						.createElement(TAG_WORKSPACE_MRU_LIST);

				rootElement.appendChild(mruListElement);

				it = miosStudioGUI.getMRU().iterator();

				while (it.hasNext()) {

					Element mru = document.createElement(TAG_WORKSPACE_MRU);

					mruListElement.appendChild(mru);

					mru.setTextContent((String) it.next());
				}

				Element midiFilterDeviceManagerGUIelement = document
						.createElement(TAG_MIDI_FILTER_DEVICE_MANAGER_GUI);

				rootElement.appendChild(midiFilterDeviceManagerGUIelement);

				Element midiFilterCurrentDirectory = document
						.createElement(TAG_MIDI_FILTER_GUI_CURRENT_DIRECTORY);

				midiFilterDeviceManagerGUIelement
						.appendChild(midiFilterCurrentDirectory);

				midiFilterCurrentDirectory.setTextContent(MidiFilterGUI
						.getCurrentDirectory());
				
				mruListElement = document
						.createElement(TAG_MIDI_FILTER_GUI_MRU_LIST);

				midiFilterDeviceManagerGUIelement.appendChild(mruListElement);

				it = MidiFilterGUI.getMRU().iterator();

				while (it.hasNext()) {

					Element mru = document
							.createElement(TAG_MIDI_FILTER_GUI_MRU);

					mruListElement.appendChild(mru);

					mru.setTextContent((String) it.next());
				}
			}

			Element hexFileUploadDeviceManagerGUIelement = document
					.createElement(TAG_HEX_FILE_UPLOAD_DEVICE_MANAGER_GUI);

			rootElement.appendChild(hexFileUploadDeviceManagerGUIelement);

			hexFileUploadDeviceManagerGUIelement.setAttribute(
					ATTR_SELECTED_TAB, intToString(miosStudioGUI
							.getHexFileUploadDeviceManagerGUI().getTabbedPane()
							.getSelectedIndex()));

			if (includeMRU) {


				Element hexFileCurrentDirectoryElement = document
						.createElement(TAG_HEX_FILE_UPLOAD_GUI_CURRENT_DIRECTORY);

				hexFileUploadDeviceManagerGUIelement
						.appendChild(hexFileCurrentDirectoryElement);

				hexFileCurrentDirectoryElement.setTextContent(HexFileUploadGUI
						.getCurrentDirectory());
				
				Element mruListElement = document
						.createElement(TAG_HEX_FILE_UPLOAD_GUI_MRU_LIST);

				hexFileUploadDeviceManagerGUIelement
						.appendChild(mruListElement);

				it = HexFileUploadGUI.getMRU().iterator();

				while (it.hasNext()) {

					Element mru = document
							.createElement(TAG_HEX_FILE_UPLOAD_GUI_MRU);

					mruListElement.appendChild(mru);

					mru.setTextContent((String) it.next());
				}
			}
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
