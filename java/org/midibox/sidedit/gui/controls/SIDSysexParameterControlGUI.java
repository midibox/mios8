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

package org.midibox.sidedit.gui.controls;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import org.midibox.sidedit.SIDSysexParameter;
import org.midibox.sidedit.SIDSysexParameterControl;
import org.midibox.sidedit.gui.MBSIDV2EditorGUI;
import org.midibox.utils.gui.FontLoader;

public class SIDSysexParameterControlGUI extends JPanel implements Observer,
		ActionListener, MouseListener {

	protected SIDSysexParameterControl midiParameter;

	protected Vector midiParameters;

	protected boolean showLabel;

	protected String labelLocation;

	protected boolean valueBelow;

	protected boolean showValue;

	protected JLabel midiParameterLabel;

	protected JTextField valueField;

	protected StringBuffer valueBuffer;

	protected Color valueFieldColor = new Color(150, 20, 0);

	protected Color labelFieldColor = Color.BLACK;

	protected boolean update = true;

	protected JDialog midiParameterPropertiesDialog;

	public SIDSysexParameterControlGUI(SIDSysexParameterControl midiParameter,
			boolean showLabel, String labelLocation, boolean valueBelow,
			boolean showValue) {

		super(new BorderLayout());
		this.midiParameter = midiParameter;
		this.showLabel = showLabel;
		this.labelLocation = labelLocation;
		this.valueBelow = valueBelow;
		this.showValue = showValue;
		this.midiParameters = new Vector();

		midiParameter.addObserver(this);

		valueBuffer = new StringBuffer();

		if ((showLabel) || (showValue)) {
			add(createLabelPanel(), labelLocation);
		}
		addMouseListener(this);
		setOpaque(false);
	}

	protected JPanel createLabelPanel() {
		JPanel labelPanel = new JPanel(new GridLayout(
				(valueBelow ? (showLabel ? 2 : 1) : 1), (valueBelow ? 1
						: (showLabel ? 2 : 1))));
		labelPanel.setOpaque(false);

		if (showLabel) {
			midiParameterLabel = new JLabel(midiParameter.getMidiName()
					.toUpperCase(),
					(showValue) ? ((valueBelow ? SwingConstants.CENTER
							: SwingConstants.RIGHT)) : SwingConstants.CENTER);
			midiParameterLabel.setOpaque(false);
			midiParameterLabel.setFont(FontLoader.getFont("uni05_53.ttf", 8f));
			midiParameterLabel.setForeground(labelFieldColor);
			labelPanel.add(midiParameterLabel);
		}

		if (showValue) {
			valueField = new JTextField();
			valueField.setHorizontalAlignment(JTextField.CENTER);
			valueField.setFont(FontLoader.getFont("uni05_53.ttf", 8f));
			valueField.setForeground(valueFieldColor);
			valueField.addActionListener(this);
			valueField.setOpaque(false);
			valueField.setBorder(null);
			labelPanel.add(valueField);
		}

		return labelPanel;
	}

	protected JTextField createValueField() {
		valueField = new JTextField();
		return valueField;
	}

	public void setValueFieldColor(Color valueFieldColor) {
		this.valueFieldColor = valueFieldColor;
		this.valueField.setForeground(valueFieldColor);
	}

	public void setLabelFieldColor(Color labelFieldColor) {
		this.labelFieldColor = labelFieldColor;
		this.midiParameterLabel.setForeground(labelFieldColor);
	}

	public SIDSysexParameterControl getMidiParameter() {
		return midiParameter;
	}

	public void addMidiParameter(SIDSysexParameterControl midiPar) {
		midiParameters.add(midiPar);
		midiPar.addObserver(this);
	}

	public void removeMidiParameter(SIDSysexParameterControl midiPar) {
		midiParameters.remove(midiPar);
		midiPar.deleteObserver(this);
	}

	public void updateGraphics() {
		if (valueField != null) {
			updateValueField();
		}
	}

	public void useAlias(boolean b) {
		midiParameter.useAlias = b;
	}

	private boolean snapExist(int i) {
		boolean b = false;
		for (int c = 0; c < midiParameter.snapvals.length; c++) {
			if (midiParameter.snapvals[c] == i) {
				b = true;
				break;
			}
		}
		return b;
	}

	public void setSnap(boolean b) {
		if (b) { // Turn on snap
			midiParameter.useAlias = true;
			midiParameter.snap = true;

			if (!snapExist(midiParameter.getMidiValue())) { // If current midi
				// value is not part
				// of the snap
				// values reset to
				// snapvals[0];
				midiParameter.setMidiValue(midiParameter.snapvals[0], true);
				for (int c = 0; c < midiParameters.size(); c++) {
					SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
							.elementAt(c);
					mp.snap = true;
					mp.useAlias = true;
				}
			} else { // Keep current midi value, only update labels
				for (int c = 0; c < midiParameters.size(); c++) {
					SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
							.elementAt(c);
					mp.snap = true;
					mp.useAlias = true;
				}
				updateGraphics();
			}

		} else { // Turn off snap
			midiParameter.useAlias = false;
			midiParameter.snap = false;
			for (int c = 0; c < midiParameters.size(); c++) {
				SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
						.elementAt(c);
				mp.snap = false;
				mp.useAlias = false;
			}
			updateGraphics();
		}
	}

	public void updateValueField() {
		String newval = new String();
		valueBuffer.delete(0, valueBuffer.length());
		valueBuffer.insert(0, midiParameter.getMidiValue());

		if ((midiParameter.getValAlias() == null) || !midiParameter.useAlias) {
			newval = valueBuffer.toString();
		} else {
			newval = (midiParameter.getValAlias())[Integer.valueOf(valueBuffer
					.toString())];
		}
		valueField.setText(newval);
	}

	public int findAliasMatch(String s) {
		String[] alias = midiParameter.getValAlias();
		int i = 0;
		while ((i < alias.length) && (!alias[i].equals(s))) {
			i++;
		}
		return i;
	}

	public void actionPerformed(ActionEvent ae) {
		Object source = ae.getSource();
		if (source == valueField) {
			try {
				int newval = 0;
				if ((midiParameter.getValAlias() == null)
						|| !midiParameter.useAlias) {
					newval = Integer.parseInt(valueField.getText());
				} else {
					newval = findAliasMatch(valueField.getText());
				}

				midiParameter.setMidiValue(newval, true);

				for (int c = 0; c < midiParameters.size(); c++) {
					SIDSysexParameterControl mp = (SIDSysexParameterControl) midiParameters
							.elementAt(c);
					mp.setMidiValue(newval, false);
				}

			} catch (Exception e) {
				midiParameter.setMidiValue(midiParameter.getMidiValue(), true);
			}
		}
	}

	public void update(Observable observable, Object object) {
		if (observable == midiParameter) {
			if (object != SIDSysexParameter.VALUE) {
				if (showLabel) {
					midiParameterLabel.setText(midiParameter.getMidiName()
							.toUpperCase());
				}
			}
			updateGraphics();
		}
	}

	public void mousePressed(MouseEvent e) {
	}

	public void mouseReleased(MouseEvent e) {
	}

	public void mouseEntered(MouseEvent e) {
		if (midiParameter.getTooltipListener().getClass() == MBSIDV2EditorGUI.class) {
			((MBSIDV2EditorGUI) midiParameter.getTooltipListener())
					.setTooltip(midiParameter.getTooltip());
		}
	}

	public void mouseExited(MouseEvent e) {
		if (midiParameter.getTooltipListener().getClass() == MBSIDV2EditorGUI.class) {
			((MBSIDV2EditorGUI) midiParameter.getTooltipListener())
					.setTooltip("");
		}
	}

	public void mouseClicked(MouseEvent e) {
	}

}