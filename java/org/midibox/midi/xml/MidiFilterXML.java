package org.midibox.midi.xml;

import java.io.File;
import java.io.FileOutputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.midibox.midi.MidiFilter;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class MidiFilterXML {

	public static void loadMidiFilter(MidiFilter midiFilter, File file) {

		DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory
				.newInstance();

		try {

			DocumentBuilder documentBuilder = documentBuilderFactory
					.newDocumentBuilder();

			Document doc = documentBuilder.parse(file);

			loadMidiFilter(midiFilter, doc);

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void loadMidiFilter(MidiFilter midiFilter, Node node) {

		NodeList children = node.getChildNodes();

		for (int n = 0; n < children.getLength(); n++) {

			loadMidiFilter(midiFilter, children.item(n));
		}

		switch (node.getNodeType()) {

		case Node.ELEMENT_NODE:
			parseElement(midiFilter, (Element) node);
		}
	}

	protected static void parseElement(MidiFilter midiFilter, Element element) {

		String name = element.getNodeName();

		NamedNodeMap attributes = element.getAttributes();

		if (name == "channels") {

		} else if (name == "channel") {

			Node number = attributes.getNamedItem("number");

			Node enable = attributes.getNamedItem("enable");

			if (number != null && enable != null) {

				int n = Integer.parseInt(number.getTextContent());

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setChannel(n, e);
			}

		} else if (name == "voiceMessages") {
			
			Node enable = attributes.getNamedItem("enable");
			
			if (enable != null) {

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setVoiceMessages(e);
			}

		} else if (name == "voiceMessage") {
			
			Node number = attributes.getNamedItem("number");

			Node enable = attributes.getNamedItem("enable");

			if (number != null && enable != null) {

				int n = Integer.parseInt(number.getTextContent());

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setVoiceMessage(n, e);
			}

		} else if (name == "systemCommonMessages") {
			
			Node enable = attributes.getNamedItem("enable");
			
			if (enable != null) {

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setSystemCommonMessages(e);
			}

		} else if (name == "systemCommonMessage") {
			
			Node number = attributes.getNamedItem("number");

			Node enable = attributes.getNamedItem("enable");

			if (number != null && enable != null) {

				int n = Integer.parseInt(number.getTextContent());

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setSystemCommonMessage(n, e);
			}

		} else if (name == "systemRealtimeMessages") {

			Node enable = attributes.getNamedItem("enable");
			
			if (enable != null) {

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setSystemRealtimeMessages(e);
			}
			
		} else if (name == "systemRealtimeMessage") {
			
			Node number = attributes.getNamedItem("number");

			Node enable = attributes.getNamedItem("enable");

			if (number != null && enable != null) {

				int n = Integer.parseInt(number.getTextContent());

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setSystemRealtimeMessage(n, e);
			}

		} else if (name == "sysexMessages") {
			
			Node enable = attributes.getNamedItem("enable");

			if (enable != null) {

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setSysexMessages(e);
			}

		} else if (name == "metaMessages") {

			Node enable = attributes.getNamedItem("enable");
			
			if (enable != null) {

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setMetaMessages(e);
			}
			
		} else if (name == "controlChangeMessages") {

		} else if (name == "controlChangeMessage") {

			Node number = attributes.getNamedItem("number");

			Node enable = attributes.getNamedItem("enable");

			if (number != null && enable != null) {

				int n = Integer.parseInt(number.getTextContent());

				boolean e = Boolean.parseBoolean(enable.getTextContent());

				midiFilter.setControlChange(n, e);
			}
		}
	}

	public static void saveMidiFilter(MidiFilter midiFilter, File file) {

		DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory
				.newInstance();

		try {

			DocumentBuilder documentBuilder = documentBuilderFactory
					.newDocumentBuilder();

			Document document = documentBuilder.newDocument();

			saveMidiFilter(midiFilter, document);

			TransformerFactory transformerFactory = TransformerFactory
					.newInstance();

			Transformer transformer = transformerFactory.newTransformer();

			transformer.transform(new DOMSource(document), new StreamResult(
					new FileOutputStream(file)));

		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	public static void saveMidiFilter(MidiFilter midiFilter, Node node) {

		Document document;

		if (node instanceof Document) {

			document = (Document) node;

		} else {
			document = node.getOwnerDocument();
		}

		Node midiFilterElement = document.createElement("midiFilter");

		node.appendChild(midiFilterElement);
		
		
		
		Element voiceMessagesElement = document.createElement("voiceMessages");

		midiFilterElement.appendChild(voiceMessagesElement);
		
		voiceMessagesElement.setAttribute("enable", midiFilter.isVoiceMessages() ? "true" : "false");

		for (int c = 0; c < midiFilter.getVoiceMessages().length; c++) {

			Element voiceMessage = document.createElement("voiceMessage");

			voiceMessagesElement.appendChild(voiceMessage);

			voiceMessage.setAttribute("number", "" + ((c | 8) << 4) );

			voiceMessage.setAttribute("enable", (midiFilter.getVoiceMessages()[c]) ? "true"
					: "false");
		}

		Node controlChangeMessagesElement = document
		.createElement("controlChangeMessages");

		voiceMessagesElement.appendChild(controlChangeMessagesElement);
		
		for (int cc = 0; cc < midiFilter.getControlChangeMessages().length; cc++) {
		
			Element controlChangeMessage = document
					.createElement("controlChangeMessage");
		
			controlChangeMessagesElement.appendChild(controlChangeMessage);
		
			controlChangeMessage.setAttribute("number", "" + (cc));
		
			controlChangeMessage.setAttribute("enable",
					(midiFilter.getControlChangeMessages()[cc]) ? "true" : "false");
		}
		
		
		Node channelsElement = document.createElement("channels");

		voiceMessagesElement.appendChild(channelsElement);

		for (int c = 0; c < midiFilter.getChannels().length; c++) {

			Element channel = document.createElement("channel");

			channelsElement.appendChild(channel);

			channel.setAttribute("number", "" + (c + 1));

			channel.setAttribute("enable", (midiFilter.getChannels()[c]) ? "true"
					: "false");
		}		
		
		Element systemCommonMessagesElement = document.createElement("systemCommonMessages");

		midiFilterElement.appendChild(systemCommonMessagesElement);
		
		systemCommonMessagesElement.setAttribute("enable", midiFilter.isSystemCommonMessages() ? "true" : "false");

		for (int c = 0; c < midiFilter.getSystemCommonMessages().length; c++) {

			Element systemCommonMessage = document.createElement("systemCommonMessage");

			systemCommonMessagesElement.appendChild(systemCommonMessage);

			systemCommonMessage.setAttribute("number", "" + (c | 0xF0));

			systemCommonMessage.setAttribute("enable", (midiFilter.getSystemCommonMessages()[c]) ? "true"
					: "false");
		}
		
		
		
		Element systemRealtimeMessagesElement = document.createElement("systemRealtimeMessages");

		midiFilterElement.appendChild(systemRealtimeMessagesElement);		

		systemRealtimeMessagesElement.setAttribute("enable", midiFilter.isSystemRealtimeMessages() ? "true" : "false");

		for (int c = 0; c < midiFilter.getSystemRealtimeMessages().length; c++) {

			Element systemRealtimeMessage = document.createElement("systemRealtimeMessage");

			systemRealtimeMessagesElement.appendChild(systemRealtimeMessage);

			systemRealtimeMessage.setAttribute("number", "" + ((c + 8 | 0xF0)));

			systemRealtimeMessage.setAttribute("enable", (midiFilter.getSystemRealtimeMessages()[c]) ? "true"
					: "false");
		}
		
		
		
		Element sysexMessagesElement = document.createElement("sysexMessages");

		midiFilterElement.appendChild(sysexMessagesElement);		

		sysexMessagesElement.setAttribute("enable", midiFilter.isSysexMessages() ? "true" : "false");
		
		
		
		Element metaMessagesElement = document.createElement("metaMessages");

		midiFilterElement.appendChild(metaMessagesElement);		

		metaMessagesElement.setAttribute("enable", midiFilter.isMetaMessages() ? "true" : "false");
	}
}
