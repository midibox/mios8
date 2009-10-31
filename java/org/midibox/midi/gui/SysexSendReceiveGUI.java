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
import java.util.LinkedList;

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

	private JButton sendBrowseButton;
	private JTextField sendFileName;
	private JButton sendStartButton;
	private JButton sendStopButton;
	private JButton sendClearButton;

	private JButton receiveBrowseButton;
	private JTextField receiveFileName;
	private JButton receiveStartButton;
	private JButton receiveStopButton;
	private JButton receiveClearButton;

	private JTextPane sysexSendTextArea;

	private JTextPane sysexReceiveTextArea;

	private JSpinner sendDelayTimeSpinner;

	public SysexSendReceiveGUI(SysexSendReceive sysexSendReceive) {
		super(new BorderLayout());

		this.sysexSendReceive = sysexSendReceive;
		sysexSendReceive.addObserver(this);

		sendBrowseButton = new JButton("Open File", ImageLoader
				.getImageIcon("open.png"));
		sendFileName = new JTextField();
		sendFileName.setEditable(false);
		sendBrowseButton.setActionCommand("sendBrowse");
		sendBrowseButton.addActionListener(this);

		sendStartButton = new JButton("Start");
		sendStartButton.setActionCommand("sendStart");
		sendStartButton.addActionListener(this);
		sendStartButton.setAlignmentX(Component.LEFT_ALIGNMENT);
		sendStopButton = new JButton("Stop");
		sendStopButton.addActionListener(this);
		sendStopButton.setActionCommand("sendStop");
		sendStopButton.setAlignmentX(Component.LEFT_ALIGNMENT);
		sendClearButton = new JButton("Clear");
		sendClearButton.addActionListener(this);
		sendClearButton.setActionCommand("sendClear");
		sendClearButton.setAlignmentX(Component.LEFT_ALIGNMENT);


		receiveBrowseButton = new JButton("Save File", ImageLoader
				.getImageIcon("open.png"));
		receiveFileName = new JTextField();
		receiveFileName.setEditable(false);
		receiveBrowseButton.setActionCommand("receiveBrowse");
		receiveBrowseButton.addActionListener(this);

		receiveStartButton = new JButton("Start");
		receiveStartButton.setActionCommand("receiveStart");
		receiveStartButton.addActionListener(this);
		receiveStartButton.setAlignmentX(Component.LEFT_ALIGNMENT);
		receiveStopButton = new JButton("Stop");
		receiveStopButton.addActionListener(this);
		receiveStopButton.setActionCommand("receiveStop");
		receiveStopButton.setAlignmentX(Component.LEFT_ALIGNMENT);
		receiveClearButton = new JButton("Clear");
		receiveClearButton.addActionListener(this);
		receiveClearButton.setActionCommand("receiveClear");
		receiveClearButton.setAlignmentX(Component.LEFT_ALIGNMENT);



		JPanel sendFileButtonsPanel = new JPanel(new BorderLayout(2, 2));
		sendFileButtonsPanel.add(sendBrowseButton, BorderLayout.WEST);
		sendFileButtonsPanel.add(sendFileName, BorderLayout.CENTER);
		sendFileButtonsPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

		JPanel sendControlButtonsPanel = new JPanel(new GridBagLayout());
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;

		gbc.insets = new Insets(5, 5, 5, 5);

		sendControlButtonsPanel.add(sendStartButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(5, 13, 5, 5);

		sendControlButtonsPanel.add(sendStopButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(5, 13, 5, 5);

		sendControlButtonsPanel.add(sendClearButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 2, 2, 2);

		gbc.weightx = 1.0;
		sendControlButtonsPanel.add(new JPanel(), gbc);
		gbc.gridx++;
		gbc.weightx = 0.0;

		gbc.fill = GridBagConstraints.NONE;

		JLabel label = new JLabel("Delay between SysEx messages (ms): ");
		sendControlButtonsPanel.add(label, gbc);
		gbc.gridx++;

		sendDelayTimeSpinner = new JSpinner(new SpinnerNumberModel(
				sysexSendReceive.getSendDelayTime(), 1, 1000, 1));
		sendDelayTimeSpinner.addChangeListener(this);
		sendControlButtonsPanel.add(sendDelayTimeSpinner, gbc);



		JPanel receiveFileButtonsPanel = new JPanel(new BorderLayout(2, 2));
		receiveFileButtonsPanel.add(receiveBrowseButton, BorderLayout.WEST);
		receiveFileButtonsPanel.add(receiveFileName, BorderLayout.CENTER);
		receiveFileButtonsPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

		JPanel receiveControlButtonsPanel = new JPanel(new GridBagLayout());
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;

		gbc.insets = new Insets(5, 5, 5, 5);

		receiveControlButtonsPanel.add(receiveStartButton, gbc);
		gbc.gridx++;
		gbc.insets = new Insets(5, 13, 5, 5);
		receiveControlButtonsPanel.add(receiveStopButton, gbc);
		gbc.gridx++;
		receiveControlButtonsPanel.add(receiveClearButton, gbc);



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

		gbc.insets = new Insets(5, 5, 5, 5);
		gbc.gridy = 0;
		gbc.gridx = 0;
		sendPanel.add(sendFileButtonsPanel, gbc);
		gbc.gridy++;
		gbc.weighty = 1.0;
		sendPanel.add(sendControlButtonsPanel, gbc);
		gbc.gridy++;
		gbc.weighty = 1.0;
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

		gbc.insets = new Insets(5, 5, 5, 5);
		gbc.gridy = 0;
		gbc.gridx = 0;
		gbc.weighty = 1.0;
		receivePanel.add(receiveFileButtonsPanel, gbc);
		gbc.gridx = 0;
		gbc.gridy++;
		gbc.weighty = 1.0;
		gbc.fill = GridBagConstraints.NONE;
		gbc.anchor = GridBagConstraints.WEST;
		receivePanel.add(receiveControlButtonsPanel, gbc);
		gbc.anchor = GridBagConstraints.CENTER;
		gbc.fill = GridBagConstraints.BOTH;
		gbc.gridy++;
		gbc.weighty = 1.0;
		receivePanel.add(sysexReceiveTextAreaScrollPane, gbc);

		JPanel mainPanel = new JPanel(new GridBagLayout());

		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.insets = new Insets(4, 4, 4, 4);

		gbc.weighty = 1.0;
		mainPanel.add(sendPanel, gbc);
		gbc.gridy++;
		mainPanel.add(receivePanel, gbc);

		add(mainPanel, BorderLayout.CENTER);

		UIUpdate();
	}

	public void UIUpdate() {
		boolean bUploading = (!sysexSendReceive.isSendCancelled() && !sysexSendReceive.isSendDone());
		boolean bReceiving = !sysexSendReceive.isReceiveStopped();

		sendBrowseButton.setEnabled(!bUploading);
		sendStartButton.setEnabled(!bUploading);
		sendStopButton.setEnabled(bUploading);
		sendClearButton.setEnabled(!bUploading);
		sendDelayTimeSpinner.setEnabled(!bUploading);
		sysexSendTextArea.setEnabled(!bUploading);

		receiveBrowseButton.setEnabled(!bReceiving);
		receiveStartButton.setEnabled(!bReceiving);
		receiveStopButton.setEnabled(bReceiving);
		//receiveClearButton.setEnabled(!bReceiving); // better to allow clear while receiving
		sysexReceiveTextArea.setEnabled(!bReceiving);
	}


	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand() == "sendStart") {
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

		} else if (e.getActionCommand() == "sendStop") {
			sysexSendReceive.sendCancel();

		} else if (e.getActionCommand() == "sendClear") {
			sysexSendTextArea.setText("");

		} else if (e.getActionCommand() == "sendBrowse") {
			onOpenSysexFile();

		} else if (e.getActionCommand() == "receiveStart") {
			sysexSendReceive.setReceiveStopped(false);

		} else if (e.getActionCommand() == "receiveStop") {
			sysexSendReceive.setReceiveStopped(true);

		} else if (e.getActionCommand() == "receiveClear") {
			sysexReceiveTextArea.setText("");

		} else if (e.getActionCommand() == "receiveBrowse") {
			onSaveSysexFile();
		}

		UIUpdate();
	}

	protected void onOpenSysexFile() {

		if (fc == null) {
			fc = new JFileChooser(currentDirectory);
			fc.addChoosableFileFilter(new SimpleFileChooserFilter(
					"SysEx files (*.syx)", "syx", "SysEx File"));
		}

		File noFile = new File("");
		File noFiles[] = { noFile };
		fc.setSelectedFile(noFile);
		fc.setSelectedFiles(noFiles);

		int nRetVal = fc.showOpenDialog(this);

		if (nRetVal == JFileChooser.APPROVE_OPTION) {
			File file = fc.getSelectedFile();
			sysexSendReceive.setSendFile(file);
			currentDirectory = fc.getCurrentDirectory().toString();
		}
	}

	protected void onSaveSysexFile() {

		if (fc == null) {
			fc = new JFileChooser(currentDirectory);
			fc.addChoosableFileFilter(new SimpleFileChooserFilter(
					"SysEx files (*.syx)", "syx", "SysEx File"));
		}

		File noFile = new File("");
		File noFiles[] = { noFile };
		fc.setSelectedFile(noFile);
		fc.setSelectedFiles(noFiles);

		int nRetVal = fc.showSaveDialog(this);

		if (nRetVal == JFileChooser.APPROVE_OPTION) {
			File file = fc.getSelectedFile();
			sysexSendReceive.setReceiveFile(file);
			currentDirectory = fc.getCurrentDirectory().toString();

			int maxSysexDataLen = 200000; // TODO: find better method... linked list?
			byte[] sysexData = new byte[maxSysexDataLen];
			int sysexDataOffset = 0;

			StyledDocument doc = sysexReceiveTextArea.getStyledDocument();
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

						if( sysexDataOffset >= maxSysexDataLen ) {
							JOptionPane.showMessageDialog(null, "Too many SysEx bytes!", "Parsing Error", JOptionPane.ERROR_MESSAGE);
							return;
						}

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
					if( !sysexSendReceive.writeSysexFile(file, sysexData, sysexDataOffset) ) {
						JOptionPane.showMessageDialog(null, "Error while writing file!", "File Access Error", JOptionPane.ERROR_MESSAGE);
						return;
					}
				}
			}
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
			} else if (object == sysexSendReceive.getSendFile()) {
				sendFileName.setText(sysexSendReceive.getSendFile().getPath());
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
				StyledDocument doc = sysexReceiveTextArea.getStyledDocument();

				LinkedList l = sysexSendReceive.getReceivedBytes();
				int i;
				for(i=0; i<l.size(); ++i) {
					byte[] data = (byte[])l.get(i);
					try {
						if( doc.getLength() > 0 )
							doc.insertString(doc.getLength(), "\n", doc.getStyle("regular"));
						doc.insertString(doc.getLength(), "F0 ", doc.getStyle("regular"));
						doc.insertString(doc.getLength(), MidiUtils.getHexString(data), doc.getStyle("regular"));
					} catch (Exception e) {
					}
				}
				sysexSendReceive.clearReceivedBytes();

			} else if (object == sysexSendReceive.getReceiveFile()) {
				receiveFileName.setText(sysexSendReceive.getReceiveFile().getPath());
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
