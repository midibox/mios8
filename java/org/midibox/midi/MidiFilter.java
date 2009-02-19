/*
 * @(#)MidiFilter.java	beta8	2006/04/23
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

package org.midibox.midi;

import java.util.Observable;

import javax.sound.midi.MetaMessage;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.SysexMessage;

public class MidiFilter extends Observable implements Receiver {

	public Object VOICE_MESSAGES = new Object();

	public Object VOICE_MESSAGE = new Object();

	public Object CONTROL_CHANGE_MESSAGE = new Object();

	public Object CHANNEL = new Object();

	public Object SYSTEM_COMMON_MESSAGES = new Object();

	public Object SYSTEM_COMMON_MESSAGE = new Object();

	public Object SYSTEM_REALTIME_MESSAGES = new Object();

	public Object SYSTEM_REALTIME_MESSAGE = new Object();

	public Object SYSEX_MESSAGES = new Object();

	public Object META_MESSAGES = new Object();

	protected Receiver receiver;

	// voice messages
	private boolean voiceMessages = true;

	private boolean[] voiceMessage = new boolean[8];

	// control change
	private boolean[] controlChangeMessage = new boolean[128];

	// channels
	private boolean[] channel = new boolean[16];

	// system common
	private boolean systemCommonMessages = true;

	private boolean[] systemCommonMessage = new boolean[8];

	// system realtime
	private boolean systemRealtimeMessages = true;

	private boolean[] systemRealtimeMessage = new boolean[8];

	// sysex messages
	private boolean sysexMessages = true;

	// meta messages
	private boolean metaMessages = true;

	public MidiFilter(Receiver receiver) {

		this.receiver = receiver;

		for (int i = 0; i < voiceMessage.length; i++) {
			voiceMessage[i] = true;
		}

		for (int i = 0; i < controlChangeMessage.length; i++) {
			controlChangeMessage[i] = true;
		}

		for (int i = 0; i < channel.length; i++) {
			channel[i] = true;
		}

		for (int i = 0; i < systemCommonMessage.length; i++) {
			systemCommonMessage[i] = true;
		}

		for (int i = 0; i < systemRealtimeMessage.length; i++) {
			systemRealtimeMessage[i] = true;
		}
	}

	public void setVoiceMessage(int statusNumber, boolean enabled) {

		setStatus(statusNumber, enabled);

		setChanged();

		notifyObservers(VOICE_MESSAGE);

		clearChanged();
	}

	public void setSystemCommonMessage(int statusNumber, boolean enabled) {

		setStatus(statusNumber, enabled);

		setChanged();

		notifyObservers(SYSTEM_COMMON_MESSAGE);

		clearChanged();
	}

	public void setSystemRealtimeMessage(int statusNumber, boolean enabled) {

		setStatus(statusNumber, enabled);

		setChanged();

		notifyObservers(SYSTEM_REALTIME_MESSAGE);

		clearChanged();
	}

	protected void setStatus(int statusNumber, boolean enabled) {

		if (statusNumber >= 0x80 && statusNumber <= 0xEF) {
			voiceMessage[(statusNumber >> 4) & 0x7] = enabled;
		}

		if (statusNumber >= 0xF0 && statusNumber <= 0xF7) {
			systemCommonMessage[(statusNumber & 0xF)] = enabled;

			if (statusNumber == ShortMessage.MIDI_TIME_CODE) {
				setStatus(0xF0, enabled);
			}
		}

		if (statusNumber >= 0xF8 && statusNumber <= 0xFF) {
			systemRealtimeMessage[(statusNumber & 0xF) - 8] = enabled;
		}
	}

	public void setChannel(int channelNumber, boolean enabled) {

		channel[channelNumber - 1] = enabled;

		setChanged();

		notifyObservers(CHANNEL);

		clearChanged();
	}

	public void setControlChange(int controlChangeNumber, boolean enabled) {

		controlChangeMessage[controlChangeNumber] = enabled;

		setChanged();

		notifyObservers(CONTROL_CHANGE_MESSAGE);

		clearChanged();
	}

	public void setVoiceMessages(boolean voiceMessages) {

		this.voiceMessages = voiceMessages;

		setChanged();

		notifyObservers(VOICE_MESSAGES);

		clearChanged();
	}

	public void setSystemCommonMessages(boolean systemCommonMessages) {

		this.systemCommonMessages = systemCommonMessages;

		setChanged();

		notifyObservers(SYSTEM_COMMON_MESSAGES);

		clearChanged();
	}

	public void setSystemRealtimeMessages(boolean systemRealtimeMessages) {

		this.systemRealtimeMessages = systemRealtimeMessages;

		setChanged();

		notifyObservers(SYSTEM_REALTIME_MESSAGES);

		clearChanged();
	}

	public void setSysexMessages(boolean sysexMessages) {
		this.sysexMessages = sysexMessages;

		setChanged();

		notifyObservers(SYSEX_MESSAGES);

		clearChanged();
	}

	public void setMetaMessages(boolean metaMessages) {
		this.metaMessages = metaMessages;

		setChanged();

		notifyObservers(META_MESSAGES);

		clearChanged();
	}

	public boolean getVoiceMessage(int statusNumber) {

		return getStatus(statusNumber);
	}

	public boolean getSystemCommonMessage(int statusNumber) {

		return getStatus(statusNumber);
	}

	public boolean getSystemRealtimeMessage(int statusNumber) {

		return getStatus(statusNumber);
	}

	protected boolean getStatus(int statusNumber) {

		if (statusNumber >= 0x80 && statusNumber <= 0xEF) {
			return voiceMessage[(statusNumber >> 4) & 0x7];
		}

		if (statusNumber >= 0xF0 && statusNumber <= 0xF7) {
			return systemCommonMessage[(statusNumber & 0xF)];
		}

		if (statusNumber >= 0xF8 && statusNumber <= 0xFF) {
			return systemRealtimeMessage[(statusNumber & 0xF) - 8];
		}

		return true;
	}

	public boolean getChannel(int channelNumber) {

		return channel[channelNumber - 1];
	}

	public boolean getControlChange(int controlChangeNumber) {

		return controlChangeMessage[controlChangeNumber];
	}

	public boolean[] getVoiceMessages() {
		return voiceMessage.clone();
	}

	public boolean[] getControlChangeMessages() {
		return controlChangeMessage.clone();
	}

	public boolean[] getChannels() {
		return channel.clone();
	}

	public boolean[] getSystemRealtimeMessages() {
		return systemRealtimeMessage.clone();
	}

	public boolean[] getSystemCommonMessages() {
		return systemCommonMessage.clone();
	}

	public boolean isVoiceMessages() {
		return voiceMessages;
	}

	public boolean isSystemCommonMessages() {
		return systemCommonMessages;
	}

	public boolean isSystemRealtimeMessages() {
		return systemRealtimeMessages;
	}

	public boolean isSysexMessages() {
		return sysexMessages;
	}

	public boolean isMetaMessages() {
		return metaMessages;
	}

	public void close() {

	}

	public void send(MidiMessage message, long timestamp) {

		// sysex messages
		if (message instanceof SysexMessage) {
			if (!sysexMessages) {
				return;
			}
		}

		// meta messages
		if (message instanceof MetaMessage) {
			if (!metaMessages) {
				return;
			}
		}

		if (message instanceof ShortMessage) {

			int status = ((ShortMessage) message).getCommand();

			if (!getStatus(status)) {
				return;
			}

			// control change
			if (status == ShortMessage.CONTROL_CHANGE
					&& !controlChangeMessage[((ShortMessage) message)
							.getData1()]) {
				return;
			}

			// channels
			if (!channel[((ShortMessage) message).getChannel()]) {
				return;
			}

		}

		if (receiver != null) {
			receiver.send(message, timestamp);
		}
	}
}
