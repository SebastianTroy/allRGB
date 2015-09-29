import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.UIManager;
import javax.swing.filechooser.FileNameExtensionFilter;

import tCode.RenderableObject;
import tools.Rand;

public class EveryRGBHillclimber extends RenderableObject {
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~ Constant Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// The width and height of the images containing one pixel of every RGB colour
	private static final int IMG_SIZE = 4096;
	// The number of unique RGB colours
	private static final int NUM_COLS = 16777216;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~ Image Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// The reference Image, The best approximation so far, Redrawn each generation
	private BufferedImage referenceImage, allRGBImage;
	// Coordinates and scale for drawing the images
	private int scale = 410, refX = 0, refY = 30, allX = 0, allY = 30;
	// The number of generations to the current solution
	private long numGenerations = 0, numImprovements = 0;

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~ Variables to limit draw rate to fps ~~~~~~~~~~~~~~~~~
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	private boolean redraw = true;
	// Used to store how long has passed since the last render
	private double timer = 0;
	private final double FPS = 0.5, MAX_TIME = 1.0 / FPS;

	@Override
	protected final void initiate() {
		// Make the Reference Image into the instructions
		referenceImage = new BufferedImage(IMG_SIZE, IMG_SIZE, BufferedImage.TYPE_INT_RGB);

		Graphics2D g = (Graphics2D) referenceImage.getGraphics();

		// Fill the background
		g.setColor(Color.WHITE);
		g.fillRect(0, 0, IMG_SIZE, IMG_SIZE);

		g.setFont(g.getFont().deriveFont(200f));

		// Write the instructions
		g.setColor(Color.BLACK);
		int y = 700;
		g.drawString("-Left click on an image to load a replacement", 5, y);
		y += 700;
		g.drawString("-Right click on an image to save it", 5, y);
		y += 700;
		g.drawString("-Resising the window to make it as small as", 5, y);
		y += 250;
		g.drawString("possible will maximise the frame rate, then", 5, y);
		y += 250;
		g.drawString("maximising and minimising the window will", 5, y);
		y += 250;
		g.drawString("allow you to view the progress.", 5, y);
		y += 250;

		g.dispose();

		// Allocate memory for our allRGB approximation
		createAllRGBImage();

		Main.frame.setSize(800, 600);
	}

	@Override
	protected final void refresh() {
		// Update the whole screen
		redraw = true;
		numGenerations = 0;
		numImprovements = 0;
	}

	@Override
	public final void tick(double secondsPassed) {
		// Make sure we redraw at least 30 times a second
		timer += secondsPassed;
		if (timer >= MAX_TIME) {
			timer -= MAX_TIME;
			redraw = true;
		}

		for (int i = 0; i < 10000; i++) {
			// pick two random pixels
			int x1 = Rand.int_(IMG_SIZE), y1 = Rand.int_(IMG_SIZE);
			int x2 = Rand.int_(IMG_SIZE), y2 = Rand.int_(IMG_SIZE);

			// calculate the current fitness of the two pixels
			int oldDifference = getDifference(x1, y1, x1, y1) + getDifference(x2, y2, x2, y2);
			// calculate the fitness if the pixels were swapped
			int newDifference = getDifference(x1, y1, x2, y2) + getDifference(x2, y2, x1, y1);

			// If the change was beneficial, swap the pixels around, want the smallest difference possible
			if (newDifference < oldDifference) {
				swapPixels(x1, y1, x2, y2);
				numImprovements++;
			}

			numGenerations++;
		}
	}

	@Override
	protected final void render(Graphics2D g) {
		// Set the background colour to Black
		g.setColor(Color.BLACK);

		// If the images need updating
		if (redraw && scale > 1) {
			g.fillRect(0, 0, Main.canvasWidth, Main.canvasHeight);
			g.drawImage(referenceImage, refX, refY, scale, scale, getObserver());
			g.drawImage(allRGBImage, allX, allY, scale, scale, getObserver());

			redraw = false;
		}

		g.fillRect(0, 0, 280, 25);
		g.setColor(Color.WHITE);
		g.drawString("Gen: " + numGenerations, 0, 15);
		g.drawString("Improvements: " + numImprovements, 110, 15);
	}

	private final void saveImage(BufferedImage img) {
		// Make sure the button in the JFileChooser says the right thing!
		UIManager.put("FileChooser.openButtonText", "Save");

		// Open a JFileChooser in the current directory
		JFileChooser saver = new JFileChooser(new File(""));

		// Give the user the option to limit their search to only image files
		FileNameExtensionFilter filter = new FileNameExtensionFilter("Images", "jpg", "jpeg", "png", "gif", "bmp");
		saver.setFileFilter(filter);

		// if the user saves image
		if (saver.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
			try {
				ImageIO.write(img, "png", saver.getSelectedFile());
			}
			catch (IOException e) {
				JOptionPane.showMessageDialog(null, "Sorry, saving the image didn't work!", "Error", JOptionPane.ERROR_MESSAGE);
				e.printStackTrace();
			}
		}
	}

