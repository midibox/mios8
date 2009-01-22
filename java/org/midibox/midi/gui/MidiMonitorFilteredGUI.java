/*
 * @(#)MidiMonitorFilteredGUI.java	beta8	2006/04/23
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
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.LinkedList;
import java.util.Observable;
import java.util.Observer;

import javax.sound.midi.MetaMessage;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.SysexMessage;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.JToolBar;
import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.StyledDocument;

import org.midibox.midi.MidiMessageReceived;
import org.midibox.midi.MidiMonitorFiltered;
import org.midibox.utils.gui.DialogOwner;
import org.midibox.utils.gui.ImageLoader;

public class MidiMonitorFilteredGUI extends JPanel implements ActionListener,
		Observer {
	private MidiMonitorFiltered midiMonitorFiltered;

	private JTextPane textArea;

	private JButton clearButton;

	private JButton filterButton;

	private JDialog filterDialog;

	int maxLength = 50000;

	public MidiMonitorFilteredGUI(MidiMonitorFiltered midiFilteredPortMonitor) {

		super(new BorderLayout());
		this.midiMonitorFiltered = midiFilteredPortMonitor;
		midiFilteredPortMonitor.getMidiMonitor().addObserver(this);

		// Create a text area.
		textArea = new JTextPane();
		textArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
		textArea.setBackground(Color.BLACK);
		textArea.setForeground(Color.LIGHT_GRAY);
		textArea.setEditable(false);
		JScrollPane areaScrollPane = new JScrollPane(textArea);
		areaScrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		areaScrollPane.setPreferredSize(new Dimension(100, 100));

		JToolBar toolbar = new JToolBar("Tool Bar", JToolBar.HORIZONTAL);
		toolbar.setRollover(true);
		toolbar.setFloatable(false);

		filterButton = new JButton(ImageLoader.getImageIcon("filter.png"));
		filterButton.addActionListener(this);
		filterButton.setMargin(new Insets(2, 2, 2, 2));
		filterButton.setToolTipText("Filter Messages");
		toolbar.add(filterButton);

		clearButton = new JButton(ImageLoader.getImageIcon("delete.png"));
		clearButton.addActionListener(this);
		clearButton.setMargin(new Insets(2, 2, 2, 2));
		clearButton.setToolTipText("Clear Screen");
		toolbar.add(clearButton);

		add(toolbar, BorderLayout.NORTH);
		add(areaScrollPane, BorderLayout.CENTER);

		// Voice Messages
		setColour(ShortMessage.NOTE_OFF, new Color(0x008000));
		setColour(ShortMessage.NOTE_ON, Color.GREEN);
		setColour(ShortMessage.POLY_PRESSURE, Color.YELLOW);
		setColour(ShortMessage.CONTROL_CHANGE, Color.CYAN);
		setColour(ShortMessage.PROGRAM_CHANGE, Color.YELLOW);
		setColour(ShortMessage.CHANNEL_PRESSURE, Color.YELLOW);
		setColour(ShortMessage.PITCH_BEND, new Color(0x8080FF));

		setColour(SysexMessage.SYSTEM_EXCLUSIVE, Color.LIGHT_GRAY);

		// System Common
		setColour(ShortMessage.MIDI_TIME_CODE, Color.BLUE);
		setColour(ShortMessage.SONG_POSITION_POINTER, Color.BLUE);
		setColour(ShortMessage.SONG_SELECT, Color.BLUE);
		setColour(ShortMessage.TUNE_REQUEST, Color.BLUE);

		// System Realtime
		setColour(ShortMessage.TIMING_CLOCK, Color.MAGENTA);
		setColour(ShortMessage.START, Color.MAGENTA);
		setColour(ShortMessage.CONTINUE, Color.MAGENTA);
		setColour(ShortMessage.STOP, Color.MAGENTA);
		setColour(ShortMessage.ACTIVE_SENSING, Color.MAGENTA);
		setColour(ShortMessage.SYSTEM_RESET, Color.MAGENTA);

		// Meta Messages
		setColour(MetaMessage.META, Color.WHITE);

		setSize(300, 300);
		setLocation(0, 0);
	}

	public void setColour(int status, Color colour) {

		Style def = StyleContext.getDefaultStyleContext().getStyle(
				StyleContext.DEFAULT_STYLE);

		StyledDocument doc = textArea.getStyledDocument();

		Style style = doc.addStyle("" + status, def);
		StyleConstants.setForeground(style, colour);
	}

	public void check() {

		LinkedList messages = midiMonitorFiltered.getMidiMonitor()
				.getReceivedMessages();

		if (!messages.isEmpty()) {

			while (!messages.isEmpty()) {

				MidiMessageReceived message = (MidiMessageReceived) messages
						.removeFirst();

				String string = midiMonitorFiltered.getMidiMonitor()
						.decodeMessage(message.midiMessage, message.timeStamp);

				// return value is null if message should be filtered (required
				// for SysEx messages)
				if (string != null) {
					StyledDocument doc = textArea.getStyledDocument();

					int status = message.midiMessage.getStatus();

					if (message.midiMessage instanceof ShortMessage) {
						status = ((ShortMessage) message.midiMessage)
								.getCommand();
					}

					try {
						doc.remove(0, Math.max(doc.getLength() - maxLength, 0));

						doc.insertString(doc.getLength(), string + "\n", doc
								.getStyle("" + status));

					} catch (BadLocationException ble) {
						System.out.println(ble.getMessage());
					}
				}
			}
			textArea.setCaretPosition(textArea.getDocument().getLength());
		}
	}

	private void showFilterDialog() {
		if (filterDialog == null) {
			final MidiFilterGUI midiFilterGUI = new MidiFilterGUI(
					midiMonitorFiltered.getMidiFilter());

			filterDialog = new JDialog(DialogOwner.getFrame(),
					"MIDI Filter Properties", false);
			filterDialog.setContentPane(midiFilterGUI);
			filterDialog.pack();
			filterDialog.setLocationRelativeTo(this);
			filterDialog.setVisible(true);

			filterDialog.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					filterDialog = null;
				}
			});
		}
		filterDialog.requestFocus();
	}

	public void update(Observable observable, Object object) {
		check();
	}

	public void actionPerformed(ActionEvent ae) {
		Object source = ae.getSource();

		if (source == filterButton) {
			showFilterDialog();
		} else if (source == clearButton) {
			textArea.setText("");
		}
	}

	public Dimension getMinimumSize() {
		return getPreferredSize();
	}
}