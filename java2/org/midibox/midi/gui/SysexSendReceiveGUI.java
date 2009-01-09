/*
 * @(#)SysexSendReceiveGUI.java	beta8	2006/04/23
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
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.File;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.StyledDocument;

import org.midibox.midi.MidiUtils;
import org.midibox.midi.SysexSendReceive;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SimpleFileChooserFilter;

public class SysexSendReceiveGUI extends JPanel implements ActionListener,
		ItemListener, ChangeListener, Observer {

	public static String currentDirectory = "";

	private static JFileChooser fc = null;

	private SysexSendReceive sysexSendReceive;

	// GUI components

	private JButton browseButton;

	private JTextField fileName;

	private JButton startButton;

	private JButton stopButton;

	private JTextPane sysexSendTextArea;

	private JTextPane sysexReceiveTextArea;

	private JSpinner sendBufferSizeSpinner;

	public SysexSendReceiveGUI(SysexSendReceive sysexSendReceive) {
		super(new BorderLayout());

		this.sysexSendReceive = sysexSendReceive;
		sysexSendReceive.addObserver(this);

		browseButton = new JButton("SysEx File", ImageLoader
				.getImageIcon("browse.png"));
		fileName = new JTextField();
		fileName.setEditable(false);
		browseButton.setActionCommand("browse");
		browseButton.addActionListener(this);

		startButton = new JButton("Send/Receive");
		startButton.setActionCommand("start");
		startButton.addActionListener(this);
		stopButton = new JButton("Stop");
		stopButton.addActionListener(this);
		stopButton.setActionCommand("stop");

		startButton.setAlignmentX(Component.LEFT_ALIGNMENT);
		stopButton.setAlignmentX(Component.LEFT_ALIGNMENT);

		JPanel fileButtonsPanel = new JPanel(new BorderLayout(2, 2));
		fileButtonsPanel.add(browseButton, BorderLayout.WEST);
		fileButtonsPanel.add(fileName, BorderLayout.CENTER);
		fileButtonsPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

		JPanel controlButtonsPanel = new JPanel(new GridBagLayout());

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;

		gbc.insets = new Insets(5, 5, 5, 5);

		controlButtonsPanel.add(startButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(5, 13, 5, 5);

		controlButtonsPanel.add(stopButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 2, 2, 2);

		gbc.weightx = 1.0;
		controlButtonsPanel.add(new JPanel(), gbc);
		gbc.gridx++;
		gbc.weightx = 0.0;

		gbc.fill = GridBagConstraints.NONE;

		JLabel label = new JLabel("Send Buffer Size (bytes): ");
		controlButtonsPanel.add(label, gbc);
		gbc.gridx++;

		sendBufferSizeSpinner = new JSpinner(new SpinnerNumberModel(
				sysexSendReceive.getSendBufferSize(), 1, 1000, 1));
		sendBufferSizeSpinner.addChangeListener(this);
		controlButtonsPanel.add(sendBufferSizeSpinner, gbc);

		JPanel sendPanel = new JPanel(new GridBagLayout());

		gbc.fill = GridBagConstraints.BOTH;
		gbc.weightx = 1.0;

		sendPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory
				.createEmptyBorder(2, 2, 2, 2), BorderFactory
				.createTitledBorder("Send")));

		sysexSendTextArea = new JTextPane();
		sysexSendTextArea.setBackground(Color.WHITE);
		sysexSendTextArea.setEditable(false);

		StyledDocument doc = sysexSendTextArea.getStyledDocument();

		Style def = StyleContext.getDefaultStyleContext().getStyle(
				StyleContext.DEFAULT_STYLE);

		doc.addStyle("regular", def);
		StyleConstants.setFontFamily(def, "Monospaced");

		JScrollPane sysexSendTextAreaScrollPane = new JScrollPane(
				sysexSendTextArea);
		sysexSendTextAreaScrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

		// Make preferred width smaller than the other panel's minimum width.
		sysexSendTextAreaScrollPane.setPreferredSize(new Dimension(300, 100));

		sendPanel.add(fileButtonsPanel, gbc);
		gbc.gridy++;
		gbc.weighty = 1.0;
		gbc.insets = new Insets(5, 5, 5, 5);
		sendPanel.add(sysexSendTextAreaScrollPane, gbc);

		JPanel receivePanel = new JPanel(new GridBagLayout());
		receivePanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory
				.createEmptyBorder(2, 2, 2, 2), BorderFactory
				.createTitledBorder("Receive")));

		sysexReceiveTextArea = new JTextPane();
		sysexReceiveTextArea.setBackground(Color.WHITE);

		sysexReceiveTextArea.setEditable(false);
		JScrollPane sysexReceiveTextAreaScrollPane = new JScrollPane(
				sysexReceiveTextArea);
		sysexReceiveTextAreaScrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

		// Make preferred width smaller than the other panel's minimum width.
		sysexReceiveTextAreaScrollPane
				.setPreferredSize(new Dimension(300, 100));

		gbc.gridy = 0;
		receivePanel.add(sysexReceiveTextAreaScrollPane, gbc);

		JPanel mainPanel = new JPanel(new GridBagLayout());

		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.insets = new Insets(4, 4, 4, 4);

		mainPanel.add(controlButtonsPanel, gbc);
		gbc.gridy++;

		gbc.weighty = 1.0;
		mainPanel.add(sendPanel, gbc);
		gbc.gridy++;
		mainPanel.add(receivePanel, gbc);

		// add(controlButtonsPanel, BorderLayout.NORTH);
		// add(mainPanel, BorderLayout.CENTER);

		add(new JLabel("*** TODO ***", JLabel.CENTER), BorderLayout.CENTER);

		UIUpdate();
	}

	public void UIUpdate() {

	}

	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand() == "start") {

		} else if (e.getActionCommand() == "stop") {

		} else if (e.getActionCommand() == "browse") {
			onOpenSysexFile();
		}
	}

	protected void onOpenSysexFile() {

		if (fc == null) {
			fc = new JFileChooser(currentDirectory);
			fc.addChoosableFileFilter(new SimpleFileChooserFilter(
					"SysEx files (*.syx)", "syx", "Sysex File"));
		}

		File noFile = new File("");
		File noFiles[] = { noFile };
		fc.setSelectedFile(noFile);
		fc.setSelectedFiles(noFiles);

		int nRetVal = fc.showOpenDialog(this);

		if (nRetVal == JFileChooser.APPROVE_OPTION) {
			File file = fc.getSelectedFile();
			sysexSendReceive.setFile(file);
			currentDirectory = fc.getCurrentDirectory().toString();
		}
	}

	public void itemStateChanged(ItemEvent e) {
		UIUpdate();
	}

	public void stateChanged(ChangeEvent e) {

		UIUpdate();
	}

	public void update(Observable observable, Object object) {
		if (observable == sysexSendReceive) {
			if (object == sysexSendReceive.getFile()) {
				sysexSendTextArea.setText("");
				StyledDocument doc = sysexSendTextArea.getStyledDocument();
				try {
					doc
							.insertString(0, MidiUtils
									.getHexString(sysexSendReceive
											.getSysexFile().getData()), doc
									.getStyle("regular"));
				} catch (Exception e) {

				}
			} else if (object == sysexSendReceive.getReceivedBytes()) {

			}
		}
	}

	public SysexSendReceive getSysexSendReceive() {
		return sysexSendReceive;
	}
}
