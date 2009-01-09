/*
 * @(#)SplashScreen.java	beta8	2006/04/23
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
import java.awt.Color;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JWindow;

public class SplashScreen extends JWindow {

	private String title = "MIOS Studio beta 8";
	
	private String filename = "splash.jpg";
	
	private String font = "uni05_53.ttf";
	
	private float fontSize = 32;

	public SplashScreen() {

		ImageIcon image = ImageLoader.getImageIcon(filename);
		setSize(image.getIconWidth(), image.getIconHeight());

		getContentPane().add(new JLabel(image){ 
			
			public void paintComponent(Graphics g){
				
				super.paintComponent(g);

				Graphics2D g2d = (Graphics2D) g;

				g2d.setFont(FontLoader.getFont(font, fontSize));

				FontMetrics fm = g2d.getFontMetrics();

				int width = fm.stringWidth(title);

				int height = fm.getHeight();

				int x = (getWidth() - width) / 2;

				int y = 60;

				g2d.setColor(Color.GREEN);

				g2d.fillRect(x - 5, y - height + 5, width + 5, height + 5);

				g2d.setColor(Color.BLACK);

				g2d.drawString(title, x, y);
			}
		}, BorderLayout.CENTER);
	}
}
