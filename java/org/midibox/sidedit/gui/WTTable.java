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
import java.awt.Graphics;
import java.awt.event.*;
import java.util.Vector;
import javax.swing.table.TableModel;
import javax.swing.*;
import java.util.Observable;
import java.util.Observer;
import java.util.EventObject;
import org.midibox.sidlibr.SIDLibController;
import java.awt.Color;

import javax.swing.BoxLayout;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.KeyStroke;
import javax.swing.ListSelectionModel;
import org.midibox.sidlibr.Bank;
import java.awt.Font;
import org.midibox.sidedit.*;
import javax.swing.table.*;
import java.awt.Component;

public class WTTable extends JPanel implements TableModelListener, Observer, MouseListener, MouseMotionListener, ActionListener {
	JTable table;
	JPopupMenu popupMenu;
	int bankNumber;
	int selectedBeforeDrag = 0;
	private int wtNumber;
	private Vector midiParams;
	private SIDSysexParameterControl[][] config;
	private boolean refreshing = false;
	public int decHexNote = 0;
	private boolean valueDragging = false;
	private int rowDragging = 0;
	private int dragpos = -1;
	private float DRAG_SPEED = 1.5F;// 0.01F;
	private float startVal = 0;
	private String[] noteString;
	private JRadioButton decButton;
	private JRadioButton hexButton;
	private JRadioButton noteButton;
	
	public WTTable(int wtNumber, Vector midiParams, SIDSysexParameterControl[][] config) {
		this.wtNumber = wtNumber;
		this.midiParams = midiParams;
		this.config = config;
		
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		setOpaque(false);
		
		table = new JTable(new WTTableModel(wtNumber));
		table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		table.getModel().addTableModelListener(this);
		table.addMouseListener(this);
		table.addMouseMotionListener(this);
		
		table.getColumnModel().getColumn(0).setPreferredWidth(30);
		for (int c=0;c<wtNumber;c++) {
			table.getColumnModel().getColumn(c+1).setCellRenderer(new ColorCellRenderer());
			table.getColumnModel().getColumn(c+1).setPreferredWidth(10);
		}
		
		table.getColumnModel().getColumn(wtNumber+1).setCellRenderer(new ColorCellRenderer());		
		table.getColumnModel().getColumn(wtNumber+1).setPreferredWidth(150);
		
		table.setPreferredScrollableViewportSize(new Dimension(table.getPreferredSize().width, Math.round(table.getPreferredSize().height/10)));
		// TK: temporary commented out - not available under MacOS, we need a replacement
		// table.setFillsViewportHeight(true);
	    
		JScrollPane scrollPane = new JScrollPane(table);
		scrollPane.setPreferredSize(new Dimension(table.getPreferredSize().width, Math.round(table.getPreferredSize().height/4)));
		
		for(int row=0;row<midiParams.size();row++) {		
			((SIDSysexParameterControl)midiParams.elementAt(row)).addObserver(this);
		}	
		for(int i=0;i<config.length;i++) {	
			for(int j=0;j<config[i].length;j++) {
				config[i][j].addObserver(this);
			}
		}	
		createNotes();
		refreshTable();
		add(scrollPane);
		
		decButton = new JRadioButton("Dec view");
		decButton.setOpaque(false);
		hexButton = new JRadioButton("Hex view");
		hexButton.setOpaque(false);
		noteButton = new JRadioButton("Note view");
		noteButton.setOpaque(false);
		
		decButton.setSelected(true);
		decButton.addActionListener(this);
		hexButton.addActionListener(this);
		noteButton.addActionListener(this);
		
		JPanel buttonPanel = new JPanel();
		buttonPanel.setOpaque(false);
		buttonPanel.add(decButton);
		buttonPanel.add(hexButton);
		buttonPanel.add(noteButton);
		
		ButtonGroup bg = new ButtonGroup();
		bg.add(decButton);
		bg.add(hexButton);
		bg.add(noteButton);
		
		add(buttonPanel);
	}	
	
