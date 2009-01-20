/*
 * @(#)HexFile.java	beta8	2006/04/23
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

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.LinkedList;

public class HexFile {

	// /////////////////////////
	// Constants
	// /////////////////////////

	public static final int BLOCK_SIZE_DEFAULT = 256;

	// address mapping for MIOS8
	public static final int BLOCK_SIZE_FLASH = 256;
	public static final long HEX_FILE_FLASH_ADDRESS_START = 0x00000000;
	public static final long HEX_FILE_FLASH_ADDRESS_END = 0x0003FFFF;
	public static final long MAPPED_FLASH_ADDRESS_START = 0x00000000;
	public static final long MAPPED_FLASH_ADDRESS_END = 0x0003FFFF;

	public static final int BLOCK_SIZE_EEPROM = 256;
	public static final long HEX_FILE_EEPROM_ADDRESS_START = 0x00F00000;
	public static final long HEX_FILE_EEPROM_ADDRESS_END = 0x00F00FFF;
	public static final long MAPPED_EEPROM_ADDRESS_START = 0x00008000;
	public static final long MAPPED_EEPROM_ADDRESS_END = 0x00008FFF;

	public static final int BLOCK_SIZE_BANKSTICK = 256;
	public static final long HEX_FILE_BANKSTICK_ADDRESS_START = 0x00400000;
	public static final long HEX_FILE_BANKSTICK_ADDRESS_END = 0x0047FFFF;
	public static final long MAPPED_BANKSTICK_ADDRESS_START = 0x00010000;
	public static final long MAPPED_BANKSTICK_ADDRESS_END = 0x0001FFFF;

	// no address mapping for MIOS32
	public static final long MIOS32_IGNORED_BSL_RANGE_START = 0x08000000;
	public static final long MIOS32_IGNORED_BSL_RANGE_END = 0x08003FFF;
	// TODO: for STM32 - later we will have to request the target device via
	// SysEx request before the BSL range will be determined

	// /////////////////////////
	// Data Members
	// /////////////////////////

	//
	// Store a list of "big" blocks which are 256 bytes in size.
	// Blocks also have to be aligned on 256 byte boundaries.
	// We assume that MIOS and application code will be partitioned
	// along 256 byte boundaries.
	//
	// NB: Block bytes default to 0xFF.
	//
	private LinkedList _blocks;

	// Used to keep track of the address offset records
	// in the file.
	private long _lBaseAddress;

	// Goes true when the EOF record is read in and parsed.
	private boolean _bEOF;

	// Stores any error during read().
	private String _sLastError;

	// /////////////////////////
	// Public interface
	// /////////////////////////

	public HexFile() {
		_clear();
	}

	public int getBlockCount() {
		return _blocks.size();
	}

	public Block getBlock(int n) {
		return (Block) _blocks.get(n);
	}

	public void dump() {
		for (int n = 0; n < _blocks.size(); n++) {
			Block block = (Block) _blocks.get(n);
			block.dump();
		}
	}

	public boolean read(String sFilename, boolean mios32) {
		_clear();

		boolean bOK = false;
		try {
			BufferedReader br = new BufferedReader(new FileReader(sFilename));
			String sLine;
			int nLineNumber = 1;
			while ((sLine = br.readLine()) != null) {
				bOK = _readLine(sLine, mios32);
				if (!bOK) {
					// Stop reading file, there was an error.
					break;
				}
				if (_bEOF) {
					// Stop reading file, EOF was seen.
					bOK = true;
					break;
				}
				nLineNumber++;
			}

			br.close();

			if (!_bEOF) {
				bOK = false;
				// Not OK, we didn't reach EOF
				// If there's no error string already,
				// the error is "No EOF".
				if (_sLastError.length() == 0) {
					_sLastError = "No EOF record in file.";
				} else {
					_sLastError = "Error in line "
							+ Integer.toString(nLineNumber) + ": "
							+ _sLastError;
				}
				System.out.println(_sLastError);
			}
		} catch (FileNotFoundException e) {
			_sLastError = e.getMessage();
			System.out.println(e);
		} catch (IOException e) {
			_sLastError = e.getMessage();
			System.out.println(e);
		}

		return bOK;
	}

	// /////////////////////////
	// Private implemetation
	// /////////////////////////

	private boolean _readLine(String sLine, boolean mios32) {
		if (sLine.length() == 0) {
			return true;
		}

		// Line must start with a colon.
		if (sLine.charAt(0) != ':') {
			_sLastError = "Line did not start with a colon.";
			return false;
		}

		// Must have even number of hex digits after the colon.
		int nHexDigits = sLine.length() - 1;
		if (nHexDigits % 2 != 0) {
			_sLastError = "Line did not have an even number of hex digits after the colon.";
			return false;
		}

		int nBufferSize = nHexDigits / 2;
		// Must have at least 5 bytes.
		// 5 = data bytes (1) + address (2) + record type (1) + data (0..255) +
		// checksum (1)
		if (nBufferSize < 5) {
			_sLastError = "Line did not have at least 5 bytes.";
			return false;
		}

		byte[] axBuffer = new byte[nBufferSize];
		byte xCalcChecksum = 0;
		for (int i = 0; i < nBufferSize; i++) {
			Integer temp = new Integer(0);
			temp = Integer.decode("0x"
					+ sLine.substring((i * 2) + 1, (i * 2) + 3));
			axBuffer[i] = temp.byteValue();
			// If not last byte (which is the checksum byte)
			if (i < (nBufferSize - 1)) {
				xCalcChecksum = (byte) (((int) xCalcChecksum + (int) axBuffer[i]) % 256);
			}
		}
		xCalcChecksum = (byte) (((int) 0x100 - (int) xCalcChecksum) & 0xFF);

		int nDataBytes = axBuffer[0];

		// Check number of data bytes matches field in record.
		// 5 = data bytes (1) + address (2) + record type (1) + checksum (1)
		if (nDataBytes != (nBufferSize - 5)) {
			_sLastError = "Line did not have specified number of data bytes.";
			return false;
		}
		long lAddress = (axBuffer[1] & 0xFF) * 256 + (axBuffer[2] & 0xFF);
		byte xRecordType = axBuffer[3];
		byte[] axData = new byte[nDataBytes];
		for (int i = 4; i < 4 + nDataBytes; i++) {
			axData[i - 4] = axBuffer[i];
		}
		byte xChecksum = axBuffer[nBufferSize - 1];

		if (xChecksum != xCalcChecksum) {
			_sLastError = "Checksum did not match.";
			return false;
		}

		if (xRecordType == 0x00) {
			long lFullAddress = _lBaseAddress + lAddress;

			Block block = new Block();

			block.lAddress = lFullAddress;
			block.lAddressMapped = lFullAddress; // will be changed of memory
			// is mapped to other target
			// address
			block.axData = axData;

			_insertBlock(block, mios32);
		} else if (xRecordType == 0x01) {
			_bEOF = true;
			return true;
		} else if (xRecordType == 0x02) {
			// This is here for completeness.
			// This record type is unobserved in MPASM hex files.
			_lBaseAddress = (axBuffer[4] & 0xFF) * 256 + (axBuffer[5] & 0xFF);
			_lBaseAddress = _lBaseAddress << 4;
		} else if (xRecordType == 0x04) {
			_lBaseAddress = (axBuffer[4] & 0xFF) * 256 + (axBuffer[5] & 0xFF);
			_lBaseAddress = _lBaseAddress << 16;
		}

		return true;
	}

	private boolean _insertBlock(Block newBlock, boolean mios32) {
		// First work out which big block this newBlock belongs to.
		// Then find this big block in the list.
		// If it exists, copy newBlock into it.
		// If it doesn't exist, create it and then copy newBlock into it.

		// TK: it's possible that a line contains code for two blocks (unaligned
		// addresses)
		// therefore we have to insert the data stepwise

		int nBytePos;
		for (nBytePos = 0; nBytePos < newBlock.axData.length; ++nBytePos) {
			byte bData = newBlock.axData[nBytePos];
			long lByteAddress = newBlock.lAddress + nBytePos;

			int nBlockSize = BLOCK_SIZE_DEFAULT;
			long lMapAddressOffset = 0;
			if (mios32) {
				// MIOS32: ignore BSL range
				if (lByteAddress >= MIOS32_IGNORED_BSL_RANGE_START
						&& lByteAddress <= MIOS32_IGNORED_BSL_RANGE_END) {
					// We don't store the block.
					return true;
				}
			} else {
				// MIOS8: map EEPROM/BankStick address
				if (lByteAddress >= HEX_FILE_FLASH_ADDRESS_START
						&& lByteAddress <= HEX_FILE_FLASH_ADDRESS_END) {
					nBlockSize = BLOCK_SIZE_FLASH;
				} else if (lByteAddress >= HEX_FILE_EEPROM_ADDRESS_START
						&& lByteAddress <= HEX_FILE_EEPROM_ADDRESS_END) {
					nBlockSize = BLOCK_SIZE_EEPROM;
					lMapAddressOffset = MAPPED_EEPROM_ADDRESS_START
							- HEX_FILE_EEPROM_ADDRESS_START;
				} else if (lByteAddress >= HEX_FILE_BANKSTICK_ADDRESS_START
						&& lByteAddress <= HEX_FILE_BANKSTICK_ADDRESS_END) {
					nBlockSize = BLOCK_SIZE_BANKSTICK;
					lMapAddressOffset = MAPPED_BANKSTICK_ADDRESS_START
							- HEX_FILE_BANKSTICK_ADDRESS_START;
				} else {
					// We don't store the block.
					return true;
				}
			}

			long lBigBlockAddress = lByteAddress / nBlockSize * nBlockSize;
			int nBigBlockIndex = -1;
			boolean bFoundBigBlock = false;
			for (int n = 0; n < _blocks.size(); n++) {
				Block block = (Block) _blocks.get(n);
				if (lBigBlockAddress == block.lAddress) {
					nBigBlockIndex = n;
					bFoundBigBlock = true;
					break;
				}
				if (lBigBlockAddress < block.lAddress) {
					nBigBlockIndex = n;
					break;
				}
			}

			if (!bFoundBigBlock) {
				Block newBigBlock = new Block();
				newBigBlock.axData = new byte[nBlockSize];
				Arrays.fill(newBigBlock.axData, (byte) 0xFF);
				newBigBlock.lAddress = lBigBlockAddress;
				newBigBlock.lAddressMapped = lBigBlockAddress
						+ lMapAddressOffset;

				if (nBigBlockIndex == -1) {
					// Append new big block.
					_blocks.add(newBigBlock);
					nBigBlockIndex = _blocks.size() - 1;
				} else {
					// Insert new big block.
					_blocks.add(nBigBlockIndex, newBigBlock);
				}
			}

			// Copy the data into the big block.
			Block bigBlock = (Block) _blocks.get(nBigBlockIndex);
			long lOffset = lByteAddress - bigBlock.lAddress;

			// System.out.println("#" + nBytePos + " Block " + lBigBlockAddress
			// + " Addr " + newBlock.lAddress + " Offset" + lOffset + ", len: "
			// + newBlock.axData.length);

			bigBlock.axData[(int) lOffset] = bData;
		}

		return true;
	}

	private void _clear() {
		_blocks = new LinkedList();
		_lBaseAddress = 0;
		_bEOF = false;
		_sLastError = "";
	}
}
