/*
 * @(#)HelpPane.java	beta8	2006/04/23
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

package org.midibox.utils.gui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.net.URL;
import java.util.Vector;

import javax.swing.JButton;
import javax.swing.JEditorPane;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JToolBar;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;

public class HelpPane extends JPanel implements ActionListener,
		HyperlinkListener {
	private JEditorPane JEP = new JEditorPane();

	private PropertyChangeSupport listeners = new PropertyChangeSupport(this);

	private JToolBar toolbar;

	private URL homePage;

	private JButton home;

	private JButton back;

	private JButton forward;

	private Vector history = new Vector();

	private int historyIndex = 0;

	private static HelpPane singleton;

	public HelpPane(URL homePage) {
		super(new BorderLayout());
		this.homePage = homePage;
		add(makeHelpPanel(), BorderLayout.CENTER);
		add(makeToolBar(), BorderLayout.NORTH);
		if (homePage != null) {
			goToURL(homePage);
		}
		updateButtons();
	}

	private JPanel makeHelpPanel() {
		JPanel helpPane = new JPanel(new BorderLayout());
		JEP.setEditable(false);
		JEP.setContentType("text/html");
		JEP.addHyperlinkListener(this);
		JScrollPane scrollPane = new JScrollPane(JEP);
		scrollPane
				.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		scrollPane
				.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		helpPane.add(scrollPane);
		return helpPane;
	}

	private JToolBar makeToolBar() {

		toolbar = new JToolBar("Tool Bar", JToolBar.HORIZONTAL);
		toolbar.setFloatable(false);
		toolbar.setRollover(true);

		Insets margin = new Insets(2, 2, 2, 2);

		back = new JButton(ImageLoader.getImageIcon("back.png"));
		back.addActionListener(this);
		back.setMargin(margin);
		back.setToolTipText("Back");
		toolbar.add(back);

		forward = new JButton(ImageLoader.getImageIcon("forward.png"));
		forward.addActionListener(this);
		forward.setMargin(margin);
		forward.setToolTipText("Forward");
		toolbar.add(forward);

		home = new JButton(ImageLoader.getImageIcon("home.png"));
		home.addActionListener(this);
		home.setMargin(margin);
		home.setToolTipText("Home");
		toolbar.add(home);

		return toolbar;
	}

	public void goToURL(URL url) {
		try {
			JEP.setPage(url);
			listeners.firePropertyChange("page_changed", null, Boolean.TRUE);

			for (int i = history.size() - 1; i > historyIndex; i--) {
				history.remove(i);
			}

			history.add(url);
			historyIndex = history.size() - 1;
			updateButtons();
		} catch (Exception err) {
			JOptionPane.showMessageDialog(this, "Invalid Link: '" + url + "'",
					"Invalid Link: '" + url + "'", JOptionPane.ERROR_MESSAGE);
		}
	}

	public void goToHistory(int index) {
		URL url = (URL) history.elementAt(index);
		try {
			JEP.setPage(url);
			listeners.firePropertyChange("page_changed", null, Boolean.TRUE);
			historyIndex = index;
			updateButtons();
		} catch (Exception err) {
			JOptionPane.showMessageDialog(this, "Invalid Link: '" + url + "'",
					"Invalid Link: '" + url + "'", JOptionPane.ERROR_MESSAGE);
		}
	}

	private void updateButtons() {
		back.setEnabled(historyIndex > 0);
		forward.setEnabled(historyIndex < history.size() - 1);
	}

	public void hyperlinkUpdate(HyperlinkEvent hle) {
		if (hle.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
			goToURL(hle.getURL());
		}
	}

	public Dimension getPreferredSize() {
		return new Dimension(400, 300);
	}

	public URL getCurrentURL() {
		return JEP.getPage();
	}

	public void addPropertyChangeListener(PropertyChangeListener l) {
		if (listeners != null) {
			listeners.addPropertyChangeListener(l);
		}
	}

	public void removePropertyChangeListener(PropertyChangeListener l) {
		listeners.removePropertyChangeListener(l);
	}

	public void actionPerformed(ActionEvent ae) {
		Object source = ae.getSource();

		if (source == back) {
			goToHistory(historyIndex - 1);
		}

		if (source == forward) {
			goToHistory(historyIndex + 1);
		}

		if (source == home) {
			goToURL(homePage);
		}
	}

	public static HelpPane createSingleton(URL url) {
		if (singleton == null) {
			singleton = new HelpPane(url);
		}
		return singleton;
	}
}
