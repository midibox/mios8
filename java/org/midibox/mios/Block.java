/*
 * @(#)Block.java	beta8	2006/04/23
 *
 * Copyright (C) 2008   Jason Williams
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

package org.midibox.mios;

import org.midibox.utils.Utils;

public class Block {
	public long lAddress;
	public long lAddressMapped;

	public byte[] axData;

	public Block() {
		lAddress = 0;
		lAddressMapped = 0;
		axData = null;
	}

	public void dump() {
		int n = 0;
		int nRows = (axData.length / 16);
		if (axData.length % 16 != 0) {
			nRows++;
		}
		for (int nRow = 0; nRow < nRows; nRow++) {
			System.out.print(Utils.longToHex(lAddress + (nRow * 16)) + "  ");

			for (int nCol = 0; nCol < 16; nCol++) {
				if (n < axData.length) {
					System.out.print(Utils.byteToHex(axData[n]));
					System.out.print(" ");
				}
				n++;
			}
			System.out.println();
		}
		System.out.println();
	}
}
