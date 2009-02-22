package org.midibox.apps.miosstudio.xml;

import org.midibox.apps.miosstudio.MIOSStudio;
import org.midibox.midi.xml.MidiDeviceManagerXML;
import org.midibox.midi.xml.MidiDeviceRoutingXML;
import org.midibox.midi.xml.MidiFilterDeviceManagerXML;
import org.midibox.midi.xml.MidiFilterDeviceXML;
import org.midibox.midi.xml.MidiKeyboardControllerDeviceXML;
import org.midibox.midi.xml.MidiMonitorFilteredDeviceXML;
import org.midibox.mios.xml.DebugFunctionDeviceXML;
import org.midibox.mios.xml.HexFileUploadDeviceManagerXML;
import org.midibox.mios.xml.LCDMessageDeviceXML;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MIOSStudioXML extends XMLUtils {

	protected MIOSStudio miosStudio;

	protected String midiThruFilterDeviceTag = "midiThruFilterDevice";

	protected String midiDeviceManagerTag = "midiDeviceManager";

	protected String midiFilterDeviceManagerTag = "midiFilterDeviceManager";

	protected String midiInPortMonitorDeviceTag = "midiInPortMonitorDevice";

	protected String midiOutPortMonitorDeviceTag = "midiOutPortMonitorDevice";

	protected String midiKeyboardControllerDeviceTag = "midiKeyboardControllerDevice";

	protected String hexFileUploadDeviceManagerTag = "hexFileUploadDeviceManager";

	protected String lcdMessageDeviceTag = "lcdMessageDevice";

	protected String debugFunctionDeviceTag = "debugFunctionDevice";

	protected String miosTerminalDeviceTag = "miosTeminalDevice";

	protected String midiDeviceRoutingTag = "midiDeviceRouting";

	protected String midiThruOutPortAttr = "midiThruOutPort";

	protected String routeIndividualDevicesAttr = "routeIndividualDevices";

	public MIOSStudioXML(MIOSStudio miosStudio, String rootElementTag) {

		super(rootElementTag);

		this.miosStudio = miosStudio;

		tags.add(midiThruFilterDeviceTag);
		tags.add(midiDeviceManagerTag);
		tags.add(midiFilterDeviceManagerTag);
		tags.add(midiOutPortMonitorDeviceTag);
		tags.add(midiInPortMonitorDeviceTag);
		tags.add(midiKeyboardControllerDeviceTag);
		tags.add(hexFileUploadDeviceManagerTag);
		tags.add(lcdMessageDeviceTag);
		tags.add(debugFunctionDeviceTag);
		tags.add(miosTerminalDeviceTag);
		tags.add(midiDeviceRoutingTag);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		if (name == rootElementTag) {

			miosStudio.setMidiThruOutPort(Boolean.parseBoolean(element
					.getAttribute(midiThruOutPortAttr)));

			miosStudio.setRouteIndividualDevices(Boolean.parseBoolean(element
					.getAttribute(routeIndividualDevicesAttr)));

		} else if (name == midiThruFilterDeviceTag) {

			MidiFilterDeviceXML midiFilterDeviceXML = new MidiFilterDeviceXML(
					miosStudio.getMidiThruFilterDevice(),
					midiThruFilterDeviceTag);

			midiFilterDeviceXML.loadXML(element);

		} else if (name == midiDeviceManagerTag) {

			MidiDeviceManagerXML midiDeviceManagerXML = new MidiDeviceManagerXML(
					miosStudio.getMidiDeviceManager(), midiDeviceManagerTag);

			midiDeviceManagerXML.loadXML(element);

		} else if (name == midiFilterDeviceManagerTag) {

			MidiFilterDeviceManagerXML midiFilterDeviceManagerXML = new MidiFilterDeviceManagerXML(
					miosStudio.getMidiFilterManager(),
					midiFilterDeviceManagerTag);
			
			midiFilterDeviceManagerXML.loadXML(element);
			
		} else if (name == midiOutPortMonitorDeviceTag) { 
			
			MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(miosStudio.getMidiOutPortMonitorDevice(), midiOutPortMonitorDeviceTag);
			
			midiMonitorFilteredDeviceXML.loadXML(element);
			
		} else if (name == midiInPortMonitorDeviceTag) { 
			
			MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(miosStudio.getMidiInPortMonitorDevice(), midiInPortMonitorDeviceTag);
			
			midiMonitorFilteredDeviceXML.loadXML(element);
			
		} else if (name == midiKeyboardControllerDeviceTag) {
			
			// TODO
			
		} else if (name == hexFileUploadDeviceManagerTag) { 
			
			// TODO
			
		} else if (name == lcdMessageDeviceTag) {
			
			// TODO
			
		} else if (name == debugFunctionDeviceTag) { 
			
			// TODO
			
		} else if (name == miosTerminalDeviceTag) { 
			
			MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(miosStudio.getMIOSTerminalDevice(), miosTerminalDeviceTag);
			
			midiMonitorFilteredDeviceXML.loadXML(element);
			
		} else if (name == midiDeviceRoutingTag) { 
		
			boolean routeIndividualDevices = miosStudio.isRouteIndividualDevices();			

			miosStudio.setRouteIndividualDevices(true);
			
			MidiDeviceRoutingXML midiDeviceRoutingXML = new MidiDeviceRoutingXML(miosStudio.getMidiDeviceRouting(), midiDeviceRoutingTag);
			
			midiDeviceRoutingXML.loadXML(element);
			
			miosStudio.setRouteIndividualDevices(routeIndividualDevices);
		}		
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		rootElement.setAttribute(routeIndividualDevicesAttr, miosStudio
				.isRouteIndividualDevices() ? "true" : "false");

		rootElement.setAttribute(midiThruOutPortAttr, miosStudio
				.isMidiThruOutPort() ? "true" : "false");

		MidiFilterDeviceXML midiFilterDeviceXML = new MidiFilterDeviceXML(
				miosStudio.getMidiThruFilterDevice(), midiThruFilterDeviceTag);

		midiFilterDeviceXML.saveXML(rootElement);

		saveMIDIXML(miosStudio);

		saveMIOSXML(miosStudio);

		MidiDeviceRoutingXML midiDeviceRoutingXML = new MidiDeviceRoutingXML(
				miosStudio.getMidiDeviceRouting(), midiDeviceRoutingTag);

		midiDeviceRoutingXML.saveXML(rootElement);
	}

	protected void saveMIDIXML(MIOSStudio miosStudio) {

		MidiDeviceManagerXML midiDeviceManagerXML = new MidiDeviceManagerXML(
				miosStudio.getMidiDeviceManager(), midiDeviceManagerTag);

		midiDeviceManagerXML.saveXML(rootElement);

		MidiFilterDeviceManagerXML midiFilterDeviceManagerXML = new MidiFilterDeviceManagerXML(
				miosStudio.getMidiFilterManager(), midiFilterDeviceManagerTag);

		midiFilterDeviceManagerXML.saveXML(rootElement);

		MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
				miosStudio.getMidiOutPortMonitorDevice(),
				midiOutPortMonitorDeviceTag);

		midiMonitorFilteredDeviceXML.saveXML(rootElement);

		midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
				miosStudio.getMidiInPortMonitorDevice(),
				midiInPortMonitorDeviceTag);

		midiMonitorFilteredDeviceXML.saveXML(rootElement);

		MidiKeyboardControllerDeviceXML midiControllerDeviceXML = new MidiKeyboardControllerDeviceXML(
				miosStudio.getMidiKeyboardControllerDevice(),
				midiKeyboardControllerDeviceTag);

		midiControllerDeviceXML.saveXML(rootElement);
	}

	protected void saveMIOSXML(MIOSStudio miosStudio) {

		HexFileUploadDeviceManagerXML hexFileUploadDeviceManagerXML = new HexFileUploadDeviceManagerXML(
				miosStudio.getHexFileUploadDeviceManager(),
				hexFileUploadDeviceManagerTag);

		hexFileUploadDeviceManagerXML.saveXML(rootElement);

		LCDMessageDeviceXML lcdMessageDeviceXML = new LCDMessageDeviceXML(
				miosStudio.getLcdMessageDevice(), lcdMessageDeviceTag);

		lcdMessageDeviceXML.saveXML(rootElement);

		DebugFunctionDeviceXML debugFunctionDeviceXML = new DebugFunctionDeviceXML(
				miosStudio.getDebugFunctionDevice(), debugFunctionDeviceTag);

		debugFunctionDeviceXML.saveXML(rootElement);

		MidiMonitorFilteredDeviceXML midiMonitorFilteredDeviceXML = new MidiMonitorFilteredDeviceXML(
				miosStudio.getMIOSTerminalDevice(), miosTerminalDeviceTag);

		midiMonitorFilteredDeviceXML.saveXML(rootElement);
	}
}
