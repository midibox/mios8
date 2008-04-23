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
import java.util.Vector;
import org.midibox.midi.MidiUtils;
import javax.sound.midi.Receiver;

import org.midibox.sidlibr.Patch;
import org.midibox.sidedit.SIDSysexInfo;
import javax.swing.*;
import java.awt.event.*;

import javax.sound.midi.MidiMessage;
import javax.sound.midi.SysexMessage;
import java.io.IOException;
public class SysExController extends Observable implements Receiver, ActionListener  {
	public static Object PATCH = new Object();
	public static Object PATCHBANK = new Object();
	public static Object ENSEMBLE = new Object();
	public static Object ENSEMBLEBANK = new Object();
	
	public static Object NONE = new Object();
	public static Object ACKNOWLEDGED = new Object();
	public static Object ERROR1 = new Object();
	public static Object ERROR2 = new Object();
	public static Object ERROR3 = new Object();
	public static Object ERROR4 = new Object();
	public static Object ERROR5 = new Object();
	public static Object ERROR6 = new Object();
		
	public Object pickMeUp;
	
	private Timer timer;
	private int timeOut = 2000;
	  
	private ProgressMonitor progress;
	
	private Object tempSyxType;
	private int tempCount;	
	private String[] tempSyx;
	private boolean tempSyxCollect = false;
	private boolean tempSyxDump = false;
	private Object tempResponse;	
	private boolean syxErrorChk = true;
	private Receiver receiver;
	private boolean waitForResponse = false;
	private int masterCore = 0;
	private int updateDelay = 1500;
	
	public SysExController(Receiver receiver) {
		this.receiver = receiver;		
	}
	
	public Receiver getReceiver() {
		return receiver;
	}
		
	public void setReceiver(Receiver receiver) {	
		this.receiver = receiver;
	}
	
	public boolean isDone() {
		return (!(tempSyxCollect||tempSyxDump));
	}
	
	public void close() {}
	
	public void scanMBSID() {
		// Check for response of the SID synth on multiple cores
	}
	
	// ****************** Receive functions ***********************	
	public void requestPatchBank(int bankNumber) {
		if (isDone()) {
			String strMessage = SIDSysexInfo.bankRequestSysex;
			strMessage = strMessage.replace("<device>", "00");
			strMessage = strMessage.replace("<bank>", "0" + Integer.toHexString(bankNumber));		
			startRequest(PATCHBANK, strMessage);			
		}
	}
	
	public void requestPatch(int patchNumber, int bankNumber) {	
		if (isDone()) {
			String patchStr = Integer.toHexString(patchNumber);
			if (patchStr.length()==1) {
				patchStr = "0" + patchStr;
			}		
			String strMessage = SIDSysexInfo.hardPatchRequestSysex;
			strMessage = strMessage.replace("<device>", "00");
			strMessage = strMessage.replace("<bank>", "0" + Integer.toHexString(bankNumber));		
			strMessage = strMessage.replace("<patch>", patchStr);		
			startRequest(PATCH, strMessage);			
		}
	}
	
	public void requestPatchBuffer(int coreNumber) {		
		if (isDone()) {
			String strMessage = SIDSysexInfo.editPatchRequestSysex;
			strMessage = strMessage.replace("<device>", "0" + Integer.toHexString(coreNumber));		
			startRequest(PATCH, strMessage);
		}
	}
	
	private void startRequest(Object type, String message) {
		tempSyxType = type;
		tempCount = -1; // will be incremented with each F0
		if (tempSyxType==PATCHBANK) {
			tempSyx = new String[128];  
		} else if (tempSyxType==PATCH) {
			tempSyx = new String[1];
		} else if (tempSyxType==ENSEMBLE) {
			tempSyx = new String[1];
		} else if (tempSyxType==ENSEMBLEBANK) {
			tempSyx = new String[128];
		}		
		progress = new ProgressMonitor(null, "", "Receiving SysEx data...", 0, tempSyx.length);
		timer = new Timer(timeOut, this);
		timer.setInitialDelay(timeOut);
		timer.start();
		tempSyxCollect = true;
		sendSyx(message);
	}
	
	private void stopRequest() {
		tempSyxCollect = false;			
		timer.stop();
		if (progress!=null) {
			progress.close();
		}
	}
		
	public void send(MidiMessage message, long timestamp) {
		String m = MidiUtils.getHexString(message.getMessage()).replace(" ", "");
		
		if (syxErrorChk) {
			checkError(m);
		}
		
		if (tempSyxCollect) {
			collectSyx(m);
		}		
		
		if (tempSyxDump) {
			dumpSyx(m);
		}
		
	}
		
	protected void collectSyx(String m) {
		if (progress.isCanceled()) {
			stopRequest();
		} else {
		        // select next array item with each F0
  		        if( m.substring(0, 2).equalsIgnoreCase("F0") ) {
			    tempSyx[++tempCount] = m;
			} else {
			    if( m.substring(0, 2).equalsIgnoreCase("F7") ) { // for windows (1024 byte limit, F7 added at beginning of next chunk)
				m = m.substring(2);
			    }
			    tempSyx[tempCount] += m;
			}

			timer.restart();
			progress.setProgress(tempCount);
			
            if( (m.substring(m.length() - 2).equalsIgnoreCase("F7")) && (tempCount == tempSyx.length-1) ) {
				stopRequest();				
				parseSysex();
			}	
		}
	}
	
