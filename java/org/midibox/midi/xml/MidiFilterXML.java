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

		} else if (name == "voiceMessage") {

		} else if (name == "systemCommonMessages") {

		} else if (name == "systemCommonMessage") {

		} else if (name == "systemRealtimeMessages") {

		} else if (name == "systemRealtimeMessage") {

		} else if (name == "sysexMessages") {

		} else if (name == "metaMessages") {

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

		Node channelsElement = document.createElement("channels");

		midiFilterElement.appendChild(channelsElement);

		for (int c = 0; c < midiFilter.getChannel().length; c++) {

			Element channel = document.createElement("channel");

			channelsElement.appendChild(channel);

			channel.setAttribute("number", "" + (c + 1));

			channel.setAttribute("enable", (midiFilter.getChannel()[c]) ? "true"
					: "false");
		}

		Node controlChangeMessagesElement = document
				.createElement("controlChangeMessages");

		midiFilterElement.appendChild(controlChangeMessagesElement);

		for (int cc = 0; cc < midiFilter.getControlChangeMessages().length; cc++) {

			Element controlChangeMessage = document
					.createElement("controlChangeMessage");

			controlChangeMessagesElement.appendChild(controlChangeMessage);

			controlChangeMessage.setAttribute("number", "" + (cc));

			controlChangeMessage.setAttribute("enable",
					(midiFilter.getControlChangeMessages()[cc]) ? "true" : "false");
		}
	}
}
