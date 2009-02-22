package org.midibox.midi.xml;

import org.midibox.midi.MidiFilter;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiFilterXML extends XMLUtils {

	protected MidiFilter midiFilter;
	
	protected String voiceMessagesTag = "voiceMessages";

	protected String voiceMessageTag = "voiceMessage";

	protected String channelsTag = "channels";

	protected String channelTag = "channel";

	protected String controlChangeMessagesTag = "controlChangeMessages";

	protected String controlChangeMessageTag = "controlChangeMessage";

	protected String systemCommonMessagesTag = "systemCommonMessages";

	protected String systemCommonMessageTag = "systemCommonMessage";

	protected String systemRealtimeMessagesTag = "systemRealtimeMessages";

	protected String systemRealtimeMessageTag = "systemRealtimeMessage";

	protected String sysexMessagesTag = "sysexMessages";

	protected String metaMessagesTag = "metaMessage";

	protected String numberAttr = "number";

	protected String enableAttr = "enable";

	public MidiFilterXML(MidiFilter midiFilter, String rootElementTag) {

		super(rootElementTag);

		this.midiFilter = midiFilter;
		
		tags.add(voiceMessagesTag);
		tags.add(voiceMessageTag);
		tags.add(channelsTag);
		tags.add(channelTag);
		tags.add(controlChangeMessagesTag);
		tags.add(controlChangeMessageTag);
		tags.add(systemCommonMessagesTag);
		tags.add(systemCommonMessageTag);
		tags.add(systemRealtimeMessagesTag);
		tags.add(systemRealtimeMessageTag);
		tags.add(sysexMessagesTag);
		tags.add(metaMessagesTag);
	}

	protected void parseElement(Element element) {
		
		super.parseElement(element);

		String name = element.getNodeName();

		String numberString = element.getAttribute(numberAttr);

		String enableString = element.getAttribute(enableAttr);

		int n = 0;

		boolean e = false;

		if (numberString != "") {

			n = Integer.parseInt(numberString);
		}

		if (enableString != "") {

			e = Boolean.parseBoolean(enableString);
		}

		if (name == channelsTag) {

		} else if (name == channelTag) {

			midiFilter.setChannel(n, e);

		} else if (name == voiceMessagesTag) {

			midiFilter.setVoiceMessages(e);

		} else if (name == voiceMessageTag) {

			midiFilter.setVoiceMessage(n, e);

		} else if (name == systemCommonMessagesTag) {

			midiFilter.setSystemCommonMessages(e);

		} else if (name == systemCommonMessageTag) {

			midiFilter.setSystemCommonMessage(n, e);

		} else if (name == systemRealtimeMessagesTag) {

			midiFilter.setSystemRealtimeMessages(e);

		} else if (name == systemRealtimeMessageTag) {

			midiFilter.setSystemRealtimeMessage(n, e);

		} else if (name == sysexMessagesTag) {

			midiFilter.setSysexMessages(e);

		} else if (name == metaMessagesTag) {

			midiFilter.setMetaMessages(e);

		} else if (name == controlChangeMessagesTag) {

		} else if (name == controlChangeMessageTag) {

			midiFilter.setControlChange(n, e);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element voiceMessagesElement = document.createElement(voiceMessagesTag);

		rootElement.appendChild(voiceMessagesElement);

		voiceMessagesElement.setAttribute(enableAttr, midiFilter
				.isVoiceMessages() ? "true" : "false");

		for (int c = 0; c < midiFilter.getVoiceMessages().length; c++) {

			Element voiceMessage = document.createElement(voiceMessageTag);

			voiceMessagesElement.appendChild(voiceMessage);

			voiceMessage.setAttribute(numberAttr, "" + ((c | 8) << 4));

			voiceMessage.setAttribute(enableAttr, (midiFilter
					.getVoiceMessages()[c]) ? "true" : "false");
		}

		Element controlChangeMessagesElement = document
				.createElement(controlChangeMessagesTag);

		voiceMessagesElement.appendChild(controlChangeMessagesElement);

		for (int cc = 0; cc < midiFilter.getControlChangeMessages().length; cc++) {

			Element controlChangeMessage = document
					.createElement(controlChangeMessageTag);

			controlChangeMessagesElement.appendChild(controlChangeMessage);

			controlChangeMessage.setAttribute(numberAttr, "" + (cc));

			controlChangeMessage.setAttribute(enableAttr, (midiFilter
					.getControlChangeMessages()[cc]) ? "true" : "false");
		}

		Element channelsElement = document.createElement(channelsTag);

		voiceMessagesElement.appendChild(channelsElement);

		for (int c = 0; c < midiFilter.getChannels().length; c++) {

			Element channel = document.createElement(channelTag);

			channelsElement.appendChild(channel);

			channel.setAttribute(numberAttr, "" + (c + 1));

			channel.setAttribute(enableAttr,
					(midiFilter.getChannels()[c]) ? "true" : "false");
		}

		Element systemCommonMessagesElement = document
				.createElement(systemCommonMessagesTag);

		rootElement.appendChild(systemCommonMessagesElement);

		systemCommonMessagesElement.setAttribute(enableAttr, midiFilter
				.isSystemCommonMessages() ? "true" : "false");

		for (int c = 0; c < midiFilter.getSystemCommonMessages().length; c++) {

			Element systemCommonMessage = document
					.createElement(systemCommonMessageTag);

			systemCommonMessagesElement.appendChild(systemCommonMessage);

			systemCommonMessage.setAttribute(numberAttr, "" + (c | 0xF0));

			systemCommonMessage.setAttribute(enableAttr, (midiFilter
					.getSystemCommonMessages()[c]) ? "true" : "false");
		}

		Element systemRealtimeMessagesElement = document
				.createElement(systemRealtimeMessagesTag);

		rootElement.appendChild(systemRealtimeMessagesElement);

		systemRealtimeMessagesElement.setAttribute(enableAttr, midiFilter
				.isSystemRealtimeMessages() ? "true" : "false");

		for (int c = 0; c < midiFilter.getSystemRealtimeMessages().length; c++) {

			Element systemRealtimeMessage = document
					.createElement(systemRealtimeMessageTag);

			systemRealtimeMessagesElement.appendChild(systemRealtimeMessage);

			systemRealtimeMessage.setAttribute(numberAttr, ""
					+ ((c + 8 | 0xF0)));

			systemRealtimeMessage.setAttribute(enableAttr, (midiFilter
					.getSystemRealtimeMessages()[c]) ? "true" : "false");
		}

		Element sysexMessagesElement = document.createElement(sysexMessagesTag);

		rootElement.appendChild(sysexMessagesElement);

		sysexMessagesElement.setAttribute(enableAttr, midiFilter
				.isSysexMessages() ? "true" : "false");

		Element metaMessagesElement = document.createElement(metaMessagesTag);

		rootElement.appendChild(metaMessagesElement);

		metaMessagesElement.setAttribute(enableAttr, midiFilter
				.isMetaMessages() ? "true" : "false");
	}
}
