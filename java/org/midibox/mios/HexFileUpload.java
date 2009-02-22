/*
 * @(#)HexFileUpload.java	beta8	2006/04/23
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

package org.midibox.mios;

import java.io.File;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

import org.midibox.utils.Utils;

public class HexFileUpload extends MIOSSysexSendReceive {

	public final static Object FILE = new Object();

	public final static Object UPLOAD_MODE = new Object();

	public final static Object WAIT_FOR_UPLOAD = new Object();

	public final static Object DELAY_TIME = new Object();
	
	public final static String REBOOT = "REBOOT";

	public final static int SMART_MODE = 0;

	public final static int DUMB_MODE = 1;

	private File file;

	//private int bankstickNo;

	private int uploadMode;

	private boolean waitForUploadRequest;

	private int delayTime;

	private long fileLastModified;

	private boolean fileLastMIOS32_Mode;

	private HexFile hexFile;

	private int currentBlock;

	private int totalBlocks;

	private boolean expectUploadRequest = false;

	private boolean expectQueryReply = false;

	private byte expectQueryReplyNumber = 0;

	private boolean expectDummyError = false;

	private boolean gotGoodAck = false;

	private int uploadAttempt = 0;

	private int currentChecksum;

	private int nUploadCase;

	public HexFileUpload(Receiver receiver) {
		super(receiver);

		this.waitForUploadRequest = false;

		this.uploadMode = 0;

		this.delayTime = 300;
	}

	public File getFile() {
		return file;
	}

	public boolean fileChanged() {
		return file.lastModified() != fileLastModified;
	}

	public void setFile(File file) {
		this.file = file;
		fileLastModified = file.lastModified();

		readHexFile();

		setChanged();
		notifyObservers(FILE);
		clearChanged();
	}

	public void readHexFile() {
		hexFile = new HexFile();
		fileLastMIOS32_Mode = isMIOS32Mode();
		if (!hexFile.read(file.getPath(), fileLastMIOS32_Mode)) {
			// TODO
		}
	}


	public int getDelayTime() {
		return delayTime;
	}

	public void setDelayTime(int delayTime) {
		this.delayTime = delayTime;

		setChanged();
		notifyObservers(DELAY_TIME);
		clearChanged();
	}

	public int getUploadMode() {
		return uploadMode;
	}

	public void setUploadMode(int uploadMode) {
		this.uploadMode = uploadMode;

		setChanged();
		notifyObservers(UPLOAD_MODE);
		clearChanged();
	}

	public boolean isWaitForUploadRequest() {
		return waitForUploadRequest;
	}

	public void setWaitForUploadRequest(boolean waitForUploadRequest) {
		this.waitForUploadRequest = waitForUploadRequest;

		setChanged();
		notifyObservers(WAIT_FOR_UPLOAD);
		clearChanged();
	}

	public void createUpload() {

		expectQueryReply = false;
		cancelled = false;
		done = false;

		setChanged();
		notifyObservers(WORKER);
		clearChanged();

		nUploadCase = 0;
		
		synchronized (this) {

			addMessage("Starting upload of " + file.getName());

			if (fileLastMIOS32_Mode != isMIOS32Mode()) {
				addMessage("MIOS version has been changed - reloading file");
				readHexFile();
			}

			// TK: determine .hex file type, we have three cases:
			// (only relevant for PIC MIOS, not for MIOS32)
			// (1) .hex file contains code between 0x0400-0x2fff: MIOS
			// range, upload via bootloader
			// (0x000-0x3ff for downward compatibility with MIOS V1.8 and
			// lower)
			// (2) .hex file contains code between 0x3000 and 0x30ff: start
			// vector of application, upload via MIOS
			// (0) .hex file contains flash data >= 0x3100 (no special
			// measure), EEPROM or BankStick data

			// in case (1) we force a reboot and always wait for an upload
			// request
			// in case (1) we upload a stall code to 0x400 in order to
			// ensure, that the new code
			// won't be started before the upload is completely passed
			// in case (2) we upload a stall code to 0x3000

			// in case (1) and (2) we upload the first block (0x400 or
			// 0x3000) at the end!

			expectUploadRequest = false;
			expectDummyError = false;
			boolean forceRebootReq = false;

			if (!isMIOS32Mode()) {
				for (int i = 0; i < hexFile.getBlockCount(); ++i) {
					Block block = hexFile.getBlock(i);
					long lAddressMapped = block.lAddressMapped;
					if (lAddressMapped >= 0x0000 && lAddressMapped <= 0x2fff) {
						nUploadCase = 1;
					} else if (nUploadCase != 1 && lAddressMapped >= 0x3000
							&& lAddressMapped <= 0x30ff) {
						nUploadCase = 2;
					}
				}

				if (uploadMode == 0 && nUploadCase == 1) {
					expectUploadRequest = true;
				}

				if (nUploadCase == 1) {
					addMessage("Hex file contains code in MIOS range, forcing reboot for upload via 1st level bootloader!\nThe reboot request will lead to an error acknowledge, please ignore!");
					forceRebootReq = true;
				}

			} else {
				nUploadCase = 1; // MIOS32 - always request reset

				expectUploadRequest = true;
				forceRebootReq = true;

				addMessage("Requesting reboot...");
			}

			totalBlocks = hexFile.getBlockCount();
			if (totalBlocks == 0) {
				done = true;
				addMessage("Hex file is empty, or wrong MIOS version selected (e.g. check for MIOS32)");
				return;
			}

			// "wait until upload request" goes here.
			if (waitForUploadRequest) {
				expectUploadRequest = true;
			}

			if (expectUploadRequest) {
				addMessage("Waiting for upload request...");
			}

			while (expectUploadRequest && !cancelled) {
				if (forceRebootReq) { // TK: ensure that expectUploadRequest
					// is set before message will be sent
					// (race condition)
					forceRebootReq = false;
					forceReboot();
				}

				try {
					wait();

					// receivedRead routine sets expectUploadRequest to
					// false once it has been received
					if (!expectUploadRequest) {
						// Thread.sleep(1500);
					}
				} catch (Exception e) {
					addMessage("Error: Cannot wait for upload request");
				}
			}

			// don't move the variable initialisation at this location!!!
			// it can happen, that a MIDIbox has been powered off during upload
			// - in this case,
			// the previous attempt to upload a code block can get stucked
			// setting currentBlock to 0 at this location ensures, that we are
			// starting with the first block!
			long timeUploadBegin = System.currentTimeMillis();
			currentBlock = 0;
			uploadAttempt = 0;

			while (!cancelled && !done) {

				// This ensures _bDone is set correctly.
				if (currentBlock >= totalBlocks) {
					done = true;
					long timeUploadEnd = System.currentTimeMillis();
					float timeUpload = (float) (timeUploadEnd - timeUploadBegin) / 1000;
					float transferRateKb = ((totalBlocks * 256) / timeUpload) / 1024;
					
					addMessage("Upload of " + (totalBlocks * 256)
							+ " bytes completed after " + timeUpload + "s ("
							+ transferRateKb + " kb/s)");

					// reboot BSL in MIOS32 mode
					if (isMIOS32Mode()) {
						forceReboot();
					} else {
						
						setChanged();
						notifyObservers(REBOOT);
						clearChanged();		
					}					
					
					break;
				}

				Block block = hexFile.getBlock(currentBlock);
				long lAddress = block.lAddress;
				long lAddressMapped = block.lAddressMapped;

				byte[] axDumpData = Utils.convertTo7Bit(block.axData);

				long lAddressExtension = 0x00;
				byte[] axAddress;

				if (!isMIOS32Mode()) {
					// build address/len field of MIOS SysEx message

					if (lAddress >= HexFile.HEX_FILE_FLASH_ADDRESS_START
							&& lAddress <= HexFile.HEX_FILE_FLASH_ADDRESS_END) {
						lAddressExtension = (lAddress >> 15) & 0x07;
						lAddressMapped &= 0x7fff;
						// System.out.println("Flash");
					} else if (lAddress >= HexFile.HEX_FILE_EEPROM_ADDRESS_START
							&& lAddress <= HexFile.HEX_FILE_EEPROM_ADDRESS_END) {
						// System.out.println("EEPROM");
					} else if (lAddress >= HexFile.HEX_FILE_BANKSTICK_ADDRESS_START
							&& lAddress <= HexFile.HEX_FILE_BANKSTICK_ADDRESS_END) {
						// System.out.println("Bankstick");
						lAddressExtension = (lAddress >> 16) & 0x07;
						lAddressMapped = (lAddressMapped & 0xffff) | 0x10000;
					}

					else {
						// Invalid range, do nothing.
						System.out.println("invalid");
						currentBlock++;
						continue;
					}

					axAddress = new byte[4];
					axAddress[0] = (byte) ((lAddressMapped >> 10) & 0x7F);
					axAddress[1] = (byte) ((lAddressMapped >> 3) & 0x7F);
					axAddress[2] = (byte) ((block.axData.length >> 10) & 0x7F);
					axAddress[3] = (byte) ((block.axData.length >> 3) & 0x7F);

				} else {
					// build address/len field of MIOS32 SysEx message

					axAddress = new byte[8];
					axAddress[0] = (byte) ((lAddressMapped >> 25) & 0x7F);
					axAddress[1] = (byte) ((lAddressMapped >> 18) & 0x7F);
					axAddress[2] = (byte) ((lAddressMapped >> 11) & 0x7F);
					axAddress[3] = (byte) ((lAddressMapped >> 4) & 0x7F);
					axAddress[4] = (byte) ((block.axData.length >> 25) & 0x7F);
					axAddress[5] = (byte) ((block.axData.length >> 18) & 0x7F);
					axAddress[6] = (byte) ((block.axData.length >> 11) & 0x7F);
					axAddress[7] = (byte) ((block.axData.length >> 4) & 0x7F);
				}

				currentChecksum = Utils
						.calculateChecksum(axAddress, axDumpData);

				byte[] axSysExData = new byte[7 + axAddress.length
						+ axDumpData.length + 2];
				axSysExData[0] = (byte) 0xF0;
				axSysExData[1] = (byte) 0x00;
				axSysExData[2] = (byte) 0x00;
				axSysExData[3] = (byte) 0x7E;
				axSysExData[4] = getMIOS_SysExId();
				axSysExData[5] = (byte) (deviceID & 0x7F);
				axSysExData[6] = (byte) (0x02 | (lAddressExtension << 4)); // Write

				System
						.arraycopy(axAddress, 0, axSysExData, 7,
								axAddress.length); // insert address
				System.arraycopy(axDumpData, 0, axSysExData,
						7 + axAddress.length, axDumpData.length); // insert
				// data

				axSysExData[axSysExData.length - 2] = (byte) currentChecksum; // Checksum
				// is
				// second-last
				axSysExData[axSysExData.length - 1] = (byte) 0xF7; // SysEx
				// status
				// byte is
				// last

				String s = "Sending block #"
						+ (currentBlock + 1)
						+ " "
						+ Utils.longToHex(block.lAddress)
						+ "-"
						+ Utils.longToHex(block.lAddress + block.axData.length
								- 1);
				// + " (" + Utils.longToHex(lAddressExtension) + ":" +
				// Utils.longToHex(lAddressMapped) + ")";

				addMessage(s);

				try {
					gotGoodAck = false;
					SysexMessage sysExMessage = new SysexMessage();
					sysExMessage.setMessage(axSysExData, axSysExData.length);

					receiver.send(sysExMessage, -1);
					uploadAttempt++;
				} catch (InvalidMidiDataException ex) {
					cancelled = true;
					addMessage("Error: " + ex.getMessage());
					break;
				}

				try {
					if (uploadMode == 0) {
						wait();
						// TK: ensure that block won't be incremented if we
						// haven't got an acknowledge
						if (gotGoodAck)
							currentBlock++;
					} else {
						wait(delayTime);
						currentBlock++;
					}

				} catch (InterruptedException e) {
					cancelled = true;
					addMessage("Error: Upload task interrupted");
					break;
				}
			}
		}
	}

	public void createQuery() {

		if (!isMIOS32Mode()) {
			addMessage("Query is only supported for MIOS32");
			return;
		}

		expectQueryReply = false;
		cancelled = false;
		done = false;

		setChanged();
		notifyObservers(WORKER);
		clearChanged();

		synchronized (this) {

			expectQueryReplyNumber = 0x01;

			while (!cancelled && !done) {

				// build query request
				byte[] queryCode = new byte[9];
				queryCode[0] = (byte) 0xF0;
				queryCode[1] = (byte) 0x00;
				queryCode[2] = (byte) 0x00;
				queryCode[3] = (byte) 0x7E;
				queryCode[4] = getMIOS_SysExId();
				queryCode[5] = (byte) (deviceID & 0x7F); // device-id
				queryCode[6] = (byte) 0x00; // query command
				queryCode[7] = (byte) expectQueryReplyNumber;
				queryCode[8] = (byte) 0xF7;

				try {
					SysexMessage sysExMessage = new SysexMessage();
					sysExMessage.setMessage(queryCode, queryCode.length);
					receiver.send(sysExMessage, -1);
				} catch (InvalidMidiDataException ex) {
					cancelled = true;
					addMessage("Error: " + ex.getMessage());
					break;
				}

				expectQueryReply = true;
				while (!cancelled && expectQueryReply) {
					try {
						wait();
					} catch (Exception e) {
						addMessage("Error: Cannot wait for query reply");
					}
				}

				if (!cancelled) {
					if (expectQueryReplyNumber < 0x09) {
						++expectQueryReplyNumber;
					} else {
						done = true;
						addMessage("All queries done.");
					}
				}
			}
		}
	}

	public void forceReboot() {

		byte[] forceRebootCode;

		if (isMIOS32Mode()) {
			forceRebootCode = new byte[9];
			forceRebootCode[0] = (byte) 0xF0;
			forceRebootCode[1] = (byte) 0x00;
			forceRebootCode[2] = (byte) 0x00;
			forceRebootCode[3] = (byte) 0x7E;
			forceRebootCode[4] = getMIOS_SysExId();
			forceRebootCode[5] = (byte) (deviceID & 0x7F); // device-id
			forceRebootCode[6] = (byte) 0x00; // query command
			forceRebootCode[7] = (byte) 0x7f; // enter BSL
			forceRebootCode[8] = (byte) 0xF7;
		} else {
			forceRebootCode = new byte[14];

			// dummy write operation, core will return error 01 "less bytes than
			// expected"
			expectDummyError = true;

			forceRebootCode[0] = (byte) 0xF0;
			forceRebootCode[1] = (byte) 0x00;
			forceRebootCode[2] = (byte) 0x00;
			forceRebootCode[3] = (byte) 0x7E;
			forceRebootCode[4] = getMIOS_SysExId();
			forceRebootCode[5] = (byte) (deviceID & 0x7F); // device-id
			forceRebootCode[6] = (byte) 0x02; // dummy write
			forceRebootCode[7] = (byte) 0x00; // dummy address
			forceRebootCode[8] = (byte) 0x00;
			forceRebootCode[9] = (byte) 0x00;
			forceRebootCode[10] = (byte) 0x00; // dummy count
			forceRebootCode[11] = (byte) 0x00;
			forceRebootCode[12] = (byte) 0x00; // dummy byte
			forceRebootCode[13] = (byte) 0xF7;
		}

		try {
			SysexMessage sysExMessage = new SysexMessage();
			sysExMessage.setMessage(forceRebootCode, forceRebootCode.length);
			receiver.send(sysExMessage, -1);
			
			setChanged();
			notifyObservers(REBOOT);
			clearChanged();	
			
		} catch (InvalidMidiDataException ex) {
			cancelled = true;
			addMessage("Error: " + ex.getMessage());
			return;
		}
	}

	protected void retryBlock() {
		if (uploadAttempt > 15) { // TKNEW
			cancelled = true;
			addMessage("Aborting after 16 errors");
			uploadAttempt = 0; // reset counter
			gotGoodAck = true;
		}
	}

	protected void receivedErrorCode(byte[] data) {
		synchronized (this) {
			super.receivedErrorCode(data);

			int errorCode = data[6];
			if (expectDummyError) {
				expectDummyError = false;
				addMessage("This was an expected error - please ignore!");
			} else if (errorCode == 8) {
				addMessage("Skipping this code block due to invalid address range!");
				notify();
			} else if (expectQueryReply && errorCode == 13) {
				expectQueryReply = false; // query not supported - skip it
				addMessage("Query Request #" + expectQueryReplyNumber
						+ " not supported by core!");
				notify();
			} else if (uploadMode == 0) {
				retryBlock();
				notify();
			}
		}
	}

	protected void receivedReturnValues(byte[] data) {
		synchronized (this) {
			StringBuffer buffer = new StringBuffer();

			if (expectQueryReply) {
				int i;
				String str = "";
				for (i = 6; i < data.length - 1; ++i) {
					str += (char) data[i];
				}

				switch (expectQueryReplyNumber) {
				case 0x01:
					buffer.append("Operating System: " + str);
					break;
				case 0x02:
					buffer.append("Board: " + str);
					break;
				case 0x03:
					buffer.append("Core Family: " + str);
					break;
				case 0x04:
					buffer.append("Chip ID: 0x" + str);
					break;
				case 0x05:
					buffer.append("Serial Number: #" + str);
					break;
				case 0x06:
					buffer.append("Flash Memory Size: " + str + " bytes");
					break;
				case 0x07:
					buffer.append("RAM Size: " + str + " bytes");
					break;
				case 0x08: // Application First Line
				case 0x09: // Application Second Line
					buffer.append(str);
					break;
				default:
					buffer.append("Query #" + expectQueryReplyNumber + ": "
							+ str);
				}
				expectQueryReply = false;

			} else if (uploadMode == 0) {

				// TK: FIXME - checksum is located at data[6] ---
				// data[data.length - 2] can fail if more than one message
				// is returned (Java/M-Audio MIDI bug)
				// byte checkSum = (byte) data[data.length - 2];
				byte checkSum = (byte) data[6];

				buffer.append("Received Checksum: ");
				buffer.append(Utils.byteToHex(checkSum));
				if (checkSum == currentChecksum) {
					buffer.append(" - OK");
					uploadAttempt = 0;
					gotGoodAck = true;
				} else {
					buffer.append(" - Error");
					retryBlock();
				}
			} else {
				buffer.append("Received acknowledgement response");
			}
			addMessage(buffer.toString());
			if (uploadMode == 0) {
				notify();
			}
		}
	}

	protected void receivedRead(byte[] data) {
		synchronized (this) {
			if (expectUploadRequest) {
				addMessage("Received Upload Request");
				expectUploadRequest = false; // must be set to false, so
				// that wait routine
				// continues
			} else {
				addMessage("Error: Received unexpected Upload Request");
			}
			currentBlock = 0; // in any case: re-starting with first block
			notify(); // TK: notify *after* expectUploadRequest has been
			// changed!
		}
	}
}
