/*
 * @(#)LCDMessageGUI.java	beta8	2006/04/23
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
import java.awt.Color;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;
import java.util.Observable;
import java.util.Observer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFormattedTextField;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.JTextArea;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingConstants;
import javax.swing.border.BevelBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.PlainDocument;

import org.midibox.mios.LCDMessage;
import org.midibox.utils.gui.HexFormatterFactory;

public class LCDMessageGUI extends JPanel implements ActionListener,
		ChangeListener, Observer {

	private LCDMessage lcdMessage;

	private JPanel deviceIDPanel;

	private JLabel deviceIDLabel;

	private JSpinner deviceIDSpinner;

	private JPanel lcdPanel;

	private JLabel widthLabel;

	private JLabel heightLabel;

	private JSpinner lcdWidthSpinner;

	private JSpinner lcdHeightSpinner;

	private JTextArea lcdTextArea;

	private JRadioButton clearScreen;

	private JRadioButton currentPos;

	private JRadioButton coOrds;

	private JLabel posXLabel;

	private JLabel posYLabel;

	private JSpinner posX;

	private JSpinner posY;

	private JButton sendMessage;

	private JButton stopMessage;

	private JCheckBox loop;

	public LCDMessageGUI(LCDMessage lcdMessage) {
		super(new BorderLayout());

		this.lcdMessage = lcdMessage;
		lcdMessage.addObserver(this);

		JScrollPane scrollPane = new JScrollPane(createLCDPanel());
		scrollPane
				.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		scrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		add(scrollPane, BorderLayout.CENTER);

		updateLCDControls();
	}

	private JPanel createLCDPanel() {

		lcdPanel = new JPanel(new GridBagLayout());
		lcdPanel.setOpaque(true);

		lcdTextArea = new JTextArea(lcdMessage.getLcdHeight(), lcdMessage
				.getLcdWidth());

		lcdTextArea.setBackground(Color.GREEN);
		lcdTextArea.setForeground(Color.BLACK);
		lcdTextArea.setBorder(new BevelBorder(BevelBorder.LOWERED));
		lcdTextArea.setFont(new Font("Monospaced", Font.BOLD, 20));
		lcdTextArea.setLineWrap(false);
		lcdTextArea.setWrapStyleWord(false);

		lcdTextArea.addFocusListener(new FocusAdapter() {
			public void focusLost(FocusEvent fe) {
				lcdMessage.setMessageText(lcdTextArea.getText());
			}
		});

		lcdTextArea.setDocument(new PlainDocument() {

			public void insertString(int off, String text, AttributeSet a)
					throws BadLocationException {

				StringBuffer buffer = new StringBuffer(getText(0, getLength()));

				buffer.insert(off, text);

				Pattern p1 = Pattern.compile("(^)(.+)($)", Pattern.MULTILINE);
				Matcher m = p1.matcher(buffer.toString());

				int pos = 0;
				int offset = 0;
				int length1 = buffer.length();

				while (m.find(pos)) {
					pos = m.end();
					buffer.delete(m.start() + offset, m.end() + offset);
					buffer.insert(m.start() + offset, m.group(1)
							+ m.group(2).substring(
									0,
									Math.min(m.group(2).length(), lcdMessage
											.getLcdWidth())) + m.group(3));
					offset = buffer.length() - length1;
				}

				p1 = Pattern.compile("\n");
				m = p1.matcher(buffer.toString());

				pos = 0;
				int noLines = 0;

				while (m.find(pos) && noLines < lcdMessage.getLcdHeight()) {
					noLines++;
					pos = m.end();
				}

				if (noLines >= lcdMessage.getLcdHeight()) {
					buffer.delete(pos - 1, buffer.length());
				}

				remove(0, getLength());
				super.insertString(0, buffer.toString(), a);
				lcdTextArea.setCaretPosition(Math.min(getLength(), off
						+ text.length()));
			}

			public void remove(int offs, int len) throws BadLocationException {
				super.remove(offs, len);
				lcdTextArea.setText(lcdTextArea.getText());
				lcdTextArea.setCaretPosition(offs);
			}
		});

		String message = lcdMessage.getMessageText();

		if (message != null) {

			lcdTextArea.setText(message);
		}

		JPanel buttonPanel = new JPanel(new GridBagLayout());

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;
		gbc.insets = new Insets(4, 4, 4, 4);

		JPanel sizePanel = new JPanel(new GridBagLayout());

		sizePanel.add(widthLabel = new JLabel("LCD Width: "), gbc);
		gbc.gridx++;

		lcdWidthSpinner = new JSpinner(new SpinnerNumberModel(lcdMessage
				.getLcdWidth(), 1, 80, 1));
		lcdWidthSpinner.addChangeListener(this);

		sizePanel.add(lcdWidthSpinner, gbc);
		gbc.gridx++;

		sizePanel.add(heightLabel = new JLabel(" Height: "), gbc);
		gbc.gridx++;

		lcdHeightSpinner = new JSpinner(new SpinnerNumberModel(lcdMessage
				.getLcdHeight(), 1, 8, 1));
		lcdHeightSpinner.addChangeListener(this);
		sizePanel.add(lcdHeightSpinner, gbc);

		gbc.gridx = 0;
		gbc.anchor = GridBagConstraints.WEST;

		ButtonGroup bg = new ButtonGroup();

		clearScreen = new JRadioButton("Clear Screen",
				lcdMessage.getMode() == LCDMessage.CLEAR_SCREEN_MODE);
		clearScreen.addActionListener(this);
		bg.add(clearScreen);
		buttonPanel.add(clearScreen, gbc);
		gbc.gridy++;

		currentPos = new JRadioButton("Current Cursor Position", lcdMessage
				.getMode() == LCDMessage.CURRENT_POS_MODE);
		currentPos.addActionListener(this);
		bg.add(currentPos);
		buttonPanel.add(currentPos, gbc);
		gbc.gridy++;

		coOrds = new JRadioButton("Offset",
				lcdMessage.getMode() == LCDMessage.COORDS_MODE);
		coOrds.addActionListener(this);
		bg.add(coOrds);
		buttonPanel.add(coOrds, gbc);
		gbc.gridy++;

		gbc.fill = GridBagConstraints.NONE;
		JPanel cordsPanel = new JPanel(new GridLayout(2, 2, 4, 4));

		cordsPanel.add(posXLabel = new JLabel("X: ", SwingConstants.RIGHT));
		posX = new JSpinner(new SpinnerNumberModel(lcdMessage.getPosX(), 0,
				127, 1));
		posX.addChangeListener(this);
		cordsPanel.add(posX);

		cordsPanel.add(posYLabel = new JLabel("Y: ", SwingConstants.RIGHT));
		posY = new JSpinner(new SpinnerNumberModel(lcdMessage.getPosY(), 0,
				127, 1));
		posY.addChangeListener(this);
		cordsPanel.add(posY);

		buttonPanel.add(cordsPanel, gbc);

		gbc.gridwidth = 1;
		gbc.gridy++;

		deviceIDPanel = new JPanel(new GridBagLayout());
		GridBagConstraints gbc2 = new GridBagConstraints();

		gbc2.gridx = 0;
		gbc2.gridy = 0;
		gbc2.insets = new Insets(2, 5, 2, 5);
		gbc2.weightx = 1.0;
		gbc2.fill = GridBagConstraints.HORIZONTAL;

		loop = new JCheckBox("Loop message", lcdMessage.isLoop());
		loop.addActionListener(this);
		deviceIDPanel.add(loop, gbc2);
		gbc2.gridx++;

		gbc2.weightx = 0.0;
		gbc2.fill = GridBagConstraints.NONE;

		deviceIDLabel = new JLabel("Device ID:");
		deviceIDPanel.add(deviceIDLabel, gbc2);
		gbc2.gridx++;

		deviceIDSpinner = new JSpinner(new SpinnerNumberModel(lcdMessage
				.getDeviceID(), 0, 127, 1));
		deviceIDSpinner.addChangeListener(this);

		JSpinner.DefaultEditor editor = (JSpinner.DefaultEditor) deviceIDSpinner
				.getEditor();

		JFormattedTextField tf = editor.getTextField();
		tf.setFormatterFactory(new HexFormatterFactory());

		deviceIDPanel.add(deviceIDSpinner, gbc2);
		gbc2.gridx++;

		gbc.fill = GridBagConstraints.HORIZONTAL;
		buttonPanel.add(deviceIDPanel, gbc);
		gbc.gridy++;

		JPanel buttons = new JPanel(new GridLayout(1, 2, 4, 4));

		sendMessage = new JButton("Send LCD Message");
		sendMessage.addActionListener(this);
		buttons.add(sendMessage);

		stopMessage = new JButton("Stop LCD Message");
		stopMessage.addActionListener(this);
		buttons.add(stopMessage, gbc);

		buttonPanel.add(buttons, gbc);

		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;
		gbc.insets = new Insets(4, 4, 4, 4);

		gbc.fill = GridBagConstraints.NONE;
		gbc.anchor = GridBagConstraints.CENTER;

		lcdPanel.add(sizePanel, gbc);
		gbc.gridy++;

		lcdPanel.add(lcdTextArea, gbc);
		gbc.gridy++;

		lcdPanel.add(buttonPanel, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		gbc.fill = GridBagConstraints.VERTICAL;

		lcdPanel.add(new JPanel(), gbc);

		return lcdPanel;
	}

	private void updateLCDControls() {

		boolean isRunning = (!lcdMessage.isCancelled() && !lcdMessage.isDone());

		widthLabel.setEnabled(!isRunning);
		lcdWidthSpinner.setEnabled(!isRunning);

		heightLabel.setEnabled(!isRunning);
		lcdHeightSpinner.setEnabled(!isRunning);

		lcdTextArea.setEnabled(!isRunning);

		sendMessage.setEnabled(!isRunning);

		clearScreen.setEnabled(!isRunning);

		currentPos.setEnabled(!isRunning);

		coOrds.setEnabled(!isRunning);

		posXLabel.setEnabled(lcdMessage.getMode() == LCDMessage.COORDS_MODE
				&& !isRunning);
		posX.setEnabled(lcdMessage.getMode() == LCDMessage.COORDS_MODE
				&& !isRunning);

		posYLabel.setEnabled(lcdMessage.getMode() == LCDMessage.COORDS_MODE
				&& !isRunning);
		posY.setEnabled(lcdMessage.getMode() == LCDMessage.COORDS_MODE
				&& !isRunning);

		loop.setEnabled(!isRunning);

		deviceIDLabel.setEnabled(!isRunning);
		deviceIDSpinner.setEnabled(!isRunning);
	}

	public void stateChanged(ChangeEvent ce) {

		Object source = ce.getSource();

		if (source == lcdWidthSpinner) {
			lcdMessage.setLcdWidth(((Integer) lcdWidthSpinner.getValue())
					.intValue());

		} else if (source == lcdHeightSpinner) {
			lcdMessage.setLcdHeight(((Integer) lcdHeightSpinner.getValue())
					.intValue());

		} else if (source == deviceIDSpinner) {
			lcdMessage.setDeviceID(((Integer) deviceIDSpinner.getValue())
					.intValue());

		} else if (source == posX) {
			lcdMessage.setPosX(((Integer) posX.getValue()).intValue());

		} else if (source == posY) {
			lcdMessage.setPosY(((Integer) posY.getValue()).intValue());

		}
	}

	public void actionPerformed(ActionEvent ae) {

		Object source = ae.getSource();

		if (source == lcdTextArea) {
			lcdMessage.setMessageText(lcdTextArea.getText());

		} else if (source == clearScreen) {
			lcdMessage.setMode(LCDMessage.CLEAR_SCREEN_MODE);

		} else if (source == currentPos) {
			lcdMessage.setMode(LCDMessage.CURRENT_POS_MODE);

		} else if (source == coOrds) {
			lcdMessage.setMode(LCDMessage.COORDS_MODE);

		} else if (source == loop) {
			lcdMessage.setLoop(loop.isSelected());

		} else if (source == sendMessage) {

			Thread t = new Thread() {
				public void run() {
					lcdMessage.createMessage();
				}
			};

			//t.setDaemon(true);
			t.start();

		} else if (source == stopMessage) {

			Thread t = new Thread() {
				public void run() {

					lcdMessage.cancelMessage();
				}
			};

			//t.setDaemon(true);
			t.start();
		}
	}

	public void update(Observable observable, Object object) {

		if (object == LCDMessage.LCD_WIDTH) {
			String text = lcdTextArea.getText();
			lcdTextArea.setText("");
			lcdTextArea.setColumns(lcdMessage.getLcdWidth());
			lcdTextArea.setText(text);
			lcdPanel.setVisible(false);
			lcdPanel.setVisible(true);
			lcdWidthSpinner.setValue(lcdMessage.getLcdWidth());

		} else if (object == LCDMessage.LCD_HEIGHT) {
			String text = lcdTextArea.getText();
			lcdTextArea.setText("");
			lcdTextArea.setRows(lcdMessage.getLcdHeight());
			lcdTextArea.setText(text);
			lcdPanel.setVisible(false);
			lcdPanel.setVisible(true);
			lcdHeightSpinner.setValue(lcdMessage.getLcdHeight());

		} else if (object == LCDMessage.MESSAGE_TEXT) {
			lcdTextArea.setText(lcdMessage.getMessageText());

		} else if (object == LCDMessage.MODE) {
			clearScreen
					.setSelected(lcdMessage.getMode() == LCDMessage.CLEAR_SCREEN_MODE);

			currentPos
					.setSelected(lcdMessage.getMode() == LCDMessage.CURRENT_POS_MODE);

			coOrds.setSelected(lcdMessage.getMode() == LCDMessage.COORDS_MODE);

		} else if (object == LCDMessage.POS_X) {
			posX.setValue(new Integer(lcdMessage.getPosX()));

		} else if (object == LCDMessage.POS_Y) {
			posY.setValue(new Integer(lcdMessage.getPosY()));

		} else if (object == LCDMessage.LOOP) {
			loop.setSelected(lcdMessage.isLoop());

		} else if (object == LCDMessage.DEVICE_ID) {
			deviceIDSpinner.setValue(new Integer(lcdMessage.getDeviceID()));

		}

		updateLCDControls();
	}
}
