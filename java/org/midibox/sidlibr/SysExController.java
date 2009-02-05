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

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;
import javax.swing.JOptionPane;
import javax.swing.ProgressMonitor;
import javax.swing.Timer;

import org.midibox.midi.MidiUtils;
import org.midibox.sidedit.SIDSysexInfo;

public class SysExController extends Observable implements Receiver,
		ActionListener {
	public static Object PATCH = new Object();
	public static Object ENSEMBLE = new Object();

	public Object pickMeUp;

	private Timer timer;
	private int timeOut = 2000;

	private ProgressMonitor progress;

	private static String IDLE = "IDLE";
	private static String COLLECTING = "COLLECTING";
	private static String DUMPING = "DUMPING";
	private static String SCANNING = "SCANNING";
	private static String FORWARDING = "FORWARDING";
	private String STATE;

	private Object tempSyxType;
	private String tempSyx = new String();
	private String[] dumpStack;
	private int dumpCount;

	private String tempResponse;
	private boolean syxErrorChk = true;
	private Receiver receiver;
	private int masterCore = 0;

	public int[] requestPatch;
	public int requestBank;
	public int requestCount;

	public SysExController(Receiver receiver) {
		this.receiver = receiver;
		timer = new Timer(timeOut, this);
		timer.setInitialDelay(timeOut);
		STATE = IDLE;
	}

	public Receiver getReceiver() {
		return receiver;
	}

	public boolean isDone() {
		return (STATE == IDLE);
	}

	public void close() {
	}

	// ****************** Forwarding functions ***********************
	public void resetForwarding() {
		String init = getForwardSyx(0);
		sendSyx(init);
	}

	public void setForwarding(int cores) {
		sendSyx(getForwardSyx(cores));
	}

	private String getForwardSyx(int cores) {
		String s = SIDSysexInfo.forwardSysex;
		s = s.replace("<device>", zeroPadToHex(masterCore));
		s = s.replace("<sids>", zeroPadToHex(cores));
		return s;
	}

	// ****************** Hardware scan functions ***********************
	public void scanHardware() {
		if (isDone()) {
			setForwarding(15);
			String message = SIDSysexInfo.pingSysex;
			message = message.replace("<device>", zeroPadToHex(masterCore));
			progress = new ProgressMonitor(null, "", "Receiving SysEx data...",
					0, 1);
			timer.start();
			STATE = SCANNING;
			sendSyx(message);
		}
	}

	private void stopScan() {
		timer.stop();
		STATE = IDLE;
		if (progress != null) {
			progress.close();
		}
	}

	// ****************** Receive functions ***********************
	public void requestPatch(int[] patchNumber, int bankNumber) {
		if (isDone()) {
			resetForwarding();
			requestPatch = patchNumber;
			if (bankNumber==-1) {
				requestBank = bankNumber;
				startRequest(ENSEMBLE);
			} else {
				requestBank = bankNumber;
				startRequest(PATCH);
			}
		}
	}
	
	public void requestPatchBuffer(int coreNumber, int patch, int bank) {
		if (isDone()) {
			resetForwarding();
			requestPatch = new int[]{patch};
			progress = new ProgressMonitor(null, "", "Receiving SysEx data...", 0, requestPatch.length);		
			STATE = COLLECTING;
			timer.start();
			requestCount = 0;	
			String message;
			if (bank==-1) {
				requestBank = bank;
				tempSyxType = ENSEMBLE;
				message = SIDSysexInfo.editEnsembleRequestSysex;
			} else {
				requestBank = bank;
				tempSyxType = PATCH;
				message = SIDSysexInfo.editPatchRequestSysex;
			}		
			message = message.replace("<device>", zeroPadToHex(coreNumber));
			sendSyx(message);
		}
	}
	
	private void requestNext() {
		requestCount++;
		if (requestCount < requestPatch.length) {
			timer.restart();
			String message = "";
			if (tempSyxType==ENSEMBLE) {
				message = SIDSysexInfo.hardEnsembleRequestSysex;				
				message = message.replace("<ensemble>", zeroPadToHex(requestPatch[requestCount]));
			} else if (tempSyxType==PATCH){
				message = SIDSysexInfo.hardPatchRequestSysex;
				message = message.replace("<bank>", zeroPadToHex(requestBank));
				message = message.replace("<patch>", zeroPadToHex(requestPatch[requestCount]));						
			}	
			message = message.replace("<device>", "00");
			sendSyx(message);
		} else {
			stopRequest();
		}
	}

	private void startRequest(Object type) {
		timer.start();
		requestCount = -1;
		tempSyxType = type;
		progress = new ProgressMonitor(null, "", "Receiving SysEx data...", 0,
				requestPatch.length);
		STATE = COLLECTING;
		requestNext();
	}

	private void stopRequest() {
		STATE = IDLE;
		timer.stop();
		if (progress != null) {
			progress.close();
		}
	}

	private void scanSyx(String m) {
		if (progress.isCanceled() || (tempResponse.substring(0, 5).equals("ERROR"))) {
			stopScan();
		} else if (tempResponse.substring(0, 12).equals("ACKNOWLEDGED")) {
			stopScan();
			pickMeUp = Integer.parseInt(m.substring(14, 16), 16);
			setChanged();
			notifyObservers("Hardware scan");
			clearChanged();
		}
	}

	protected void collectSyx(String m) {
		if (progress.isCanceled()) {
			stopRequest();
		} else {
			if (!(m.indexOf(SIDSysexInfo.acknowledgedSysex.replace("<device>",
					"00")) == 0)) {
				if (m.substring(0, 2).equalsIgnoreCase("F0")) { // select next array item with each F0
					tempSyx = m;
				} else {
					if (m.substring(0, 2).equalsIgnoreCase("F7")) { // for windows (1024 byte limit, F7 added at beginning of next chunk)
						m = m.substring(2);
					}
					tempSyx += m;
				}
				timer.restart();
				progress.setProgress(requestCount);
				if (m.substring(m.length() - 2).equalsIgnoreCase("F7")) {
					parseSysex();
				}
			}
		}
	}

	private void parseSysex() {
		if (tempSyxType == PATCH) {
			Patch tempPatch = new Patch(receiver,512);
			String status = tempPatch.parsePatch(tempSyx);
			if (statusCheck(status)) {
				pickMeUp = tempPatch;
				setChanged();
				notifyObservers("Patch ready");
				clearChanged();
				requestNext();
			} else {
				stopRequest();
			}
		} else if (tempSyxType == ENSEMBLE) {
			Patch tempEnsemble = new Patch(receiver,256);
			String status = tempEnsemble.parsePatch(tempSyx);
			if (statusCheck(status)) {
				pickMeUp = tempEnsemble;
				setChanged();
				notifyObservers("Patch ready");
				clearChanged();
				requestNext();
			} else {
				stopRequest();
			}
		}
	}

	public boolean statusCheck(String status) {
		boolean b = false;
		if (status == "succesful") {
			b = true;
		} else if (status == "checksum error") {
			JOptionPane.showMessageDialog(null, "Checksum error!", "Error",
					JOptionPane.ERROR_MESSAGE);
		} else if (status == "parse error error") {
			JOptionPane.showMessageDialog(null,
					"An error occured while parsing a patch!", "Error",
					JOptionPane.ERROR_MESSAGE);
		}
		return b;
	}

	// ****************** Transmit functions ***********************
	public void dumpPatch(Patch[] p, int[] patchNumber, int bankNumber) {
		if (isDone()) {
			String[] s = new String[p.length + 1];
			s[0] = getForwardSyx(0);
			for (int i = 0; i < p.length; i++) {
				String dataStr = p[i].getSysexString();
				String strMessage;
				if (p[1].isEnsemble()) {
					strMessage = SIDSysexInfo.hardEnsembleDumpSysex;
					strMessage = strMessage.replace("<ensemble>", zeroPadToHex(patchNumber[i]));					
				} else {
					strMessage = SIDSysexInfo.hardPatchDumpSysex;
					strMessage = strMessage.replace("<bank>", zeroPadToHex(bankNumber));
					strMessage = strMessage.replace("<patch>", zeroPadToHex(patchNumber[i]));					
				}				
				strMessage = strMessage.replace("<device>",	zeroPadToHex(masterCore));
				strMessage = strMessage.replace("<data><checksum>", dataStr);
				s[i + 1] = strMessage;
			}
			startDump(s);
		}
	}
	
	public void dumpPatchToBuffer(Patch p, int cores) {
		if (isDone() && cores != 0) {
			String strMessage;
			if (p.isEnsemble()) {
				strMessage = SIDSysexInfo.editEnsembleDumpSysex;
			} else {
				strMessage = SIDSysexInfo.editPatchDumpSysex;
			}
			strMessage = strMessage.replace("<device>",	zeroPadToHex(masterCore));
			strMessage = strMessage.replace("<data><checksum>", p.getSysexString());
			startDump(new String[] { getForwardSyx(cores), strMessage });
		}
	}
	
	public void dumpPatchBank(Bank b, int bankNumber) {
		if (isDone()) {
			Patch[] p = new Patch[b.bankSize - 1];
			int[] pNr = new int[b.bankSize - 1];
			for (int i = 1; i < b.bankSize; i++) {
				p[i - 1] = b.getPatchAt(i);
				pNr[i - 1] = i;
			}
			dumpPatch(p, pNr, bankNumber);
		}
	}
	
	private void startDump(String[] s) {
		dumpStack = s;
		dumpCount = 0;
		System.out.println("Transmitting SysEx data...");
		progress = new ProgressMonitor(null, "", "Transmitting SysEx data...",
				0, dumpStack.length);
		timer.start();
		STATE = DUMPING;
		tempResponse = "";
		sendNext();
	}

	private void stopDump() {
		STATE = IDLE;
		timer.stop();
		if (progress != null) {
			progress.close();
		}
	}

	private void sendNext() {
		if (dumpCount == dumpStack.length) { // If no more to send then finish
			stopDump();
			setChanged();
			notifyObservers("Dump completed");
			clearChanged();
		} else { // send Syx and wait for next call of ()
			sendSyx(dumpStack[dumpCount++]);
			progress.setProgress(dumpCount);
			timer.restart();
		}
	}

	protected void dumpSyx(String m) {
		if (progress.isCanceled()
				|| (tempResponse.substring(0, 5).equals("ERROR"))) {
			stopDump();
		} else if (tempResponse.substring(0, 12).equals("ACKNOWLEDGED")) {
			tempResponse = "";
			sendNext();
		}
	}

	// ****************** General functions ***********************
	public void send(MidiMessage message, long timestamp) {
		String m = MidiUtils.getHexString(message.getMessage())
				.replace(" ", "");

		if (syxErrorChk) {
			checkError(m);
		}
		
		if (STATE == COLLECTING) {
			collectSyx(m);
		}

		if (STATE == DUMPING) {
			dumpSyx(m);
		}

		if (STATE == SCANNING) {
			scanSyx(m);
		}
	}

	public void actionPerformed(ActionEvent ae) {
		if (ae.getSource() == timer) {
			if (STATE == COLLECTING) {
				stopRequest();
			}
			if (STATE == DUMPING) {
				stopDump();
			}

			if (STATE == SCANNING) {
				stopScan();
			}
			JOptionPane
					.showMessageDialog(
							null,
							"The MBSID V2 is not responding. Please check all connections!",
							"Error", JOptionPane.ERROR_MESSAGE);
		}
	}

	protected void checkError(String m) {
		if (m.indexOf(SIDSysexInfo.acknowledgedSysex.replace("<device>", "00")) == 0) {
			tempResponse = "ACKNOWLEDGED" + m.substring(14, 16);
		} else if (m.equals(SIDSysexInfo.error1Sysex.replace("<device>", "00"))) {
			tempResponse = "ERROR1";
			JOptionPane.showMessageDialog(null,
					"MBSID says: Received less bytes then expected!", "Error",
					JOptionPane.ERROR_MESSAGE);
		} else if (m.equals(SIDSysexInfo.error2Sysex.replace("<device>", "00"))) {
			tempResponse = "ERROR2";
			JOptionPane.showMessageDialog(null, "MBSID says: Wrong checksum!",
					"Error", JOptionPane.ERROR_MESSAGE);
		} else if (m.equals(SIDSysexInfo.error3Sysex.replace("<device>", "00"))) {
			tempResponse = "ERROR3";
			JOptionPane
					.showMessageDialog(
							null,
							"MBSID says: Bankstick or patch/drumset/ensemble not available!",
							"Error", JOptionPane.ERROR_MESSAGE);
		} else if (m.equals(SIDSysexInfo.error4Sysex.replace("<device>", "00"))) {
			tempResponse = "ERROR4";
			JOptionPane.showMessageDialog(null,
					"MBSID says: Parameter not available!", "Error",
					JOptionPane.ERROR_MESSAGE);
		} else if (m.equals(SIDSysexInfo.error5Sysex.replace("<device>", "00"))) {
			tempResponse = "ERROR5";
			JOptionPane.showMessageDialog(null,
					"MBSID says: RAM access not supported!", "Error",
					JOptionPane.ERROR_MESSAGE);
		} else if (m.equals(SIDSysexInfo.error6Sysex.replace("<device>", "00"))) {
			tempResponse = "ERROR6";
			JOptionPane.showMessageDialog(null,
					"MBSID says: BankStick too small for 128 patches!",
					"Error", JOptionPane.ERROR_MESSAGE);
		} else {
			// System.out.println("MBSID: " + m);
		}
	}

	protected void sendSyx(String s) {
		SysexMessage sysexMessage = new SysexMessage();
		try {
			byte[] abMessage = toByteArray(s);
			sysexMessage.setMessage(abMessage, abMessage.length);
		} catch (Exception e) {
			JOptionPane.showMessageDialog(null,
					"The System Exclusive data could not be sent!", "Error",
					JOptionPane.ERROR_MESSAGE);
		}
		receiver.send(sysexMessage, -1);
	}

	protected byte[] toByteArray(String s) {
		int nLengthInBytes = s.length() / 2;
		byte[] abMessage = new byte[nLengthInBytes];
		for (int i = 0; i < nLengthInBytes; i++) {
			abMessage[i] = (byte) Integer.parseInt(s
					.substring(i * 2, i * 2 + 2), 16);
		}
		return abMessage;
	}

	protected String zeroPadToHex(int i) {
		String s = "00" + Integer.toHexString(i);
		s = s.substring(s.length() - 2);
		return s;
	}

}