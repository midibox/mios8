/*
 * @(#)SIDV2librarian.java	beta1	2008/01/21
 *
 * Copyright (C) 2008    Rutger Vlek (rutgervlek@hotmail.com)
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

package org.midibox.sidedit.gui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JToggleButton;
import javax.swing.border.BevelBorder;

import org.midibox.sidedit.SIDEditController;
import org.midibox.sidedit.gui.bassline.BasslineGUI;
import org.midibox.sidedit.gui.drum.DrumGUI;
import org.midibox.sidedit.gui.lead.LeadGUI;
import org.midibox.sidedit.gui.multi.MultiGUI;
import org.midibox.sidlibr.Patch;
import org.midibox.utils.gui.ImageLoader;
import org.midibox.utils.gui.MyButtonUI;

public class MBSIDV2EditorGUI extends JPanel implements Observer,
		ActionListener {

	private SIDEditController sidEditController;

	private JPanel editPanel;

	private JLabel tooltipLabel;

	private JToggleButton stereoLink, oscillatorLink, core1Button, core2Button,
			core3Button, core4Button;

	public MBSIDV2EditorGUI() {

		setLayout(new BorderLayout());

		add(createStatusBar(), BorderLayout.SOUTH);
		editPanel = new JPanel();
		add(editPanel, BorderLayout.NORTH);
	}

	public void editThis(SIDEditController sidEditController, int cores) {

		this.sidEditController = sidEditController;
		sidEditController.setTooltipListener(this);
		Patch p = sidEditController.getPatch();
		stereoLink.setSelected(true);
		oscillatorLink.setSelected(false);
		setTooltip("");
		core1Button.setSelected(((cores & 0x01) > 0));
		core2Button.setSelected(((cores & 0x02) > 0));
		core3Button.setSelected(((cores & 0x04) > 0));
		core4Button.setSelected(((cores & 0x08) > 0));

		sidEditController.addObserver(this);
		remove(editPanel);

		if (p.getEngine() == p.LEAD) {
			editPanel = new LeadGUI(sidEditController);
		} else if (p.getEngine() == p.BASSLINE) {
			editPanel = new BasslineGUI(sidEditController);
		} else if (p.getEngine() == p.DRUM) {
			editPanel = new DrumGUI(sidEditController);
		} else if (p.getEngine() == p.MULTI) {
			editPanel = new MultiGUI(sidEditController);
		}

		add(editPanel);

		repaint();
		setVisible(true);
	}

	public SIDEditController getSIDEditController() {
		return sidEditController;
	}

	public void setSIDEditController(SIDEditController sidEditController) {
		this.sidEditController = sidEditController;
	}

	public void update(Observable observable, Object object) {

	}

	public JPanel createStatusBar() {
		JPanel statusBar = new JPanel();
		statusBar.setLayout(new BorderLayout());
		statusBar.setBorder(BorderFactory.createRaisedBevelBorder());

		stereoLink = new JToggleButton("Stereo link");
		stereoLink.setPreferredSize(new Dimension(
				stereoLink.getPreferredSize().width, 20));
		stereoLink.addActionListener(this);

		oscillatorLink = new JToggleButton("Oscillator link");
		oscillatorLink.setPreferredSize(new Dimension(oscillatorLink
				.getPreferredSize().width, 20));
		oscillatorLink.addActionListener(this);

		JPanel buttonPanel = new JPanel();
		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));
		buttonPanel.setBorder(BorderFactory
				.createBevelBorder(BevelBorder.LOWERED));
		JLabel coreLabel = new JLabel("Cores: ");
		core1Button = new JToggleButton();
		core1Button.setUI(new MyButtonUI(ImageLoader.getImageIcon("txOn.png"),
				ImageLoader.getImageIcon("txOff.png")));
		core1Button.setEnabled(false);
		core2Button = new JToggleButton();
		core2Button.setUI(new MyButtonUI(ImageLoader.getImageIcon("txOn.png"),
				ImageLoader.getImageIcon("txOff.png")));
		core2Button.setEnabled(false);
		core3Button = new JToggleButton();
		core3Button.setUI(new MyButtonUI(ImageLoader.getImageIcon("txOn.png"),
				ImageLoader.getImageIcon("txOff.png")));
		core3Button.setEnabled(false);
		core4Button = new JToggleButton();
		core4Button.setUI(new MyButtonUI(ImageLoader.getImageIcon("txOn.png"),
				ImageLoader.getImageIcon("txOff.png")));
		core4Button.setEnabled(false);
		buttonPanel.add(coreLabel);
		buttonPanel.add(core1Button);
		buttonPanel.add(core2Button);
		buttonPanel.add(core3Button);
		buttonPanel.add(core4Button);

		JLabel empty1Label = new JLabel();
		empty1Label.setBorder(BorderFactory
				.createBevelBorder(BevelBorder.LOWERED));

		tooltipLabel = new JLabel("");
		tooltipLabel.setBorder(BorderFactory
				.createBevelBorder(BevelBorder.LOWERED));

		JPanel linkPanel = new JPanel();
		linkPanel.setLayout(new BoxLayout(linkPanel, BoxLayout.X_AXIS));
		linkPanel.add(stereoLink);
		linkPanel.add(oscillatorLink);

		statusBar.add(linkPanel, BorderLayout.WEST);
		statusBar.add(tooltipLabel, BorderLayout.CENTER);
		statusBar.add(buttonPanel, BorderLayout.EAST);

		return statusBar;
	}

	public void setTooltip(String s) {
		tooltipLabel.setText(s);
	}

	public void actionPerformed(ActionEvent ae) {
		editPanel.repaint();
		if (ae.getSource() == stereoLink) {
			if (editPanel.getClass() == LeadGUI.class) {
				((LeadGUI) editPanel).stereoLink(stereoLink.isSelected());
			} else if (editPanel.getClass() == BasslineGUI.class) {
				((BasslineGUI) editPanel).stereoLink(stereoLink.isSelected());
			} else if (editPanel.getClass() == DrumGUI.class) {
				((DrumGUI) editPanel).stereoLink(stereoLink.isSelected());
			} else if (editPanel.getClass() == MultiGUI.class) {
				((MultiGUI) editPanel).stereoLink(stereoLink.isSelected());
			}
		} else if (ae.getSource() == oscillatorLink) {
			if (editPanel.getClass() == LeadGUI.class) {
				((LeadGUI) editPanel).oscLink(oscillatorLink.isSelected());
			} else if (editPanel.getClass() == BasslineGUI.class) {
				// Do nothing
			} else if (editPanel.getClass() == DrumGUI.class) {
				// Do nothing
			} else if (editPanel.getClass() == MultiGUI.class) {
				((MultiGUI) editPanel).oscLink(oscillatorLink.isSelected());
			}
		}
	}
}