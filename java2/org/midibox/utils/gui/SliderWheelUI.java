/*
 * @(#)SliderWheelUI.java	beta8	2006/04/23
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

import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.plaf.basic.BasicSliderUI;

public class SliderWheelUI extends BasicSliderUI {

	private ImageIcon vertThumbIcon2;

	private ImageIcon horizThumbIcon2;

	private int thumbHeight;

	public SliderWheelUI(ImageIcon horizThumbIcon2, ImageIcon vertThumbIcon2,
			int thumbHeight) {
		super(null);
		this.horizThumbIcon2 = horizThumbIcon2;
		this.vertThumbIcon2 = vertThumbIcon2;
		this.thumbHeight = thumbHeight;
	}

	public void setThumbLocation(int x, int y) {
		thumbRect.setLocation(x, y);
		slider.repaint();
	}

	public void paint(Graphics g, JComponent c) {

		if (c.hasFocus()) {
			paintFocus(g);
		}
		paintThumb(g);
	}

	public void paintThumb(Graphics g) {

		ImageIcon icon;

		if (slider.getOrientation() == slider.VERTICAL) {
			icon = vertThumbIcon2;
		} else {
			icon = horizThumbIcon2;
		}

		int noImages = (icon.getIconWidth() / icon.getIconHeight()) - 1;
		int pos1 = (int) ((((icon.getIconHeight()) - slider.getWidth()) / 2));
		int pos2 = (int) (((float) (slider.getValue()) / ((float) slider
				.getMaximum())) * noImages)
				* icon.getIconHeight();

		icon.paintIcon(slider, g, -pos1 - pos2, (slider.getHeight() - icon
				.getIconHeight()) / 2);
		g.dispose();
	}

	public void paintTrack(Graphics g) {
		return;
	}

	public Dimension getThumbSize() {
		if (slider.getOrientation() == slider.VERTICAL) {
			return new Dimension(vertThumbIcon2.getIconHeight(), thumbHeight);
		} else {
			return new Dimension(thumbHeight, horizThumbIcon2.getIconHeight());
		}
	}
}
