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

import javax.swing.table.AbstractTableModel;
import org.midibox.sidlibr.*;

public class WTTableModel extends AbstractTableModel {	
	private String[] columnNames = {"Pos","Value","Control"};
	private Object[][] data;
	private int wtSteps = 128;
	private int wtNumber;
	
	public WTTableModel (int wtNumber) {
		this.wtNumber = wtNumber;
		
		data = new Object[wtSteps][wtNumber+2];
		for (int c = 0; c < wtSteps; c++) {
			data[c][0] = Integer.toString(c);			
			data[c][wtNumber+1] = Integer.toString(0);
		}		
	}
	
    public int getColumnCount() {
        return wtNumber+2;
    }

    public int getRowCount() {
        return data.length;
    }

    public String getColumnName(int col) {
    	if (col==0) {
    		return columnNames[0];
    	} else if (col<=wtNumber) {
    		return Integer.toString(col);
    	} else if (col > wtNumber) {
    		return columnNames[col-wtNumber];
    	} else {
    		return "Unknown";
    	}
    }

    public Object getValueAt(int row, int col) {
        return data[row][col];
    }

    public boolean isCellEditable(int row, int col) {
        if (col == wtNumber+1) {
            return true;
        } else {
            return false;
        }
    }
   
    public void setValueAt(Object value, int row, int col) {
        data[row][col] = value;
        fireTableCellUpdated(row, col);
    }
}