	private final BufferedImage loadImage(BufferedImage origionalImage) {
		BufferedImage img = new BufferedImage(IMG_SIZE, IMG_SIZE, BufferedImage.TYPE_INT_RGB);

		/*
		 * FIRST - load the image
		 */

		// Make sure the button in the JFileChooser says the right thing!
		UIManager.put("FileChooser.openButtonText", "Open");

		// Open a JFileChooser in the current directory
		JFileChooser chooser = new JFileChooser(new File(""));

		// Give the user the option to limit their search to only image files
		FileNameExtensionFilter filter = new FileNameExtensionFilter("Images", "jpg", "jpeg", "png", "gif", "bmp");
		chooser.setFileFilter(filter);

		// if the user chooses an image
		if (chooser.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
			try {
				// Set the reference image to a user selected file
				img = ImageIO.read(chooser.getSelectedFile());
			}
			catch (Exception e) {
				JOptionPane.showMessageDialog(null, "Sorry but that image doesn't work!", "Error", JOptionPane.ERROR_MESSAGE);
				e.printStackTrace();
			}
		} else
			return origionalImage;

		/*
		 * SECOND - Make sure it is the correct size
		 */
		if (img.getWidth() != IMG_SIZE || img.getHeight() != IMG_SIZE) {
			// Create a new image of the correct size
			BufferedImage tempImage = new BufferedImage(IMG_SIZE, IMG_SIZE, BufferedImage.TYPE_INT_RGB);

			// Draw a scaled version of the loaded image onto it
			Graphics2D g = (Graphics2D) tempImage.getGraphics();
			g.drawImage(img, 0, 0, IMG_SIZE, IMG_SIZE, getObserver());
			g.dispose();

			// make the temp image the reference image
			img = tempImage;
		}

		return img;
	}

	private final void createAllRGBImage() {
		// Allocate memory
		allRGBImage = new BufferedImage(IMG_SIZE, IMG_SIZE, BufferedImage.TYPE_INT_RGB);

		// Fill the image with one pixel of every RGB value
		int rgb = -NUM_COLS; // -NUM_COLS == new Color(0,0,0).getRGB() - 1;
		for (int x = 0; x < IMG_SIZE; x++)
			for (int y = 0; y < IMG_SIZE; y++) {
				rgb++;
				allRGBImage.setRGB(x, y, rgb);
			}
	}

	/**
	 * A measure of how different the pixel from the referenceImage at (x, y) is from the pixel in the allRGBImage at (x, y).
	 * 
	 * @param x - the x index of the pixels
	 * @param y - the y index of the pixels
	 * 
	 * @return - A number in the range 0->195075, where 0 == identical RGB values and 195075 == opposite RGB values.
	 */
	private final int getDifference(int refx, int refy, int rgbx, int rgby) {
		int refRGB = referenceImage.getRGB(refx, refy);
		int newRGB = allRGBImage.getRGB(rgbx, rgby);

		int deltaRed = (refRGB >> 16) & 0x0ff - (newRGB >> 16) & 0x0ff;
		int deltaGreen = (refRGB >> 8) & 0x0ff - (newRGB >> 8) & 0x0ff;
		int deltaBlue = (refRGB >> 0) & 0x0ff - (newRGB >> 0) & 0x0ff;

		return (deltaRed * deltaRed) + (deltaGreen * deltaGreen) + (deltaBlue * deltaBlue);
	}

	/**
	 * Swaps the RGB values in the allRGBImage, for the pixel at (x1, y1) with the pixel at (x2, y2)
	 * 
	 * @param x1 - the x coordinate for the first pixel
	 * @param y1 - the y coordinate for the first pixel
	 * @param x2 - the x coordinate for the second pixel
	 * @param y2 - the y coordinate for the second pixel
	 */
	private final void swapPixels(int x1, int y1, int x2, int y2) {
		int temp = allRGBImage.getRGB(x1, y1);
		allRGBImage.setRGB(x1, y1, allRGBImage.getRGB(x2, y2));
		allRGBImage.setRGB(x2, y2, temp);
	}

	/**
	 * If the window is resized, re-size and re-space the images.
	 */
	@Override
	public void componentResized(ComponentEvent e) {
		// 339, 36

		// set the scale to the largest size that doesn't obscure the menu, but no less than 1
		scale = Math.max(1, Math.min((Main.canvasWidth - 100) / 2, Main.canvasHeight - 60));

		// space the images evenly across the screen
		int horizontal_spacing = (Main.canvasWidth - (2 * scale)) / 3;

		refX = horizontal_spacing;
		allX = (horizontal_spacing * 2) + scale;

		// redraw the images
		redraw = true;
	}

	/**
		 * 
		 */
	@Override
	public void mousePressed(MouseEvent e) {
		// If mouse over reference image
		if (e.getX() < Main.canvasWidth / 2) {
			// If left mouse button
			if (e.getButton() == MouseEvent.BUTTON1) {
				referenceImage = loadImage(referenceImage);
				refresh();
			}
			// If right Mouse Button
			else if (e.getButton() == MouseEvent.BUTTON3)
				saveImage(referenceImage);
		}
		// If mouse over allRGB image
		else if (e.getX() > Main.canvasWidth / 2) {
			// If left mouse button
			if (e.getButton() == MouseEvent.BUTTON1) {
				JOptionPane option = new JOptionPane();
				option.setMessageType(JOptionPane.YES_NO_OPTION);

				if (JOptionPane.showConfirmDialog(null, "Would you like to reset the image?", "RESET", JOptionPane.YES_NO_OPTION) == JOptionPane.OK_OPTION)
					createAllRGBImage();
				else if (JOptionPane.showConfirmDialog(null, "Would you like to load a previous image?", "RESET", JOptionPane.YES_NO_OPTION) == JOptionPane.OK_OPTION) {
					allRGBImage = loadImage(allRGBImage);
					refresh();
				}
			}
			// If right Mouse Button
			else if (e.getButton() == MouseEvent.BUTTON3)
				saveImage(allRGBImage);
		}
	}
}
