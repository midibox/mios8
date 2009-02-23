package org.midibox.midi.xml;

import org.midibox.midi.MidiParameterControl;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiParameterControlXML extends MidiParameterXML {

	public final static String TAG_ROOT_ELEMENT = "midiParameterControl";

	public final static String ATTR_RECEIVE = "receive";

	public final static String ATTR_SEND = "send";

	public final static String ATTR_GLOBAL = "global";

	public final static String ATTR_LEARN = "learn";

	public final static String ATTR_TYPE = "type";

	public final static String ATTR_DEFAULT_VALUE = "defaultValue";

	protected MidiParameterControl midiParameterControl;

	public MidiParameterControlXML(MidiParameterControl midiParameterControl,
			String rootElementTag) {

		super(midiParameterControl, rootElementTag);

		this.midiParameterControl = midiParameterControl;
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			midiParameterControl.setReceive(stringToBoolean(element
					.getAttribute(ATTR_RECEIVE)));

			midiParameterControl.setSend(stringToBoolean(element
					.getAttribute(ATTR_SEND)));

			midiParameterControl.setGlobal(stringToBoolean(element
					.getAttribute(ATTR_GLOBAL)));

			midiParameterControl.setLearn(stringToBoolean(element
					.getAttribute(ATTR_LEARN)));

			midiParameterControl.setType(stringToInt(element
					.getAttribute(ATTR_TYPE)));

			midiParameterControl.setMidiDefaultValue(stringToInt(element
					.getAttribute(ATTR_DEFAULT_VALUE)));
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(ATTR_TYPE, intToString(midiParameterControl
				.getType()));

		rootElement.setAttribute(ATTR_DEFAULT_VALUE,
				intToString(midiParameterControl.getMidiDefaultValue()));

		rootElement.setAttribute(ATTR_SEND,
				booleanToString(midiParameterControl.isSend()));

		rootElement.setAttribute(ATTR_RECEIVE,
				booleanToString(midiParameterControl.isReceive()));

		rootElement.setAttribute(ATTR_GLOBAL,
				booleanToString(midiParameterControl.isGlobal()));

		rootElement.setAttribute(ATTR_LEARN,
				booleanToString(midiParameterControl.isLearn()));
	}
}
