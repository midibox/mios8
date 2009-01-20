/*
 * @(#)SimpleFileChooserFilter.java	beta8	2006/04/23
 *
 * Copyright (C) 2008    Adam King
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

import java.io.File;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.swing.JFileChooser;
import javax.swing.filechooser.FileFilter;

public class SimpleFileChooserFilter extends FileFilter {

	private Hashtable fileTypes;

	private String filterDescription;

	public SimpleFileChooserFilter(String filterDescription) {
		this.filterDescription = filterDescription;
		fileTypes = new Hashtable();
	}

	public SimpleFileChooserFilter(String filterDescription, String extension,
			String description) {
		this.filterDescription = filterDescription;
		fileTypes = new Hashtable();
		addFileType(extension, description);
	}

	public void addFileType(String extension, String description) {
		fileTypes.put(extension, description);
	}

	public boolean accept(File f) {
		if (f.isDirectory()) {
			return true;
		}

		String extension = getExtension(f);

		if (extension != null) {
			if (fileTypes.containsKey(extension)) {
				return true;
			} else {
				return false;
			}
		}
		return false;
	}

	public String getDescription() {

		String description = filterDescription + " (";

		Enumeration e = fileTypes.keys();

		while (e.hasMoreElements()) {
			String key = (String) e.nextElement();
			description += ("*." + key + ((e.hasMoreElements()) ? ", " : ")"));
		}

		return description;
	}

	public void setFilterForChooser(JFileChooser fileChooser) {

		Enumeration e = fileTypes.keys();

		while (e.hasMoreElements()) {
			String key = (String) e.nextElement();
			String value = (String) fileTypes.get(key);
			fileChooser.addChoosableFileFilter(new SimpleFileChooserFilter(
					value, key, value));
		}

		fileChooser.addChoosableFileFilter(this);
	}

	public String getTypeDescription(File f) {
		String extension = getExtension(f);
		String type = null;

		if (extension != null) {
			type = (String) fileTypes.get(extension);
		}
		return type;
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
