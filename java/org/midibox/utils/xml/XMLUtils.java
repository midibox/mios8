package org.midibox.utils.xml;

import java.io.File;
import java.io.FileOutputStream;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "";

	protected Document document;

	protected Vector tags;

	protected String rootElementTag;

	protected Element rootElement;

	public XMLUtils(String rootElementTag) {

		this.rootElementTag = rootElementTag;

		tags = new Vector();

		tags.add(rootElementTag);
	}

	public void loadXML(File file) {

		DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory
				.newInstance();

		try {

			DocumentBuilder documentBuilder = documentBuilderFactory
					.newDocumentBuilder();

			document = documentBuilder.parse(file);

			loadXML(document);

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void loadXML(Node node) {

		switch (node.getNodeType()) {

		case Node.ELEMENT_NODE:

			if (tags.contains(node.getNodeName())) {
				parseElement((Element) node);
			}
		}

		NodeList children = node.getChildNodes();

		for (int n = 0; n < children.getLength(); n++) {

			loadXML(children.item(n));
		}
	}

	protected void parseElement(Element element) {

	}

	public void saveXML(File file) {

		DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory
				.newInstance();

		try {

			DocumentBuilder documentBuilder = documentBuilderFactory
					.newDocumentBuilder();

			Document document = documentBuilder.newDocument();

			saveXML(document);

			TransformerFactory transformerFactory = TransformerFactory
					.newInstance();

			Transformer transformer = transformerFactory.newTransformer();

			FileOutputStream fileOutputStream = new FileOutputStream(file);

			transformer.transform(new DOMSource(document), new StreamResult(
					fileOutputStream));

			fileOutputStream.close();

		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	public void saveXML(Node node) {

		if (node instanceof Document) {

			document = (Document) node;

		} else {

			document = node.getOwnerDocument();
		}

		rootElement = document.createElement(rootElementTag);

		node.appendChild(rootElement);
	}

	protected String intToString(int i) {

		return "" + i;
	}

	protected String booleanToString(boolean b) {

		return b ? "true" : "false";
	}

	protected int stringToInt(String string) {

		return Integer.parseInt(string);
	}

	protected boolean stringToBoolean(String string) {

		return Boolean.parseBoolean(string);
	}
}
