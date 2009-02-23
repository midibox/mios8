package org.midibox.midi.xml;

import org.midibox.midi.MidiParameter;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Node;

public class MidiParameterXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "midiParameter";

	public final static String ATTR_CHANNEL = "channel";

	public final static String ATTR_STATUS = "status";

	public final static String ATTR_VALUE = "value";

	public final static String ATTR_NUMBER = "number";

	public final static String ATTR_HIGH_RESOLUTION = "highResolution";

	protected MidiParameter midiParameter;

	protected MidiParameterXML(MidiParameter midiParameter,
			String rootElementTag) {

		super(rootElementTag);

		this.midiParameter = midiParameter;
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_CHANNEL, intToString(midiParameter
				.getMidiChannel()));

		rootElement.setAttribute(ATTR_STATUS, intToString(midiParameter
				.getMidiStatus()));

		rootElement.setAttribute(ATTR_VALUE, intToString(midiParameter
				.getMidiValue()));

		rootElement.setAttribute(ATTR_NUMBER, intToString(midiParameter
				.getMidiNumber()));

		rootElement.setAttribute(ATTR_HIGH_RESOLUTION,
				booleanToString(midiParameter.isHighResolution()));
	}
}
