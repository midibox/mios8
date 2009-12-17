/*
 * @(#)DebugFunctionGUI.java	beta8	2006/04/23
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
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Arrays;
import java.util.EventObject;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.AbstractCellEditor;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JFormattedTextField;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.SpinnerNumberModel;
import javax.swing.TransferHandler;
import javax.swing.border.BevelBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableCellEditor;

import org.midibox.midi.MidiUtils;
import org.midibox.mios.DebugFunction;
import org.midibox.mios.DebugFunctionParameters;
import org.midibox.utils.ResourceLoader;
import org.midibox.utils.gui.HelpPane;
import org.midibox.utils.gui.HexFormatterFactory;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SimpleTransferHandler;

public class DebugFunctionGUI extends JPanel implements ActionListener,
		ChangeListener, FocusListener, Observer {

	private DebugFunction debugFunction;

	private JPanel deviceIDPanel;

	private JLabel deviceIDLabel;

	private JSpinner deviceIDSpinner;

	private boolean showHex = true;

	private JTable debugFunctionsTable;

	private DebugFunctionParametersTransferHandler transferHandler;

	private DefaultTableModel debugFunctionsDataModel;

	private JComboBox debugFunctionsComboBox;

	private JRadioButton debugFunctionsRadio;

	private JRadioButton sramReadRadio;

	private JRadioButton sramWriteRadio;

	private JRadioButton displayHexButton;

	private JRadioButton displayDecButton;

	private JButton startDebugMessage;

	private JButton stopDebugMessage;

	private JLabel delayTimeLabel;

	private JSpinner delayTimeSpinner;

	private JButton cutButton;

	private JButton copyButton;

	private JButton pasteButton;

	private JButton upButton;

	private JButton downButton;

	private JButton deleteButton;

	private JButton helpButton;

	private JPopupMenu popMenu;

	private JLabel sramReadAddressLabel;

	private JLabel sramWriteAddressLabel;

	private JLabel sramReadCounterLabel;

	private JLabel sramWriteDataLabel;

	private JTextField sramReadAddress;

	private JTextField sramWriteAddress;

	private JTextField sramReadCounter;

	private JTextField sramWriteData;

	private JTextArea returnValuesText;

	// private Clipboard systemClipboard;

	private HelpPane helpPane;

	public DebugFunctionGUI(DebugFunction debugTask) {

		super(new BorderLayout());

		this.debugFunction = debugTask;
		debugTask.addObserver(this);

		helpPane = HelpPane.createSingleton(ResourceLoader
				.getResource("help/mios_debug.html"));

		add(createIDPanel(), BorderLayout.NORTH);
		add(createDebugPanel(), BorderLayout.CENTER);

		checkRows();
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

		deviceIDSpinner = new JSpinner(new SpinnerNumberModel(debugFunction
				.getDeviceID(), 0, 127, 1));
		deviceIDSpinner.addChangeListener(this);
		JSpinner.DefaultEditor editor = (JSpinner.DefaultEditor) deviceIDSpinner
				.getEditor();
		JFormattedTextField tf = editor.getTextField();
		tf.setFormatterFactory(new HexFormatterFactory());
		deviceIDPanel.add(deviceIDSpinner, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 5, 2, 5);

		gbc.weightx = 1.0;
		deviceIDPanel.add(new JPanel(), gbc);
		gbc.gridx++;

		gbc.weightx = 0.0;

		ButtonGroup bg2 = new ButtonGroup();

		displayHexButton = new JRadioButton("Hex", true);
		displayHexButton.setToolTipText("Hexidecimal Format");
		displayHexButton.addActionListener(this);
		bg2.add(displayHexButton);
		deviceIDPanel.add(displayHexButton, gbc);
		gbc.gridx++;

		displayDecButton = new JRadioButton("Dec", false);
		displayDecButton.setToolTipText("Decimal Format");
		displayDecButton.addActionListener(this);
		bg2.add(displayDecButton);
		deviceIDPanel.add(displayDecButton, gbc);
		gbc.gridx++;

		return deviceIDPanel;
	}

	private JPanel createDebugPanel() {
		JPanel debugPanel = new JPanel();

		debugPanel.setLayout(new BoxLayout(debugPanel, BoxLayout.Y_AXIS));
		debugPanel.setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));

		JPanel inputPanel = new JPanel(new BorderLayout());

		JPanel functionBuilder = new JPanel(new BorderLayout());
		functionBuilder.setBorder(BorderFactory.createCompoundBorder(
				BorderFactory.createEmptyBorder(5, 5, 5, 5), BorderFactory
						.createTitledBorder("Function Builder")));

		JPanel toolBar = new JPanel();
		toolBar.setLayout(new GridBagLayout());

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;

		gbc.insets = new Insets(2, 2, 0, 2);

		cutButton = new JButton(ImageLoader.getImageIcon("cut.png"));
		cutButton.setToolTipText("Cut row(s)");
		cutButton.addActionListener(this);
		cutButton.setActionCommand("cut");
		cutButton.setMargin(new Insets(2, 2, 2, 2));
		toolBar.add(cutButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 1, 0, 2);

		copyButton = new JButton(ImageLoader.getImageIcon("copy.png"));
		copyButton.setToolTipText("Copy row(s)");
		copyButton.addActionListener(this);
		copyButton.setActionCommand("copy");
		copyButton.setMargin(new Insets(2, 2, 2, 2));
		toolBar.add(copyButton, gbc);
		gbc.gridx++;

		pasteButton = new JButton(ImageLoader.getImageIcon("paste.png"));
		pasteButton.setToolTipText("Paste row(s)");
		pasteButton.addActionListener(this);
		pasteButton.setActionCommand("paste");
		pasteButton.setMargin(new Insets(2, 2, 2, 2));
		toolBar.add(pasteButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 5, 0, 2);

		upButton = new JButton(ImageLoader.getImageIcon("up.png"));
		upButton.setToolTipText("Move row(s) up");
		upButton.addActionListener(this);
		upButton.setActionCommand("up");
		upButton.setMargin(new Insets(2, 2, 2, 2));
		toolBar.add(upButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 1, 0, 2);

		downButton = new JButton(ImageLoader.getImageIcon("down.png"));
		downButton.setToolTipText("Move row(s) down");
		downButton.addActionListener(this);
		downButton.setActionCommand("down");
		downButton.setMargin(new Insets(2, 2, 2, 2));
		toolBar.add(downButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 5, 0, 2);

		deleteButton = new JButton(ImageLoader.getImageIcon("delete.png"));
		deleteButton.setToolTipText("Delete row(s)");
		deleteButton.addActionListener(this);
		deleteButton.setActionCommand("delete");
		deleteButton.setMargin(new Insets(2, 2, 2, 2));
		toolBar.add(deleteButton, gbc);
		gbc.gridx++;

		helpButton = new JButton(ImageLoader.getImageIcon("help.png"));
		helpButton.setToolTipText("Lookup MIOS Functions reference");
		helpButton.addActionListener(this);
		helpButton.setActionCommand("help");
		helpButton.setMargin(new Insets(2, 2, 2, 2));
		toolBar.add(helpButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(2, 2, 2, 2);
		gbc.weightx = 1.0;
		toolBar.add(new JLabel(), gbc);
		gbc.gridx++;
		gbc.weightx = 0.0;

		delayTimeLabel = new JLabel("Delay (ms):");
		toolBar.add(delayTimeLabel, gbc);
		gbc.gridx++;

		delayTimeSpinner = new JSpinner(new SpinnerNumberModel(debugFunction
				.getDelayTime(), 0, 2000, 1));
		delayTimeSpinner.addChangeListener(this);
		toolBar.add(delayTimeSpinner, gbc);

		functionBuilder.add(toolBar, BorderLayout.NORTH);

		Object[] functionNames = debugFunction.getMiosVectorNames().toArray();
		Arrays.sort(functionNames);

		debugFunctionsComboBox = new JComboBox(functionNames);

		debugFunctionsDataModel = new DebugFunctionParametersTableModel(
				debugFunction.getDebugFunctionParameters());

		debugFunctionsTable = new JTable(debugFunctionsDataModel);

		debugFunctionsTable.setDefaultRenderer(Integer.class,
				new DebugFunctionParametersTableCellRenderer());

		debugFunctionsTable.setDefaultEditor(Integer.class,
				new DebugFunctionParametersDefaultCellEditor());

		debugFunctionsTable.getColumnModel().getColumn(0).setCellEditor(
				new DebugFunctionParametersDropDownCellEditor());

		debugFunctionsTable.getColumnModel().getColumn(0).setPreferredWidth(
				debugFunctionsComboBox.getPreferredSize().width);

		debugFunctionsTable.setPreferredScrollableViewportSize(new Dimension(
				debugFunctionsTable.getPreferredScrollableViewportSize().width,
				debugFunctionsComboBox.getPreferredSize().height * 3));

		debugFunctionsTable.setRowHeight(debugFunctionsComboBox
				.getPreferredSize().height);

		debugFunctionsTable.setRowSelectionAllowed(true);

		debugFunctionsTable.setColumnSelectionAllowed(false);

		transferHandler = new DebugFunctionParametersTransferHandler();

		popMenu = new JPopupMenu();

		JMenuItem menuItem = new JMenuItem("Cut");
		menuItem.addActionListener(this);
		menuItem.setActionCommand("cut");
		popMenu.add(menuItem);

		menuItem = new JMenuItem("Copy");
		menuItem.addActionListener(this);
		menuItem.setActionCommand("copy");
		popMenu.add(menuItem);

		menuItem = new JMenuItem("Paste");
		menuItem.addActionListener(this);
		menuItem.setActionCommand("paste");
		popMenu.add(menuItem);

		popMenu.addSeparator();

		menuItem = new JMenuItem("Move Up");
		menuItem.addActionListener(this);
		menuItem.setActionCommand("up");
		popMenu.add(menuItem);

		menuItem = new JMenuItem("Move Down");
		menuItem.addActionListener(this);
		menuItem.setActionCommand("down");
		popMenu.add(menuItem);

		popMenu.addSeparator();

		menuItem = new JMenuItem("Delete");
		menuItem.addActionListener(this);
		menuItem.setActionCommand("delete");
		popMenu.add(menuItem);

		popMenu.addSeparator();

		menuItem = new JMenuItem("Help");
		menuItem.addActionListener(this);
		menuItem.setActionCommand("help");
		popMenu.add(menuItem);

		debugFunctionsTable.addMouseListener(new MouseAdapter() {
			public void mouseReleased(MouseEvent me) {
				if (debugFunctionsRadio.isSelected()
						&& (debugFunction.isCancelled() || debugFunction
								.isDone())) {
					if (me.getButton() == MouseEvent.BUTTON3) {
						int index = debugFunctionsTable.rowAtPoint(me
								.getPoint());
						int[] selectedRows = debugFunctionsTable
								.getSelectedRows();
						boolean currentRowSelected = false;
						for (int r = 0; r < selectedRows.length; r++) {
							if (selectedRows[r] == index) {
								currentRowSelected = true;
							}
						}
						if (!currentRowSelected) {
							debugFunctionsTable.setRowSelectionInterval(index,
									index);
						}
						popMenu.show(debugFunctionsTable, me.getX(), me.getY());
					}
				}
			}
		});

		debugFunctionsTable.addKeyListener(new KeyAdapter() {
			KeyStroke cut = KeyStroke.getKeyStroke(KeyEvent.VK_X,
					ActionEvent.CTRL_MASK, true);

			KeyStroke copy = KeyStroke.getKeyStroke(KeyEvent.VK_C,
					ActionEvent.CTRL_MASK, true);

			// KeyStroke paste =
			// KeyStroke.getKeyStroke(KeyEvent.VK_V,ActionEvent.CTRL_MASK,false);

			public void keyReleased(KeyEvent ke) {
				if (debugFunctionsRadio.isSelected()
						&& (debugFunction.isCancelled() || debugFunction
								.isDone())) {
					int keyCode = ke.getKeyCode();

					if (keyCode == KeyEvent.VK_DELETE) {
						deleteRows();
					} else if (keyCode == cut.getKeyCode()) {
						// cutRows();
					} else if (keyCode == copy.getKeyCode()) {
						copyRows();
					}// else if (keyCode == paste.getKeyCode()) {
					// pasteRows();
					// }
				}
			}
		});

		JScrollPane fbscrollPane = new JScrollPane(debugFunctionsTable);
		fbscrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		fbscrollPane.setBorder(BorderFactory.createCompoundBorder(BorderFactory
				.createEmptyBorder(2, 2, 2, 2), new BevelBorder(
				BevelBorder.LOWERED)));

		debugFunctionsTable.setBackground(Color.WHITE);
		fbscrollPane.getViewport().setBackground(Color.WHITE);
		functionBuilder.add(fbscrollPane);

		inputPanel.add(functionBuilder, BorderLayout.CENTER);

		JPanel sram = new JPanel(new GridLayout(2, 1));

		JPanel sramRead = new JPanel(new GridBagLayout());
		gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;
		gbc.insets = new Insets(2, 2, 2, 2);

		sramRead.setBorder(BorderFactory.createCompoundBorder(BorderFactory
				.createEmptyBorder(5, 5, 5, 5), BorderFactory
				.createTitledBorder("SRAM Read")));

		sramReadAddressLabel = new JLabel("Address:");
		gbc.anchor = GridBagConstraints.WEST;
		sramRead.add(sramReadAddressLabel, gbc);
		gbc.gridx++;

		gbc.anchor = GridBagConstraints.EAST;
		sramReadAddress = new JTextField(changeBase(debugFunction
				.getSramReadAddress(), showHex), 6);
		sramReadAddress.addActionListener(this);
		sramReadAddress.setHorizontalAlignment(JTextField.RIGHT);
		sramReadAddress.addFocusListener(this);
		sramRead.add(sramReadAddress, gbc);
		gbc.gridx = 0;
		gbc.gridy++;

		gbc.anchor = GridBagConstraints.WEST;
		sramReadCounterLabel = new JLabel("No. Bytes:");
		sramRead.add(sramReadCounterLabel, gbc);
		gbc.gridx++;

		gbc.anchor = GridBagConstraints.EAST;
		sramReadCounter = new JTextField(changeBase(debugFunction
				.getSramReadCounter(), showHex), 6);
		sramReadCounter.addActionListener(this);
		sramReadCounter.setHorizontalAlignment(JTextField.RIGHT);
		sramReadCounter.addFocusListener(this);
		sramRead.add(sramReadCounter, gbc);

		gbc.gridy++;
		gbc.weighty = 1.0;
		gbc.fill = GridBagConstraints.VERTICAL;
		sramRead.add(Box.createVerticalGlue(), gbc);

		JPanel sramWrite = new JPanel(new GridBagLayout());

		sramWrite.setBorder(BorderFactory.createCompoundBorder(BorderFactory
				.createEmptyBorder(5, 5, 5, 5), BorderFactory
				.createTitledBorder("SRAM Write")));

		gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;
		gbc.insets = new Insets(2, 2, 2, 2);

		gbc.anchor = GridBagConstraints.WEST;
		sramWriteAddressLabel = new JLabel("Address:");
		sramWrite.add(sramWriteAddressLabel, gbc);
		gbc.gridx++;

		gbc.anchor = GridBagConstraints.EAST;
		sramWriteAddress = new JTextField(changeBase(debugFunction
				.getSramWriteAddress(), showHex), 6);
		sramWriteAddress.addActionListener(this);
		sramWriteAddress.setHorizontalAlignment(JTextField.RIGHT);
		sramWriteAddress.addFocusListener(this);
		sramWrite.add(sramWriteAddress, gbc);
		gbc.gridx = 0;
		gbc.gridy++;

		gbc.anchor = GridBagConstraints.WEST;
		sramWriteDataLabel = new JLabel("Data:");
		sramWrite.add(sramWriteDataLabel, gbc);
		gbc.gridx++;

		gbc.anchor = GridBagConstraints.EAST;
		sramWriteData = new JTextField(changeBase(debugFunction
				.getSramWriteData(), showHex), 6);
		sramWriteData.addActionListener(this);
		sramWriteData.setHorizontalAlignment(JTextField.RIGHT);
		sramWriteData.addFocusListener(this);
		sramWrite.add(sramWriteData, gbc);

		gbc.gridy++;
		gbc.weighty = 1.0;
		gbc.fill = GridBagConstraints.VERTICAL;
		sramWrite.add(Box.createVerticalGlue(), gbc);

		sram.add(sramRead);
		sram.add(sramWrite);

		inputPanel.add(sram, BorderLayout.EAST);

		JPanel returnValues = new JPanel(new BorderLayout());
		returnValues.setBorder(BorderFactory.createCompoundBorder(BorderFactory
				.createEmptyBorder(5, 5, 5, 5), BorderFactory
				.createTitledBorder("Return Values")));
		returnValuesText = new JTextArea();
		returnValuesText.setLineWrap(true);
		returnValuesText.setWrapStyleWord(true);
		returnValuesText.setEditable(false);
		returnValuesText.setBackground(Color.WHITE);
		returnValuesText.setFont(new Font("Monospaced", Font.PLAIN, 12));
		JScrollPane returnValuesScrollPane = new JScrollPane(returnValuesText);
		returnValuesScrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		returnValuesScrollPane.setPreferredSize(debugFunctionsTable
				.getPreferredScrollableViewportSize());
		returnValues.add(returnValuesScrollPane);

		JPanel buttonPanel = new JPanel();
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));

		startDebugMessage = new JButton("Start");
		startDebugMessage.addActionListener(this);
		buttonPanel.add(startDebugMessage);

		buttonPanel.add(Box.createHorizontalStrut(4));

		stopDebugMessage = new JButton("Stop");
		stopDebugMessage.addActionListener(this);
		buttonPanel.add(stopDebugMessage);

		buttonPanel.add(Box.createHorizontalStrut(4));

		ButtonGroup bg = new ButtonGroup();

		debugFunctionsRadio = new JRadioButton("Function Builder", true);
		bg.add(debugFunctionsRadio);
		debugFunctionsRadio.addActionListener(this);
		buttonPanel.add(debugFunctionsRadio);

		buttonPanel.add(Box.createHorizontalStrut(4));

		sramReadRadio = new JRadioButton("SRAM Read ", false);
		bg.add(sramReadRadio);
		sramReadRadio.addActionListener(this);
		buttonPanel.add(sramReadRadio);

		buttonPanel.add(Box.createHorizontalStrut(4));

		sramWriteRadio = new JRadioButton("SRAM Write", false);
		bg.add(sramWriteRadio);
		sramWriteRadio.addActionListener(this);
		buttonPanel.add(sramWriteRadio);

		buttonPanel.add(Box.createHorizontalGlue());

		buttonPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

		debugPanel.add(buttonPanel);
		debugPanel.add(inputPanel);
		debugPanel.add(returnValues);

		updateDebugControls();
		return debugPanel;
	}

	private void changeBaseValues(boolean toHex) {

		updateDebugControls();

		sramReadAddress.setText(changeBase(debugFunction.getSramReadAddress(),
				showHex));

		sramReadCounter.setText(changeBase(debugFunction.getSramReadCounter(),
				showHex));

		sramWriteAddress.setText(changeBase(
				debugFunction.getSramWriteAddress(), showHex));

		sramWriteData.setText(changeBase(debugFunction.getSramWriteData(),
				showHex));

		debugFunctionsDataModel.fireTableDataChanged();

		StringBuffer buffer = new StringBuffer(returnValuesText.getText());
		Pattern p1, p2;

		if (!toHex) {
			p1 = Pattern.compile("(\\s|^+)([0-9A-F]+)(\\s|$+)");
			p2 = Pattern.compile("(\n|^+)([0-9A-F]+)(:+)");

		} else {
			p1 = Pattern.compile("(\\s|^+)([0-9]+)(\\s|$+)");
			p2 = Pattern.compile("(\n|^+)([0-9]+)(:+)");
		}

		Matcher m = p1.matcher(buffer.toString());

		int pos = 0;
		int offset = 0;
		int length1 = buffer.length();

		while (m.find(pos)) {
			pos = m.end() - 1;
			buffer.delete(m.start() + offset, m.end() + offset);
			buffer.insert(m.start() + offset, m.group(1)
					+ changeBaseByte(decodeString(m.group(2), !toHex), toHex)
					+ m.group(3));
			offset = buffer.length() - length1;
		}

		Matcher m2 = p2.matcher(buffer.toString());

		pos = 0;
		offset = 0;
		length1 = buffer.length();

		while (m2.find(pos)) {
			StringBuffer tempBuffer = new StringBuffer(m2.group(2).replaceAll(
					".", "0"));
			String tempValue = changeBase(decodeString(m2.group(2), !toHex),
					toHex);

			tempBuffer.replace(tempBuffer.length() - tempValue.length(),
					tempBuffer.length(), tempValue);

			pos = m2.end() - 1;
			buffer.delete(m2.start() + offset, m2.end() + offset);
			buffer.insert(m2.start() + offset, m2.group(1)
					+ tempBuffer.toString() + m2.group(3));
			offset = buffer.length() - length1;
		}

		returnValuesText.setText(buffer.toString());
	}

	private String changeBase(int val, boolean toHex) {
		if (toHex) {
			return Integer.toHexString(val).toUpperCase();
		} else {
			return Integer.toString(val);
		}
	}

	private String changeBaseByte(int val, boolean toHex) {
		if (toHex) {
			return MidiUtils.getHexString(val);
		} else {
			return MidiUtils.getDecimalString(val);
		}
	}

	private int decodeString(String string, boolean toHex) {
		if (string == null) {
			return 0;
		} else {
			try {
				if (toHex) {
					string = string.replaceAll("^[0Oo][Xx]{1}", "");
					string = string.replaceAll("^#{1}", "");
					return Integer.parseInt(string, 16);
				} else {
					string = string.replaceAll("^0+", "");
					string = string.replaceAll("^[x|X]{1}", "0x");
					return Integer.decode(string).intValue();
				}
			} catch (NumberFormatException e) {
				return 0;
			}
		}
	}

	private void updateDebugControls() {

		boolean isRunning = (!debugFunction.isCancelled() && !debugFunction
				.isDone());

		deviceIDLabel.setEnabled(!isRunning);
		deviceIDSpinner.setEnabled(!isRunning);

		displayHexButton.setEnabled(!isRunning);
		displayDecButton.setEnabled(!isRunning);

		debugFunctionsTable.setEnabled(debugFunctionsRadio.isSelected()
				&& !isRunning);
		debugFunctionsTable.setDragEnabled(debugFunctionsRadio.isSelected()
				&& !isRunning);
		debugFunctionsTable.setTransferHandler((debugFunctionsRadio
				.isSelected() && !isRunning) ? transferHandler : null);

		debugFunctionsComboBox.setEnabled(debugFunctionsRadio.isSelected()
				&& !isRunning);

		cutButton.setEnabled(debugFunctionsRadio.isSelected() && !isRunning);
		copyButton.setEnabled(debugFunctionsRadio.isSelected() && !isRunning);
		pasteButton.setEnabled(debugFunctionsRadio.isSelected() && !isRunning);
		upButton.setEnabled(debugFunctionsRadio.isSelected() && !isRunning);
		downButton.setEnabled(debugFunctionsRadio.isSelected() && !isRunning);
		deleteButton.setEnabled(debugFunctionsRadio.isSelected() && !isRunning);
		helpButton.setEnabled(debugFunctionsRadio.isSelected() && !isRunning);
		delayTimeLabel.setEnabled(debugFunctionsRadio.isSelected()
				&& !isRunning);
		delayTimeLabel.setEnabled(debugFunctionsRadio.isSelected()
				&& !isRunning);
		delayTimeSpinner.setEnabled(debugFunctionsRadio.isSelected()
				&& !isRunning);

		sramReadAddressLabel.setEnabled(sramReadRadio.isSelected()
				&& !isRunning);
		sramReadAddress.setEnabled(sramReadRadio.isSelected() && !isRunning);
		sramReadCounterLabel.setEnabled(sramReadRadio.isSelected()
				&& !isRunning);
		sramReadCounter.setEnabled(sramReadRadio.isSelected() && !isRunning);

		sramWriteAddressLabel.setEnabled(sramWriteRadio.isSelected()
				&& !isRunning);
		sramWriteAddress.setEnabled(sramWriteRadio.isSelected() && !isRunning);
		sramWriteDataLabel
				.setEnabled(sramWriteRadio.isSelected() && !isRunning);
		sramWriteData.setEnabled(sramWriteRadio.isSelected() && !isRunning);

		startDebugMessage.setEnabled(!isRunning);
		stopDebugMessage.setEnabled(isRunning);
		debugFunctionsRadio.setEnabled(!isRunning);
		sramReadRadio.setEnabled(!isRunning);
		sramWriteRadio.setEnabled(!isRunning);
	}

	private void cutRows() {
		Clipboard systemClipboard = Toolkit.getDefaultToolkit()
				.getSystemClipboard();
		transferHandler.exportToClipboard(debugFunctionsTable, systemClipboard,
				TransferHandler.MOVE);
	}

	private void copyRows() {
		Clipboard systemClipboard = Toolkit.getDefaultToolkit()
				.getSystemClipboard();
		transferHandler.exportToClipboard(debugFunctionsTable, systemClipboard,
				TransferHandler.COPY);
	}

	private void pasteRows() {
		Clipboard systemClipboard = Toolkit.getDefaultToolkit()
				.getSystemClipboard();
		transferHandler.importData(debugFunctionsTable, systemClipboard
				.getContents(this));
	}

	private void moveRowsUp() {
		moveRows(true);
	}

	private void moveRowsDown() {
		moveRows(false);
	}

	private void moveRows(boolean up) {
		int[] selectedRows = debugFunctionsTable.getSelectedRows();
		int numRows = selectedRows.length;

		if (numRows > 0) {

			DebugFunctionParameters[] params = new DebugFunctionParameters[numRows];

			for (int r = 0; r < numRows; r++) {
				params[r] = (DebugFunctionParameters) debugFunction
						.getDebugFunctionParameters()
						.elementAt(selectedRows[r]);
			}

			int addIndex;
			int insertIndex;

			if (up) {
				addIndex = (selectedRows[0] - 1);
				insertIndex = Math.max(addIndex, 0);
			} else {
				addIndex = (selectedRows[numRows - 1] + 2);
				insertIndex = Math.min(addIndex, debugFunctionsTable
						.getRowCount());
			}

			int addCount = numRows;

			for (int r = numRows - 1; r >= 0; r--) {
				debugFunction.insertDebugFunctionParameters(params[r], Math
						.max(insertIndex, 0));
			}

			if (addCount > 0) {
				for (int i = 0; i < selectedRows.length; i++) {
					if (selectedRows[i] > addIndex) {
						selectedRows[i] += addCount;
					}
				}
			}

			debugFunction.removeDebugFunctionParameters(selectedRows);

			if (up) {
				debugFunctionsTable.setRowSelectionInterval(insertIndex,
						insertIndex + (selectedRows.length - 1));
			} else {
				debugFunctionsTable.setRowSelectionInterval(insertIndex - 1,
						insertIndex - 1 - (selectedRows.length - 1));
			}
		}
	}

	private void deleteRows() {
		int[] selectedRows = debugFunctionsTable.getSelectedRows();

		for (int r = selectedRows.length - 1; r >= 0; r--) {
			debugFunction.removeDebugFunctionParameters(selectedRows[r]);
		}
	}

	public void showHelp() {
		String filename = ResourceLoader.getResource("help/mios_fun.html")
				.toString();

		int[] rows = debugFunctionsTable.getSelectedRows();
		if (rows.length > 0) {
			Integer value = (Integer) debugFunctionsTable.getModel()
					.getValueAt(rows[0], 0);
			if (value != null) {
				if (debugFunction.getMiosVectorValues().contains(value)) {
					filename += ("#" + debugFunction.getMiosVectorNames()
							.elementAt(
									debugFunction.getMiosVectorValues()
											.indexOf(value)));
				} else {
					filename += "#top";
				}
			}
		} else {
			filename += "#top";
		}
		URL url;
		try {
			url = new URL(filename);
		} catch (MalformedURLException murle) {
			url = ResourceLoader.getResource("help/mios_fun.html");
		}
		helpPane.goToURL(url);
	}

	private void checkRows() {

		int rowCount = debugFunctionsTable.getModel().getRowCount();

		if (rowCount < 1) {
			debugFunction.createDebugFunctionParameters();
		} else {
			int value = ((Integer) debugFunctionsTable.getModel().getValueAt(
					rowCount - 1, 0)).intValue();
			if (value != 0) {
				debugFunction.createDebugFunctionParameters();
			}
		}
	}

	public void stateChanged(ChangeEvent ce) {

		Object source = ce.getSource();

		if (source == deviceIDSpinner) {
			debugFunction.setDeviceID(((Integer) deviceIDSpinner.getValue())
					.intValue());
		} else if (source == delayTimeSpinner) {
			debugFunction.setDelayTime(((Integer) delayTimeSpinner.getValue())
					.intValue());
		}
	}

	public void actionPerformed(ActionEvent ae) {
		Object source = ae.getSource();

		if ((source == displayDecButton && showHex)
				|| (source == displayHexButton && !showHex)) {

			showHex = displayHexButton.isSelected();
			changeBaseValues(showHex);

		} else if (source == debugFunctionsRadio || source == sramReadRadio
				|| source == sramWriteRadio) {

			if (debugFunctionsRadio.isSelected()) {
				debugFunction.setMode(DebugFunction.DEBUG_FUNCTIONS_MODE);

			} else if (sramReadRadio.isSelected()) {
				debugFunction.setMode(DebugFunction.SRAM_READ_MODE);

			} else if (sramWriteRadio.isSelected()) {
				debugFunction.setMode(DebugFunction.SRAM_WRITE_MODE);
			}

		} else if (source == startDebugMessage) {

			if (debugFunction.getMode() != DebugFunction.SRAM_READ_MODE
					|| (debugFunction.getMode() == DebugFunction.SRAM_READ_MODE && debugFunction
							.getSramReadCounter() > 0)) {
				returnValuesText.setText("");

				Thread t = new Thread() {
					public void run() {
						debugFunction.createDebugFunctionsTask(showHex);
					}
				};

				// t.setDaemon(true);
				t.start();

			} else {
				JOptionPane.showMessageDialog(this,
						"Number of bytes to read has to be greater than 0",
						"SRAM Read Error", JOptionPane.ERROR_MESSAGE);
				return;
			}

		} else if (source == stopDebugMessage) {

			debugFunction.cancel();

		} else if (ae.getActionCommand().equals("cut")) {
			cutRows();
		} else if (ae.getActionCommand().equals("copy")) {
			copyRows();
		} else if (ae.getActionCommand().equals("paste")) {
			pasteRows();
		} else if (ae.getActionCommand().equals("up")) {
			moveRowsUp();
		} else if (ae.getActionCommand().equals("down")) {
			moveRowsDown();
		} else if (ae.getActionCommand().equals("delete")) {
			deleteRows();
		} else if (ae.getActionCommand().equals("help")) {
			showHelp();

		} else if (source == sramReadAddress) {
			debugFunction.setSramReadAddress(decodeString(sramReadAddress
					.getText(), displayHexButton.isSelected()));

		} else if (source == sramReadCounter) {
			debugFunction.setSramReadCounter(decodeString(sramReadCounter
					.getText(), displayHexButton.isSelected()));

		} else if (source == sramWriteAddress) {
			debugFunction.setSramWriteAddress(decodeString(sramWriteAddress
					.getText(), displayHexButton.isSelected()));

		} else if (source == sramWriteData) {
			debugFunction.setSramWriteData(decodeString(
					sramWriteData.getText(), displayHexButton.isSelected()));
		}
	}

	public void focusGained(FocusEvent fe) {

	}

	public void focusLost(FocusEvent fe) {

		Object source = fe.getSource();

		if (source == sramReadAddress) {
			debugFunction.setSramReadAddress(decodeString(sramReadAddress
					.getText(), displayHexButton.isSelected()));

		} else if (source == sramReadCounter) {
			debugFunction.setSramReadCounter(decodeString(sramReadCounter
					.getText(), displayHexButton.isSelected()));

		} else if (source == sramWriteAddress) {
			debugFunction.setSramWriteAddress(decodeString(sramWriteAddress
					.getText(), displayHexButton.isSelected()));

		} else if (source == sramWriteData) {
			debugFunction.setSramWriteData(decodeString(
					sramWriteData.getText(), displayHexButton.isSelected()));
		}
	}

	public void update(Observable observable, Object object) {

		if (object == DebugFunction.MESSAGES || object == DebugFunction.WORKER) {
			while (!debugFunction.getMessages().isEmpty()) {
				returnValuesText.append(debugFunction.getMessages()
						.removeFirst()
						+ "\n");
				returnValuesText.setCaretPosition(returnValuesText
						.getDocument().getLength());
			}
			updateDebugControls();

		} else if (object == DebugFunction.DEVICE_ID) {

			deviceIDSpinner.setValue(new Integer(debugFunction.getDeviceID()));

		} else if (object == DebugFunction.DELAY_TIME) {
			delayTimeSpinner
					.setValue(new Integer(debugFunction.getDelayTime()));

		} else if (object == DebugFunction.MODE) {
			debugFunctionsRadio
					.setSelected(debugFunction.getMode() == DebugFunction.DEBUG_FUNCTIONS_MODE);

			sramReadRadio
					.setSelected(debugFunction.getMode() == DebugFunction.SRAM_READ_MODE);

			sramWriteRadio
					.setSelected(debugFunction.getMode() == DebugFunction.SRAM_WRITE_MODE);

			updateDebugControls();

		} else if (object == DebugFunction.SRAM_READ_ADDRESS) {
			sramReadAddress.setText(changeBase(debugFunction
					.getSramReadAddress(), showHex));

		} else if (object == DebugFunction.SRAM_READ_COUNTER) {
			sramReadCounter.setText(changeBase(debugFunction
					.getSramReadCounter(), showHex));

		} else if (object == DebugFunction.SRAM_WRITE_ADDRESS) {
			sramWriteAddress.setText(changeBase(debugFunction
					.getSramWriteAddress(), showHex));

		} else if (object == DebugFunction.SRAM_WRITE_DATA) {
			sramWriteData.setText(changeBase(debugFunction.getSramWriteData(),
					showHex));

		} else if (object == DebugFunction.DEBUG_FUNCTION_PARAMETERS) {
			debugFunctionsDataModel.fireTableDataChanged();
			checkRows();
		}
	}

	private class DebugFunctionParametersTableCellRenderer extends
			DefaultTableCellRenderer {

		public Component getTableCellRendererComponent(JTable table,
				Object value, boolean isSelected, boolean hasFocus, int row,
				int column) {

			JLabel label = (JLabel) super.getTableCellRendererComponent(table,
					value, isSelected, hasFocus, row, column);

			if (column == 0) {

				if (debugFunction.getMiosVectorValues().contains(value)) {

					label.setText(debugFunction.getMiosVectorNames().elementAt(
							debugFunction.getMiosVectorValues().indexOf(value))
							.toString());
				} else {
					label.setText("");
				}
				label.setHorizontalAlignment(JLabel.LEFT);
			} else {
				if (showHex) {
					label.setText(Integer.toHexString(
							((Integer) value).intValue()).toUpperCase());
				}
				label.setHorizontalAlignment(JLabel.RIGHT);
			}

			return label;
		}
	}

	private class DebugFunctionParametersDefaultCellEditor extends
			AbstractCellEditor implements TableCellEditor {

		private JTextField textField;

		public DebugFunctionParametersDefaultCellEditor() {
			textField = new JTextField();
			textField.setBorder(null);
			textField.setHorizontalAlignment(JTextField.RIGHT);
		}

		public boolean isCellEditable(EventObject anEvent) {
			if (anEvent instanceof MouseEvent) {
				return ((MouseEvent) anEvent).getClickCount() >= 2;
			}
			return true;
		}

		public Component getTableCellEditorComponent(JTable table,
				Object value, boolean isSelected, int row, int column) {

			if (column > 0 && column < 5) {
				textField.setText((showHex) ? Integer.toHexString(
						((Integer) value).intValue()).toUpperCase()
						: ((Integer) value).toString());

				return textField;

			} else {
				return null;
			}
		}

		public Object getCellEditorValue() {
			return new Integer(decodeString(textField.getText(), showHex));
		}
	}

	private class DebugFunctionParametersDropDownCellEditor extends
			AbstractCellEditor implements TableCellEditor {

		public DebugFunctionParametersDropDownCellEditor() {
			debugFunctionsComboBox.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ae) {
					fireEditingStopped();
				}
			});
		}

		public Object getCellEditorValue() {
			return debugFunction.getMiosVectorValues().elementAt(
					debugFunction.getMiosVectorNames().indexOf(
							debugFunctionsComboBox.getSelectedItem()));
		}

		protected void fireEditingCanceled() {
			super.fireEditingCanceled();
			checkRows();
		}

		protected void fireEditingStopped() {
			super.fireEditingStopped();
			checkRows();
		}

		public Component getTableCellEditorComponent(JTable table,
				Object value, boolean isSelected, int row, int column) {

			if (column == 0) {

				if (debugFunction.getMiosVectorValues().contains(value)) {

					debugFunctionsComboBox.setSelectedItem(debugFunction
							.getMiosVectorNames().elementAt(
									debugFunction.getMiosVectorValues()
											.indexOf(value)));
				} else {
					debugFunctionsComboBox.setSelectedIndex(0);
				}

				return debugFunctionsComboBox;
			} else {
				return null;
			}
		}
	}

	private class DebugFunctionParametersTableModel extends DefaultTableModel {

		private String[] headers = { "Function", "WREG", "MIOS_PARAM1",
				"MIOS_PARAM2", "MIOS_PARAM3" };

		private Vector debugFunctionParameters;

		public DebugFunctionParametersTableModel(Vector debugFunctionParameters) {
			this.debugFunctionParameters = debugFunctionParameters;
		}

		public int getColumnCount() {
			return headers.length;
		}

		public int getRowCount() {
			if (debugFunctionParameters != null) {
				return debugFunctionParameters.size();
			} else {
				return 0;
			}
		}

		public String getColumnName(int col) {
			return headers[col];
		}

		public Object getValueAt(int row, int col) {

			DebugFunctionParameters params = (DebugFunctionParameters) debugFunctionParameters
					.elementAt(row);

			if (col == 0) {
				return new Integer(params.getAddress());
			} else if (col == 1) {
				return new Integer(params.getWreg());
			} else if (col == 2) {
				return new Integer(params.getParam1());
			} else if (col == 3) {
				return new Integer(params.getParam2());
			} else if (col == 4) {
				return new Integer(params.getParam3());
			} else {
				return null;
			}
		}

		public void setValueAt(Object value, int row, int col) {

			DebugFunctionParameters params = (DebugFunctionParameters) debugFunctionParameters
					.elementAt(row);

			if (col == 0) {
				params.setAddress(((Integer) value).intValue());
			} else if (col == 1) {
				params.setWreg(((Integer) value).intValue());
			} else if (col == 2) {
				params.setParam1(((Integer) value).intValue());
			} else if (col == 3) {
				params.setParam2(((Integer) value).intValue());
			} else if (col == 4) {
				params.setParam3(((Integer) value).intValue());
			}
		}

		public Class getColumnClass(int c) {
			return getValueAt(0, c).getClass();
		}

		public boolean isCellEditable(int row, int col) {
			return true;
		}
	}

	private class DebugFunctionParametersTransferHandler extends
			SimpleTransferHandler {
		private int[] rowNos = null;

		private JTable target;

		protected int addIndex = -1; // Location where items were added

		protected int addCount = 0; // Number of items added.

		public DebugFunctionParametersTransferHandler() {
			super();
			supportedDataFlavors = new DataFlavor[] { DataFlavor.stringFlavor };

			dropOnSelf = true;
		}

		public int getSourceActions(JComponent c) {
			return COPY_OR_MOVE;
		}

		public boolean importData(JComponent c, Transferable t) {
			target = (JTable) c;
			int index = target.getSelectedRow();
			if (index < 0)
				index = target.getRowCount();

			if (rowNos != null && index >= rowNos[0]
					&& index <= rowNos[rowNos.length - 1]) {
				rowNos = null;
				return false;
			}

			if (canImport(c, t.getTransferDataFlavors())) {

				if (hasFlavor(t.getTransferDataFlavors(),
						DataFlavor.stringFlavor)) {
					String[] rows = null;

					try {
						rows = ((String) t
								.getTransferData(DataFlavor.stringFlavor))
								.split("\n");
					} catch (Exception e) {
						e.printStackTrace();
					}

					for (int i = rows.length - 1; i >= 0; i--) {

						DebugFunctionParameters params = new DebugFunctionParameters();

						addCount = rows.length;
						addIndex = index;

						String[] cols = new String[5];
						String imports[] = rows[i].split(",");

						System.arraycopy(imports, 0, cols, 0, Math.min(
								cols.length, imports.length));

						try {

							params.setAddress(Integer.parseInt(cols[0]));
							params.setWreg(Integer.parseInt(cols[1]));
							params.setParam1(Integer.parseInt(cols[2]));
							params.setParam2(Integer.parseInt(cols[3]));
							params.setParam3(Integer.parseInt(cols[4]));

						} catch (NumberFormatException nfe) {
							return false;
						}
						debugFunction.insertDebugFunctionParameters(params,
								index);
					}
				} else {
					justImported = false;
					return false;
				}
				justImported = true;
				return true;
			}
			return false;
		}

		protected Transferable createTransferable(JComponent c) {
			justImported = false;
			justExported = true;
			JTable table = (JTable) c;
			rowNos = table.getSelectedRows();

			DebugFunctionParameters[] params = new DebugFunctionParameters[rowNos.length];

			for (int t = 0; t < rowNos.length; t++) {
				params[t] = (DebugFunctionParameters) debugFunction
						.getDebugFunctionParameters().elementAt(rowNos[t]);
			}

			return new DebugFunctionParametersTransferable(params);
		}

		protected void exportDone(JComponent c, Transferable data, int action) {
			if (action == MOVE) {
				if (justImported) {
					if (addCount > 0) {
						for (int i = 0; i < rowNos.length; i++) {
							if (rowNos[i] > addIndex) {
								rowNos[i] += addCount;
							}
						}
					}
				}
				if (rowNos != null) {
					for (int i = rowNos.length - 1; i >= 0; i--) {
						debugFunction.removeDebugFunctionParameters(rowNos[i]);
					}
				}
			}
			rowNos = null;
			target = null;
			addCount = 0;
			addIndex = -1;
			justExported = false;
		}
	}
}
