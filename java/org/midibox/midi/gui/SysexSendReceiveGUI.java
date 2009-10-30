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
import java.util.StringTokenizer;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.JOptionPane;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.StyledDocument;
import javax.swing.text.Segment;

import org.midibox.midi.MidiUtils;
import org.midibox.midi.SysexSendReceive;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SimpleFileChooserFilter;

public class SysexSendReceiveGUI extends JPanel implements ActionListener,
		ItemListener, ChangeListener, Observer {

	private static String currentDirectory = "";

	private static JFileChooser fc = null;

	private SysexSendReceive sysexSendReceive;

	// GUI components

	private JButton browseButton;

	private JTextField fileName;

	private JButton startButton;

	private JButton stopButton;

	private JTextPane sysexSendTextArea;

	private JTextPane sysexReceiveTextArea;

	private JSpinner sendDelayTimeSpinner;

	public SysexSendReceiveGUI(SysexSendReceive sysexSendReceive) {
		super(new BorderLayout());

		this.sysexSendReceive = sysexSendReceive;
		sysexSendReceive.addObserver(this);

		browseButton = new JButton("SysEx File", ImageLoader
				.getImageIcon("open.png"));
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

		JLabel label = new JLabel("Delay between F0 (ms): ");
		controlButtonsPanel.add(label, gbc);
		gbc.gridx++;

		sendDelayTimeSpinner = new JSpinner(new SpinnerNumberModel(
				sysexSendReceive.getSendDelayTime(), 1, 1000, 1));
		sendDelayTimeSpinner.addChangeListener(this);
		controlButtonsPanel.add(sendDelayTimeSpinner, gbc);

		JPanel sendPanel = new JPanel(new GridBagLayout());

		gbc.fill = GridBagConstraints.BOTH;
		gbc.weightx = 1.0;

		sendPanel.setBorder(BorderFactory.createCompoundBorder(BorderFactory
				.createEmptyBorder(2, 2, 2, 2), BorderFactory
				.createTitledBorder("Send")));

		sysexSendTextArea = new JTextPane();
		sysexSendTextArea.setBackground(Color.WHITE);
		sysexSendTextArea.setEditable(true);

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

		sysexReceiveTextArea.setEditable(true);
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

		add(controlButtonsPanel, BorderLayout.NORTH);
		add(mainPanel, BorderLayout.CENTER);

		UIUpdate();
	}

	public void UIUpdate() {
		boolean bUploading = (!sysexSendReceive.isCancelled() && !sysexSendReceive.isDone());

		startButton.setEnabled(!bUploading);
		browseButton.setEnabled(!bUploading);
		sendDelayTimeSpinner.setEnabled(!bUploading);
		stopButton.setEnabled(bUploading);
		sysexSendTextArea.setEnabled(!bUploading);
		sysexReceiveTextArea.setEnabled(!bUploading);
	}


	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand() == "start") {
			byte[] sysexData = new byte[sysexSendReceive.getSendBufferSize()];
			int sysexDataOffset = 0;

			StyledDocument doc = sysexSendTextArea.getStyledDocument();
			int len = doc.getLength();

			if( len > 0 ) {
				int nleft = len;
				Segment text = new Segment();
				int offs = 0;
				text.setPartialReturn(true);   
				while (nleft > 0) {
					try {
						doc.getText(offs, nleft, text);
					} catch (Exception ex) {
						JOptionPane.showMessageDialog(null, "Error while parsing hex digits!", "Syntax Error", JOptionPane.ERROR_MESSAGE);
						return;
					}

					StringTokenizer st = new StringTokenizer(text.toString());
					while (st.hasMoreTokens()) {
						int value;

						try { 
							value = (int)java.lang.Integer.parseInt(st.nextToken(), 16); 
							sysexData[sysexDataOffset++] = (byte)value;
						} catch(Exception ex) {
							JOptionPane.showMessageDialog(null, "Found invalid hexadecimal digit!", "Syntax Error", JOptionPane.ERROR_MESSAGE);
							return;
						}
					}

					nleft -= text.count;
					offs += text.count;
				}

				if( sysexDataOffset > 0 ) {
					final byte[] finalSysexData = sysexData;
					final int finalSysexDataOffset = sysexDataOffset;

					Thread t = new Thread() {
							public void run() {
								sysexSendReceive.startSend(finalSysexData, finalSysexDataOffset);
							}
						};
					
					t.start();
				}
			}

		} else if (e.getActionCommand() == "stop") {
			sysexSendReceive.cancel();

		} else if (e.getActionCommand() == "browse") {
			onOpenSysexFile();
		}

		UIUpdate();
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

		Object source = e.getSource();

		if (source == sendDelayTimeSpinner) {
			sysexSendReceive.setSendDelayTime(((Integer) sendDelayTimeSpinner.getValue()).intValue());
		}

		UIUpdate();
	}

	public void update(Observable observable, Object object) {
		if (observable == sysexSendReceive) {
			if (object == sysexSendReceive.WORKER ) {
			} else if (object == sysexSendReceive.getFile()) {
				fileName.setText(sysexSendReceive.getFile().getPath());
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

			UIUpdate();
		}
	}

	public SysexSendReceive getSysexSendReceive() {
		return sysexSendReceive;
	}

	public static String getCurrentDirectory() {
		return currentDirectory;
	}

	public static void setCurrentDirectory(String currentDirectory) {
		SysexSendReceiveGUI.currentDirectory = currentDirectory;
	}

}
