/*
 * @(#)DebugFunctionParametersTransferable.java	beta8	2006/04/23
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

package org.midibox.mios.gui;

import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;

import org.midibox.mios.DebugFunctionParameters;

public class DebugFunctionParametersTransferable implements Transferable,
		ClipboardOwner {

	private DebugFunctionParameters[] debugFunctionParameters;

	private DataFlavor[] dataFlavors;

	public DebugFunctionParametersTransferable(
			DebugFunctionParameters[] debugFunctionParameters) {
		this.debugFunctionParameters = debugFunctionParameters;
		dataFlavors = new DataFlavor[] { DataFlavor.stringFlavor };
	}

	public Object getTransferData(DataFlavor flavor) {

		if (flavor.equals((DataFlavor.stringFlavor))) {

			StringBuffer sbf = new StringBuffer();

			for (int d = 0; d < debugFunctionParameters.length; d++) {
				DebugFunctionParameters params = debugFunctionParameters[d];

				sbf.append(params.getAddress() + "," + params.getWreg() + ","
						+ params.getParam1() + "," + params.getParam2() + ","
						+ params.getParam3() + "\n");
			}
			return sbf.toString();
		}
		return null;
	}

	public DataFlavor[] getTransferDataFlavors() {
		return dataFlavors;
	}

	public boolean isDataFlavorSupported(DataFlavor flavor) {
		for (int f = 0; f < dataFlavors.length; f++) {
			if (flavor.equals(dataFlavors[f])) {
				return true;
			}
		}
		return false;
	}

	public void lostOwnership(Clipboard arg0, Transferable arg1) {
		// TODO Auto-generated method stub

	}
}
