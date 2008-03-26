/*
 * @(#)MyButtonUI.java	beta7	2006/04/23
 *
 * Copyright (C) 2006    Adam King (adamjking@optusnet.com.au)
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

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.AbstractButton;
import javax.swing.ButtonModel;
import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.UIManager;
import javax.swing.plaf.basic.BasicToggleButtonUI;

public class MyButtonUI extends BasicToggleButtonUI {

	private final static Color FOCUS_COLOR = UIManager.getColor("Slider.focus");

	protected ImageIcon onIcon;

	protected ImageIcon offIcon;

	protected Dimension d;

	public MyButtonUI(ImageIcon on, ImageIcon off) {
		onIcon = on;
		offIcon = off;
		d = new Dimension(onIcon.getIconWidth() + 4, onIcon.getIconHeight() + 4);
	}

	public void paint(Graphics g, JComponent c) {
		AbstractButton b = (AbstractButton) c;
		ButtonModel model = b.getModel();
		c.setBorder(null);
		c.setOpaque(false);

		if (model.isSelected() || model.isPressed()) {
			onIcon.paintIcon(c, g, (c.getWidth() - onIcon.getIconWidth()) / 2,
					(c.getHeight() - onIcon.getIconHeight()) / 2);
		} else {
			offIcon.paintIcon(c, g,
					(c.getWidth() - offIcon.getIconWidth()) / 2,
					(c.getHeight() - offIcon.getIconHeight()) / 2);
		}

		if (b.hasFocus()) {
			paintFocus(g, b);
		}
		g.dispose();
	}

	protected void paintFocus(Graphics g, AbstractButton c) {
		AbstractButton b = (AbstractButton) c;

		g.setColor(FOCUS_COLOR);
		Graphics2D g2d = (Graphics2D) g;

		g2d.setStroke(new BasicStroke(1f, BasicStroke.CAP_BUTT,
				BasicStroke.JOIN_ROUND, 1f, new float[] { 1f }, 0f));

		int width = b.getWidth();
		int height = b.getHeight();

		g2d.drawLine(0, 0, width, 0);
		g2d.drawLine(width - 1, 0, width - 1, height);
		g2d.drawLine(0, height - 1, width, height - 1);
		g2d.drawLine(0, 0, 0, height);

		g2d.setStroke(new BasicStroke());
		g2d.setColor(Color.BLACK);
	}

	public Dimension getPreferredSize(JComponent c) {
		return d;
	}

	public Dimension getMinimumSize(JComponent c) {
		return d;
	}
}
