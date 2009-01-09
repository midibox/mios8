/*
 * @(#)MidiParameter.java	beta8	2006/04/23
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

import javax.sound.midi.MidiMessage;
import javax.sound.midi.Receiver;
import javax.sound.midi.ShortMessage;

public class MidiParameter extends Observable implements Receiver {

	public final static int NOTE_OFF = ShortMessage.NOTE_OFF;

	public final static int NOTE_ON = ShortMessage.NOTE_ON;

	public final static int AFTERTOUCH = ShortMessage.POLY_PRESSURE;

	public final static int CONTROL_CHANGE = ShortMessage.CONTROL_CHANGE;

	public final static int PROGRAM_CHANGE = ShortMessage.PROGRAM_CHANGE;

	public final static int CHANNEL_PRESSURE = ShortMessage.CHANNEL_PRESSURE;

	public final static int PITCH_BEND = ShortMessage.PITCH_BEND;

	public final static Object STATUS = new Object();

	public final static Object CHANNEL = new Object();

	public final static Object NUMBER = new Object();

	public final static Object VALUE = new Object();

	public final static Object DEFAULT_VALUE = new Object();

	public final static Object LEARN = new Object();

	public final static Object HIGH_RESOLUTION = new Object();

	protected Receiver receiver;

	protected int channel;

	protected int status;

	protected int value;

	protected int number;

	protected int defaultValue;

	protected int type;

	protected boolean learn;

	protected boolean highResolution;

	public MidiParameter(Receiver receiver, int status, int channel,
			int number, int value, int defaultValue) {
		setReceiver(receiver);
		setMidiStatus(status);
		setMidiChannel(channel);
		setMidiNumber(number);
		setMidiValue(value, false);
		setHighResolution(false);
		setMidiDefaultValue(defaultValue);
	}

	public Receiver getReceiver() {
		return receiver;
	}

	protected void setReceiver(Receiver receiver) {
		this.receiver = receiver;
	}

	public int getMidiStatus() {
		return status;
	}

	public void setMidiStatus(int status) {

		this.status = status;

		setChanged();
		notifyObservers(STATUS);
		clearChanged();
	}

	public int getMidiChannel() {
		return channel;
	}

	public void setMidiChannel(int channel) {

		this.channel = channel;

		setChanged();
		notifyObservers(CHANNEL);
		clearChanged();
	}

	public int getMidiValue() {
		return value & getMidiMaxValue();
	}

	public void setMidiValue(int value, boolean forward) {

		this.value = value & getMidiMaxValue();

		if (forward) {
			createMessage();
		}

		setChanged();
		notifyObservers(VALUE);
		clearChanged();
	}

	public int getMidiMaxValue() {

		int currentStatus = getMidiStatus();

		if (currentStatus == PITCH_BEND
				|| (currentStatus == CONTROL_CHANGE && highResolution)) {
			return 16383;
		} else {
			return 127;
		}
	}

	public String getMidiName() {

		if (status == NOTE_ON || status == NOTE_OFF) {
			return "Note " + MidiUtils.getKeyName(number);
		} else if (status == AFTERTOUCH) {
			return "Aftertouch " + MidiUtils.getKeyName(number);
		} else if (status == CONTROL_CHANGE) {
			if (highResolution) {
				return MidiUtils.cc14BitNames[number];
			} else {
				return MidiUtils.ccNames[number];
			}
		} else if (status == PROGRAM_CHANGE) {
			return "Program Change";
		} else if (status == CHANNEL_PRESSURE) {
			return "Channel Pressure";
		} else if (status == PITCH_BEND) {
			return "Pitch Bend";
		} else {
			return "undefined";
		}
	}

	public int getMidiNumber() {
		return number;
	}

	public void setMidiNumber(int number) {
		this.number = number;

		int currentStatus = getMidiStatus();

		setChanged();
		notifyObservers(NUMBER);
		clearChanged();
	}

	public boolean isLearn() {
		return learn;
	}

	public void setLearn(boolean learn) {
		this.learn = learn;

		setChanged();
		notifyObservers(LEARN);
		clearChanged();
	}

	public void createMessage() {
		ShortMessage message = new ShortMessage();
		if (receiver != null) {
			try {

				int data1 = 0;
				int data2 = 0;

				if (status == NOTE_ON || status == NOTE_OFF) {
					data1 = number & 0x7F;
					data2 = value & 0x7F;
				} else if (status == AFTERTOUCH) {
					data1 = number & 0x7F;
					data2 = value & 0x7F;
				} else if (status == CONTROL_CHANGE) {

					if (highResolution
							&& ((number < 64) || (number < 102 && number > 97))) {

						int msbNumber;
						int lsbNumber;

						if (number < 64) {
							if (number < 32) {
								msbNumber = number;
								lsbNumber = number + 32;
							} else {
								msbNumber = number - 32;
								lsbNumber = number;
							}
						} else {
							if (number % 2 == 0) {
								msbNumber = number + 1;
								lsbNumber = number;
							} else {
								msbNumber = number;
								lsbNumber = number - 1;
							}
						}

						int lsbValue = (value) & 0x7F;
						int msbValue = (value >> 7) & 0x7F;

						message
								.setMessage(status, channel, msbNumber,
										msbValue);
						receiver.send(message, -1);

						message = new ShortMessage();

						message
								.setMessage(status, channel, lsbNumber,
										lsbValue);
						receiver.send(message, -1);

						return;
					} else {
						data1 = number & 0x7F;
						data2 = value & 0x7F;
					}
				} else if (status == PROGRAM_CHANGE) {
					data1 = value & 0x7F;
					data2 = 0;
				} else if (status == CHANNEL_PRESSURE) {
					data1 = value & 0x7F;
					data2 = 0;
				} else if (status == PITCH_BEND) {
					data1 = (value) & 0x7F;
					data2 = (value >> 7) & 0x7F;
				}

				message.setMessage(status, channel, data1, data2);
				receiver.send(message, -1);
			} catch (Exception ex) {
				ex.printStackTrace();
			}
		}
	}

	protected void midiLearn(MidiMessage message) {
		if (message instanceof ShortMessage) {

			setMidiChannel(((ShortMessage) message).getChannel());

			int incomingStatus = ((ShortMessage) message).getCommand();

			if (incomingStatus == NOTE_OFF || incomingStatus == NOTE_ON) {

				setMidiStatus(NOTE_ON);
				setMidiNumber(((ShortMessage) message).getData1());
				setMidiDefaultValue(((ShortMessage) message).getData2());

			} else if (incomingStatus == AFTERTOUCH) {

				setMidiStatus(AFTERTOUCH);
				setMidiNumber(((ShortMessage) message).getData1());
				setMidiDefaultValue(((ShortMessage) message).getData2());

			} else if (incomingStatus == CONTROL_CHANGE) {
				setMidiStatus(CONTROL_CHANGE);
				setMidiNumber(((ShortMessage) message).getData1());
				if (highResolution
						&& ((number < 64) || (number < 102 && number > 97))) {
					if (number < 64) {
						if (number < 32) {
							setMidiValue(
									((ShortMessage) message).getData2() << 7,
									false);
						} else {
							setMidiValue(((ShortMessage) message).getData2(),
									false);
						}
					} else {
						if (number % 2 == 0) {
							setMidiValue(((ShortMessage) message).getData2(),
									false);
						} else {
							setMidiValue(
									((ShortMessage) message).getData2() << 7,
									false);
						}
					}
				} else {
					setMidiDefaultValue(((ShortMessage) message).getData2());
				}
			} else if (incomingStatus == PROGRAM_CHANGE) {

				setMidiStatus(PROGRAM_CHANGE);
				setMidiDefaultValue(((ShortMessage) message).getData1());

			} else if (incomingStatus == CHANNEL_PRESSURE) {

				setMidiStatus(CHANNEL_PRESSURE);
				setMidiDefaultValue(((ShortMessage) message).getData1());

			} else if (incomingStatus == PITCH_BEND) {

				setMidiStatus(PITCH_BEND);
				setMidiDefaultValue(((ShortMessage) message).getData1() & 0x7F
						| (((ShortMessage) message).getData2() << 7));

			}
		}
	}

	public void send(MidiMessage message, long lTimeStamp) {

		if (isLearn()) {
			midiLearn(message);
			setLearn(false);
		} else {
			if (message instanceof ShortMessage) {

				if (channel == ((ShortMessage) message).getChannel()) {

					int incomingStatus = ((ShortMessage) message).getCommand();

					if ((incomingStatus == NOTE_OFF || incomingStatus == NOTE_ON)
							&& (status == NOTE_OFF || status == NOTE_ON)) {
						if (number == ((ShortMessage) message).getData1()) {
							setMidiValue(incomingStatus == NOTE_OFF ? 0
									: ((ShortMessage) message).getData2(),
									false);
						}
					} else if ((incomingStatus == AFTERTOUCH)
							&& (status == AFTERTOUCH)) {
						if (number == ((ShortMessage) message).getData1()) {
							setMidiValue(incomingStatus == NOTE_OFF ? 0
									: ((ShortMessage) message).getData2(),
									false);
						}
					} else if (incomingStatus == CONTROL_CHANGE
							&& status == CONTROL_CHANGE) {

						int incomingNumber = ((ShortMessage) message)
								.getData1();
						int incomingValue = ((ShortMessage) message).getData2();

						if (highResolution
								&& ((number < 64) || (number < 102 && number > 97))) {

							if (number < 64) {
								if (number < 32) {
									if (incomingNumber == number) {
										int newValue = value & 0x007F;
										newValue = newValue
												| incomingValue << 7;
										setMidiValue(newValue, false);
									} else if (incomingNumber == number + 32) {
										int newValue = value & 0x3F80;
										newValue = newValue | incomingValue;
										setMidiValue(newValue, false);
									}
								} else {
									if (incomingNumber == number) {
										int newValue = value & 0x3F80;
										newValue = newValue | incomingValue;
										setMidiValue(newValue, false);
									} else if (incomingNumber == number - 32) {
										int newValue = value & 0x007F;
										newValue = newValue
												| incomingValue << 7;
										setMidiValue(newValue, false);
									}
								}
							} else {
								if (number % 2 == 0) {
									if (incomingNumber == number + 1) {
										int newValue = value & 0x007F;
										newValue = newValue
												| incomingValue << 7;
										setMidiValue(newValue, false);
									} else if (incomingNumber == number) {
										int newValue = value & 0x3F80;
										newValue = newValue | incomingValue;
										setMidiValue(newValue, false);
									}
								} else {
									if (incomingNumber == number - 1) {
										int newValue = value & 0x3F80;
										newValue = newValue | incomingValue;
										setMidiValue(newValue, false);
									} else if (incomingNumber == number) {
										int newValue = value & 0x007F;
										newValue = newValue
												| incomingValue << 7;
										setMidiValue(newValue, false);
									}
								}
							}
						} else {
							if (number == incomingNumber) {
								setMidiValue(incomingValue, false);
							}
						}
					} else if (incomingStatus == PROGRAM_CHANGE
							&& status == PROGRAM_CHANGE) {
						setMidiValue(((ShortMessage) message).getData1(), false);
					} else if (incomingStatus == CHANNEL_PRESSURE
							&& status == CHANNEL_PRESSURE) {
						setMidiValue(((ShortMessage) message).getData1(), false);
					} else if (incomingStatus == PITCH_BEND
							&& status == PITCH_BEND) {
						setMidiValue(((ShortMessage) message).getData1() & 0x7F
								| (((ShortMessage) message).getData2() << 7),
								false);
					}
				}
			}
		}
	}

	public int getMidiDefaultValue() {
		return defaultValue & getMidiMaxValue();
	}

	public void setMidiDefaultValue(int value) {

		this.defaultValue = value & getMidiMaxValue();

		setChanged();
		notifyObservers(DEFAULT_VALUE);
		clearChanged();
	}

	public void close() {

	}

	public boolean isHighResolution() {
		return highResolution;
	}

	public void setHighResolution(boolean highResolution) {
		this.highResolution = highResolution;

		setChanged();
		notifyObservers(HIGH_RESOLUTION);
		clearChanged();
	}
}
