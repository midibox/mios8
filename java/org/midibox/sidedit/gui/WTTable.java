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

import java.awt.Dimension;

import javax.swing.event.*;
import java.awt.event.*;

import javax.swing.table.TableModel;
import javax.swing.*;
import java.util.Observable;
import java.util.Observer;
import java.util.EventObject;
import org.midibox.sidlibr.SIDLibController;

import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.KeyStroke;
import javax.swing.ListSelectionModel;
import org.midibox.sidlibr.Bank;
import java.awt.Font;

public class WTTable extends JPanel implements TableModelListener, ListSelectionListener, Observer, ActionListener {
	JTable table;
	JPopupMenu popupMenu;
	int bankNumber;
	int selectedBeforeDrag = 0;
	private int wtNumber;
	public WTTable(int wtNumber) {
		this.wtNumber = wtNumber;
		//sidLibController.addObserver(this);
		
		table = new JTable(new WTTableModel(wtNumber/*data in*/));
		table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		table.getModel().addTableModelListener(this);		
		table.getSelectionModel().addListSelectionListener(this);
		
		//table.getColumnModel().getColumn(1).setCellEditor(new PatchNameEditor());
		table.getColumnModel().getColumn(0).setPreferredWidth(30);
		for (int c=0;c<wtNumber;c++) {
			table.getColumnModel().getColumn(c+1).setPreferredWidth(10);
		}
		table.getColumnModel().getColumn(wtNumber+1).setPreferredWidth(70);
		
		table.setPreferredScrollableViewportSize(new Dimension(table.getPreferredSize().width, Math.round(table.getPreferredSize().height/4)));
	    table.setFillsViewportHeight(true);
	    
		JScrollPane scrollPane = new JScrollPane(table);
		scrollPane.setPreferredSize(new Dimension(table.getPreferredSize().width, Math.round(table.getPreferredSize().height/4)));
		
		add(scrollPane);
	}
	
	public void tableChanged(TableModelEvent e) {
		/*
		int row = e.getFirstRow();
		int column = e.getColumn();
        TableModel model = (TableModel)e.getSource();
        Object data = model.getValueAt(row, column);
        if ((column == 1) && !(sidLibController.getBank(bankNumber).getPatchAt(row).getPatchName().equals((String)data))) {
        	sidLibController.getBank(bankNumber).getPatchAt(row).setPatchName((String)data);
        }
        */
    }
	
	public void update(Observable observable, Object object) {
		/*
		if (object=="Data changed") {
			Bank b = sidLibController.getBank(bankNumber);
			TableModel m = table.getModel();
			for (int c = 0; c < b.bankSize; c++) {
				m.setValueAt(b.getPatchAt(c).getPatchName(), c, 1);
				m.setValueAt(b.getPatchAt(c).getEngineStr(), c, 2);				
			}
		}
		if (object=="Rename") {
			table.editCellAt(table.getSelectedRow(), 1, new EventObject(this));
		}
		*/
	}
	
	public int getSelectedRow() {
		return table.getSelectedRow();
	}
    
	public void actionPerformed(ActionEvent ae) {
    	//sidLibController.actionPerformed(ae);
    }
	
	public void valueChanged(ListSelectionEvent event) {
		/*
		int patchNumber = table.getSelectedRow();
		if (patchNumber < 0) {
			patchNumber = 0;
		}
		sidLibController.setCurrentPatchNumber(patchNumber);
		*/
	}	
}
