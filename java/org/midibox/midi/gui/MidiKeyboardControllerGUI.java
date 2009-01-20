/*
 * @(#)MidiKeyboardControllerGUI.java	beta8	2006/04/23
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
 *
 */

/*
 * Portions based on MidiSynth.java by Brian Lichtenwalter
 * Original copyright below:
 *
 * MidiSynth.java	1.15	99/12/03
 *
 * Copyright (c) 1999 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Sun grants you ("Licensee") a non-exclusive, royalty free, license to use,
 * modify and redistribute this software in source and binary code form,
 * provided that i) this copyright notice and license appear on all copies of
 * the software; and ii) Licensee does not utilize the software in a manner
 * which is disparaging to Sun.
 *
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. SUN AND ITS LICENSORS SHALL NOT BE
 * LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THE SOFTWARE OR ITS DERIVATIVES. IN NO EVENT WILL SUN OR ITS
 * LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT,
 * INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER
 * CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF
 * OR INABILITY TO USE SOFTWARE, EVEN IF SUN HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the Software for such purposes.
 */

package org.midibox.midi.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JSpinner;
import javax.swing.JToggleButton;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.MouseInputAdapter;

import org.midibox.midi.MidiKeyboardController;
import org.midibox.midi.MidiParameterControl;
import org.midibox.utils.gui.FontLoader;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.Knob;
import org.midibox.utils.gui.MyButtonUI;
import org.midibox.utils.gui.SliderWheelUI;

/**
 * Description
 * 
 * @version 1.0 05 April 2005
 * @author Adam King
 */

