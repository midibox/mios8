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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;

public class SIDLibController extends Observable implements Observer,
		ActionListener {
	private Patch[] copyPasteBuffer;
	private InitPatches initPatches = new InitPatches();
	private FileHandler fileHandler = new FileHandler(initPatches);
	private int[] currentPatchNumber = new int[] { 0 };
	private int currentBankNumber = 1;
	private int[] requestBankIndices;
	private Bank[] patchBanks = new Bank[8];
	private Boolean openEditor = false;
	private int coresHardware = 15; // AND mask for selected cores --- we
	// assume, that all cores are available by
	// default
	private int coresSelected = 1;

	private SysExController sysexController;

	public SIDLibController(SysExController sysexController) {
		this.sysexController = sysexController;
		sysexController.addObserver(this);
		sysexController.setInitPatches(initPatches);

		patchBanks[0] = new Bank(sysexController.getReceiver(), true,
				initPatches);
		for (int i = 1; i < patchBanks.length; i++) {
			patchBanks[i] = new Bank(sysexController.getReceiver(), false,
					initPatches);
		}

		requestBankIndices = new int[128];
		for (int i = 0; i < requestBankIndices.length; i++) {
			requestBankIndices[i] = i;
		}
	}

	public void scanHardware() {
		sysexController.scanHardware();
	}

	public void setCores(int i) {
		coresSelected = i;
		setChanged();
		notifyObservers("Cores changed");
		clearChanged();
	}

	public int getCores() {
		return coresSelected & coresHardware;
	}

	public int getCoresHardware() {
		return coresHardware;
	}

	public void editCurrentPatch() {
		if (coresHardware == 0 | coresSelected == 0) {
			// Open directly (offline mode)
			setChanged();
			notifyObservers("Edit");
			clearChanged();
		} else {
			if (sysexController.isDone()) {
				// Sends notification to SIDV2librarianGUI when done...
				openEditor = true;
				sysexController.dumpPatchToBuffer(patchBanks[currentBankNumber]
						.getPatchAt(currentPatchNumber[0]), getCores());
			}
		}
	}

	public Patch getCurrentPatch() {
		return patchBanks[currentBankNumber].getPatchAt(currentPatchNumber[0]);
	}

	public Bank getBank(int bankNumber) {
		return patchBanks[bankNumber];
	}

	public void setCurrentBankNumber(int i) {
		currentBankNumber = i;
		setChanged();
		notifyObservers("Bank changed");
		clearChanged();
	}

	public int getCurrentBankNumber() {
		return currentBankNumber;
	}

	public void setCurrentPatchNumber(int[] iarray) {
		currentPatchNumber = iarray;
	}

	public int getCurrentPatchNumber() {
		return currentPatchNumber[0];
	}

	public void setPatchAt(Patch p, int patchNumber, int bankNumber) {
		patchBanks[bankNumber].setPatchAt(patchNumber, p);
	}

	public boolean isEnsembleBank() {
		return currentBankNumber == 0;
	}

	// ****************** Edit functions ***********************
	public void editCopy() {
		copyPasteBuffer = patchBanks[currentBankNumber]
				.getPatchesAt(currentPatchNumber);
	}

	public void editPaste() {
		if ((copyPasteBuffer != null)
				&& patchBanks[currentBankNumber].isEnsembleBank() == copyPasteBuffer[0]
						.isEnsemble()) {
			for (int i = 0; i < copyPasteBuffer.length; i++) {
				if (currentPatchNumber[0] + i > patchBanks[currentBankNumber].bankSize - 1) {
					break;
				} else {
					patchBanks[currentBankNumber].setPatchAt(
							currentPatchNumber[0] + i, copyPasteBuffer[i]
									.clone());
				}
			}
			setChanged();
			notifyObservers("Data changed");
			clearChanged();
		}
	}

	public void editCut() {
		copyPasteBuffer = patchBanks[currentBankNumber]
				.getPatchesAt(currentPatchNumber);
		editClear();
	}

	public void editClear() {
		int s;
		if (patchBanks[currentBankNumber].isEnsembleBank()) {
			s = 256;
		} else {
			s = 512;
		}
		for (int i = 0; i < currentPatchNumber.length; i++) {
			patchBanks[currentBankNumber].setPatchAt(currentPatchNumber[i],
					new Patch(sysexController.getReceiver(), s, initPatches));
		}
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
		if (currentBankNumber == 0) {
			patchBanks[currentBankNumber] = new Bank(sysexController
					.getReceiver(), true, initPatches);
		} else {
			patchBanks[currentBankNumber] = new Bank(sysexController
					.getReceiver(), false, initPatches);
		}
		setChanged();
		notifyObservers("Data changed");
		clearChanged();
	}

	public void initCurrentPatch(Object object) {
		if (patchBanks[currentBankNumber].isEnsembleBank()) {
			for (int i = 0; i < currentPatchNumber.length; i++) {
				patchBanks[currentBankNumber].setPatchAt(currentPatchNumber[i],
						new Patch(sysexController.getReceiver(), 256,
								initPatches));
			}
		} else if (object != null) {
			for (int i = 0; i < currentPatchNumber.length; i++) {
				patchBanks[currentBankNumber].setPatchAt(currentPatchNumber[i],
						new Patch(sysexController.getReceiver(), 512,
								initPatches));
				patchBanks[currentBankNumber].getPatchAt(currentPatchNumber[i])
						.setEngine(object);
			}
		}
		setChanged();
		notifyObservers("Data changed");
		clearChanged();
	}

	// ****************** Event handling functions ***********************
	public void update(Observable observable, Object object) {
		if (object == "Hardware scan") {
			coresHardware = (Integer) sysexController.pickMeUp;
			setChanged();
			notifyObservers("Cores changed");
			clearChanged();
		} else if (object == "Patch ready") {
			patchBanks[sysexController.requestBank + 1].setPatchAt(
					sysexController.requestPatch[sysexController.requestCount],
					(Patch) sysexController.pickMeUp);
			setChanged();
			notifyObservers("Data changed");
			clearChanged();
		} else if ((object == "Dump completed") && (openEditor)) {
			openEditor = false;
			setChanged();
			notifyObservers("Edit");
			clearChanged();
		}
	}

	public void actionPerformed(ActionEvent ae) {
		if (ae.getActionCommand().equals("Load bank")) {
			Bank tempBank = fileHandler.loadPatchBank(sysexController
					.getReceiver(), patchBanks[currentBankNumber]
					.isEnsembleBank());
			if (tempBank != null) {
				patchBanks[currentBankNumber] = tempBank;
				setChanged();
				notifyObservers("Data changed");
				clearChanged();
			}
		} else if (ae.getActionCommand().equals("Save bank")) {
			fileHandler.savePatchBank(patchBanks[currentBankNumber],
					currentBankNumber - 1);
		} else if (ae.getActionCommand().equals("Load patch")) {
			Patch tempPatch = fileHandler.loadPatch(sysexController
					.getReceiver(), patchBanks[currentBankNumber]
					.isEnsembleBank());
			if (tempPatch != null) {
				patchBanks[currentBankNumber].setPatchAt(currentPatchNumber[0],
						tempPatch);
				setChanged();
				notifyObservers("Data changed");
				clearChanged();
			}
		} else if (ae.getActionCommand().equals("Save patch")) {
			fileHandler.savePatch(patchBanks[currentBankNumber]
					.getPatchAt(currentPatchNumber[0]), currentBankNumber - 1,
					currentPatchNumber[0]);
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
		} else if (ae.getActionCommand().equals("Init ensemble")) {
			initCurrentPatch(null);
		} else if (ae.getActionCommand().equals("Init current bank")) {
			initCurrentBank();
		} else if (ae.getActionCommand().equals("Scan hardware")) {
			sysexController.scanHardware();
		} else if (ae.getActionCommand().equals("Transmit patch to buffer")) {
			sysexController.dumpPatchToBuffer(patchBanks[currentBankNumber]
					.getPatchAt(currentPatchNumber[0]), getCores());
		} else if (ae.getActionCommand().equals("Transmit patch to memory")) {
			Patch[] p = new Patch[currentPatchNumber.length];
			for (int i = 0; i < currentPatchNumber.length; i++) {
				p[i] = patchBanks[currentBankNumber]
						.getPatchAt(currentPatchNumber[i]);
			}
			sysexController.dumpPatch(p, currentPatchNumber,
					currentBankNumber - 1);
		} else if (ae.getActionCommand().equals("Transmit bank to memory")) {
			sysexController.dumpPatchBank(patchBanks[currentBankNumber],
					currentBankNumber - 1);
		} else if (ae.getActionCommand().equals("Receive patch from buffer")) {
			sysexController.requestPatchBuffer(0, currentPatchNumber[0],
					currentBankNumber - 1);
		} else if (ae.getActionCommand().equals("Receive patch from memory")) {
			sysexController.requestPatch(currentPatchNumber,
					currentBankNumber - 1);
		} else if (ae.getActionCommand().equals("Receive bank from memory")) {
			sysexController.requestPatch(requestBankIndices,
					currentBankNumber - 1);
		}
	}
}