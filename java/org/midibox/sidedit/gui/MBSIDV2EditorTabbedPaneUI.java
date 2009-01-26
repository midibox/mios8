package org.midibox.sidedit.gui;

import java.awt.Color;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;

import javax.swing.UIManager;
import javax.swing.plaf.basic.BasicTabbedPaneUI;

public class MBSIDV2EditorTabbedPaneUI extends BasicTabbedPaneUI {

	private Color startGradient;

	private Color endGradient;

	public MBSIDV2EditorTabbedPaneUI(Color startGradient, Color endGradient) {
		super();

		this.startGradient = startGradient;
		this.endGradient = endGradient;
	}

	protected void paintContentBorder(Graphics g, int tabPlacement,
			int selectedIndex) {
		int width = tabPane.getWidth();
		int height = tabPane.getHeight();
		Insets insets = tabPane.getInsets();
		Insets tabAreaInsets = getTabAreaInsets(tabPlacement);

		int x = insets.left;
		int y = insets.top;
		int w = width - insets.right - insets.left;
		int h = height - insets.top - insets.bottom;

		boolean tabsOverlapBorder = UIManager
				.getBoolean("TabbedPane.tabsOverlapBorder");

		boolean tabsOpaque = UIManager.getBoolean("TabbedPane.tabsOpaque");

		boolean contentOpaque = UIManager
				.getBoolean("TabbedPane.contentOpaque");

		Color selectedColor = UIManager.getColor("TabbedPane.selected");

		switch (tabPlacement) {
		case LEFT:
			x += calculateTabAreaWidth(tabPlacement, runCount, maxTabWidth);
			if (tabsOverlapBorder) {
				x -= tabAreaInsets.right;
			}
			w -= (x - insets.left);
			break;
		case RIGHT:
			w -= calculateTabAreaWidth(tabPlacement, runCount, maxTabWidth);
			if (tabsOverlapBorder) {
				w += tabAreaInsets.left;
			}
			break;
		case BOTTOM:
			h -= calculateTabAreaHeight(tabPlacement, runCount, maxTabHeight);
			if (tabsOverlapBorder) {
				h += tabAreaInsets.top;
			}
			break;
		case TOP:
		default:
			y += calculateTabAreaHeight(tabPlacement, runCount, maxTabHeight);
			if (tabsOverlapBorder) {
				y -= tabAreaInsets.bottom;
			}
			h -= (y - insets.top);
		}

		if (tabPane.getTabCount() > 0 && (contentOpaque || tabPane.isOpaque())) {

			// Custom painting here

			Graphics2D g2d = (Graphics2D) g;
			GradientPaint gradient = new GradientPaint(0, 0, startGradient, w,
					h, endGradient);
			g2d.setPaint(gradient);
			g2d.fillRect(x, y, w, h);
		}

		paintContentBorderTopEdge(g, tabPlacement, selectedIndex, x, y, w, h);
		paintContentBorderLeftEdge(g, tabPlacement, selectedIndex, x, y, w, h);
		paintContentBorderBottomEdge(g, tabPlacement, selectedIndex, x, y, w, h);
		paintContentBorderRightEdge(g, tabPlacement, selectedIndex, x, y, w, h);
	}
}
