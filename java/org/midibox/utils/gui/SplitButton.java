package org.midibox.utils.gui;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JToolBar;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.*;
import java.awt.Component;
import java.awt.Insets;

public class SplitButton extends JToolBar implements ActionListener {
	private JButton button, splitButton;
	private JPopupMenu dropMenu;
	
	public SplitButton(JButton button,JPopupMenu dropMenu) {
		this.button = button;
		this.dropMenu = dropMenu;		
		setFloatable(false);
		this.setBorderPainted(false);		
		ImageIcon icon = ImageLoader.getImageIcon("pulldown.png");
		splitButton = new JButton();
		splitButton.setIcon(icon);
		splitButton.addActionListener(this);	
		splitButton.setFocusPainted(false);
		splitButton.setPreferredSize(new Dimension(icon.getIconWidth(), splitButton.getPreferredSize().height));
		splitButton.setMargin(new Insets(2,0,2,0));
		setMargin(new Insets(-1,-2,-3,-2));
		add(button);
		add(splitButton);
	}
	public void actionPerformed(ActionEvent e) { 
		if (e.getSource()==splitButton) {
			Component c = (Component)button; 
			dropMenu.show(c, 0, c.getHeight()); 
		}
    }
}


