/*
 * @(#)
 *
 * Copyright (C) 2006  Adam King (adamjking@optusnet.com.au)
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

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Rectangle;

import javax.swing.ImageIcon;
import javax.swing.JSlider;
import javax.swing.plaf.basic.BasicSliderUI;

public class FaderUI extends BasicSliderUI {

	private ImageIcon vertThumbIcon2;

	private ImageIcon horizThumbIcon2;

	private boolean drawGroove;

	private Color darkShadow = new Color(102, 102, 102);

	private Color shadow = new Color(55, 55, 55);

	private Color highlight = Color.WHITE;

	public FaderUI(ImageIcon horizThumbIcon2, ImageIcon vertThumbIcon2,
			boolean drawGroove) {
		super(null);
		this.horizThumbIcon2 = horizThumbIcon2;
		this.vertThumbIcon2 = vertThumbIcon2;
		this.drawGroove = drawGroove;
	}

	public void paintThumb(Graphics g) {

		Rectangle knobBounds = thumbRect;

		g.translate(knobBounds.x, knobBounds.y);

		if (slider.getOrientation() == JSlider.HORIZONTAL) {
			horizThumbIcon2.paintIcon(slider, g, 0, 0);
		} else {
			vertThumbIcon2.paintIcon(slider, g, 0, 0);
		}
		g.translate(-knobBounds.x, -knobBounds.y);
		g.dispose();
	}

	public void paintTrack(Graphics g) {
		Color trackColor = !slider.isEnabled() ? shadow : slider
				.getForeground();

		boolean leftToRight = slider.getComponentOrientation().isLeftToRight();

		g.translate(trackRect.x, trackRect.y);

		int trackLeft = 0;
		int trackTop = 0;
		int trackRight = 0;
		int trackBottom = 0;

		// Draw the track
		if (slider.getOrientation() == JSlider.HORIZONTAL) {
			trackTop = (int) (getThumbSize().getHeight() / 2)
					- (getTrackWidth() / 2);
			trackBottom = (trackTop + getTrackWidth());
			trackRight = trackRect.width - 1;
		} else {
			trackLeft = ((int) (getThumbSize().getWidth() / 2) - (getTrackWidth() / 2));
			trackRight = (trackLeft + getTrackWidth());
			trackBottom = trackRect.height - 1;
		}

		if (slider.isEnabled()) {
			g.setColor(darkShadow);
			g.drawRect(trackLeft, trackTop, (trackRight - trackLeft) - 1,
					(trackBottom - trackTop) - 1);

			g.setColor(highlight);
			g.drawLine(trackLeft + 1, trackBottom, trackRight, trackBottom);
			g.drawLine(trackRight, trackTop + 1, trackRight, trackBottom);

			g.setColor(shadow);
			g.drawLine(trackLeft + 1, trackTop + 1, trackRight - 2,
					trackTop + 1);
			g.drawLine(trackLeft + 1, trackTop + 1, trackLeft + 1,
					trackBottom - 2);
		} else {
			g.setColor(shadow);
			g.drawRect(trackLeft, trackTop, (trackRight - trackLeft) - 1,
					(trackBottom - trackTop) - 1);
		}

		int xoff;
		int yoff;

		if (slider.getOrientation() == JSlider.HORIZONTAL) {
			xoff = 0;
			yoff = getTrackWidth() / 2;
		} else {
			xoff = getTrackWidth() / 2;
			yoff = 0;
		}

		if (drawGroove) {

			int grooveLeft = (trackLeft + xoff)
					- (int) getThumbSize().getWidth() / 2;
			int grooveTop = (trackTop + yoff)
					- (int) getThumbSize().getHeight() / 2;
			int grooveRight = (trackRight - xoff)
					+ (int) getThumbSize().getWidth() / 2;
			;
			int grooveBottom = (trackBottom - yoff)
					+ (int) getThumbSize().getHeight() / 2;

			if (slider.isEnabled()) {
				g.setColor(darkShadow);
				g.drawRect(grooveLeft, grooveTop,
						(grooveRight - grooveLeft) - 1,
						(grooveBottom - grooveTop) - 1);

				g.setColor(highlight);
				g.drawLine(grooveLeft + 1, grooveBottom, grooveRight,
						grooveBottom);
				g.drawLine(grooveRight, grooveTop + 1, grooveRight,
						grooveBottom);

				g.setColor(shadow);
				g.drawLine(grooveLeft + 1, grooveTop + 1, grooveRight - 2,
						grooveTop + 1);
				g.drawLine(grooveLeft + 1, grooveTop + 1, grooveLeft + 1,
						grooveBottom - 2);
			} else {
				g.setColor(shadow);
				g.drawRect(grooveLeft, grooveTop,
						(grooveRight - grooveLeft) - 1,
						(grooveBottom - grooveTop) - 1);
			}
		}

		// Draw the fill
		int middleOfThumb = 0;
		int fillTop = 0;
		int fillLeft = 0;
		int fillBottom = 0;
		int fillRight = 0;

		if (slider.getOrientation() == JSlider.HORIZONTAL) {
			middleOfThumb = thumbRect.x + (thumbRect.width / 2);
			middleOfThumb -= trackRect.x; // To compensate for the
			// g.translate()
			fillTop = !slider.isEnabled() ? trackTop : trackTop + 1;
			fillBottom = !slider.isEnabled() ? trackBottom - 1
					: trackBottom - 2;

			if (!drawInverted()) {
				fillLeft = !slider.isEnabled() ? trackLeft : trackLeft + 1;
				fillRight = middleOfThumb;
			} else {
				fillLeft = middleOfThumb;
				fillRight = !slider.isEnabled() ? trackRight - 1
						: trackRight - 2;
			}
		} else {
			middleOfThumb = thumbRect.y + (thumbRect.height / 2);
			middleOfThumb -= trackRect.y; // To compensate for the
			// g.translate()
			fillLeft = !slider.isEnabled() ? trackLeft : trackLeft + 1;
			fillRight = !slider.isEnabled() ? trackRight - 1 : trackRight - 2;

			if (!drawInverted()) {
				fillTop = middleOfThumb;
				fillBottom = !slider.isEnabled() ? trackBottom - 1
						: trackBottom - 2;
			} else {
				fillTop = !slider.isEnabled() ? trackTop : trackTop + 1;
				fillBottom = middleOfThumb;
			}
		}

		if (slider.isEnabled()) {
			g.setColor(slider.getBackground());
			g.drawLine(fillLeft, fillTop, fillRight, fillTop);
			g.drawLine(fillLeft, fillTop, fillLeft, fillBottom);

			g.setColor(shadow);
			g.fillRect(fillLeft + 1, fillTop + 1, fillRight - fillLeft,
					fillBottom - fillTop);
		}

		g.setColor(shadow);
		g.fillRect(trackLeft + 1, trackTop + 1, (trackRight - trackLeft) - 2,
				(trackBottom - trackTop) - 2);

		g.translate(-trackRect.x, -trackRect.y);
	}

	public Dimension getThumbSize() {
		if (slider.getOrientation() == JSlider.HORIZONTAL) {
			return new Dimension(horizThumbIcon2.getIconWidth(),
					horizThumbIcon2.getIconHeight());
		} else {
			return new Dimension(vertThumbIcon2.getIconWidth(), vertThumbIcon2
					.getIconHeight());
		}
	}

	protected int getTrackWidth() {
		final double kIdealTrackWidth = 5.0;
		return (int) kIdealTrackWidth;
	}

	/**
	 * Returns the longer dimension of the slide bar. (The slide bar is only the
	 * part that runs directly under the thumb)
	 */
	protected int getTrackLength() {
		if (slider.getOrientation() == JSlider.HORIZONTAL) {
			return trackRect.width;
		}
		return trackRect.height;
	}
}
