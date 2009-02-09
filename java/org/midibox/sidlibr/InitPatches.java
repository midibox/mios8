package org.midibox.sidlibr;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;

import javax.sound.midi.Receiver;
import javax.swing.JOptionPane;

import org.midibox.midi.MidiUtils;

public class InitPatches {
	private int[] lead;
	private int[] bassline;
	private int[] drum;
	private int[] multi;
	private int[] ensemble;
	
	public InitPatches() {
		lead = loadPatch("lead.syx",512);
		bassline = loadPatch("bassline.syx",512);
		drum = loadPatch("drum.syx",512);
		multi = loadPatch("multi.syx",512);
		ensemble = loadPatch("ensemble.syx",256);		
	}
	
	public int[] getInitLead() {
		return lead.clone();
	}
	
	public int[] getInitBassline() {
		return bassline.clone();
	}
	
	public int[] getInitDrum() {
		return drum.clone();
	}
	
	public int[] getInitMulti() {
		return multi.clone();
	}
	
	public int[] getInitEnsemble() {
		return ensemble.clone();
	}
	
	public int[] loadPatch(String filename, int patchSize) {
		int[] patch = new int[patchSize];
		try {
			byte[] data = loadData(filename);
			if (data != null) {
				String s = MidiUtils.getHexString(data).replace(" ", "");		
				int dsi = 20; // Index where real patch data starts
				for (int i = 0; i < patchSize; i++) {
					String s1 = s.substring((4 * i) + dsi + 1, (4 * i) + dsi + 2);
					String s2 = s.substring((4 * i) + dsi + 3, (4 * i) + dsi + 4);
					int temp = Integer.parseInt(s1 + s2, 16);
					patch[i] = temp;
				}
			}
		} catch (IOException e) {
			JOptionPane.showMessageDialog(null,	"An error has occurred while reading built-in init patch!", "Error", JOptionPane.ERROR_MESSAGE);
		}
		return patch;
	}
	
	private byte[] loadData(String s) throws IOException {
		byte[] b = null;
		try {
			URL url = this.getClass().getResource("/patches/" + s);		
			File file = new File(url.toURI());
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
		} catch (URISyntaxException e) {
			JOptionPane.showMessageDialog(null,	"An error has occurred while reading built-in init patch!", "Error", JOptionPane.ERROR_MESSAGE);
		}
		return b;
	}
}