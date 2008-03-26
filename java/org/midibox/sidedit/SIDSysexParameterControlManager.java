/*
 * @(#)SIDV2librarian.java	beta1	2008/01/21
 *
 * Copyright (C) 2008    Rutger Vlek (rutgervlek@hotmail.com)
 *
 * This application is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this application; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package org.midibox.sidedit;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.Observable;
import java.util.Observer;

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;

public class SIDSysexParameterControlManager extends Observable implements
		Observer, Receiver {

	protected LinkedList sysexParameters;	

	protected Receiver receiver;

	public SIDSysexParameterControlManager(Receiver receiver) {
		
		this.receiver = receiver;
		sysexParameters = new LinkedList();
	}

	public Receiver getReceiver() {
		return receiver;
	}

	public void setReceiver(Receiver receiver) {
		this.receiver = receiver;

		Iterator it = sysexParameters.iterator();

		while (it.hasNext()) {
			SIDSysexParameterControl sysexParameter = (SIDSysexParameterControl) it
					.next();
			sysexParameter.setReceiver(receiver);
			
		}
	}

	public LinkedList getSIDSysexParameters() {
		return sysexParameters;
	}

	public void addSIDSysexParameter(SIDSysexParameter sysexParameter) {
		sysexParameters.remove(sysexParameter);
		sysexParameters.add(sysexParameter);
		sysexParameter.deleteObserver(this);
		sysexParameter.addObserver(this);
	}

	public void removeSIDSysexParameter(SIDSysexParameter sysexParameter) {
		sysexParameter.deleteObserver(this);
		sysexParameter.addObserver(this);
	}

	public void close() {

	}

	public void send(MidiMessage message, long timestamp) {
		Object[] parametersArray = sysexParameters.toArray();

		for (int p = 0; p < parametersArray.length; p++) {
			if (parametersArray[p] != null) {
				((SIDSysexParameter) parametersArray[p]).send(message, timestamp);
			}
		}
	}

	public void update(Observable observable, Object object) {
		
	}
}