public class MidiKeyboardControllerGUI extends JPanel implements Observer,
		ChangeListener, ActionListener {

	private MidiKeyboardController midiKeyboardController;

	protected static Color menuForegroundColor = Color.BLACK;

	protected static Color menuBorderColor = new Color(91, 91, 91);

	protected static Color menuBackgroundColor = new Color(180, 180, 180);

	private final Color[] channelColours = { new Color(245, 145, 255),
			new Color(0, 255, 255), new Color(255, 255, 0),
			new Color(141, 245, 197), new Color(206, 186, 244),
			new Color(192, 192, 192), new Color(0, 178, 176),
			new Color(255, 0, 0), new Color(253, 124, 53),
			new Color(128, 0, 128), new Color(128, 128, 0),
			new Color(10, 140, 80), new Color(0, 117, 225),
			new Color(128, 64, 8), new Color(224, 0, 238), new Color(0, 0, 255) };

	private Piano piano;

	private JSpinner channelNo;

	private JSpinner velocity;

	private JSpinner bankSelect;

	private JSpinner programNo;

	private JSlider modWheel;

	private JSlider pitchWheel;

	private JPanel mainPanel;

	public MidiKeyboardControllerGUI(
			MidiKeyboardController midiKeyboardController) {

		this.midiKeyboardController = midiKeyboardController;
		midiKeyboardController.addObserver(this);

		setLayout(new BorderLayout());

		mainPanel = new JPanel();
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));

		JPanel topPanel = new JPanel(new BorderLayout());

		JPanel spinnerPanel = new JPanel(new GridBagLayout());
		spinnerPanel.setOpaque(false);

		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridheight = 1;
		gbc.gridwidth = 1;
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.insets = new Insets(0, 2, 4, 2);
		gbc.fill = GridBagConstraints.HORIZONTAL;

		JLabel channelNoLabel = new JLabel("CHANNEL", SwingConstants.CENTER);
		channelNoLabel.setFont(FontLoader.getFont("uni05_53.ttf", 8));
		spinnerPanel.add(channelNoLabel, gbc);
		gbc.gridy++;

		channelNo = new JSpinner(new SpinnerNumberModel(midiKeyboardController
				.getGlobalChannel() + 1, 1, 16, 1));
		channelNo.addChangeListener(this);
		channelNo.setFont(FontLoader.getFont("uni05_53.ttf", 8));
		((JSpinner.DefaultEditor) channelNo.getEditor()).getTextField()
				.setBackground(
						channelColours[midiKeyboardController
								.getGlobalChannel()]);
		spinnerPanel.add(channelNo, gbc);
		gbc.gridx++;
		gbc.gridy = 0;

		JLabel velocityLabel = new JLabel("VELOCITY", SwingConstants.CENTER);
		velocityLabel.setFont(FontLoader.getFont("uni05_53.ttf", 8));
		spinnerPanel.add(velocityLabel, gbc);
		gbc.gridy++;

		velocity = new JSpinner(new SpinnerNumberModel(midiKeyboardController
				.getVelocityVal(), 0, 127, 1));
		velocity.addChangeListener(this);
		velocity.setFont(FontLoader.getFont("uni05_53.ttf", 8));
		spinnerPanel.add(velocity, gbc);
		gbc.gridx = 0;
		gbc.gridy++;

		gbc.insets = new Insets(4, 2, 2, 2);

		bankSelect = new JSpinner(new SpinnerNumberModel(0, 0,
				midiKeyboardController.getBankSelect().getMidiMaxValue(), 1));
		spinnerPanel.add(new MidiParameterControlSpinner(midiKeyboardController
				.getBankSelect(), bankSelect, "BANK"), gbc);
		gbc.gridx++;

		programNo = new JSpinner(new SpinnerNumberModel(0, 0, 127, 1));
		spinnerPanel.add(new MidiParameterControlSpinner(midiKeyboardController
				.getProgramChange(), programNo, "PROGRAM"), gbc);

		topPanel.add(spinnerPanel, BorderLayout.WEST);

		JPanel controllersPanel = new JPanel(new BorderLayout());
		controllersPanel.setOpaque(false);

		JPanel knobPanel = new JPanel(new GridLayout(1, midiKeyboardController
				.getControllers().size()));
		knobPanel.setOpaque(false);

		for (int c = 0; c < midiKeyboardController.getControllers().size(); c++) {
			Knob knob = new Knob(ImageLoader
					.getImageIcon("plasticknob_strip.png"));
			MidiParameterControl midiParameter = (MidiParameterControl) midiKeyboardController
					.getControllers().elementAt(c);
			MidiParameterControlGUI midiParameterGUI = new MidiParameterControlKnob(
					midiParameter, knob, true, BorderLayout.SOUTH, false, false);
			knobPanel.add(midiParameterGUI);
		}

		controllersPanel.add(knobPanel, BorderLayout.NORTH);

		JPanel buttonPanel = new JPanel(new GridLayout(1,
				midiKeyboardController.getControllers().size()));
		buttonPanel.setOpaque(false);

		for (int b = 0; b < midiKeyboardController.getButtons().size(); b++) {
			JToggleButton button = new JToggleButton();
			button.setUI(new MyButtonUI(ImageLoader
					.getImageIcon("metal_button_small_on.png"), ImageLoader
					.getImageIcon("metal_button_small_off.png")));
			MidiParameterControl midiParameter = (MidiParameterControl) midiKeyboardController
					.getButtons().elementAt(b);
			MidiParameterControlGUI midiParameterGUI = new MidiParameterControlToggleButton(
					midiParameter, button, false, true, BorderLayout.SOUTH,
					false, false);
			buttonPanel.add(midiParameterGUI);
		}

		controllersPanel.add(buttonPanel, BorderLayout.SOUTH);

		topPanel.add(controllersPanel, BorderLayout.CENTER);

		topPanel.setOpaque(false);
		topPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
		mainPanel.add(topPanel);

		JPanel bottomPanel = new JPanel(new BorderLayout());
		JPanel expressionPanel = createExpressionPanel();

		piano = new Piano();
		bottomPanel.add(piano, BorderLayout.CENTER);
		bottomPanel.add(expressionPanel, BorderLayout.WEST);
		bottomPanel.setOpaque(false);

		Dimension d1 = piano.getPreferredSize();
		Dimension d2 = expressionPanel.getPreferredSize();
		bottomPanel.setPreferredSize(new Dimension(d1.width + d2.width,
				d1.height));

		mainPanel.add(bottomPanel);
		topPanel.setPreferredSize(new Dimension(
				bottomPanel.getPreferredSize().width, topPanel
						.getPreferredSize().height));

		mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 20));
		mainPanel.setOpaque(false);
		add(mainPanel, BorderLayout.SOUTH);
		// add(menuPanel, BorderLayout.NORTH);
	}

	public MidiKeyboardController getMidiKeyboardController() {
		return midiKeyboardController;
	}

	public void setMidiKeyboardController(
			MidiKeyboardController midiKeyboardController) {
		this.midiKeyboardController = midiKeyboardController;
	}

	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		Graphics2D g2d = (Graphics2D) g;
		GradientPaint gradient = new GradientPaint(0, 0, new Color(245, 245,
				245), getWidth(), getHeight(), new Color(205, 205, 205), true);
		g2d.setPaint(gradient);
		g2d.fillRect(0, 0, getWidth(), getHeight());
	}

	public JPanel createExpressionPanel() {
		JPanel expressionPanel = new JPanel(new GridLayout(1, 2));
		expressionPanel.setOpaque(false);

		pitchWheel = new JSlider(JSlider.VERTICAL, 0, 100, 50);
		pitchWheel.setPreferredSize(new Dimension(30, 71));
		pitchWheel.setUI(new SliderWheelUI(ImageLoader
				.getImageIcon("expressionwheel_horizontal_strip.png"),
				ImageLoader.getImageIcon("expressionwheel_vertical_strip.png"),
				15));
		pitchWheel.setOpaque(false);

		pitchWheel.addMouseListener(new MouseAdapter() {
			public void mouseReleased(MouseEvent me) {
				pitchWheel.setValue(pitchWheel.getMaximum() / 2);
			}

			public void mouseExited(MouseEvent me) {
				pitchWheel.setValue(pitchWheel.getMaximum() / 2);
			}
		});

		pitchWheel.addKeyListener(new KeyAdapter() {
			public void keyReleased(KeyEvent ke) {
				pitchWheel.setValue(pitchWheel.getMaximum() / 2);
			}
		});

		expressionPanel.add(new MidiParameterControlSlider(
				midiKeyboardController.getPitchWheel(), pitchWheel, false,
				BorderLayout.SOUTH, false, false));

		modWheel = new JSlider(JSlider.VERTICAL, 0, 100, 50);
		modWheel.setPreferredSize(new Dimension(30, 71));
		modWheel.setUI(new SliderWheelUI(ImageLoader
				.getImageIcon("expressionwheel_horizontal_strip.png"),
				ImageLoader.getImageIcon("expressionwheel_vertical_strip.png"),
				15));
		modWheel.setOpaque(false);

		expressionPanel.add(new MidiParameterControlSlider(
				midiKeyboardController.getModWheel(), modWheel, false,
				BorderLayout.SOUTH, false, false));

		expressionPanel.setOpaque(false);

		return expressionPanel;
	}

	public void stateChanged(ChangeEvent ce) {
		Object source = ce.getSource();

		if (source == channelNo) {
			midiKeyboardController.setGlobalChannel(((Integer) channelNo
					.getModel().getValue()).intValue() - 1);
		} else if (source == velocity) {
			midiKeyboardController.setVelocityVal(((Integer) velocity
					.getModel().getValue()).intValue());
		}
	}

	public void update(Observable observable, Object object) {
		((JSpinner.DefaultEditor) channelNo.getEditor()).getTextField()
				.setBackground(
						channelColours[midiKeyboardController
								.getGlobalChannel()]);
		if (midiKeyboardController.getGlobalChannel() < 7) {
			((JSpinner.DefaultEditor) channelNo.getEditor()).getTextField()
					.setForeground(Color.BLACK);
		} else {
			((JSpinner.DefaultEditor) channelNo.getEditor()).getTextField()
					.setForeground(Color.WHITE);
		}
		piano.repaint();
	}

	public void actionPerformed(ActionEvent ae) {

	}

	public class Piano extends JPanel implements MouseListener {

		private Color highlightColour = new Color(0, 255, 0, 180);

		private ImageIcon wKeyD = ImageLoader.getImageIcon("wKeyD.png");

		private ImageIcon wKeyU = ImageLoader.getImageIcon("wKeyU.png");

		private ImageIcon bKeyD = ImageLoader.getImageIcon("bKeyD.png");

		private ImageIcon bKeyU = ImageLoader.getImageIcon("bKeyU.png");

		private boolean release = true;

		private boolean clearChannel = false;

		Vector keys = new Vector();

		Vector whiteKeys = new Vector();

		Vector blackKeys = new Vector();

		Key prevKey;

		final int kw = 16, kh = 70;

		public Piano() {
			setLayout(new BorderLayout());
			setPreferredSize(new Dimension(midiKeyboardController
					.getNoOctaves()
					* 7 * kw, kh + 7));

			int whiteIDs[] = { 0, 2, 4, 5, 7, 9, 11 };

			for (int i = 0, x = 0; i < midiKeyboardController.getNoOctaves(); i++) {
				for (int j = 0; j < 7; j++, x += kw) {
					int keyNum = i * 12 + whiteIDs[j];
					whiteKeys
							.add(new Key(
									(MidiParameterControl) midiKeyboardController
											.getKeys().elementAt(keyNum), x, 0,
									kw, kh));
				}
			}
			for (int i = 0, x = 0; i < midiKeyboardController.getNoOctaves(); i++, x += kw) {
				int keyNum = i * 12;
				blackKeys.add(new Key(
						(MidiParameterControl) midiKeyboardController.getKeys()
								.elementAt(keyNum + 1), (x += kw) - 4, 0,
						kw / 2, 40));
				blackKeys.add(new Key(
						(MidiParameterControl) midiKeyboardController.getKeys()
								.elementAt(keyNum + 3), (x += kw) - 4, 0,
						kw / 2, 40));
				x += kw;
				blackKeys.add(new Key(
						(MidiParameterControl) midiKeyboardController.getKeys()
								.elementAt(keyNum + 6), (x += kw) - 4, 0,
						kw / 2, 40));
				blackKeys.add(new Key(
						(MidiParameterControl) midiKeyboardController.getKeys()
								.elementAt(keyNum + 8), (x += kw) - 4, 0,
						kw / 2, 40));
				blackKeys.add(new Key(
						(MidiParameterControl) midiKeyboardController.getKeys()
								.elementAt(keyNum + 10), (x += kw) - 4, 0,
						kw / 2, 40));
			}

			keys.addAll(blackKeys);
			keys.addAll(whiteKeys);

			addMouseMotionListener(new MouseInputAdapter() {

				public void mouseDragged(MouseEvent e) {
					if (e.isShiftDown()) {
						Key key = getKey(e.getPoint());
						if (key != null && prevKey != key) {
							key.assignChannel();
						}
						prevKey = key;
						repaint();
					} else {
						Key key = getKey(e.getPoint());
						if (prevKey != null && prevKey != key) {
							prevKey.setKeyOn(false);
						}
						if (key != null && prevKey != key) {
							key.setKeyOn(true);
						}
						prevKey = key;
						repaint();
					}
				}
			});
			addMouseListener(this);
		}

		public void mousePressed(MouseEvent e) {
			if (e.isShiftDown()) {
				prevKey = getKey(e.getPoint());
				if (prevKey != null) {
					if (e.getButton() == MouseEvent.BUTTON3) {
						clearChannel = true;
					} else {
						clearChannel = false;
					}
					prevKey.assignChannel();
					repaint();
				}
			} else {
				if (e.getButton() == MouseEvent.BUTTON3) {
					release = false;
				} else {
					release = true;
				}
				prevKey = getKey(e.getPoint());
				if (prevKey != null) {
					prevKey.setKeyOn(true);
				}
			}
		}

		public void mouseReleased(MouseEvent e) {
			if (e.isShiftDown()) {

			} else {
				if (prevKey != null) {
					prevKey.setKeyOn(false);
				}
				if (e.getButton() == MouseEvent.BUTTON3) {
					release = true;
				}
				prevKey = null;
			}
		}

		public void mouseExited(MouseEvent e) {
			if (e.isShiftDown()) {
				prevKey = null;
			} else {
				if (prevKey != null) {
					if (prevKey.isKeyOn()) {
						prevKey.setKeyOn(false);
					}
					repaint();
					prevKey = null;
				}
			}
		}

		public void mouseClicked(MouseEvent e) {
		}

		public void mouseEntered(MouseEvent e) {
		}

		public Key getKey(Point point) {
			for (int i = 0; i < keys.size(); i++) {
				if (((Key) keys.get(i)).contains(point)) {
					return (Key) keys.get(i);
				}
			}
			return null;
		}

		public void paint(Graphics g) {
			Graphics2D g2d = (Graphics2D) g;
			g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
					RenderingHints.VALUE_ANTIALIAS_ON);

			Dimension d = getSize();

			g2d.setBackground(getBackground());
			g2d.clearRect(0, 0, d.width, d.height);

			g2d.setColor(Color.white);
			g2d.fillRect(0, 0, midiKeyboardController.getNoOctaves() * 7 * kw,
					kh);

			for (int i = 0; i < whiteKeys.size(); i++) {
				Key key = (Key) whiteKeys.get(i);
				if (key.isKeyOn()) {
					wKeyD.paintIcon(this, g, key.x, 7);

					if (key.midiParameter.getMidiNumber() == 60) {
						g.setColor(Color.LIGHT_GRAY);
						g.fillOval((key.x + key.width / 2) - 3,
								(key.y + key.height) - 15, 6, 6);
					}

					g.setColor(highlightColour);
					g.fillRect(key.x, key.y + 8, key.width, key.height - 6);
				} else {
					wKeyU.paintIcon(this, g, key.x, 7);

					if (key.midiParameter.getMidiNumber() == 60) {
						g.setColor(Color.LIGHT_GRAY);
						g.fillOval((key.x + key.width / 2) - 3,
								(key.y + key.height) - 15, 6, 6);
					}
				}

				if (!key.midiParameter.isGlobal()) {
					g.setColor(channelColours[key.midiParameter
							.getMidiChannel()]);
				} else {
					g.setColor(Color.WHITE);
				}
				g.fillRect(key.x, key.y + 4, key.width, 3);

				g.setColor(channelColours[midiKeyboardController
						.getGlobalChannel()]);
				g.fillRect(key.x, key.y, key.width, 4);
			}

			for (int i = 0; i < blackKeys.size(); i++) {
				Key key = (Key) blackKeys.get(i);
				if (key.isKeyOn()) {
					bKeyD.paintIcon(this, g, key.x, 7);
					g.setColor(highlightColour);
					g2d.fillRect(key.x, key.y + 8, key.width, key.height - 7);
				} else {
					bKeyU.paintIcon(this, g, key.x, 7);
				}
				if (!key.midiParameter.isGlobal()) {
					g.setColor(channelColours[key.midiParameter
							.getMidiChannel()]);
				} else {
					g.setColor(Color.WHITE);
				}
				g.fillRect(key.x, key.y + 4, key.width, 3);
				g.setColor(channelColours[midiKeyboardController
						.getGlobalChannel()]);
				g.fillRect(key.x, key.y, key.width, 4);
			}

			g.setColor(Color.DARK_GRAY);
			g.drawLine(0, 0,
					midiKeyboardController.getNoOctaves() * 7 * kw - 1, 0);
			g.drawLine(0, 0, 0, 6);
			g.drawLine((midiKeyboardController.getNoOctaves() * 7 * kw) - 1, 0,
					(midiKeyboardController.getNoOctaves() * 7 * kw) - 1, 6);
		}

		public class Key extends Rectangle implements Observer {

			private MidiParameterControl midiParameter;

			public Key(MidiParameterControl midiParameter, int x, int y,
					int width, int height) {
				super(x, y, width, height + 6);
				this.midiParameter = midiParameter;
				midiParameter.addObserver(this);
			}

			public boolean isKeyOn() {
				return (midiParameter.isMidiValueOn());
			}

			public void setKeyOn(boolean sendVal) {
				if (sendVal) {
					if (!isKeyOn()) {
						midiParameter.setMidiValueOn(true, true);
					}
				} else {
					if (release && isKeyOn()) {
						midiParameter.setMidiValueOn(false, true);
					}
				}
			}

			public void assignChannel() {
				if (clearChannel) {
					midiParameter.setGlobal(true);
				} else {
					midiParameter.setGlobal(false);
				}
			}

			public void update(Observable observable, Object object) {
				piano.repaint(x, y, width, height + 1);
			}
		}
	}
}