	public void refreshTable() {		
		refreshing = true;
		TableModel m = table.getModel();
		for(int row=0;row<midiParams.size();row++) {	
			String newval = interpret(((SIDSysexParameterControl)midiParams.elementAt(row)).getMidiValue());
			m.setValueAt(newval, row, wtNumber+1);
			
			// Set loop indicator
			for(int col=1;col<wtNumber+1;col++) {
				if (config[col-1][2].getMidiValue()==row) {
					m.setValueAt("128",row,col);
				} else {
					m.setValueAt("",row,col);
				}
			}
		}
		refreshing = false;
	}
	
	private int parseHexDec(String s) {
		int v = 0;
		switch (decHexNote) {
			case 0:	// Decimal
				v = Integer.parseInt(s);
				break;			
			case 1:	// Hexadecimal
				v = Integer.parseInt(s, 16);
				break;
			case 2:	// Note view (only lower part)
				v = Integer.parseInt(s);
				break;
			default: throw new NumberFormatException("Input not recognized!");
		}
		return v;
	}
	
	private int findNote(String s) {
		int v = 0;
		for(int c=0;c<128;c++) {				
			if (noteString[c].equalsIgnoreCase(s)) {
				v = c;
				break;
			}
			if (c==127) {
				throw new NumberFormatException("Note string not recognized");
			}
		}
		return v;
	}
	
