package org.midibox.apps.miosstudiosid.xml;

import org.midibox.apps.miosstudio.MIOSStudio;
import org.midibox.apps.miosstudio.xml.MIOSStudioXML;

public class MIOSStudioSIDXML extends MIOSStudioXML {

	public final static String TAG_ROOT_ELEMENT = "miosStudio";

	public MIOSStudioSIDXML(MIOSStudio miosStudio, String rootElementTag) {

		super(miosStudio, rootElementTag);
	}
}
