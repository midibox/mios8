/*
 * @(#)SimpleTransferHandler.java	beta7	2006/04/23
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

import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;

import javax.swing.JComponent;
import javax.swing.TransferHandler;

public class SimpleTransferHandler extends TransferHandler {

	protected DataFlavor[] supportedDataFlavors;

	protected boolean justImported = false;

	protected boolean justExported = false;

	protected boolean dropOnSelf = false;

	public int getSourceActions(JComponent c) {
		return COPY;
	}

	public boolean canImport(JComponent c, DataFlavor[] flavors) {
		try {
			if (justExported && !dropOnSelf) {
				return false;
			} else {
				for (int f = 0; f < flavors.length; f++) {
					if (hasFlavor(supportedDataFlavors, flavors[f])) {
						return true;
					}
				}
				return false;
			}
		} catch (Exception e) {
			return false;
		}
	}

	public void setDropOnSelf(boolean dropOnSelf) {
		this.dropOnSelf = dropOnSelf;
	}

	protected boolean hasFlavor(DataFlavor[] flavors, DataFlavor flavor) {
		for (int i = 0; i < flavors.length; i++) {
			if (flavor.equals(flavors[i])) {
				return true;
			}
		}
		return false;
	}

	protected void exportDone(JComponent c, Transferable data, int action) {
		justExported = false;
	}
}
