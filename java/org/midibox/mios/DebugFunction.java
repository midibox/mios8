/*
 * @(#)DebugFunction.java	beta8	2006/04/23
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

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Iterator;
import java.util.Vector;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

import org.midibox.midi.MidiUtils;
import org.midibox.utils.ResourceLoader;

public class DebugFunction extends MIOSSysexSendReceive {

	public final static Object MODE = new Object();

	public final static Object DEBUG_FUNCTION_PARAMETERS = new Object();

	public final static Object DELAY_TIME = new Object();

	public final static Object SRAM_READ_ADDRESS = new Object();

	public final static Object SRAM_READ_COUNTER = new Object();

	public final static Object SRAM_WRITE_ADDRESS = new Object();

	public final static Object SRAM_WRITE_DATA = new Object();

	public final static int DEBUG_FUNCTIONS_MODE = 1;

	public final static int SRAM_READ_MODE = 2;

	public final static int SRAM_WRITE_MODE = 3;

	protected int mode;

	protected Vector miosVectorNames;

	protected Vector miosVectorValues;

	protected Vector debugFunctionParameters;

	protected int delayTime;

	protected int sramReadAddress;

	protected int sramReadCounter;

	protected int sramWriteAddress;

	protected int sramWriteData;

	private boolean returnHex;

	private Vector messageParameters;

	private int noReturnValues = 0;

	private int colNo = 0;

	private byte[] dump;

	private int dumpAddress = 0;

	public DebugFunction(Receiver receiver) {
		super(receiver);

		this.mode = DEBUG_FUNCTIONS_MODE;

		this.debugFunctionParameters = new Vector();

		this.delayTime = 300;

		this.sramReadAddress = 0;

		this.sramReadCounter = 0;

		this.sramWriteAddress = 0;

		this.sramWriteData = 0;

		importDebugfunctions();
	}

	private void importDebugfunctions() {
		{
			miosVectorNames = new Vector();

			miosVectorValues = new Vector();

			try {
				BufferedReader br = new BufferedReader(new InputStreamReader(
						ResourceLoader
								.getResourceAsStream("mios/mios_vectors.inc")));

				String sLine;
				while ((sLine = br.readLine()) != null) {

					if (!sLine.startsWith("MIOS_RESERVED")) {

						String[] temp = sLine.split("\\s+");

						miosVectorNames.addElement(temp[0]);

						miosVectorValues.addElement(Integer.decode(temp[2]));
					}
				}
			} catch (Exception e) {

			}
		}
	}

	public Vector getMiosVectorNames() {
		return miosVectorNames;
	}

	public Vector getMiosVectorValues() {
		return miosVectorValues;
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

	public int getMode() {
		return mode;
	}

	public void setMode(int mode) {
		this.mode = mode;

		setChanged();
		notifyObservers(MODE);
		clearChanged();
	}

	public int getSramReadAddress() {
		return sramReadAddress;
	}

	public void setSramReadAddress(int readAddress) {
		this.sramReadAddress = readAddress;

		setChanged();
		notifyObservers(SRAM_READ_ADDRESS);
		clearChanged();
	}

	public int getSramReadCounter() {
		return sramReadCounter;
	}

	public void setSramReadCounter(int readCounter) {
		this.sramReadCounter = readCounter;

		setChanged();
		notifyObservers(SRAM_READ_COUNTER);
		clearChanged();
	}

	public int getSramWriteAddress() {
		return sramWriteAddress;
	}

	public void setSramWriteAddress(int writeAddress) {
		this.sramWriteAddress = writeAddress;

		setChanged();
		notifyObservers(SRAM_WRITE_ADDRESS);
		clearChanged();
	}

	public int getSramWriteData() {
		return sramWriteData;
	}

	public void setSramWriteData(int writeData) {
		this.sramWriteData = writeData;

		setChanged();
		notifyObservers(SRAM_WRITE_DATA);
		clearChanged();
	}

	public Vector getDebugFunctionParameters() {
		return debugFunctionParameters;
	}

	public void insertDebugFunctionParameters(DebugFunctionParameters params,
			int pos) {
		if (params != null) {
			debugFunctionParameters.insertElementAt(params, pos);
		}

		setChanged();
		notifyObservers(DEBUG_FUNCTION_PARAMETERS);
		clearChanged();
	}

	public void insertDebugFunctionParameters(DebugFunctionParameters[] params,
			int pos) {
		for (int i = params.length - 1; i >= 0; i--) {
			insertDebugFunctionParameters(params[i], pos);
		}
	}

	public void addDebugFunctionParameters(DebugFunctionParameters params) {
		insertDebugFunctionParameters(params, debugFunctionParameters.size());
	}

	public void addDebugFunctionParameters(DebugFunctionParameters[] params) {
		for (int i = params.length - 1; i >= 0; i--) {
			addDebugFunctionParameters(params[i]);
		}
	}

	public DebugFunctionParameters createDebugFunctionParameters() {

		DebugFunctionParameters params = new DebugFunctionParameters();

		addDebugFunctionParameters(params);

		return params;
	}

	public void removeDebugFunctionParameters(int paramNo) {

		debugFunctionParameters.removeElementAt(paramNo);

		setChanged();
		notifyObservers(DEBUG_FUNCTION_PARAMETERS);
		clearChanged();
	}

	public void removeDebugFunctionParameters(int[] paramNos) {
		for (int i = paramNos.length - 1; i >= 0; i--) {
			removeDebugFunctionParameters(paramNos[i]);
		}
	}

	public void createDebugFunctionsTask(boolean returnHex) {

		Vector messageParameters = new Vector();

		if (mode == DEBUG_FUNCTIONS_MODE) {

			Iterator it = debugFunctionParameters.iterator();

			while (it.hasNext()) {
				DebugFunctionParameters params = (DebugFunctionParameters) it
						.next();

				if (params.address != 0) {
					messageParameters.add(params);
				}
			}

		} else if (mode == SRAM_READ_MODE) {

			DebugFunctionParameters readParameter = new DebugFunctionParameters();

			readParameter.address = sramReadAddress;
			readParameter.wreg = sramReadCounter >> 8 & 0xFF;
			readParameter.param1 = sramReadCounter & 0xFF;
			readParameter.param2 = 0;
			readParameter.param3 = 0;

			messageParameters.add(readParameter);

		} else if (mode == SRAM_WRITE_MODE) {

			DebugFunctionParameters writeParameter = new DebugFunctionParameters();

			writeParameter.address = sramWriteAddress;
			writeParameter.wreg = sramWriteData;
			writeParameter.param1 = 0;
			writeParameter.param2 = 0;
			writeParameter.param3 = 0;

			messageParameters.add(writeParameter);
		}

		noReturnValues = 0;

		sendMessage(returnHex, messageParameters);
	}

	private void sendMessage(boolean returnHex, Vector messageParameters) {

		cancelled = false;

		done = false;

		setChanged();
		notifyObservers(WORKER);
		clearChanged();

		this.returnHex = returnHex;

		this.messageParameters = messageParameters;

		Iterator it = messageParameters.iterator();

		while (it.hasNext()) {

			DebugFunctionParameters parameters = (DebugFunctionParameters) it
					.next();

			byte[] debugCommandMessage = new byte[20];
			debugCommandMessage[0] = (byte) 0xF0;
			debugCommandMessage[1] = (byte) 0x00;
			debugCommandMessage[2] = (byte) 0x00;
			debugCommandMessage[3] = (byte) 0x7E;
			debugCommandMessage[4] = (byte) 0x40;
			debugCommandMessage[5] = (byte) deviceID; // device-id
			debugCommandMessage[6] = (byte) 0x0D;
			debugCommandMessage[7] = (byte) mode; // command
			debugCommandMessage[8] = (byte) ((parameters.address >> 14) & 0x7f); // au
			debugCommandMessage[9] = (byte) ((parameters.address >> 7) & 0x7f); // ah
			debugCommandMessage[10] = (byte) ((parameters.address >> 0) & 0x7f); // al
			debugCommandMessage[11] = (byte) ((parameters.wreg >> 4) & 0x0f); // wh
			debugCommandMessage[12] = (byte) ((parameters.wreg) & 0x0f); // wl
			debugCommandMessage[13] = (byte) ((parameters.param1 >> 4) & 0x0f); // mp1h
			debugCommandMessage[14] = (byte) ((parameters.param1) & 0x0f); // mp1l
			debugCommandMessage[15] = (byte) ((parameters.param2 >> 4) & 0x0f); // mp2h
			debugCommandMessage[16] = (byte) ((parameters.param2) & 0x0f); // mp2l
			debugCommandMessage[17] = (byte) ((parameters.param3 >> 4) & 0x0f); // mp3h
			debugCommandMessage[18] = (byte) ((parameters.param3) & 0x0f); // mp3l
			debugCommandMessage[19] = (byte) 0xF7;

			dumpAddress = parameters.address;

			try {
				SysexMessage sysExMessage = new SysexMessage();
				sysExMessage.setMessage(debugCommandMessage,
						debugCommandMessage.length);

				receiver.send(sysExMessage, -1);
			} catch (InvalidMidiDataException ex) {
				System.out.println(ex.toString());
			}

			try {
				Thread.sleep(delayTime);
			} catch (InterruptedException e) {
				cancelled = true;
				addMessage("Upload task interrupted");
				return;
			}
		}
	}

	protected void receivedReturnValues(byte[] data) {
		String returnValues = "";

		if (mode == 1) {

			int wreg = (int) ((data[6] << 4) | data[7]);
			int param1 = (int) ((data[8] << 4) | data[9]);
			int param2 = (int) ((data[10] << 4) | data[11]);
			int param3 = (int) ((data[12] << 4) | data[13]);

			returnValues += "WREG: "
					+ ((returnHex) ? MidiUtils.intToHex(wreg) : MidiUtils
							.getDecimalString(wreg))
					+ "    MIOS_PARAM1: "
					+ ((returnHex) ? MidiUtils.intToHex(param1) : MidiUtils
							.getDecimalString(param1))
					+ "    MIOS_PARAM2: "
					+ ((returnHex) ? MidiUtils.intToHex(param2) : MidiUtils
							.getDecimalString(param2))
					+ "    MIOS_PARAM3: "
					+ ((returnHex) ? MidiUtils.intToHex(param3) : MidiUtils
							.getDecimalString(param3));

			addMessage(returnValues);
			noReturnValues++;

			if (noReturnValues >= messageParameters.size()) {
				done = true;
				addMessage("Done");
			}
		} else if (mode == 2) {
			dump = new byte[sramReadCounter * 2];
			int useableBytes = ((data[data.length - 1] == (byte) 0xF7) ? data.length - 1
					: data.length) - 6;
			System.arraycopy(data, 6, dump, noReturnValues, useableBytes);
			noReturnValues += useableBytes;

			if (noReturnValues == dump.length) {
				receivedDump();
			}

		} else {
			done = true;
			addMessage("Received write acknowledgement response");
		}
	}

	protected void receivedContinuedSysex(byte[] data) {
		if (mode == 2) {
			int useableBytes = ((data[data.length - 1] == (byte) 0xF7) ? data.length - 1
					: data.length);
			System.arraycopy(data, 0, dump, noReturnValues, useableBytes);
			noReturnValues += useableBytes;

			if (noReturnValues == dump.length) {
				receivedDump();
			}
		}
	}

	private void receivedDump() {

		StringBuffer buffer = new StringBuffer();

		String length = "" + (dumpAddress + dump.length);
		length = length.replaceAll(".", "0");

		for (int i = 0; i < dump.length; i += 2) {
			int val = (dump[i] << 4 | dump[(i + 1)] & 0xFF);

			if (colNo == 0) {
				if (i > 0) {
					buffer.append("\n");
				}
				buffer.append(length);
				String value = ((returnHex) ? Integer.toHexString(
						(dumpAddress + i / 2)).toUpperCase() : ""
						+ (dumpAddress + i / 2));
				buffer.replace(buffer.length() - value.length(), buffer
						.length(), value);
				buffer.append(": ");
			}

			buffer.append((returnHex) ? MidiUtils.getHexString(val) : MidiUtils
					.getDecimalString(val));

			if (colNo + 1 == 8) {
				buffer.append("  ");
			} else if (colNo + 1 < 16) {
				buffer.append(" ");
			}

			colNo = ++colNo % 16;
		}
		done = true;
		addMessage(buffer.toString() + "\nDone");
	}
}