	private void createNotes() {
		noteString = new String[256];
		String[] notes = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};		
		for (int i = 0; i < 128; i++) {
			if (i == 0) {
				noteString[i] = "---";
			}
			else if (i == 1) {
				noteString[i] = "+++";
			}
			else if (i < 124) {
				int octave = (int) Math.floor((i)/12)-2;
				if (octave < 0) {
					noteString[i] = notes[(i)%12] + octave;
				}
				else {
					noteString[i] = notes[(i)%12] + octave;
				}
			}
			else if (i < 128) {
				noteString[i] = "Ky" + Integer.toString(i-123);
			}
		}
	}
	
	private void parseInput(String value, int row) {
		int v = ((SIDSysexParameterControl)midiParams.elementAt(row)).getMidiValue();
		try {	
			if (value.equals("---")) {
				v = 128;
			} else if (value.equals("+++")) {
				v = 129;
			} else if (value.substring(0,1).equals("-")) {			// Relative negative value					
				int temp = parseHexDec(value.substring(1));					
				if (-temp < -64) {
					v = 0;						
				} else if (-temp > 0) {
					v = 64;						
				} else {
					v = -temp+64;
				}
			} else if (value.substring(0,1).equals("+")) {			// Relative positive value
				int temp = parseHexDec(value.substring(1));	
				if (temp < 0) {
					v = 63;
				} else if (temp > 63) {
					v = 127;						
				} else {
					v = temp+64;
				}
			} else {												// Absolute value
				int temp;
				if (decHexNote==2) {
					temp = findNote(value);
				} else {
					temp = parseHexDec(value);
				}					
				if (temp < 0) {
					v = 128;
				} else if (temp > 127) {
					v = 255;						
				} else {
					v = temp+128;
				}
			}
		} catch (NumberFormatException e) {
			// Revert!								
		}
		((SIDSysexParameterControl)midiParams.elementAt(row)).setMidiValue(v, true);
	}

	private String interpretHexDec(int i) {
		String s = null;
		switch (decHexNote) {
		case 0:	// Decimal
			s = Integer.toString(i);
			break;			
		case 1:	// Hexadecimal
			s = "00" + Integer.toHexString(i).toUpperCase();
			s = s.substring(s.length()-2);
			break;
		case 2: // Note view
			s = Integer.toString(i);		
			break;	
		}
		return s;
	}
	
	private String interpret(int i) {
		String s = "";
		if (i < 64) {
			s = "-" + interpretHexDec(-i+64);
		} else if (i < 128) {
			s = "+" + interpretHexDec(i-64);
		} else {
			if (decHexNote==2) {
				s = noteString[i-128];				
			} else {
				s = interpretHexDec(i-128);
			}	
		}
		return s;
	}
	
	public void tableChanged(TableModelEvent e) {
		if (!refreshing) {
			int row = e.getFirstRow();
			int column = e.getColumn();
	        TableModel model = (TableModel)e.getSource();
	        Object data = model.getValueAt(row, column);
	        if (column == wtNumber+1) {
	        	parseInput((String)data,row);
	        }   
		}		     
    }
	
	public void update(Observable observable, Object object) {
		refreshTable();
	}
	
	public void actionPerformed(ActionEvent e) { 
        if ((e.getSource()==decButton)||(e.getSource()==hexButton)||(e.getSource()==noteButton)) {
        	if (decButton.isSelected()) {
        		decHexNote = 0;
        	} else if (hexButton.isSelected()) {
        		decHexNote = 1;
        	} else if (noteButton.isSelected()) {
        		decHexNote = 2;
        	}
        	refreshTable();
        }
    }
	
	public int getSelectedRow() {
		return table.getSelectedRow();
	}
	
	public void mousePressed(MouseEvent e) {
		if (table.getSelectedColumn()==wtNumber+1) {
			valueDragging = true;
			rowDragging = table.getSelectedRow();
			startVal = (float)((SIDSysexParameterControl)midiParams.elementAt(rowDragging)).getMidiValue();
			dragpos = e.getX() - e.getY();						
		}			
	}

    public void mouseReleased(MouseEvent e) {valueDragging = false;}

    public void mouseEntered(MouseEvent e) {}

    public void mouseExited(MouseEvent e) {}

    public void mouseClicked(MouseEvent e) {}

    public void mouseMoved(MouseEvent me) {}

	public void mouseDragged(MouseEvent me) {
		if (valueDragging) {
			float f = DRAG_SPEED * (float) ((me.getX() - me.getY()) - dragpos);
			int newval = Math.round(startVal + f);
			if (newval<0) {
				newval = 0;
			} else if (newval>255) {
				newval = 255;
			}
			table.getModel().setValueAt(interpret(newval), rowDragging, wtNumber+1);			
		}
	}
	
	public class ColorCellRenderer extends JComponent implements TableCellRenderer {
		Color curColor;
        String curSymbol;
        int val;
        
        public Component getTableCellRendererComponent(JTable table, Object value,
                boolean isSelected, boolean hasFocus, int rowIndex, int vColIndex) {
        	
        	if (vColIndex == wtNumber+1) {
        		curColor = Color.RED;        		
        		val = ((SIDSysexParameterControl)midiParams.elementAt(rowIndex)).getMidiValue();
        		curSymbol = (String)value;
        	}
        	
        	if ((vColIndex > 0) && (vColIndex < wtNumber+1)) {
	        	int b = config[vColIndex-1][0].getMidiValue();
				int e = config[vColIndex-1][1].getMidiValue();
				int l = config[vColIndex-1][2].getMidiValue();
	        	
	        	if ((rowIndex <= e) && (rowIndex >= b)) {
	        		curColor = Color.BLACK;
	        	} else {
	        		curColor = table.getBackground();
	        	}
	        	if (rowIndex==l) {
	        		curSymbol = "O";
	        	} else {
	        		curSymbol = "";
	        	}
        	}
            return this;
        }
        
        public void paint(Graphics g) {            
            if (curColor == Color.RED) {	// Red value indicator
            	g.setColor(curColor);
            	g.fillRect(0, 0, Math.round(((float)val/255)*(getWidth()-1)), getHeight()-1);
            } else {
            	g.setColor(curColor);		// Fill color
            	g.fillRect(0, 0, getWidth()-1, getHeight()-1);
            }            
            if (curColor==Color.BLACK) {	// Swap text color to contrast with background
            	g.setColor(Color.WHITE);            	
            } else {
            	g.setColor(Color.BLACK);
            }            
            g.drawString(curSymbol, 2, 12);	// Add loop symbol (if required)
        }    
    }
}
