import java.awt.Color;
import java.awt.Graphics2D;

import tCode.RenderableObject;
import tComponents.components.TButton;
import tComponents.components.TMenu;

public class Menu extends RenderableObject
	{
		protected void initiate()
			{
				TMenu menu = new TMenu(0, 0, Main.canvasWidth, Main.canvasHeight, TMenu.VERTICAL);
				add(menu);

				// @formatter:off
				TButton hillclimber = new TButton("Every RGB Hillclimber"){@Override public final void pressed(){ changeRenderableObject(new EveryRGBHillclimber());}};
				TButton hillclimberNegative = new TButton("Every RGB Hillclimber Negative"){@Override public final void pressed(){ changeRenderableObject(new EveryRGBHillclimberNegative());}};
				TButton exact = new TButton("Every RGB Exact"){@Override public final void pressed(){ changeRenderableObject(new EveryRGBExact());}};
				TButton ovals = new TButton("Ovals (not everyRGB)"){@Override public final void pressed(){ changeRenderableObject(new PictureApproximator());}};
				// @formatter:on
				
				menu.add(hillclimber);
				menu.add(hillclimberNegative);
				menu.add(exact);
				menu.add(ovals);
			}

		protected void render(Graphics2D g)
			{
				g.setColor(Color.BLACK);
				g.fillRect(0, 0, Main.canvasWidth, Main.canvasHeight);
			}
	}