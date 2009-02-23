package org.midibox.midi.xml;

import java.util.Iterator;
import java.util.Vector;

import javax.sound.midi.MidiDevice;

import org.midibox.midi.MidiDeviceManager;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiDeviceManagerXML extends XMLUtils {

	public static final String TAG_ROOT_ELEMENT = "midiDeviceManager";

	public static final String TAG_SELECTED_MIDI_READ_DEVICES = "selectedMidiReadDevices";

	public static final String TAG_SELECTED_MIDI_WRITE_DEVICES = "selectedMidiWriteDevices";

	public static final String TAG_SELECTED_MIDI_READ_DEVICE = "selectedMidiReadDevice";

	public static final String TAG_SELECTED_MIDI_WRITE_DEVICE = "selectedMidiWriteDevice";

	protected MidiDeviceManager midiDeviceManager;

	public MidiDeviceManagerXML(MidiDeviceManager midiDeviceManager,
			String rootElementTag) {

		super(rootElementTag);

		this.midiDeviceManager = midiDeviceManager;

		tags.add(TAG_SELECTED_MIDI_READ_DEVICES);
		tags.add(TAG_SELECTED_MIDI_WRITE_DEVICES);
		tags.add(TAG_SELECTED_MIDI_READ_DEVICE);
		tags.add(TAG_SELECTED_MIDI_WRITE_DEVICE);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			midiDeviceManager.rescanDevices();

		} else if (name == TAG_SELECTED_MIDI_READ_DEVICES) {

			Iterator it = ((Vector) midiDeviceManager
					.getSelectedMidiReadDevices().clone()).iterator();

			while (it.hasNext()) {

				MidiDevice midiDevice = (MidiDevice) it.next();

				midiDeviceManager.deselectMidiReadDevice(midiDevice);
			}

		} else if (name == TAG_SELECTED_MIDI_READ_DEVICE) {

			int hash = Integer.parseInt(element
					.getAttribute(MidiDeviceXML.ATTR_HASH_CODE));

			MidiDevice midiDevice = findDeviceHash(midiDeviceManager
					.getMidiReadDevices(), hash);

			if (midiDevice != null) {

				midiDeviceManager.selectMidiReadDevice(midiDevice);
			}

		} else if (name == TAG_SELECTED_MIDI_WRITE_DEVICES) {

			Iterator it = ((Vector) midiDeviceManager
					.getSelectedMidiWriteDevices().clone()).iterator();

			while (it.hasNext()) {

				MidiDevice midiDevice = (MidiDevice) it.next();

				midiDeviceManager.deselectMidiWriteDevice(midiDevice);
			}

		} else if (name == TAG_SELECTED_MIDI_WRITE_DEVICE) {

			int hash = Integer.parseInt(element
					.getAttribute(MidiDeviceXML.ATTR_HASH_CODE));

			MidiDevice midiDevice = findDeviceHash(midiDeviceManager
					.getMidiWriteDevices(), hash);

			if (midiDevice != null) {

				midiDeviceManager.selectMidiWriteDevice(midiDevice);
			}
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element selectedMidiReadDevicesElement = document
				.createElement(TAG_SELECTED_MIDI_READ_DEVICES);

		rootElement.appendChild(selectedMidiReadDevicesElement);

		Iterator it = midiDeviceManager.getSelectedMidiReadDevices().iterator();

		while (it.hasNext()) {

			MidiDevice midiDevice = (MidiDevice) it.next();

			MidiDeviceXML midiDeviceXML = new MidiDeviceXML(midiDevice,
					TAG_SELECTED_MIDI_READ_DEVICE);

			midiDeviceXML.saveXML(selectedMidiReadDevicesElement);
		}

		Element selectedMidiWriteDevicesElement = document
				.createElement(TAG_SELECTED_MIDI_WRITE_DEVICES);

		rootElement.appendChild(selectedMidiWriteDevicesElement);

		it = midiDeviceManager.getSelectedMidiWriteDevices().iterator();

		while (it.hasNext()) {

			MidiDevice midiDevice = (MidiDevice) it.next();

			MidiDeviceXML midiDeviceXML = new MidiDeviceXML(midiDevice,
					TAG_SELECTED_MIDI_WRITE_DEVICE);

			midiDeviceXML.saveXML(selectedMidiWriteDevicesElement);
		}
	}

	protected MidiDevice findDeviceHash(Vector midiDevices, int hash) {

		Iterator it = midiDevices.iterator();

		while (it.hasNext()) {

			MidiDevice midiDevice = (MidiDevice) it.next();

			if (midiDevice.getDeviceInfo().toString().hashCode() == hash) {

				return midiDevice;
			}
		}

		return null;
	}
}
