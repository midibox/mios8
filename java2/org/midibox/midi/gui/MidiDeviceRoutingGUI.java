/*
 * @(#)MidiDeviceRoutingGUI.java	beta8	2006/04/23
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

import java.awt.BasicStroke;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.sound.midi.MidiDevice;
import javax.swing.BorderFactory;
import javax.swing.DefaultListCellRenderer;
import javax.swing.DefaultListModel;
import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JToggleButton;
import javax.swing.ListSelectionModel;
import javax.swing.SwingConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import org.midibox.midi.MidiDeviceRouting;
import org.midibox.midi.MidiFilterDevice;
import org.midibox.midi.MidiMapDevice;
import org.midibox.midi.MidiRouterDevice;
import org.midibox.utils.gui.GuiUtils;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.SimpleTransferHandler;

public class MidiDeviceRoutingGUI extends JPanel implements MouseListener,
		ChangeListener, ListSelectionListener, ActionListener, Observer {

	private MidiDeviceRouting midiDeviceRouting;

	private static Icon openIcon = ImageLoader.getImageIcon("midiPortOpen.png");

	private static Icon closedIcon = ImageLoader
			.getImageIcon("midiPortClosed.png");

	private Hashtable icons = new Hashtable();

	private DefaultListModel midiReadDevicesListModel;

	private JList midiReadDevicesList;

	private JScrollPane midiReadDevicesScroller;

	private DefaultListModel midiWriteDevicesListModel;

	private JList midiWriteDevicesList;

	private JScrollPane midiWriteDevicesScroller;

	private JPopupMenu connectionMenu;

	private JButton connectButton;

	private JButton disconnectButton;

	private JButton disconnectallButton;

	private JButton midiDeviceManagerButton;

	private JButton midiRouterButton;

	private JButton filterButton;

	/*
	 * private JButton mapButton;
	 */

	private JToggleButton releaseButton;

	private JPanel wirePanel;

	private JDialog midiDeviceManagerDialog;

	private JDialog midiRouterManagerGUI;

	private JDialog midiFilterManagerDialog;

	/*
	 * private JDialog midiMapManagerDialog;
	 */

	private Hashtable midiDevicePropertiesDialogs;

	public MidiDeviceRoutingGUI(MidiDeviceRouting midiDeviceRouting) {
		super(new BorderLayout());

		this.midiDeviceRouting = midiDeviceRouting;
		midiDeviceRouting.addObserver(this);

		midiDevicePropertiesDialogs = new Hashtable();

		addMouseListener(this);

		addMidiDeviceIcon(MidiRouterDevice.class, ImageLoader
				.getImageIcon("virtualMidiDevice.png"));
		addMidiDeviceIcon(MidiFilterDevice.class, ImageLoader
				.getImageIcon("filter.png"));
		addMidiDeviceIcon(MidiMapDevice.class, ImageLoader
				.getImageIcon("map.png"));

		midiReadDevicesListModel = new DefaultListModel();
		midiWriteDevicesListModel = new DefaultListModel();

		midiReadDevicesList = new JList(midiReadDevicesListModel);
		midiReadDevicesList
				.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
		midiReadDevicesList.setLayoutOrientation(JList.VERTICAL);
		midiReadDevicesList.setVisibleRowCount(6);
		midiReadDevicesList.setCellRenderer(new MyListCellRenderer());
		midiReadDevicesList.addMouseListener(this);
		midiReadDevicesList.setBackground(Color.WHITE);
		midiWriteDevicesList = new JList(midiWriteDevicesListModel);
		midiWriteDevicesList
				.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
		midiWriteDevicesList.setLayoutOrientation(JList.VERTICAL);
		midiWriteDevicesList.setVisibleRowCount(6);
		midiWriteDevicesList.setCellRenderer(new MyListCellRenderer());
		midiWriteDevicesList.addMouseListener(this);
		midiWriteDevicesList.setBackground(Color.WHITE);

		JPanel listPane = new JPanel(new GridBagLayout());

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 1;
		gbc.gridy = 1;
		gbc.gridwidth = 1;
		gbc.gridheight = 1;
		gbc.insets = new Insets(2, 0, 2, 0);
		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.weightx = 1.0;
		gbc.weighty = 0.0;

		listPane.setOpaque(false);

		listPane.add(new JLabel("MIDI Devices - Readable",
				SwingConstants.CENTER), gbc);
		gbc.gridy++;

		midiReadDevicesScroller = new JScrollPane(midiReadDevicesList);
		midiReadDevicesScroller.getViewport().addChangeListener(this);
		midiReadDevicesScroller
				.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		midiReadDevicesScroller
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

		gbc.fill = GridBagConstraints.BOTH;
		gbc.weighty = 1.0;
		gbc.weighty = 1.0;

		listPane.add(midiReadDevicesScroller, gbc);
		gbc.gridx++;

		wirePanel = new JPanel() {

			public void paintComponent(Graphics g) {
				super.paintComponent(g);
				Graphics2D g2d = (Graphics2D) g;
				g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
						RenderingHints.VALUE_ANTIALIAS_ON);

				Iterator it = getMidiDeviceRouting().getMidiReadDevices()
						.iterator();

				int i = 0;

				while (it.hasNext()) {

					MidiDevice transmittingDevice = (MidiDevice) it.next();

					Iterator it2 = getMidiDeviceRouting().getMidiWriteDevices()
							.iterator();

					int o = 0;
					while (it2.hasNext()) {
						MidiDevice receivingDevice = (MidiDevice) it2.next();

						int connected = getMidiDeviceRouting()
								.devicesConnected(transmittingDevice,
										receivingDevice);

						if (connected != MidiDeviceRouting.DISCONNECTED) {

							Rectangle rec1 = midiReadDevicesList.getCellBounds(
									i, i);
							Point p1 = new Point(0,
									((rec1.height / 2) + rec1.y)
											+ midiReadDevicesScroller.getY()
											- midiReadDevicesScroller
													.getVerticalScrollBar()
													.getValue()
											- wirePanel.getY());

							Rectangle rec2 = midiWriteDevicesList
									.getCellBounds(o, o);
							Point p2 = new Point(getWidth(),
									((rec2.height / 2) + rec2.y)
											+ midiReadDevicesScroller.getY()
											- midiWriteDevicesScroller
													.getVerticalScrollBar()
													.getValue()
											- wirePanel.getY());

							if (connected == MidiDeviceRouting.LOGICALLY_CONNECTED) {
								g2d.setStroke(new BasicStroke(2f,
										BasicStroke.CAP_BUTT,
										BasicStroke.JOIN_ROUND, 1f,
										new float[] { 3f, 6f }, 0f));
							} else {
								g2d.setStroke(new BasicStroke(2));
							}
							g2d.drawLine(p1.x, p1.y, p2.x, p2.y);
						}
						o++;
					}
					i++;
				}
			}
		};

		wirePanel.setOpaque(false);

		listPane.add(wirePanel, gbc);

		GuiUtils.setComponentSize(wirePanel, 80, 80);

		gbc.gridx++;
		gbc.gridy--;

		gbc.fill = GridBagConstraints.HORIZONTAL;
		gbc.weightx = 1.0;
		gbc.weighty = 0.0;

		listPane.add(new JLabel("MIDI Devices - Writeable",
				SwingConstants.CENTER), gbc);
		gbc.gridy++;

		midiWriteDevicesScroller = new JScrollPane(midiWriteDevicesList);
		midiWriteDevicesScroller
				.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		midiWriteDevicesScroller
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		midiWriteDevicesScroller.getViewport().addChangeListener(this);

		gbc.fill = GridBagConstraints.BOTH;
		gbc.weightx = 1.0;
		gbc.weighty = 1.0;

		listPane.add(midiWriteDevicesScroller, gbc);

		int maxPreferredWidth = Math.max(midiReadDevicesScroller
				.getPreferredSize().width, midiWriteDevicesScroller
				.getPreferredSize().width);
		int maxPreferredHeight = Math.max(midiReadDevicesScroller
				.getPreferredSize().height, midiWriteDevicesScroller
				.getPreferredSize().height);

		GuiUtils.setComponentSize(midiReadDevicesScroller, maxPreferredWidth,
				maxPreferredHeight);
		GuiUtils.setComponentSize(midiWriteDevicesScroller, maxPreferredWidth,
				maxPreferredHeight);

		midiReadDevicesList.setDragEnabled(true);
		midiWriteDevicesList.setDragEnabled(true);
		midiReadDevicesList.setTransferHandler(new MidiDevicesTransferHandler(
				false));
		midiWriteDevicesList.setTransferHandler(new MidiDevicesTransferHandler(
				true));
		midiReadDevicesList.addListSelectionListener(this);
		midiWriteDevicesList.addListSelectionListener(this);

		listPane.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
		listPane.setOpaque(false);
		add(listPane, BorderLayout.CENTER);

		JPanel buttonPane = new JPanel(new GridBagLayout());
		gbc = new GridBagConstraints();

		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridwidth = 1;
		gbc.fill = GridBagConstraints.BOTH;

		gbc.insets = new Insets(5, 5, 5, 5);

		connectButton = new JButton("Connect");
		connectButton.setToolTipText("Connect Selected Devices");
		connectButton.addActionListener(this);
		buttonPane.add(connectButton, gbc);
		gbc.gridy++;

		gbc.insets = new Insets(5, 5, 10, 5);

		midiDeviceManagerButton = new JButton("Select Devices");
		midiDeviceManagerButton.setToolTipText("Select Hardware Devices");
		midiDeviceManagerButton.addActionListener(this);
		buttonPane.add(midiDeviceManagerButton, gbc);
		gbc.gridx++;
		gbc.gridy--;

		gbc.insets = new Insets(5, 5, 5, 5);

		gbc.gridwidth = 2;
		disconnectButton = new JButton("Disconnect");
		disconnectButton.setToolTipText("Disconnect Selected Devices");
		disconnectButton.addActionListener(this);
		buttonPane.add(disconnectButton, gbc);
		gbc.gridy++;

		gbc.insets = new Insets(5, 5, 10, 4);

		gbc.gridwidth = 1;
		midiRouterButton = new JButton(ImageLoader
				.getImageIcon("virtualMidiDevice.png"));
		midiRouterButton.setToolTipText("Add/Remove Virtual Ports");
		midiRouterButton.addActionListener(this);
		midiRouterButton.setMargin(new Insets(2, 8, 2, 8));
		buttonPane.add(midiRouterButton, gbc);
		gbc.gridx++;

		gbc.insets = new Insets(5, 4, 10, 5);

		filterButton = new JButton(ImageLoader.getImageIcon("filter.png"));
		filterButton.setToolTipText("Add/Remove Filters");
		filterButton.addActionListener(this);
		filterButton.setMargin(new Insets(2, 8, 2, 8));
		buttonPane.add(filterButton, gbc);
		gbc.gridx++;

		/*
		 * gbc.insets = new Insets(5, 4, 10, 5);
		 * 
		 * mapButton = new JButton(ImageLoader.getImageIcon("map.png"));
		 * mapButton.setToolTipText("Add/Remove Maps");
		 * mapButton.addActionListener(this); mapButton.setMargin(new Insets(2,
		 * 8, 2, 8)); buttonPane.add(mapButton, gbc); gbc.gridx++;
		 */

		gbc.gridy--;

		gbc.insets = new Insets(5, 5, 5, 5);

		disconnectallButton = new JButton("Disconnect All");
		disconnectallButton.setToolTipText("Disconnect All Devices");
		disconnectallButton.addActionListener(this);
		buttonPane.add(disconnectallButton, gbc);
		gbc.gridy++;

		gbc.insets = new Insets(5, 5, 10, 5);

		releaseButton = new JToggleButton("Release Ports", midiDeviceRouting
				.getPortsReleased());
		releaseButton.setToolTipText("Release MIDI Ports");
		releaseButton.addActionListener(this);
		buttonPane.add(releaseButton, gbc);

		buttonPane.setOpaque(false);
		add(buttonPane, BorderLayout.SOUTH);
		populateTrees();
	}

	public MidiDeviceRouting getMidiDeviceRouting() {
		return midiDeviceRouting;
	}

	public void addMidiDeviceIcon(Object object, Icon icon) {
		icons.put(object, icon);
	}

	public Icon getMidiDeviceIcon(MidiDevice md) {
		if (icons.containsKey(md)) {
			return (Icon) icons.get(md);
		}

		if (icons.containsKey(md.getClass())) {
			return (Icon) icons.get(md.getClass());
		}

		try {
			if (md.isOpen()) {
				return openIcon;
			}

			return closedIcon;

		} catch (Exception e) {
			return closedIcon;
		}
	}

	public void redrawAll() {
		midiReadDevicesList.repaint();
		midiWriteDevicesList.repaint();
		wirePanel.repaint();
		setButtonStates();
	}

	private void buildConnectionMenu(boolean output) {

		final MidiDevice sourceDevice = (MidiDevice) ((output) ? midiDeviceRouting
				.getMidiWriteDevices()
				: midiDeviceRouting.getMidiReadDevices())
				.elementAt(((output) ? midiWriteDevicesList
						: midiReadDevicesList).getSelectedIndex());
		final Vector targetDevices = ((output) ? midiDeviceRouting
				.getMidiReadDevices() : midiDeviceRouting.getMidiWriteDevices());

		connectionMenu = new JPopupMenu(sourceDevice.getDeviceInfo().getName());

		JMenu connectMenu = new JMenu("Connect to ...");

		connectionMenu.add(connectMenu);

		connectionMenu.setLabel(sourceDevice.getDeviceInfo().getName());

		Iterator it = targetDevices.iterator();

		while (it.hasNext()) {

			MidiDevice targetDevice = (MidiDevice) it.next();

			final MidiDevice transmittingDevice = ((output) ? targetDevice
					: sourceDevice);
			final MidiDevice receivingDevice = ((output) ? sourceDevice
					: targetDevice);
			final JCheckBoxMenuItem connect = new JCheckBoxMenuItem(
					targetDevice.getDeviceInfo().getName(),
					midiDeviceRouting.devicesConnected(transmittingDevice,
							receivingDevice) != MidiDeviceRouting.DISCONNECTED);

			connect.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent ae) {
					if (connect.isSelected()) {
						midiDeviceRouting.connectDevices(transmittingDevice,
								receivingDevice);
					} else {
						midiDeviceRouting.disconnectDevices(transmittingDevice,
								receivingDevice);
					}
				}
			});
			connectMenu.add(connect);
		}

		JMenuItem properties = new JMenuItem("MIDI Device Properties");
		properties.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				showMidiDevicePropertyDialog(sourceDevice);
			}
		});
		connectionMenu.add(properties);
	}

	private void showMidiDevicePropertyDialog(MidiDevice midiDeviceIn) {

		final MidiDevice midiDevice = midiDeviceIn;

		if (!midiDevicePropertiesDialogs.containsKey(midiDevice)) {

			final MidiDeviceProperties midiDeviceProperties = new MidiDeviceProperties(
					midiDevice);

			JDialog midiDevicePropertiesDialog = new JDialog();
			midiDevicePropertiesDialog.setContentPane(midiDeviceProperties);
			midiDevicePropertiesDialog.setModal(false);
			midiDevicePropertiesDialog.setTitle(midiDevice.getDeviceInfo()
					.getName());
			midiDevicePropertiesDialog.pack();
			midiDevicePropertiesDialog.setLocationRelativeTo(this);
			midiDevicePropertiesDialog.setVisible(true);

			midiDevicePropertiesDialogs.put(midiDevice,
					midiDevicePropertiesDialog);

			midiDevicePropertiesDialog.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					midiDevicePropertiesDialogs.remove(midiDevice);
				}
			});
		}
		((JDialog) midiDevicePropertiesDialogs.get(midiDevice)).requestFocus();
	}

	private void showMidiDeviceManagerDialog() {
		if (midiDeviceManagerDialog == null) {
			final MidiDeviceManagerGUI midiDeviceManagerGUI = new MidiDeviceManagerGUI(
					midiDeviceRouting.getMidiDeviceManager());

			midiDeviceManagerDialog = new JDialog();
			midiDeviceManagerDialog.setContentPane(midiDeviceManagerGUI);
			midiDeviceManagerDialog.setModal(false);
			midiDeviceManagerDialog.setTitle("MIDI Devices");
			midiDeviceManagerDialog.pack();
			midiDeviceManagerDialog.setLocationRelativeTo(this);
			midiDeviceManagerDialog.setVisible(true);

			midiDeviceManagerDialog.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					midiDeviceRouting.getMidiDeviceManager().deleteObserver(
							midiDeviceManagerGUI);
					midiDeviceManagerDialog = null;
				}
			});
		}
		midiDeviceManagerDialog.requestFocus();
	}

	private void showMidiRouterManagerDialog() {
		if (midiRouterManagerGUI == null) {
			final MidiRouterDeviceManagerGUI midiRouterDeviceManagerGUI = new MidiRouterDeviceManagerGUI(
					midiDeviceRouting.getMidiRouterDeviceManager());

			midiRouterManagerGUI = new JDialog();
			midiRouterManagerGUI.setContentPane(midiRouterDeviceManagerGUI);
			midiRouterManagerGUI.setModal(false);
			midiRouterManagerGUI.setTitle("Virtual Ports");
			midiRouterManagerGUI.pack();
			midiRouterManagerGUI.setLocationRelativeTo(this);
			midiRouterManagerGUI.setVisible(true);

			midiRouterManagerGUI.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					midiDeviceRouting.getMidiRouterDeviceManager()
							.deleteObserver(midiRouterDeviceManagerGUI);
					midiRouterManagerGUI = null;
				}
			});
		}
		midiRouterManagerGUI.requestFocus();
	}

	private void showFilterManagerDialog() {
		if (midiFilterManagerDialog == null) {
			final MidiFilterDeviceManagerGUI midiFilterManagerGUI = new MidiFilterDeviceManagerGUI(
					midiDeviceRouting.getMidiFilterManager());

			midiFilterManagerDialog = new JDialog();
			midiFilterManagerDialog.setModal(false);
			midiFilterManagerDialog.setTitle("Filters");
			midiFilterManagerDialog.setContentPane(midiFilterManagerGUI);
			midiFilterManagerDialog.pack();
			midiFilterManagerDialog.setLocationRelativeTo(this);
			midiFilterManagerDialog.setVisible(true);

			midiFilterManagerDialog.addWindowListener(new WindowAdapter() {
				public void windowClosing(WindowEvent we) {
					midiDeviceRouting.getMidiFilterManager().deleteObserver(
							midiFilterManagerGUI);
					midiFilterManagerDialog = null;
				}
			});
		}
		midiFilterManagerDialog.requestFocus();
	}

	/*
	 * private void showMapManagerDialog() { if (midiMapManagerDialog == null) {
	 * final MidiMapDeviceManagerGUI midiMapManagerGUI = new
	 * MidiMapDeviceManagerGUI( midiDeviceRouting.getMidiMapManager());
	 * 
	 * midiMapManagerDialog = new JDialog();
	 * midiMapManagerDialog.setModal(false);
	 * midiMapManagerDialog.setTitle("Maps");
	 * midiMapManagerDialog.setContentPane(midiMapManagerGUI);
	 * midiMapManagerDialog.pack();
	 * midiMapManagerDialog.setLocationRelativeTo(this);
	 * midiMapManagerDialog.setVisible(true);
	 * 
	 * midiMapManagerDialog.addWindowListener(new WindowAdapter() { public void
	 * windowClosing(WindowEvent we) {
	 * midiDeviceRouting.getMidiMapManager().deleteObserver( midiMapManagerGUI);
	 * midiMapManagerDialog = null; } }); } midiMapManagerDialog.requestFocus(); }
	 */

	private void populateTrees() {

		midiReadDevicesListModel.removeAllElements();

		Iterator it = midiDeviceRouting.getMidiReadDevices().iterator();

		while (it.hasNext()) {
			midiReadDevicesListModel.addElement(((MidiDevice) it.next())
					.getDeviceInfo().getName());
		}

		midiReadDevicesList.setSelectedIndex(0);

		midiWriteDevicesListModel.removeAllElements();

		it = midiDeviceRouting.getMidiWriteDevices().iterator();

		while (it.hasNext()) {
			midiWriteDevicesListModel.addElement(((MidiDevice) it.next())
					.getDeviceInfo().getName());
		}

		midiWriteDevicesList.setSelectedIndex(0);

		updateUI();
	}

	private void setButtonStates() {

		int[] selectedReadIndices = midiReadDevicesList.getSelectedIndices();
		int[] selectedWriteIndices = midiWriteDevicesList.getSelectedIndices();

		connectButton.setEnabled(false);
		disconnectButton.setEnabled(false);

		if (selectedReadIndices.length > 0 && selectedWriteIndices.length > 0) {

			for (int r = 0; r < selectedReadIndices.length; r++) {

				MidiDevice transmittingDevice = (MidiDevice) midiDeviceRouting
						.getMidiReadDevices().elementAt(selectedReadIndices[r]);

				for (int w = 0; w < selectedWriteIndices.length; w++) {

					MidiDevice receivingDevice = (MidiDevice) midiDeviceRouting
							.getMidiWriteDevices().elementAt(
									selectedWriteIndices[w]);

					if (midiDeviceRouting.devicesConnected(transmittingDevice,
							receivingDevice) != MidiDeviceRouting.DISCONNECTED) {
						disconnectButton.setEnabled(true);
					} else {
						connectButton.setEnabled(true);
					}
				}
			}
		}
	}

	public void valueChanged(ListSelectionEvent lse) {
		setButtonStates();
	}

	public void mouseClicked(MouseEvent me) {

	}

	public void mouseReleased(MouseEvent me) {
		Object source = me.getSource();

		if (source == midiReadDevicesList || source == midiWriteDevicesList) {
			JList source2 = (JList) source;
			if (me.getButton() == MouseEvent.BUTTON3) {
				int index = source2.locationToIndex(me.getPoint());
				source2.setSelectedIndex(index);
				buildConnectionMenu((source == midiWriteDevicesList) ? true
						: false);
				connectionMenu.show(source2, me.getX(), me.getY());
			}
		}
	}

	public void mousePressed(MouseEvent me) {

	}

	public void mouseEntered(MouseEvent me) {

	}

	public void mouseExited(MouseEvent me) {

	}

	public void stateChanged(ChangeEvent ce) {
		wirePanel.repaint();
	}

	public Dimension getMinimumSize() {
		return getPreferredSize();
	}

	public void update(Observable observable, Object object) {
		if (observable == midiDeviceRouting) {
			if (object == midiDeviceRouting.getMidiReadDevices()
					|| object == midiDeviceRouting.getMidiWriteDevices()) {
				populateTrees();
			}
			redrawAll();
		}
	}

	public void actionPerformed(ActionEvent ae) {
		Object source = ae.getSource();

		if (source == connectButton) {

			int[] selectedReadIndices = midiReadDevicesList
					.getSelectedIndices();
			int[] selectedWriteIndices = midiWriteDevicesList
					.getSelectedIndices();

			for (int r = 0; r < selectedReadIndices.length; r++) {

				MidiDevice transmittingDevice = (MidiDevice) midiDeviceRouting
						.getMidiReadDevices().elementAt(selectedReadIndices[r]);

				for (int w = 0; w < selectedWriteIndices.length; w++) {
					MidiDevice receivingDevice = (MidiDevice) midiDeviceRouting
							.getMidiWriteDevices().elementAt(
									selectedWriteIndices[w]);

					midiDeviceRouting.connectDevices(transmittingDevice,
							receivingDevice);
				}

			}
		} else if (source == disconnectButton) {

			int[] selectedReadIndices = midiReadDevicesList
					.getSelectedIndices();
			int[] selectedWriteIndices = midiWriteDevicesList
					.getSelectedIndices();

			for (int r = 0; r < selectedReadIndices.length; r++) {

				MidiDevice transmittingDevice = (MidiDevice) midiDeviceRouting
						.getMidiReadDevices().elementAt(selectedReadIndices[r]);

				for (int w = 0; w < selectedWriteIndices.length; w++) {
					MidiDevice receivingDevice = (MidiDevice) midiDeviceRouting
							.getMidiWriteDevices().elementAt(
									selectedWriteIndices[w]);

					midiDeviceRouting.disconnectDevices(transmittingDevice,
							receivingDevice);
				}
			}
		} else if (source == disconnectallButton) {
			midiDeviceRouting.disconnectAll();
		} else if (source == midiDeviceManagerButton) {
			showMidiDeviceManagerDialog();
		} else if (source == midiRouterButton) {
			showMidiRouterManagerDialog();
		} else if (source == filterButton) {
			showFilterManagerDialog();
		} /*
			 * else if (source == mapButton) { showMapManagerDialog(); }
			 */
		else if (source == releaseButton) {
			midiDeviceRouting.setPortsReleased(releaseButton.isSelected());
		}
	}

	public class MyListCellRenderer extends DefaultListCellRenderer {
		public Component getListCellRendererComponent(JList list, Object value,
				int index, boolean isSelected, boolean hasFocus) {

			super.getListCellRendererComponent(list, value, index, isSelected,
					hasFocus);

			setIcon(getMidiDeviceIcon(((MidiDevice) ((list == midiWriteDevicesList) ? midiDeviceRouting
					.getMidiWriteDevices()
					: midiDeviceRouting.getMidiReadDevices()).elementAt(index))));

			return this;
		}
	}

	public class MidiDevicesTransferHandler extends SimpleTransferHandler {

		private boolean output;

		public MidiDevicesTransferHandler(boolean output) {
			super();
			this.output = output;
			dropOnSelf = false;
			supportedDataFlavors = new DataFlavor[] { MidiDeviceTransferable.LOCAL_JVM_MIDI_DEVICE_FLAVOR };
		}

		public boolean importData(JComponent c, Transferable t) {

			if (canImport(c, supportedDataFlavors)) {

				int[] indices = ((output) ? midiWriteDevicesList
						: midiReadDevicesList).getSelectedIndices();

				for (int d = 0; d < indices.length; d++) {

					MidiDevice localDevice = (MidiDevice) ((output) ? midiDeviceRouting
							.getMidiWriteDevices()
							: midiDeviceRouting.getMidiReadDevices())
							.elementAt(indices[d]);

					try {
						MidiDevice[] remoteDevices = (MidiDevice[]) t
								.getTransferData(MidiDeviceTransferable.LOCAL_JVM_MIDI_DEVICE_FLAVOR);

						for (int d2 = 0; d2 < remoteDevices.length; d2++) {

							MidiDevice remoteDevice = remoteDevices[d2];

							if (output) {
								midiDeviceRouting.connectDevices(remoteDevice,
										localDevice);
							} else {
								midiDeviceRouting.connectDevices(localDevice,
										remoteDevice);
							}
						}
					} catch (Exception e) {
						return false;
					}
				}
				return true;
			}
			return false;
		}

		protected Transferable createTransferable(JComponent c) {
			justExported = true;
			int[] indices = ((output) ? midiWriteDevicesList
					: midiReadDevicesList).getSelectedIndices();

			MidiDevice[] midiDevices = new MidiDevice[indices.length];

			for (int d = 0; d < midiDevices.length; d++) {
				midiDevices[d] = (MidiDevice) ((output) ? midiDeviceRouting
						.getMidiWriteDevices() : midiDeviceRouting
						.getMidiReadDevices()).elementAt(indices[d]);
			}
			return new MidiDeviceTransferable(midiDevices);
		}
	}
}
