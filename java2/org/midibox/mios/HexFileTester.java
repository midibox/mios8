/*
 * @(#)HexFileTester.java	beta8	2006/04/23
 *
 * Copyright (C) 2008    Jason Williams
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

public class HexFileTester {
	public static void main(String[] args) {
		if (args.length != 1) {
			System.out.println("usage: HexFileTester file.hex");
			return;
		}

		HexFile hexFile = new HexFile();
		if (hexFile.read(args[0], false)) {
			for (int n = 0; n < hexFile.getBlockCount(); n++) {
				Block block = hexFile.getBlock(n);
				long lAddressMapped = block.lAddressMapped;

				byte[] axOutData = Utils.convertTo7Bit(block.axData);
				byte[] axHeader = new byte[4];

				axHeader[0] = (byte) ((lAddressMapped >> 10) & 0x7F);
				axHeader[1] = (byte) ((lAddressMapped >> 3) & 0x7F);
				axHeader[2] = (byte) ((block.axData.length >> 10) & 0x7F);
				axHeader[3] = (byte) ((block.axData.length >> 3) & 0x7F);

				byte xCS = Utils.calculateChecksum(axHeader, axOutData);
				System.out.println("Block "
						+ Utils.longToHex(block.lAddressMapped)
						+ "-"
						+ Utils.longToHex(block.lAddressMapped
								+ block.axData.length - 1) + " Checksum="
						+ Utils.byteToHex(xCS));

			}
		}
	}
}
