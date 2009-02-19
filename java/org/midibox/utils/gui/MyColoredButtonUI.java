package org.midibox.utils.gui;


import java.awt.Color;
import java.awt.Graphics;

import javax.swing.AbstractButton;
import javax.swing.ImageIcon;
import javax.swing.JComponent;

public class MyColoredButtonUI extends MyButtonUI {

	public static final int CIRCLE = 0;

	public static final int RECTANGLE = 1;

	private int shape;

	private Color color;

	public MyColoredButtonUI(ImageIcon on, ImageIcon off, int shape, Color color) {
		super(on, off);
		this.shape = shape;
		this.color = color;
	}

	public void paint(Graphics g, JComponent c) {
		g.setColor(color);
		if (((AbstractButton) c).getModel().isSelected()) {
			if (shape == CIRCLE) {
				g.fillOval(4, 4, d.width - 8, d.height - 8);
			} else if (shape == RECTANGLE) {
				g.fillRect(2, 2, d.width - 4, d.height - 4);
			}
		}

		super.paint(g, c);
	}
}