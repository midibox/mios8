package org.midibox.midi.xml;

import java.util.Iterator;

import org.midibox.midi.MidiParameterControl;
import org.midibox.midi.MidiParameterControlManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiParameterControlManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "midiParameterControlManager";

	public final static String TAG_MIDI_PARAMETER_CONTROLS = "midiParameterControls";

	public final static String ATTR_GLOBAL_CHANNEL = "globalChannel";

	protected MidiParameterControlManager midiParameterControlManager;

	public MidiParameterControlManagerXML(
			MidiParameterControlManager midiParameterControlManager,
			String rootElementTag) {

		super(rootElementTag);

		this.midiParameterControlManager = midiParameterControlManager;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_GLOBAL_CHANNEL,
				intToString(midiParameterControlManager.getGlobalChannel()));

		Element midiParameterControlsElement = document
				.createElement(TAG_MIDI_PARAMETER_CONTROLS);

		rootElement.appendChild(midiParameterControlsElement);

		Iterator it = midiParameterControlManager.getMidiParameters()
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
