package org.midibox.sidedit.gui.ensemble;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;

import org.midibox.sidedit.gui.controls.SIDSysexParameterControlGUI;
import org.midibox.sidedit.gui.controls.SIDSysexParameterControlKnob;

public class SIDGUI extends JPanel {

	public SIDGUI(Vector sidGUIv) {
		setLayout(new BorderLayout());
		this.setOpaque(false);

		JPanel panel1 = new JPanel();
		panel1.setLayout(new BoxLayout(panel1, BoxLayout.Y_AXIS));

		panel1.add(createMain(sidGUIv));
		panel1.add(createFlag(sidGUIv));
		panel1.add(createInv(sidGUIv));
		panel1.add(createFilCal(sidGUIv));		
		panel1.add(createKnobs(sidGUIv));		

		panel1.setOpaque(false);
		this.add(panel1, BorderLayout.NORTH);
	}

	protected JPanel createMain(Vector midiParameterGUI) {
		JPanel mainPanel = new JPanel();
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.X_AXIS));
		mainPanel.setBorder(BorderFactory.createEtchedBorder());
		mainPanel.setBorder(BorderFactory.createTitledBorder("General"));
		mainPanel.setOpaque(false);
		
		mainPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(0));
		mainPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(1));
		int[] snapvals = new int[206];
		for(int i=50;i<256;i++) {
			snapvals[i-50] = i;
		}
		((SIDSysexParameterControlKnob) midiParameterGUI.elementAt(11)).setSnapvals(snapvals);
		((SIDSysexParameterControlGUI)midiParameterGUI.elementAt(11)).setSnap(true);
		mainPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(11));
		((SIDSysexParameterControlKnob) midiParameterGUI.elementAt(12)).setSnapvals(new int[]{0, 1, 2, 3, 4});
		((SIDSysexParameterControlGUI)midiParameterGUI.elementAt(12)).setSnap(true);
		mainPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(12));
		mainPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(2));
		
		return mainPanel;
	}
	
	protected JPanel createFilCal(Vector midiParameterGUI) {
		JPanel filPanel = new JPanel();
		filPanel.setLayout(new GridLayout(2, 2));
		filPanel.setBorder(BorderFactory.createEtchedBorder());
		filPanel.setBorder(BorderFactory.createTitledBorder("Filter calibration"));
		filPanel.setOpaque(false);
		
		// Filter calibration
		filPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(21));
		filPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(23));
		filPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(22));
		filPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(24));
		return filPanel;
	}
	
	protected JPanel createInv(Vector midiParameterGUI) {
		JPanel invPanel = new JPanel();
		invPanel.setLayout(new GridLayout(1, 8));
		invPanel.setBorder(BorderFactory.createEtchedBorder());
		invPanel.setBorder(BorderFactory.createTitledBorder("Invert analog outputs"));
		invPanel.setOpaque(false);
		
		// AOUT inversion flags
		invPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(13));
		invPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(14));
		invPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(15));
		invPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(16));
		invPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(17));
		invPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(18));
		invPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(19));
		invPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(20));
		return invPanel;
	}
	
	protected JPanel createFlag(Vector midiParameterGUI) {
		JPanel flagPanel = new JPanel();
		flagPanel.setLayout(new GridLayout(1, 8));
		flagPanel.setBorder(BorderFactory.createEtchedBorder());
		flagPanel.setBorder(BorderFactory.createTitledBorder("Flags"));
		flagPanel.setOpaque(false);
		
		//Flags
		flagPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(3));
		flagPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(4));
		flagPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(5));
		flagPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(6));
		flagPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(7));
		flagPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(8));
		flagPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(9));
		flagPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(10));
		return flagPanel;
	}

	protected JPanel createKnobs(Vector midiParameterGUI) {
		JPanel knobsPanel = new JPanel(new GridLayout(1, 6));
		knobsPanel.setOpaque(false);
		knobsPanel.add(createKnob(midiParameterGUI, 0, "Instrument 1"));
		knobsPanel.add(createKnob(midiParameterGUI, 5, "Instrument 2"));
		knobsPanel.add(createKnob(midiParameterGUI, 10, "Instrument 3"));
		knobsPanel.add(createKnob(midiParameterGUI, 15, "Instrument 4"));
		knobsPanel.add(createKnob(midiParameterGUI, 20, "Instrument 5"));
		knobsPanel.add(createKnob(midiParameterGUI, 25, "Instrument 6"));
		return knobsPanel;
	}

	protected JPanel createKnob(Vector midiParameterGUI, int offset, String s) {
		JPanel knobPanel = new JPanel();
		knobPanel.setLayout(new BoxLayout(knobPanel, BoxLayout.Y_AXIS));
		knobPanel.setBorder(BorderFactory.createEtchedBorder());
		knobPanel.setBorder(BorderFactory.createTitledBorder(s));
		knobPanel.setOpaque(false);

		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(25 + offset));
		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(26 + offset));
		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(27 + offset));
		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(28 + offset));
		knobPanel.add((SIDSysexParameterControlGUI) midiParameterGUI.elementAt(29 + offset));

		return knobPanel;
	}
}
