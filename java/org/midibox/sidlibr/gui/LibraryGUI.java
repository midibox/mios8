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

package org.midibox.sidlibr.gui;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.event.ChangeListener;

import org.midibox.sidlibr.SIDLibController;

public class LibraryGUI extends JPanel implements ChangeListener, Observer {
	private SIDLibController sidLibController;
	private JTabbedPane tabbedPane = new JTabbedPane();
	private BankTable[] bankTables = new BankTable[8];
	private static String[] bankNames = { "ENS", "A", "B", "C", "D", "E", "F",
			"G" };

	public LibraryGUI(SIDLibController sidLibController) {
		this.sidLibController = sidLibController;
		for (int c = 0; c < bankTables.length; c++) {
			bankTables[c] = new BankTable(sidLibController, c);
			tabbedPane.addTab(bankNames[c], bankTables[c]);
		}
		add(tabbedPane);
		tabbedPane.addChangeListener(this);
		tabbedPane.setSelectedIndex(1);
	}

	public void stateChanged(javax.swing.event.ChangeEvent e) {
		sidLibController.setCurrentBankNumber(tabbedPane.getSelectedIndex());
		int[] patchNumber = bankTables[tabbedPane.getSelectedIndex()]
				.getSelectedRows();
		if (patchNumber.length == 0) {
			bankTables[tabbedPane.getSelectedIndex()].resetSelection();
			patchNumber = new int[] { 0 };
		}
		sidLibController.setCurrentPatchNumber(patchNumber);
	}

	public void update(Observable observable, Object object) {
		this.repaint();
	}
}
