package org.midibox.sidedit.gui;

import javax.swing.JToggleButton;

import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SeqButtonUI;

import javax.swing.JButton;

import java.util.Observable;
import java.util.Observer;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JPanel;

public class SequencerButton extends JButton implements Observer, ActionListener {
	private int realValue;
	SIDSysexParameterControl midiParameter1;
	SIDSysexParameterControl midiParameter2;
	
	public SequencerButton(SIDSysexParameterControl midiParameter1, SIDSysexParameterControl midiParameter2) {		
		this.midiParameter1 = midiParameter1;
		this.midiParameter2 = midiParameter2;
		
		setUI(new SeqButtonUI());		
		addActionListener(this);
		midiParameter1.addObserver(this);
		midiParameter2.addObserver(this);
		updateGraphics();
	}
	
	public void updateGraphics() {
		if ((midiParameter1.getMidiValue()==0) && (midiParameter2.getMidiValue()==0)) {
			realValue = 0;						
		} else if ((midiParameter1.getMidiValue()==1) && (midiParameter2.getMidiValue()==0)) {
			realValue = 1;			
		} else if ((midiParameter1.getMidiValue()==0) && (midiParameter2.getMidiValue()==1)) {
			realValue = 2;			
		} else if ((midiParameter1.getMidiValue()==1) && (midiParameter2.getMidiValue()==1)) {
			realValue = 3;			
		}
		((SeqButtonUI)getUI()).setValue(realValue);	
		repaint();
	}
	
	public void update(Observable observable, Object object) {
		updateGraphics();		
	}
	
	public void actionPerformed(ActionEvent ae) {
		Object source = ae.getSource();
		if (source == this) {
			realValue = (realValue+1)%4;
			//setImage(realValue);
			switch(realValue) {
				case 0: midiParameter1.setMidiValue(0,true);midiParameter2.setMidiValue(0,true); break;
				case 1: midiParameter1.setMidiValue(1,true);midiParameter2.setMidiValue(0,true); break;
				case 2: midiParameter1.setMidiValue(0,true);midiParameter2.setMidiValue(1,true); break;
				case 3: midiParameter1.setMidiValue(1,true);midiParameter2.setMidiValue(1,true); break;
			}
		}
	}
	
	
	
	
}