	private void parseSysex() {
		if (tempSyxType==PATCHBANK) {				
			Bank tempBank = new Bank(receiver);
			String s = "";
			for (int c=0;c<tempSyx.length;c++) {
				if (tempSyx[c] != null) { 
					s = s + tempSyx[c];
				} else {break;}
			}
			String status = tempBank.parseBankSyx(s);
			if (statusCheck(status)) {
				pickMeUp = tempBank;
				setChanged();
				notifyObservers("Patchbank ready");
				clearChanged();
			}
		} else if (tempSyxType==PATCH) {	
			Patch tempPatch = new Patch(receiver);	
			String status = tempPatch.parsePatch(tempSyx[0]);
			if (statusCheck(status)) {
				pickMeUp = tempPatch;
				setChanged();
				notifyObservers("Patch ready");
				clearChanged();
			}
		} else if (tempSyxType==ENSEMBLE) {
			setChanged();
			notifyObservers("Ensemble ready");
			clearChanged();
		} else if (tempSyxType==ENSEMBLEBANK) {
			setChanged();
			notifyObservers("Ensemblebank ready");
			clearChanged();
		}
	}
	
	public boolean statusCheck(String status) {
		boolean b = false;
		if (status=="succesful") {
			b = true;
		} else if (status=="checksum error") {
			JOptionPane.showMessageDialog(null,"Checksum error!","Error",JOptionPane.ERROR_MESSAGE);
		} else if (status=="parse error error") {
			JOptionPane.showMessageDialog(null,"An error occured while parsing a patch!","Error",JOptionPane.ERROR_MESSAGE);
		}		
		return b;
	}
	
	public void actionPerformed(ActionEvent ae) {
		if (ae.getSource() == timer) {
			if (tempSyxCollect) {
				stopRequest();
				if ((tempCount==128)) {				
					parseSysex();
				} else {			
					JOptionPane.showMessageDialog(null,"SysEx receive time out! Please check the connections to the MBSID V2.","Error",JOptionPane.ERROR_MESSAGE);
				}
			}
			if (tempSyxDump) {
				stopDump();
				JOptionPane.showMessageDialog(null,"The MBSID V2 is not responding!","Error",JOptionPane.ERROR_MESSAGE);
			}
		}
	}
	
	// ****************** Transmit functions ***********************
	public void dumpPatch(Patch p, int patchNumber, int bankNumber) {
		if (isDone()) {
			String dataStr = p.getSysexString();
			String patchStr = Integer.toHexString(patchNumber);
			if (patchStr.length()==1) {
				patchStr = "0" + patchStr;
			}		
			String strMessage = SIDSysexInfo.hardPatchDumpSysex;
			strMessage = strMessage.replace("<device>", "0" + Integer.toHexString(masterCore));
			strMessage = strMessage.replace("<bank>", "0" + Integer.toHexString(bankNumber));	
			strMessage = strMessage.replace("<patch>", patchStr);
			strMessage = strMessage.replace("<data><checksum>", dataStr);
			String[] s = new String[1];
			s[0] = strMessage;
	 		startDump(s, PATCH, true);
		}
	}
	
	public void dumpPatchToBuffer(Patch p, boolean[] cores) {
		if (isDone()) {
			int c = 0;
			for(int i=0;i<cores.length;i++) {if (cores[i]) {c++;}}
			String[] s = new String[c];
			
			boolean wfResponse;
			if ((c==1) && (cores[masterCore])) {
				wfResponse = true;				
			} else {
				wfResponse = false;
				updateDelay = c*1000;			// Delay for 500 seconds per core before 'done'
			}	
			
			if (c!=0) {
				c = 0;
				for(int i=0;i<cores.length;i++) {
					if (cores[i]) {
						String dataStr = p.getSysexString();
						String strMessage = SIDSysexInfo.editPatchDumpSysex;							
						strMessage = strMessage.replace("<device>", "0" + Integer.toHexString(i));
						strMessage = strMessage.replace("<data><checksum>", dataStr);
						s[c] = strMessage;
						c++;
					}
				}
		 		startDump(s, PATCH, wfResponse);
			}	
			
		}
	}
	
	public void dumpPatchBank(Bank b, int bankNumber) {
		if (isDone()) {
			String[] s = new String[b.bankSize-1];		
			for(int i=1;i<b.bankSize;i++) {
				String dataStr = b.getPatchAt(i).getSysexString();
				String patchStr = Integer.toHexString(i);
				if (patchStr.length()==1) {
					patchStr = "0" + patchStr;
				}		
				String strMessage = SIDSysexInfo.hardPatchDumpSysex;
				strMessage = strMessage.replace("<device>", "0" + Integer.toHexString(masterCore));
				strMessage = strMessage.replace("<bank>", "0" + Integer.toHexString(bankNumber));	
				strMessage = strMessage.replace("<patch>", patchStr);
				strMessage = strMessage.replace("<data><checksum>", dataStr);
				s[i-1] = strMessage;
			}		
			startDump(s, PATCH, true);			
		}
	}
	
