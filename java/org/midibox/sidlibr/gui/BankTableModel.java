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

import javax.swing.table.AbstractTableModel;

import org.midibox.sidlibr.Bank;

public class BankTableModel extends AbstractTableModel {
	private String[] patchColumnNames = { "#", "Patch Name", "Engine" };
	private String[] ensembleColumnNames = { "#", "Ensemble Name"};
	private Object[][] data;
	private boolean isEnsembleBank;

	public BankTableModel(Bank b) {
		if (b.isEnsembleBank()) {
			isEnsembleBank = true;
			data = new Object[b.bankSize][2];
			for (int c = 0; c < b.bankSize; c++) {
				data[c][0] = intToStr3(c + 1);
				data[c][1] = "Ensemble " + intToStr3(c + 1);
			}
		} else {
			isEnsembleBank = false;
			data = new Object[b.bankSize][3];
			for (int c = 0; c < b.bankSize; c++) {
				data[c][0] = intToStr3(c + 1);
				data[c][1] = b.getPatchAt(c).getPatchName();
				data[c][2] = b.getPatchAt(c).getEngineStr();
			}
		}
	}

	public int getColumnCount() {
		return data[0].length;
	}

	public int getRowCount() {
		return data.length;
	}

	public String getColumnName(int col) {
		if (isEnsembleBank) {
			return ensembleColumnNames[col];
		} else {
			return patchColumnNames[col];
		}
	}

	public Object getValueAt(int row, int col) {
		return data[row][col];
	}

	public boolean isCellEditable(int row, int col) {
		if (col == 1 & !isEnsembleBank) {
			return true;
		} else {
			return false;
		}
	}

	public void setValueAt(Object value, int row, int col) {
		data[row][col] = value;
		fireTableCellUpdated(row, col);
	}

	private String intToStr3(int i) {
		String s = Integer.toString(i);
		switch (s.length()) {
		case 1:
			s = "00" + s;
			break;
		case 2:
			s = "0" + s;
			break;
		}
		return s;
	}
}