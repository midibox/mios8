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

import java.util.Vector;
import javax.sound.midi.Receiver;
import org.midibox.sidedit.SIDSysexParameter;
import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.sidedit.SIDSysexParameterControlManager;
import org.midibox.sidlibr.Patch;

public class SIDEditController extends SIDSysexParameterControlManager {
	private Patch patch;
	private Vector engine;
	private Object tooltipListener;
	
	public SIDEditController(Patch p) {
		super(p.getReceiver());
		this.patch = p.clone();
		engine = EngineSpecs.getEngine(patch);
		
		for (int b = 0; b < engine.size(); b++) {
			Vector v = (Vector) engine.elementAt(b);
			for (int c = 0; c < v.size(); c++) {
				SIDSysexParameterControl midiParameter = (SIDSysexParameterControl) v.elementAt(c);
				addSIDSysexParameter(midiParameter);
			}
		}
	}
	
	public Vector getEngine() {
		return engine;
	}
	
	public void setTooltipListener(Object object) {
		for (int b = 0; b < engine.size(); b++) {
			Vector v = (Vector) engine.elementAt(b);
			for (int c = 0; c < v.size(); c++) {
				SIDSysexParameterControl midiParameter = (SIDSysexParameterControl) v.elementAt(c);
				midiParameter.setTooltipListener(object);
			}
		}
	}
	
	public Patch getPatch() {
		return patch;
	}

	public String getPatchName() {
		return patch.getPatchName();
	}
	
	public void setPatchName(String s) {
		patch.setPatchName(s);
	}
	
	public void setCores(boolean[] b) {
		patch.setCores(b);
	}	
	
	public void Save() {
		setChanged();
		notifyObservers("Save editor patch");	
	}
}