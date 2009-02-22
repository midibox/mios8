/*
 * @(#)MidiParameterControlGUI.java	beta8	2006/04/23
 *
 * Copyright (C) 2008    Adam King (adamjking@optusnet.com.au)
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

package org.midibox.midi.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import org.midibox.midi.MidiParameter;
import org.midibox.midi.MidiParameterControl;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.FontLoader;

public class MidiParameterControlGUI extends JPanel implements Observer,
		ActionListener, MouseListener {

	protected MidiParameterControl midiParameterControl;

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

	public MidiParameterControlGUI(MidiParameterControl midiParameterControl,
			boolean showLabel, String labelLocation, boolean valueBelow,
			boolean showValue) {

		super(new BorderLayout());

		this.midiParameterControl = midiParameterControl;
		this.showLabel = showLabel;
		this.labelLocation = labelLocation;
		this.valueBelow = valueBelow;
		this.showValue = showValue;

		midiParameterControl.addObserver(this);

		valueBuffer = new StringBuffer();

		if (showLabel) {
			add(createLabelPanel(), labelLocation);
		}

		addMouseListener(this);

		setOpaque(false);
	}

	protected JPopupMenu createPopMenu() {
		JPopupMenu popupMenu = new JPopupMenu(midiParameterControl.getMidiName());

		JMenuItem menuItem = new JMenuItem("MIDI Properties");
		menuItem.setActionCommand("midiproperties");
		menuItem.addActionListener(this);
		popupMenu.add(menuItem);

		return popupMenu;
	}

	protected JPanel createLabelPanel() {

		JPanel labelPanel = new JPanel((showValue) ? new GridLayout(
				(valueBelow ? 2 : 1), (valueBelow ? 1 : 2)) : new GridLayout(1,
				1));
		labelPanel.setOpaque(false);

		midiParameterLabel = new JLabel(midiParameterControl.getMidiName()
				.toUpperCase(),
				(showValue) ? ((valueBelow ? SwingConstants.CENTER
						: SwingConstants.RIGHT)) : SwingConstants.CENTER);
		midiParameterLabel.setOpaque(false);
		midiParameterLabel.setFont(FontLoader.getFont("uni05_53.ttf", 8f));
		midiParameterLabel.setForeground(labelFieldColor);
		labelPanel.add(midiParameterLabel);

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

	public MidiParameterControl getMidiParameterControl() {
		return midiParameterControl;
	}

	public void updateGraphics() {
		if (valueField != null) {
			updateValueField();
		}
	}

	public void updateValueField() {
		valueBuffer.delete(0, valueBuffer.length());
		valueBuffer.insert(0, midiParameterControl.getMidiValue());
		valueField.setText(valueBuffer.toString());
	}

	private void showMidiParameterProperties() {
		if (midiParameterPropertiesDialog == null) {
			final MidiParameterControlProperties midiParameterProperties = new MidiParameterControlProperties(
					getMidiParameterControl());

			midiParameterPropertiesDialog = new JDialog(DialogOwner.getFrame(),
					"MIDI Properties", false);
			midiParameterPropertiesDialog
					.setContentPane(midiParameterProperties);
			midiParameterPropertiesDialog.pack();
			midiParameterPropertiesDialog.setLocationRelativeTo(this);
			midiParameterPropertiesDialog.setVisible(true);

			midiParameterPropertiesDialog
					.addWindowListener(new WindowAdapter() {
						public void windowClosing(WindowEvent we) {
							midiParameterControl
									.deleteObserver(midiParameterProperties);
							midiParameterPropertiesDialog = null;
						}
					});
		}
		midiParameterPropertiesDialog.requestFocus();
	}

	public void mouseClicked(MouseEvent e) {

	}

	public void mouseEntered(MouseEvent e) {

	}

	public void mouseExited(MouseEvent e) {

	}

	public void mousePressed(MouseEvent e) {
		if (e.getButton() == MouseEvent.BUTTON3) {
			createPopMenu().show(this, e.getX(), e.getY());
		}
	}

	public void mouseReleased(MouseEvent e) {

	}

	public void actionPerformed(ActionEvent ae) {
		Object source = ae.getSource();
		if (source == valueField) {
			try {
				midiParameterControl.setMidiValue(Integer.parseInt(valueField
						.getText()), true);
			} catch (Exception e) {
				midiParameterControl.setMidiValue(midiParameterControl.getMidiValue(), true);
			}
		} else if (ae.getActionCommand().equals("midilearn")) {

		} else if (ae.getActionCommand().equals("midiproperties")) {
			showMidiParameterProperties();
		}
	}

	public void update(Observable observable, Object object) {
		if (observable == midiParameterControl) {
			if (object != MidiParameter.VALUE) {
				if (showLabel) {
					midiParameterLabel.setText(midiParameterControl.getMidiName()
							.toUpperCase());
				}
			}
			updateGraphics();
		}
	}
}