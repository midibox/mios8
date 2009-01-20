/*
 * @(#)MemoryReadWriteGUI.java	beta8	2006/04/23
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

package org.midibox.mios.gui;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.util.Observable;
import java.util.Observer;

import javax.swing.ButtonGroup;
import javax.swing.JFormattedTextField;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

import org.midibox.mios.MemoryReadWrite;
import org.midibox.utils.gui.HexFormatterFactory;

public class MemoryReadWriteGUI extends JPanel implements Observer {

	private MemoryReadWrite readWriteTask;

	private JPanel deviceIDPanel;

	private JLabel deviceIDLabel;

	private JLabel bankstickNoLabel;

	private JSpinner deviceIDSpinner;

	private JSpinner bankstickNoSpinner;

	private JRadioButton displayHexButton;

	private JRadioButton displayDecButton;

	public MemoryReadWriteGUI(MemoryReadWrite readWriteTask) {

		super(new BorderLayout());

		this.readWriteTask = readWriteTask;

		// add(createIDPanel(), BorderLayout.NORTH);
		// add(createReadWriteTab(), BorderLayout.CENTER);

		add(new JLabel("*** TODO ***", JLabel.CENTER));
	}

	private JPanel createReadWriteTab() {
		JPanel readWritePanel = new JPanel(new BorderLayout());
		return readWritePanel;
	}

	private JPanel createIDPanel() {
		deviceIDPanel = new JPanel();
		deviceIDPanel.setLayout(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();

		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;

		gbc.insets = new Insets(2, 5, 2, 5);

		deviceIDLabel = new JLabel("Device ID:");
		deviceIDPanel.add(deviceIDLabel, gbc);
		gbc.gridx++;

		deviceIDSpinner = new JSpinner(new SpinnerNumberModel(0, 0, 127, 1));
		JSpinner.DefaultEditor editor = (JSpinner.DefaultEditor) deviceIDSpinner
				.getEditor();
		JFormattedTextField tf = editor.getTextField();
		tf.setFormatterFactory(new HexFormatterFactory());
		deviceIDPanel.add(deviceIDSpinner, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 15, 2, 5);

		bankstickNoLabel = new JLabel("BankStick No:");
		deviceIDPanel.add(bankstickNoLabel, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 5, 2, 5);

		bankstickNoSpinner = new JSpinner(new SpinnerNumberModel(1, 1, 8, 1));
		deviceIDPanel.add(bankstickNoSpinner, gbc);
		gbc.gridx++;

		gbc.weightx = 1.0;
		deviceIDPanel.add(new JPanel(), gbc);
		gbc.gridx++;

		gbc.weightx = 0.0;

		ButtonGroup bg2 = new ButtonGroup();

		displayHexButton = new JRadioButton("Hex", true);
		displayHexButton.setToolTipText("Hexidecimal Format");
		// displayHexButton.addActionListener(this);
		bg2.add(displayHexButton);
		deviceIDPanel.add(displayHexButton, gbc);
		gbc.gridx++;

		displayDecButton = new JRadioButton("Dec", false);
		displayDecButton.setToolTipText("Decimal Format");
		// displayDecButton.addActionListener(this);
		bg2.add(displayDecButton);
		deviceIDPanel.add(displayDecButton, gbc);
		gbc.gridx++;

		return deviceIDPanel;
	}

	public void update(Observable observable, Object object) {

	}
}
