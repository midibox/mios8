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

package org.midibox.sidlibr;

import java.util.Observable;
import java.util.Observer;

import org.midibox.midi.MidiUtils;
import javax.sound.midi.Receiver;

import org.midibox.sidlibr.Patch;
import org.midibox.sidedit.SIDSysexInfo;
import javax.swing.*;
import java.awt.event.*;

import javax.sound.midi.MidiMessage;
import javax.sound.midi.SysexMessage;
import java.io.IOException;
public class SIDLibController extends Observable implements Observer, ActionListener  {
	private Object copyPasteBuffer;
	private FileHandler fileHandler = new FileHandler();	
	
	private int currentPatchNumber = 0;
	private int currentBankNumber = 0;
	private int[] requestBankIndices;
	private Bank[] patchBanks = new Bank[7];
	private EnsBank ensBank;
	
	private Boolean openEditor = false;
	private int coresHardware = 0;
	private int coresSelected = 1;
		
	private SysExController sysexController;
	
	public SIDLibController(SysExController sysexController) {
		this.sysexController = sysexController;
		sysexController.addObserver(this);
		//ensBank = new EnsBank(sysexController.getReceiver());
		for (int i=0; i<patchBanks.length;i++) {
			patchBanks[i] = new Bank(sysexController.getReceiver());
		}
		requestBankIndices = new int[128];
		for(int i=0;i<requestBankIndices.length;i++) {requestBankIndices[i] = i;}
	}
	
	public void scanHardware(){
		sysexController.scanHardware();
	}
	
	public void setCores(int i) {
		coresSelected = i; 
	}
	
	public int getCores() {
		return coresSelected&coresHardware;
	}
	
	public void editCurrentPatch() {	
		if (coresHardware==0 | coresSelected==0) {
			// Open directly (offline mode)
			setChanged();
			notifyObservers("Edit");
			clearChanged();
		} else {
			if (sysexController.isDone()) {
				// Sends notification to SIDV2librarianGUI when done...
				openEditor = true;
				sysexController.dumpPatchToBuffer(patchBanks[currentBankNumber].getPatchAt(currentPatchNumber), getCores());
			}
		}
	}
	
	public Patch getCurrentPatch() {
		return patchBanks[currentBankNumber].getPatchAt(currentPatchNumber);
	}
	
	public Bank getBank(int bankNumber) {
		return patchBanks[bankNumber];
	}
	
	public EnsBank getEnsBank() {
		return ensBank;
	}
	
	public void setCurrentBankNumber(int i) {
		currentBankNumber = i;
	}
	
	public int getCurrentBankNumber() {
		return currentBankNumber;
	}
	
	public void setCurrentPatchNumber(int i) {
		currentPatchNumber = i;
	}
	
	public int getCurrentPatchNumber() {
		return currentPatchNumber;
	}
	
	public void setPatchAt(Patch p, int patchNumber, int bankNumber) {
		patchBanks[bankNumber].setPatchAt(patchNumber,p);			
	}

	// ****************** Edit functions ***********************
	public void editCopy() {
		copyPasteBuffer = patchBanks[currentBankNumber].getPatchAt(currentPatchNumber);		
	}
	
	public void editPaste() {
		if ((copyPasteBuffer.getClass()==Patch.class) && (patchBanks[currentBankNumber].getPatchAt(currentPatchNumber).getClass()==Patch.class)) {
			patchBanks[currentBankNumber].setPatchAt(currentPatchNumber,((Patch)copyPasteBuffer).clone());
		}		
		setChanged();
		notifyObservers("Data changed");
		clearChanged();
	}
	
	public void editSwap(int a, int b) {
		 Patch temp_a = patchBanks[currentBankNumber].getPatchAt(a);
		 Patch temp_b = patchBanks[currentBankNumber].getPatchAt(b);
		 patchBanks[currentBankNumber].setPatchAt(a,temp_b);
		 patchBanks[currentBankNumber].setPatchAt(b,temp_a);
		 setChanged();
		 notifyObservers("Data changed");
		 clearChanged();
	}
	
	public void editCut() {
		copyPasteBuffer = patchBanks[currentBankNumber].getPatchAt(currentPatchNumber);
		patchBanks[currentBankNumber].setPatchAt(currentPatchNumber, new Patch(sysexController.getReceiver()));
		setChanged();
		notifyObservers("Data changed");
		clearChanged();
	}
	
	public void editClear() {
		patchBanks[currentBankNumber].setPatchAt(currentPatchNumber, new Patch(sysexController.getReceiver()));
		setChanged();
		notifyObservers("Data changed");
		clearChanged();
	}
	
	public void editRename() {
		setChanged();
		notifyObservers("Rename");
		clearChanged();
	}
	
	public void initCurrentBank() {
		patchBanks[currentBankNumber] = new Bank(sysexController.getReceiver());
		setChanged();
		notifyObservers("Data changed");
		clearChanged();
	}
	
