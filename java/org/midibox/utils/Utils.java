/*
 * @(#)Utils.java	beta8	2006/04/23
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

package org.midibox.utils;

public class Utils {
	private static char _acHexDigits[] = { '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	public static String byteToHex(byte x) {
		return "" + _acHexDigits[(x & 0xF0) >> 4] + _acHexDigits[x & 0x0F];
	}

	public static String intToHex(int n) {
		return "" + _acHexDigits[(int) (n & 0x0000F000) >> 12]
				+ _acHexDigits[(int) (n & 0x00000F00) >> 8]
				+ _acHexDigits[(int) (n & 0x000000F0) >> 4]
				+ _acHexDigits[(int) (n & 0x0000000F)];
	}

	public static String longToHex(long l) {
		return "" + _acHexDigits[(int) (l & 0xF0000000) >> 28]
				+ _acHexDigits[(int) (l & 0x0F000000) >> 24]
				+ _acHexDigits[(int) (l & 0x00F00000) >> 20]
				+ _acHexDigits[(int) (l & 0x000F0000) >> 16]
				+ _acHexDigits[(int) (l & 0x0000F000) >> 12]
				+ _acHexDigits[(int) (l & 0x00000F00) >> 8]
				+ _acHexDigits[(int) (l & 0x000000F0) >> 4]
				+ _acHexDigits[(int) (l & 0x0000000F)];
	}

	public static String bytesToString(byte[] axData) {
		String s = new String();
		for (int n = 0; n < axData.length; n++) {
			s += byteToHex(axData[n]);
		}
		return s;
	}

	public static byte[] convertTo7Bit(byte[] axData) {
		// 8-bit to 7-bit conversion:
		// Each block is effectively treated as a set of bits
		// which is then divided into 7-bit chunks.
		// Thus: "FF 00 FF" => "7F 40 1F 70"
		// "11111111 00000000 11111111" => "-1111111 -1000000 -0011111 -1110000"

		// Calculate required 7-bit bytes to encode 8-bit bytes.
		int nBytes = (axData.length * 8) / 7;
		// Round up to nearest byte.
		if ((axData.length * 8) % 7 != 0) {
			nBytes++;
		}

		byte[] axOutData = new byte[nBytes];
		int nOutDataIndex = 0;

		byte xBitBucket = 0x00;
		int nBitBucketBits = 0;
		for (int n = 0; n < axData.length; n++) {
			byte xData = axData[n];
			for (int nBitCounter = 0; nBitCounter < 8; nBitCounter++) {
				// Accumulate bits from 8 bit byte into 7 bit bucket.
				xBitBucket = (byte) ((xBitBucket << 1) | ((xData & 0x80) == 0x80 ? 0x01
						: 0x00));
				xData = (byte) (xData << 1);

				nBitBucketBits++;
				// If bit bucket full
				if (nBitBucketBits == 7) {
					// Output bit bucket.
					axOutData[nOutDataIndex] = xBitBucket;
					// Clear bit bucket and counters.
					xBitBucket = 0x00;
					nBitBucketBits = 0;
					nOutDataIndex++;
				}
			}
		}

		// Pad the remaining bit bucket bits and output.
		if (nBitBucketBits > 0) {
			while (nBitBucketBits < 7) {
				xBitBucket = (byte) (xBitBucket << 1);
				nBitBucketBits++;
			}

			// Output bit bucket.
			axOutData[nOutDataIndex] = xBitBucket;
		}

		return axOutData;
	}

	public static byte calculateChecksum(byte[] axData1, byte[] axData2) {
		byte xChecksum = 0x00;
		for (int n1 = 0; n1 < axData1.length; n1++) {
			xChecksum = (byte) ((xChecksum + axData1[n1]) & 0xFF);
		}
		for (int n2 = 0; n2 < axData2.length; n2++) {
			xChecksum = (byte) ((xChecksum + axData2[n2]) & 0xFF);
		}
		return (byte) ((-xChecksum) & 0x7F);
	}
}
