/*
 * @(#)MidiActivityGUIjava	beta8	2006/04/23
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

import java.awt.Dimension;
import java.awt.Graphics;
import java.util.Observable;
import java.util.Observer;

import javax.swing.ImageIcon;
import javax.swing.JComponent;

import org.midibox.midi.MidiActivity;

public class MidiActivityGUI extends JComponent implements Observer {

	private MidiActivity midiActivity;

	private ImageIcon offImage, onImage;

	private Dimension d;

	private ImageIcon image;

	public MidiActivityGUI(MidiActivity midiActivity, ImageIcon offImage,
			ImageIcon onImage) {
		this.midiActivity = midiActivity;

		this.offImage = offImage;
		this.onImage = onImage;
		image = offImage;
		d = new Dimension(offImage.getIconWidth(), offImage.getIconHeight());

		midiActivity.addObserver(this);
	}

	public void close() {

	}

	private void check() {
		if (System.currentTimeMillis() - midiActivity.getTime() < 100) {
			image = onImage;
		} else {
			image = offImage;
		}
		repaint();
	}

	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		image.paintIcon(this, g, (getWidth() - image.getIconWidth()) / 2,
				(getHeight() - image.getIconHeight()) / 2);
	}

	public Dimension getMinimumSize() {
		return d;
	}

	public Dimension getPreferredSize() {
		return d;
	}

	public Dimension getMaximumSize() {
		return d;
	}

	public void update(Observable observable, Object object) {
		check();
	}
}