	public void initCurrentPatch(Object object) {
		patchBanks[currentBankNumber].setPatchAt(currentPatchNumber, new Patch(sysexController.getReceiver()));
		patchBanks[currentBankNumber].getPatchAt(currentPatchNumber).setEngine(object);
		setChanged();
		notifyObservers("Data changed");	
		clearChanged();
	}
	
	// ****************** Event handling functions ***********************
	public void update(Observable observable, Object object) {
		if (object=="Hardware scan") {
			coresHardware = (Integer)sysexController.pickMeUp;
		} else if (object=="Patch ready") {
			patchBanks[sysexController.requestBank].setPatchAt(sysexController.requestPatch[sysexController.requestCount], (Patch)sysexController.pickMeUp);
			setChanged();
			notifyObservers("Data changed");
			clearChanged();
		} else if (object=="Ensemble ready") {
			//ensBanks[currentBankNumber].setEnsAt(currentPatchNumber, (Ensemble)sysexController.pickMeUp);
			setChanged();
			notifyObservers("Data changed");
			clearChanged();			
		} else if ((object=="Dump completed")&&(openEditor)) {
			openEditor = false;
			setChanged();
			notifyObservers("Edit");
			clearChanged();			
		}
	}
	
	public void actionPerformed(ActionEvent ae) {
		if (ae.getActionCommand().equals("Load bank")) {
			Bank tempBank = fileHandler.loadPatchBank(sysexController.getReceiver());
			if (tempBank!=null) {
				patchBanks[currentBankNumber] = tempBank;
				setChanged();
				notifyObservers("Data changed");
				clearChanged();
			}
		} else if (ae.getActionCommand().equals("Save bank")) {
			fileHandler.savePatchBank(patchBanks[currentBankNumber], currentBankNumber);			
		} else if (ae.getActionCommand().equals("Load patch")) {
			Patch tempPatch = fileHandler.loadPatch(sysexController.getReceiver());
			if (tempPatch!=null) {
				patchBanks[currentBankNumber].setPatchAt(currentPatchNumber, tempPatch);
				setChanged();
				notifyObservers("Data changed");
				clearChanged();
			}
		} else if (ae.getActionCommand().equals("Save patch")) {
			fileHandler.savePatch(patchBanks[currentBankNumber].getPatchAt(currentPatchNumber), currentBankNumber, currentPatchNumber);			
		} else if (ae.getActionCommand().equals("Edit")) {
			editCurrentPatch();
		} else if (ae.getActionCommand().equals("Rename")) {
			editRename();
		} else if (ae.getActionCommand().equals("Cut")) {
			editCut();
		} else if (ae.getActionCommand().equals("Copy")) {
			editCopy();
		} else if (ae.getActionCommand().equals("Paste")) {
			editPaste();
		} else if (ae.getActionCommand().equals("Clear")) {
			editClear();
		} else if (ae.getActionCommand().equals("Init LEAD patch")) {
			initCurrentPatch(Patch.LEAD);
		} else if (ae.getActionCommand().equals("Init BASSLINE patch")) {
			initCurrentPatch(Patch.BASSLINE);
		} else if (ae.getActionCommand().equals("Init DRUM patch")) {
			initCurrentPatch(Patch.DRUM);
		} else if (ae.getActionCommand().equals("Init MULTI patch")) {
			initCurrentPatch(Patch.MULTI);
		} else if (ae.getActionCommand().equals("Init current bank")) {
			initCurrentBank();
		} else if (ae.getActionCommand().equals("Scan hardware")) {
			sysexController.scanHardware();
		} else if (ae.getActionCommand().equals("Transmit patch to buffer")) {
			sysexController.dumpPatchToBuffer(patchBanks[currentBankNumber].getPatchAt(currentPatchNumber), getCores());			
		} else if (ae.getActionCommand().equals("Transmit patch to memory")) {
			sysexController.dumpPatch(new Patch[]{patchBanks[currentBankNumber].getPatchAt(currentPatchNumber)}, new int[]{currentPatchNumber}, currentBankNumber);			
		} else if (ae.getActionCommand().equals("Transmit bank to memory")) {
			sysexController.dumpPatchBank(patchBanks[currentBankNumber], currentBankNumber);
		} else if (ae.getActionCommand().equals("Receive patch from buffer")) {
			sysexController.requestPatchBuffer(0, currentPatchNumber, currentBankNumber);				
		} else if (ae.getActionCommand().equals("Receive patch from memory")) {						  
			sysexController.requestPatch(new int[]{currentPatchNumber}, currentBankNumber);				
		} else if (ae.getActionCommand().equals("Receive bank from memory")) {
			sysexController.requestPatch(requestBankIndices, currentBankNumber);				
		}
	}
}