	private void startDump(String[] s, Object type, boolean w) {
		System.out.println("Start dump");
		waitForResponse = w;
		tempSyx = s;
		tempSyxType = type;
		tempCount = 0;				
		progress = new ProgressMonitor(null, "", "Transmitting SysEx data...", 0, tempSyx.length);
		timer = new Timer(timeOut, this);
		timer.setInitialDelay(timeOut);
		timer.start();
		tempSyxDump = true;
		tempResponse=NONE;
		sendNext();
	}
	
	private void stopDump() {
		System.out.println("Stop dump");
		waitForResponse = true;
		tempSyxDump = false;	
		timer.stop();
		if (progress!=null) {
			progress.close();
		}
	}
	
	private void sendNext() {
		System.out.println("Sending next");
		if (tempCount==tempSyx.length) {			// If no more to send then finish
			if (!(waitForResponse)) {
				timer.stop();
				try {
					Thread.currentThread().sleep(updateDelay);
					}
				catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			stopDump();
			setChanged();
			notifyObservers("Dump completed");
			clearChanged();
		} else {
			if (waitForResponse) {						// If waitForResponse send Syx and wait for next call of sendNext()
				sendSyx(tempSyx[tempCount]);				
				tempCount++;
				progress.setProgress(tempCount);
				timer.restart();
			} else {									// If not waitForResponse send everything immediately
				sendSyx(tempSyx[tempCount]);		
				tempCount++;
				progress.setProgress(tempCount);
				timer.restart();
				sendNext();		
			}
		}
	}
	
	protected void dumpSyx(String m) {
		if (progress.isCanceled() || (tempResponse==ERROR1)||(tempResponse==ERROR2)||(tempResponse==ERROR3)||(tempResponse==ERROR4)||(tempResponse==ERROR5)||(tempResponse==ERROR6)) {
			stopDump();
		} else if ((tempResponse==ACKNOWLEDGED)&&(waitForResponse)) {
			tempResponse=NONE;
			sendNext();			
		}
	}
	
	protected void checkError(String m) {
		if (m.indexOf(SIDSysexInfo.acknowledgedSysex.replace("<device>", "00"))== 0) {
			tempResponse=ACKNOWLEDGED;						
		}else if (m.equals(SIDSysexInfo.error1Sysex.replace("<device>", "00"))) {
			tempResponse=ERROR1;
			JOptionPane.showMessageDialog(null,"MBSID says: Received less bytes then expected!","Error",JOptionPane.ERROR_MESSAGE);
		}else if (m.equals(SIDSysexInfo.error2Sysex.replace("<device>", "00"))) {
			tempResponse=ERROR2;
			JOptionPane.showMessageDialog(null,"MBSID says: Wrong checksum!","Error",JOptionPane.ERROR_MESSAGE);
		}else if (m.equals(SIDSysexInfo.error3Sysex.replace("<device>", "00"))) {
			tempResponse=ERROR3;
			JOptionPane.showMessageDialog(null,"MBSID says: Bankstick or patch/drumset/ensemble not available!","Error",JOptionPane.ERROR_MESSAGE);
		}else if (m.equals(SIDSysexInfo.error4Sysex.replace("<device>", "00"))) {
			tempResponse=ERROR4;
			JOptionPane.showMessageDialog(null,"MBSID says: Parameter not available!","Error",JOptionPane.ERROR_MESSAGE);
		}else if (m.equals(SIDSysexInfo.error5Sysex.replace("<device>", "00"))) {
			tempResponse=ERROR5;
			JOptionPane.showMessageDialog(null,"MBSID says: RAM access not supported!","Error",JOptionPane.ERROR_MESSAGE);
		}else if (m.equals(SIDSysexInfo.error6Sysex.replace("<device>", "00"))) {
			tempResponse=ERROR6;
			JOptionPane.showMessageDialog(null,"MBSID says: BankStick too small for 128 patches!","Error",JOptionPane.ERROR_MESSAGE);
		}else {
			//System.out.println("MBSID: " + m);					
		}	
	}
	
	protected void sendSyx(String s) {
		SysexMessage sysexMessage = new SysexMessage();	
		try {
			byte[] abMessage = toByteArray(s);
			sysexMessage.setMessage(abMessage, abMessage.length);	
		} catch (Exception e) {			
			JOptionPane.showMessageDialog(null,"The System Exclusive data could not be sent!","Error",JOptionPane.ERROR_MESSAGE);
		}
		receiver.send(sysexMessage, -1);
	}
	
	protected byte[] toByteArray(String s) {
		int	nLengthInBytes = s.length() / 2;
		byte[]	abMessage = new byte[nLengthInBytes];
		for (int i = 0; i < nLengthInBytes; i++)
		{
			abMessage[i] = (byte) Integer.parseInt(s.substring(i * 2, i * 2 + 2), 16);				
		}		
		return abMessage;
	}
	
}