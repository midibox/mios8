package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import org.midibox.midi.MidiFilter;
import org.midibox.midi.MidiFilterManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiFilterManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "midiFilterManager";

	public final static String TAG_MIDI_FILTERS = "midiFilters";

	protected MidiFilterManager midiFilterManager;

	public MidiFilterManagerXML(MidiFilterManager midiFilterManager,
			String rootElementTag) {

		super(rootElementTag);

		this.midiFilterManager = midiFilterManager;

		addTag(TAG_MIDI_FILTERS);
		addTag(MidiFilterXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == TAG_MIDI_FILTERS) {

			Iterator it = ((Vector) midiFilterManager.getMidiFilters().clone())
					.iterator();

			while (it.hasNext()) {

				MidiFilter midiFilter = (MidiFilter) it.next();

				midiFilterManager.removeMidiFilter(midiFilter);
			}

		} else if (name == MidiFilterXML.TAG_ROOT_ELEMENT) {

			MidiFilter midiFilter = new MidiFilter(null);

			MidiFilterXML midiFilterXML = new MidiFilterXML(midiFilter,
					MidiFilterXML.TAG_ROOT_ELEMENT);

			midiFilterManager.addMidiFilter(midiFilter);

			midiFilterXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element midiFiltersElement = document.createElement(TAG_MIDI_FILTERS);

		rootElement.appendChild(midiFiltersElement);

		Iterator it = midiFilterManager.getMidiFilters().iterator();

		while (it.hasNext()) {

			MidiFilter midiFilter = (MidiFilter) it.next();

			MidiFilterXML midiFilterXML = new MidiFilterXML(midiFilter,
					MidiFilterXML.TAG_ROOT_ELEMENT);

			midiFilterXML.saveXML(midiFiltersElement);
		}
	}
}
