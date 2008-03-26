/*
 * @(#)FontLoader.java	beta7	2006/04/23
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

import java.awt.Font;
import java.net.URL;
import java.util.Hashtable;

public class FontLoader {

	private static Hashtable fonts = new Hashtable();

	public static Font getFont(String fontName, float fontSize) {

		Font font;

		if (fonts.containsKey(fontName + fontSize)) {
			font = (Font) fonts.get(fontName + fontSize);
		} else {
			//ClassLoader cl = ImageLoader.class.getClassLoader();
			try {	
				/*
				font = Font.createFont(Font.TRUETYPE_FONT,
						cl.getResourceAsStream("fonts/" + fontName))
						.deriveFont(fontSize);
				*/
				font = Font.createFont(Font.TRUETYPE_FONT,
						FontLoader.class.getResourceAsStream("/fonts/" + fontName))
						.deriveFont(fontSize);
				
				fonts.put(fontName + fontSize, font);
			} catch (Exception e) {
				font = null;
			}
		}
		return font;
	}
}
