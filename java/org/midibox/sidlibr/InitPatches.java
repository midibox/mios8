package org.midibox.sidlibr;

import java.io.IOException;
import java.io.InputStream;

import javax.swing.JOptionPane;

import org.midibox.midi.MidiUtils;
import org.midibox.utils.ResourceLoader;

public class InitPatches {
	private int[] lead;
	private int[] bassline;
	private int[] drum;
	private int[] multi;
	private int[] ensemble;

	public InitPatches() {
		lead = loadPatch("lead.syx", 512);
		bassline = loadPatch("bassline.syx", 512);
		drum = loadPatch("drum.syx", 512);
		multi = loadPatch("multi.syx", 512);
		ensemble = loadPatch("ensemble.syx", 256);
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
					String s1 = s.substring((4 * i) + dsi + 1, (4 * i) + dsi
							+ 2);
					String s2 = s.substring((4 * i) + dsi + 3, (4 * i) + dsi
							+ 4);
					int temp = Integer.parseInt(s1 + s2, 16);
					patch[i] = temp;
				}
			}
		} catch (IOException e) {
			JOptionPane.showMessageDialog(null,
					"An error has occurred while reading built-in init patch!",
					"Error", JOptionPane.ERROR_MESSAGE);
		}
		return patch;
	}

	private byte[] loadData(String s) throws IOException {
		byte[] b = null;
		b = new byte[65536];
		int numBytes = 0;
		InputStream in = null;
		try {
			in = ResourceLoader.getResourceAsStream("patches/" + s);
			numBytes = in.read(b, 0, 65536);
		} finally {
			if (in != null) {
				in.close();
			}
		}
		// TODO: we return an array with 65536 members...
		// it would be better to extract [0:numBytes], but I don't know how to
		// do this w/o creating a new array...
		return b;
	}
}