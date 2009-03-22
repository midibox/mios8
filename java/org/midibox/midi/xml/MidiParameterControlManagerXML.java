package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import org.midibox.midi.MidiParameterControl;
import org.midibox.midi.MidiParameterControlManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class MidiParameterControlManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "midiParameterControlManager";

	public final static String TAG_MIDI_PARAMETER_CONTROLS = "midiParameterControls";

	public final static String ATTR_GLOBAL_CHANNEL = "globalChannel";

	protected MidiParameterControlManager midiParameterControlManager;

	public MidiParameterControlManagerXML(
			MidiParameterControlManager midiParameterControlManager,
			String rootElementTag) {

		super(rootElementTag);

		addTag(TAG_MIDI_PARAMETER_CONTROLS);

		this.midiParameterControlManager = midiParameterControlManager;
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			midiParameterControlManager.setGlobalChannel(stringToInt(element
					.getAttribute(ATTR_GLOBAL_CHANNEL)));

		} else if (name == TAG_MIDI_PARAMETER_CONTROLS) {

			Vector midiParameterControls = midiParameterControlManager
					.getMidiParametersControls();

			NodeList controllerElements = element.getChildNodes();

			for (int i = 0; i < midiParameterControls.size(); i++) {

				MidiParameterControl midiParameterControl = (MidiParameterControl) midiParameterControls
						.elementAt(i);

				MidiParameterControlXML midiParameterControlXML = new MidiParameterControlXML(
						midiParameterControl,
						MidiParameterControlXML.TAG_ROOT_ELEMENT);

				Node child = controllerElements.item(i);

				midiParameterControlXML.loadXML(child);
			}
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_GLOBAL_CHANNEL,
				intToString(midiParameterControlManager.getGlobalChannel()));

		Element midiParameterControlsElement = document
				.createElement(TAG_MIDI_PARAMETER_CONTROLS);

		rootElement.appendChild(midiParameterControlsElement);

		Iterator it = midiParameterControlManager.getMidiParametersControls()
				.iterator();

		while (it.hasNext()) {

			MidiParameterControl midiParameterControl = (MidiParameterControl) it
					.next();

			MidiParameterControlXML midiParameterControlXML = new MidiParameterControlXML(
					midiParameterControl,
					MidiParameterControlXML.TAG_ROOT_ELEMENT);

			midiParameterControlXML.saveXML(midiParameterControlsElement);
		}
	}
}
