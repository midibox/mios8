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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import javax.sound.midi.Receiver;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileFilter;

import org.midibox.midi.MidiUtils;
import org.midibox.sidedit.SIDSysexInfo;

public class FileHandler {
	final JFileChooser fc = new JFileChooser();

	public FileHandler() {
		fc.addChoosableFileFilter(new SysExFilter());
	}

	public void savePatchBank(Bank b, int bankNumber) {
		try {
			saveData(toByteArray(getBankDumpSysex(b, bankNumber)));
		} catch (IOException e) {
			JOptionPane.showMessageDialog(null,
					"An error has occurred while writing the file!", "Error",
					JOptionPane.ERROR_MESSAGE);
		}
	}

	public void saveEnsembleBank() {

	}

	public void savePatch(Patch p, int bankNumber, int patchNumber) {
		try {
			saveData(toByteArray(getPatchDumpSysex(p, bankNumber, patchNumber)));
		} catch (IOException e) {
			JOptionPane.showMessageDialog(null,
					"An error has occurred while writing the file!", "Error",
					JOptionPane.ERROR_MESSAGE);
		}
	}

	public void saveEnsemble() {

	}

	public Bank loadPatchBank(Receiver receiver) {
		Bank b = null;
		try {
			byte[] data = loadData();
			if (data != null) {
				String s = MidiUtils.getHexString(data).replace(" ", "");
				if ((s.length() != 128 * Bank.patchSize)
						&& (s.length() != 64 * Bank.patchSize)) {
					JOptionPane
							.showMessageDialog(
									null,
									"This file does not contain a bank with 64 or 128 valid patches!",
									"Error", JOptionPane.ERROR_MESSAGE);
				} else {
					Bank tempBank = new Bank(receiver);
					String status = tempBank.parseBankSyx(s);
					if (statusCheck(status)) {
						b = tempBank;
					}
				}
			}
		} catch (IOException e) {
			JOptionPane.showMessageDialog(null,
					"An error has occurred while reading the file!", "Error",
					JOptionPane.ERROR_MESSAGE);
		}
		return b;
	}

	public Patch loadPatch(Receiver receiver) {
		Patch p = null;
		try {
			byte[] data = loadData();
			if (data != null) {
				String s = MidiUtils.getHexString(data).replace(" ", "");
				;
				if (s.length() != Bank.patchSize) {
					JOptionPane.showMessageDialog(null,
							"This file does not contain a valid patch!",
							"Error", JOptionPane.ERROR_MESSAGE);
				} else {
					Patch tempPatch = new Patch(receiver);
					String status = tempPatch.parsePatch(s);
					if (statusCheck(status)) {
						p = tempPatch;
					}
				}
			}
		} catch (IOException e) {
			JOptionPane.showMessageDialog(null,
					"An error has occurred while reading the file!", "Error",
					JOptionPane.ERROR_MESSAGE);
		}
		return p;
	}

	public void loadEnsemble() {

	}

	public void loadEnsembleBank() {

	}

	private void saveData(byte[] b) throws IOException {
		boolean write = false;
		while (!write) {
			int returnVal = fc.showSaveDialog(null);
			if (returnVal == JFileChooser.APPROVE_OPTION) {
				if (fc.getSelectedFile().exists()) {
					int response = JOptionPane.showConfirmDialog(null,
							"File already exists! Do you want to overwrite?",
							"Overwrite?", JOptionPane.YES_NO_OPTION,
							JOptionPane.QUESTION_MESSAGE);
					if (response == JOptionPane.YES_OPTION) {
						write = true;
					}
				} else {
					write = true;
				}

			} else {
				break;
			}
		}
		if (write) {
			File file = fc.getSelectedFile();
			FileOutputStream out = null;
			try {
				out = new FileOutputStream(file);
				for (int i = 0; i < b.length; i++) {
					out.write(b[i]);
				}
			} finally {
				if (out != null) {
					out.close();
				}
			}
		}
	}

	private byte[] loadData() throws IOException {
		byte[] b = null;
		int returnVal = fc.showOpenDialog(null);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
			File file = fc.getSelectedFile();
			FileInputStream in = null;
			b = new byte[(int) file.length()];
			try {
				in = new FileInputStream(file);
				for (int i = 0; i < file.length(); i++) {
					b[i] = (byte) in.read();
				}
			} finally {
				if (in != null) {
					in.close();
				}
			}
		}
		return b;
	}

	private String getBankDumpSysex(Bank b, int bankNumber) {
		String s = "";
		for (int i = 0; i < b.bankSize; i++) {
			s = s + getPatchDumpSysex(b.getPatchAt(i), bankNumber, i);
		}
		return s;
	}

	private String getPatchDumpSysex(Patch p, int bankNumber, int patchNumber) {
		String dataStr = p.getSysexString();
		String patchStr = Integer.toHexString(patchNumber);
		if (patchStr.length() == 1) {
			patchStr = "0" + patchStr;
		}
		String strMessage = SIDSysexInfo.hardPatchDumpSysex;
		strMessage = strMessage.replace("<device>", "00");
		strMessage = strMessage.replace("<bank>", "0"
				+ Integer.toHexString(bankNumber));
		strMessage = strMessage.replace("<patch>", patchStr);
		strMessage = strMessage.replace("<data><checksum>", dataStr);
		return strMessage;
	}

	private byte[] toByteArray(String s) {
		int nLengthInBytes = s.length() / 2;
		byte[] abMessage = new byte[nLengthInBytes];
		for (int i = 0; i < nLengthInBytes; i++) {
			abMessage[i] = (byte) Integer.parseInt(s
					.substring(i * 2, i * 2 + 2), 16);
		}
		return abMessage;
	}

	private boolean statusCheck(String status) {
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

	public class SysExFilter extends FileFilter {
		public final static String syx = "syx";

		public String getExtension(File f) {
			String ext = null;
			String s = f.getName();
			int i = s.lastIndexOf('.');

			if (i > 0 && i < s.length() - 1) {
				ext = s.substring(i + 1).toLowerCase();
			}
			return ext;
		}

		public boolean accept(File f) {
			if (f.isDirectory()) {
				return true;
			}
			String extension = getExtension(f);
			if (extension != null) {
				if (extension.equals(syx)) {
					return true;
				} else {
					return false;
				}
			}
			return false;
		}

		public String getDescription() {
			return "SysEx files (*.syx)";
		}
	}
}
