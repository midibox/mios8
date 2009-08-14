package org.midibox.midi.xml;

import org.midibox.midi.SysexSendReceive;
import org.midibox.utils.xml.XMLUtils;
import org.w3c.dom.Node;

public class SysexSendReceiveXML extends XMLUtils {

	public final static String TAG_ROOT_ELEMENT = "sysexSendReceive";

	protected SysexSendReceive sysexSendReceive;

	public SysexSendReceiveXML(SysexSendReceive sysexSendReceive,
			String rootElementTag) {

		super(rootElementTag);

		this.sysexSendReceive = sysexSendReceive;
	}

	public void saveXML(Node node) {

		super.saveXML(node);
	}
}
