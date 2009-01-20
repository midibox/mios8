/*
 * @(#)SimpleFileFilter.java	beta8	2006/04/23
 *
 * Copyright (C) 2008   Adam King (adamjking@optusnet.com.au)
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

package org.midibox.utils;

import java.io.File;
import java.io.FileFilter;
import java.util.Vector;

public class SimpleFileFilter implements FileFilter {

	private Vector fileTypes;

	public SimpleFileFilter() {
		fileTypes = new Vector();
	}

	public boolean accept(File pathname) {

		if (pathname.isDirectory()) {
			return true;
		}

		String extension = getExtension(pathname);

		if (extension != null) {
			if (fileTypes.contains(extension)) {
				return true;
			} else {
				return false;
			}
		}
		return false;
	}

	public void addFileType(String extension) {
		fileTypes.add(extension);
	}

	private static String getExtension(File f) {
		String ext = null;
		if (f != null) {
			String s = f.getName();
			int i = s.lastIndexOf('.');

			if (i > 0 && i < s.length() - 1) {
				ext = s.substring(i + 1).toLowerCase();
			}
		}
		return ext;
	}
}
