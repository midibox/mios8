package org.midibox.midi.xml;

import org.midibox.midi.MidiMonitorFiltered;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiMonitorFilteredXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "midiMonitorFiltered";

	protected MidiMonitorFiltered midiMonitorFiltered;

	public MidiMonitorFilteredXML(MidiMonitorFiltered midiMonitorFiltered,
			String rootElementTag) {

		super(rootElementTag);

		this.midiMonitorFiltered = midiMonitorFiltered;

		addTag(MidiFilterXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == MidiFilterXML.TAG_ROOT_ELEMENT) {

			MidiFilterXML midiFilterXML = new MidiFilterXML(midiMonitorFiltered
					.getMidiFilter(), MidiFilterXML.TAG_ROOT_ELEMENT);

			midiFilterXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		MidiFilterXML midiFilterXML = new MidiFilterXML(midiMonitorFiltered
				.getMidiFilter(), MidiFilterXML.TAG_ROOT_ELEMENT);

		midiFilterXML.saveXML(rootElement);
	}
}
