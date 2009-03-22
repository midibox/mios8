package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import org.midibox.midi.MidiFilterDevice;
import org.midibox.midi.MidiFilterDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiFilterDeviceManagerXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "midiFilterDeviceManager";

	public final static String TAG_MIDI_FILTER_DEVICES = "midiFilterDevices";

	protected MidiFilterDeviceManager midiFilterDeviceManager;

	public MidiFilterDeviceManagerXML(
			MidiFilterDeviceManager midiFilterDeviceManager,
			String rootElementTag) {

		super(rootElementTag);

		this.midiFilterDeviceManager = midiFilterDeviceManager;

		addTag(TAG_MIDI_FILTER_DEVICES);
		addTag(MidiFilterDeviceXML.TAG_ROOT_ELEMENT);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

		} else if (name == TAG_MIDI_FILTER_DEVICES) {

			Iterator it = ((Vector) midiFilterDeviceManager
					.getMidiFilterDevices().clone()).iterator();

			while (it.hasNext()) {

				MidiFilterDevice midiFilterDevice = (MidiFilterDevice) it
						.next();

				midiFilterDeviceManager
						.removeMidiFilterDevice(midiFilterDevice);
			}

		} else if (name == MidiFilterDeviceXML.TAG_ROOT_ELEMENT) {

			MidiFilterDevice midiFilterDevice = new MidiFilterDevice(element
					.getAttribute(MidiFilterDeviceXML.ATTR_NAME));

			MidiFilterDeviceXML midiFilterDeviceXML = new MidiFilterDeviceXML(
					midiFilterDevice, MidiFilterDeviceXML.TAG_ROOT_ELEMENT);

			midiFilterDeviceManager.addMidiFilterDevice(midiFilterDevice);

			midiFilterDeviceXML.loadXML(element);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element midiFilterDevicesElement = document
				.createElement(TAG_MIDI_FILTER_DEVICES);

		rootElement.appendChild(midiFilterDevicesElement);

		Iterator it = midiFilterDeviceManager.getMidiFilterDevices().iterator();

		while (it.hasNext()) {

			MidiFilterDevice midiFilterDevice = (MidiFilterDevice) it.next();

			MidiFilterDeviceXML midiFilterDeviceXML = new MidiFilterDeviceXML(
					midiFilterDevice, MidiFilterDeviceXML.TAG_ROOT_ELEMENT);

			midiFilterDeviceXML.saveXML(midiFilterDevicesElement);
		}
	}
}
