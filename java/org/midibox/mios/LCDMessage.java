/*
 * @(#)LCDMessage.java	beta8	2006/04/23
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

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.Receiver;
import javax.sound.midi.SysexMessage;

public class LCDMessage extends MIOSSysexSendReceive {

	public final static Object LCD_WIDTH = new Object();

	public final static Object LCD_HEIGHT = new Object();

	public final static Object MESSAGE_TEXT = new Object();

	public final static Object MODE = new Object();

	public final static Object POS_X = new Object();

	public final static Object POS_Y = new Object();

	public final static Object LOOP = new Object();

	public final static int CLEAR_SCREEN_MODE = 0;

	public final static int COORDS_MODE = 1;

	public final static int CURRENT_POS_MODE = 2;

	public final static int STOP_MODE = 3;

	private int lcdWidth;

	private int lcdHeight;

	private String messageText;

	private int mode;

	private int posX;

	private int posY;

	private boolean loop;

	public LCDMessage(Receiver receiver) {

		super(receiver);

		this.lcdWidth = 16;

		this.lcdHeight = 2;

		this.messageText = "";

		this.mode = CLEAR_SCREEN_MODE;

		this.posX = 0;

		this.posY = 0;

		this.loop = false;
	}

	public int getLcdHeight() {
		return lcdHeight;
	}

	public void setLcdHeight(int lcdHeight) {
		this.lcdHeight = lcdHeight;
		
		setChanged();
		notifyObservers(LCD_HEIGHT);
		clearChanged();
	}

	public int getLcdWidth() {
		return lcdWidth;
	}

	public void setLcdWidth(int lcdWidth) {
		this.lcdWidth = lcdWidth;

		setChanged();
		notifyObservers(LCD_WIDTH);
		clearChanged();
	}

	public String getMessageText() {
		return messageText;
	}

	public void setMessageText(String messageText) {
		this.messageText = messageText;

		setChanged();
		notifyObservers(MESSAGE_TEXT);
		clearChanged();
	}

	public boolean isLoop() {
		return loop;
	}

	public void setLoop(boolean loop) {
		this.loop = loop;

		setChanged();
		notifyObservers(LOOP);
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

	public int getPosX() {
		return posX;
	}

	public void setPosX(int posX) {
		this.posX = posX;

		setChanged();
		notifyObservers(POS_X);
		clearChanged();
	}

	public int getPosY() {
		return posY;
	}

	public void setPosY(int posY) {
		this.posY = posY;

		setChanged();
		notifyObservers(POS_Y);
		clearChanged();
	}

	public void createMessage() {
		sendMessage(messageText, mode);
	}

	public void cancelMessage() {
		sendMessage("a", STOP_MODE);
		cancelled = true;
	}

	private void sendMessage(String message, int mode) {

		cancelled = false;

		done = false;

		setChanged();
		notifyObservers(WORKER);
		clearChanged();

		int cursor = mode;

		String[] messageLines = message.split("\n");
		byte[][] messageLinesBytes = new byte[messageLines.length][];

		for (int l = 0; l < messageLines.length; l++) {
			messageLinesBytes[l] = messageLines[l].getBytes();

			if (cursor == COORDS_MODE || l > 0) {
				messageLinesBytes[l] = addPos(messageLinesBytes[l],
						(cursor == COORDS_MODE) ? posX : 0,
						((cursor == COORDS_MODE) ? posY : 0) + l);
			}
		}

		byte[][] lcdMessageCommand = new byte[messageLines.length][];

		for (int l = 0; l < messageLines.length; l++) {

			lcdMessageCommand[l] = new byte[8 + messageLinesBytes[l].length + 1];

			lcdMessageCommand[l][0] = (byte) 0xF0;
			lcdMessageCommand[l][1] = (byte) 0x00;
			lcdMessageCommand[l][2] = (byte) 0x00;
			lcdMessageCommand[l][3] = (byte) 0x7E;
			lcdMessageCommand[l][4] = (byte) 0x40;
			lcdMessageCommand[l][5] = (byte) (deviceID & 0x7F); // device-id
			lcdMessageCommand[l][6] = (byte) 0x08; // lcd message command

			lcdMessageCommand[l][7] = (byte) ((l > 0) ? COORDS_MODE : cursor); // lcd
			// command

			System.arraycopy(messageLinesBytes[l], 0, lcdMessageCommand[l], 8,
					messageLinesBytes[l].length);

			lcdMessageCommand[l][lcdMessageCommand[l].length - 1] = (byte) 0xF7;
		}

		do {
			for (int l = 0; l < messageLines.length; l++) {
				try {

					SysexMessage sysExMessage = new SysexMessage();
					sysExMessage.setMessage(lcdMessageCommand[l],
							lcdMessageCommand[l].length);
					receiver.send(sysExMessage, -1);
				} catch (InvalidMidiDataException ex) {
					return;
				}
			}
			if (loop && cursor != STOP_MODE) {
				try {
					Thread.sleep(1500);
				} catch (Exception e) {

				}
			}
		} while (loop && cursor != STOP_MODE && !cancelled);

		done = true;
		addMessage("Done");
	}

	private byte[] addPos(byte[] message, int x, int y) {

		byte[] newMessage = new byte[message.length + 2];
		newMessage[0] = (byte) x;
		newMessage[1] = (byte) y;
		System.arraycopy(message, 0, newMessage, 2, message.length);
		message = newMessage;
		newMessage = null;
		return message;
	}
}
