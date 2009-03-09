package org.midibox.midi.xml;

import java.util.HashMap;
import java.util.Iterator;

import javax.sound.midi.MidiDevice;

import org.midibox.midi.MidiDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiDeviceManagerXML extends XMLUtils {

	public static final String TAG_ROOT_ELEMENT = "midiDeviceManager";

	public static final String TAG_DESELECTED_MIDI_READ_DEVICES = "deselectedMidiReadDevices";

	public static final String TAG_DESELECTED_MIDI_WRITE_DEVICES = "deselectedMidiWriteDevices";

	public static final String TAG_DESELECTED_MIDI_READ_DEVICE = "deselectedMidiReadDevice";

	public static final String TAG_DESELECTED_MIDI_WRITE_DEVICE = "deselectedMidiWriteDevice";

	protected MidiDeviceManager midiDeviceManager;

	public MidiDeviceManagerXML(MidiDeviceManager midiDeviceManager,
			String rootElementTag) {

		super(rootElementTag);

		this.midiDeviceManager = midiDeviceManager;

		tags.add(TAG_DESELECTED_MIDI_READ_DEVICES);
		tags.add(TAG_DESELECTED_MIDI_WRITE_DEVICES);
		tags.add(TAG_DESELECTED_MIDI_READ_DEVICE);
		tags.add(TAG_DESELECTED_MIDI_WRITE_DEVICE);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			midiDeviceManager.rescanDevices();

		} else if (name == TAG_DESELECTED_MIDI_READ_DEVICES) {

		} else if (name == TAG_DESELECTED_MIDI_READ_DEVICE) {

			int hash = stringToInt(element
					.getAttribute(MidiDeviceXML.ATTR_HASH_CODE));

			MidiDevice midiDevice = findDeviceHash(midiDeviceManager
					.getMidiReadDevices(), hash);

			if (midiDevice != null) {

				midiDeviceManager.deselectMidiReadDevice(midiDevice);
			}

		} else if (name == TAG_DESELECTED_MIDI_WRITE_DEVICES) {

		} else if (name == TAG_DESELECTED_MIDI_WRITE_DEVICE) {

			int hash = stringToInt(element
					.getAttribute(MidiDeviceXML.ATTR_HASH_CODE));

			MidiDevice midiDevice = findDeviceHash(midiDeviceManager
					.getMidiWriteDevices(), hash);

			if (midiDevice != null) {

				midiDeviceManager.deselectMidiWriteDevice(midiDevice);
			}
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element deselectedMidiReadDevicesElement = document
				.createElement(TAG_DESELECTED_MIDI_READ_DEVICES);

		rootElement.appendChild(deselectedMidiReadDevicesElement);

		Iterator it = midiDeviceManager.getMidiReadDevices().values()
				.iterator();

		while (it.hasNext()) {

			MidiDevice midiDevice = (MidiDevice) it.next();

			if (!midiDeviceManager.getSelectedMidiReadDevices().values()
					.contains(midiDevice)) {

				MidiDeviceXML midiDeviceXML = new MidiDeviceXML(midiDevice,
						TAG_DESELECTED_MIDI_READ_DEVICE);

				midiDeviceXML.saveXML(deselectedMidiReadDevicesElement);
			}
		}

		Element deselectedMidiWriteDevicesElement = document
				.createElement(TAG_DESELECTED_MIDI_WRITE_DEVICES);

		rootElement.appendChild(deselectedMidiWriteDevicesElement);

		it = midiDeviceManager.getMidiWriteDevices().values().iterator();

		while (it.hasNext()) {

			MidiDevice midiDevice = (MidiDevice) it.next();

			if (!midiDeviceManager.getSelectedMidiWriteDevices().values()
					.contains(midiDevice)) {

				MidiDeviceXML midiDeviceXML = new MidiDeviceXML(midiDevice,
						TAG_DESELECTED_MIDI_WRITE_DEVICE);

				midiDeviceXML.saveXML(deselectedMidiWriteDevicesElement);
			}
		}
	}

	protected MidiDevice findDeviceHash(HashMap midiDevices, int hash) {

		return (MidiDevice) midiDevices.get(intToString(hash));
	}
}
