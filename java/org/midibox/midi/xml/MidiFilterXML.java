package org.midibox.midi.xml;

import org.midibox.midi.MidiFilter;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

public class MidiFilterXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "midiFilter";

	public final static String TAG_VOICE_MESSAGES = "voiceMessages";

	public final static String TAG_VOICE_MESSAGE = "voiceMessage";

	public final static String TAG_CHANNELS = "channels";

	public final static String TAG_CHANNEL = "channel";

	public final static String TAG_CONTROL_CHANGE_MESSAGES = "controlChangeMessages";

	public final static String TAG_CONTROL_CHANGE_MESSAGE = "controlChangeMessage";

	public final static String TAG_SYSTEM_COMMON_MESSAGES = "systemCommonMessages";

	public final static String TAG_SYSTEM_COMMON_MESSAGE = "systemCommonMessage";

	public final static String TAG_SYSTEM_REALTIME_MESSAGES = "systemRealtimeMessages";

	public final static String TAG_SYSTEM_REALTIME_MESSAGE = "systemRealtimeMessage";

	public final static String TAG_SYSEX_MESSAGES = "sysexMessages";

	public final static String TAG_META_MESSAGES = "metaMessage";

	public final static String ATTR_NUMBER = "number";

	public final static String ATTR_ENABLE = "enable";

	protected MidiFilter midiFilter;

	public MidiFilterXML(MidiFilter midiFilter, String rootElementTag) {

		super(rootElementTag);

		this.midiFilter = midiFilter;

		addTag(TAG_VOICE_MESSAGES);
		addTag(TAG_VOICE_MESSAGE);
		addTag(TAG_CHANNELS);
		addTag(TAG_CHANNEL);
		addTag(TAG_CONTROL_CHANGE_MESSAGES);
		addTag(TAG_CONTROL_CHANGE_MESSAGE);
		addTag(TAG_SYSTEM_COMMON_MESSAGES);
		addTag(TAG_SYSTEM_COMMON_MESSAGE);
		addTag(TAG_SYSTEM_REALTIME_MESSAGES);
		addTag(TAG_SYSTEM_REALTIME_MESSAGE);
		addTag(TAG_SYSEX_MESSAGES);
		addTag(TAG_META_MESSAGES);
	}

	protected void parseElement(Element element) {

		super.parseElement(element);

		String name = element.getNodeName();

		String numberString = element.getAttribute(ATTR_NUMBER);

		String enableString = element.getAttribute(ATTR_ENABLE);

		int n = 0;

		boolean e = false;

		if (numberString != "") {

			n = stringToInt(numberString);
		}

		if (enableString != "") {

			e = stringToBoolean(enableString);
		}

		if (name == TAG_CHANNELS) {

		} else if (name == TAG_CHANNEL) {

			midiFilter.setChannel(n, e);

		} else if (name == TAG_VOICE_MESSAGES) {

			midiFilter.setVoiceMessages(e);

		} else if (name == TAG_VOICE_MESSAGE) {

			midiFilter.setVoiceMessage(n, e);

		} else if (name == TAG_SYSTEM_COMMON_MESSAGES) {

			midiFilter.setSystemCommonMessages(e);

		} else if (name == TAG_SYSTEM_COMMON_MESSAGE) {

			midiFilter.setSystemCommonMessage(n, e);

		} else if (name == TAG_SYSTEM_REALTIME_MESSAGES) {

			midiFilter.setSystemRealtimeMessages(e);

		} else if (name == TAG_SYSTEM_REALTIME_MESSAGE) {

			midiFilter.setSystemRealtimeMessage(n, e);

		} else if (name == TAG_SYSEX_MESSAGES) {

			midiFilter.setSysexMessages(e);

		} else if (name == TAG_META_MESSAGES) {

			midiFilter.setMetaMessages(e);

		} else if (name == TAG_CONTROL_CHANGE_MESSAGES) {

		} else if (name == TAG_CONTROL_CHANGE_MESSAGE) {

			midiFilter.setControlChange(n, e);
		}
	}

	public void saveXML(Node node) {

		super.saveXML(node);

		Element voiceMessagesElement = document
				.createElement(TAG_VOICE_MESSAGES);

		rootElement.appendChild(voiceMessagesElement);

		voiceMessagesElement.setAttribute(ATTR_ENABLE,
				booleanToString(midiFilter.isVoiceMessages()));

		for (int c = 0; c < midiFilter.getVoiceMessages().length; c++) {

			Element voiceMessage = document.createElement(TAG_VOICE_MESSAGE);

			voiceMessagesElement.appendChild(voiceMessage);

			voiceMessage.setAttribute(ATTR_NUMBER, intToString((c | 8) << 4));

			voiceMessage.setAttribute(ATTR_ENABLE, booleanToString(midiFilter
					.getVoiceMessages()[c]));
		}

		Element controlChangeMessagesElement = document
				.createElement(TAG_CONTROL_CHANGE_MESSAGES);

		voiceMessagesElement.appendChild(controlChangeMessagesElement);

		for (int cc = 0; cc < midiFilter.getControlChangeMessages().length; cc++) {

			Element controlChangeMessage = document
					.createElement(TAG_CONTROL_CHANGE_MESSAGE);

			controlChangeMessagesElement.appendChild(controlChangeMessage);

			controlChangeMessage.setAttribute(ATTR_NUMBER, intToString(cc));

			controlChangeMessage.setAttribute(ATTR_ENABLE,
					booleanToString(midiFilter.getControlChangeMessages()[cc]));
		}

		Element channelsElement = document.createElement(TAG_CHANNELS);

		voiceMessagesElement.appendChild(channelsElement);

		for (int c = 0; c < midiFilter.getChannels().length; c++) {

			Element channel = document.createElement(TAG_CHANNEL);

			channelsElement.appendChild(channel);

			channel.setAttribute(ATTR_NUMBER, intToString(c));

			channel.setAttribute(ATTR_ENABLE, booleanToString(midiFilter
					.getChannels()[c]));
		}

		Element systemCommonMessagesElement = document
				.createElement(TAG_SYSTEM_COMMON_MESSAGES);

		rootElement.appendChild(systemCommonMessagesElement);

		systemCommonMessagesElement.setAttribute(ATTR_ENABLE,
				booleanToString(midiFilter.isSystemCommonMessages()));

		for (int c = 0; c < midiFilter.getSystemCommonMessages().length; c++) {

			Element systemCommonMessage = document
					.createElement(TAG_SYSTEM_COMMON_MESSAGE);

			systemCommonMessagesElement.appendChild(systemCommonMessage);

			systemCommonMessage
					.setAttribute(ATTR_NUMBER, intToString(c | 0xF0));

			systemCommonMessage.setAttribute(ATTR_ENABLE,
					booleanToString(midiFilter.getSystemCommonMessages()[c]));
		}

		Element systemRealtimeMessagesElement = document
				.createElement(TAG_SYSTEM_REALTIME_MESSAGES);

		rootElement.appendChild(systemRealtimeMessagesElement);

		systemRealtimeMessagesElement.setAttribute(ATTR_ENABLE,
				booleanToString(midiFilter.isSystemRealtimeMessages()));

		for (int c = 0; c < midiFilter.getSystemRealtimeMessages().length; c++) {

			Element systemRealtimeMessage = document
					.createElement(TAG_SYSTEM_REALTIME_MESSAGE);

			systemRealtimeMessagesElement.appendChild(systemRealtimeMessage);

			systemRealtimeMessage.setAttribute(ATTR_NUMBER, ""
					+ ((c + 8 | 0xF0)));

			systemRealtimeMessage.setAttribute(ATTR_ENABLE,
					booleanToString(midiFilter.getSystemRealtimeMessages()[c]));
		}

		Element sysexMessagesElement = document
				.createElement(TAG_SYSEX_MESSAGES);

		rootElement.appendChild(sysexMessagesElement);

		sysexMessagesElement.setAttribute(ATTR_ENABLE,
				booleanToString(midiFilter.isSysexMessages()));

		Element metaMessagesElement = document.createElement(TAG_META_MESSAGES);

		rootElement.appendChild(metaMessagesElement);

		metaMessagesElement.setAttribute(ATTR_ENABLE,
				booleanToString(midiFilter.isMetaMessages()));
	}
}
