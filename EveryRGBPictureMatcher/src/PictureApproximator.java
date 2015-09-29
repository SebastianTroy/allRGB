import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.Iterator;
import java.util.LinkedList;

import javax.imageio.ImageIO;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileNameExtensionFilter;

import org.imgscalr.Scalr;

import tCode.RenderableObject;
import tools.ColTools;
import tools.Rand;

public class PictureApproximator extends RenderableObject {
	// The maximum size the images can be, they will be scaled down if they are
	private static final int MAX_WIDTH = 450, MAX_HEIGHT = 450;

	// The reference Image, The best approximation so far, Redrawn each generation
	private BufferedImage refImg, bestApproxImg, newApproxImg;

	// Used to align the images into the centre of their halves of the screen
	private int xAlign, yAlign;

	// If the best approximate image has been updated, it'll need to be redrawn
	private boolean redrawImages = true;

	// The {@link Shape}s that will be drawn onto the approximate image
	private LinkedList<Oval> shapes = new LinkedList<Oval>();

	// The number of generations to the current solution
	private int numGenerations = 0;

	// The fitness of the current best approximation
	private int currentFitness = 0;

	// This colour has no affect on the algorithm, adjusting it simply makes some images easier to see
	private Color backgroundCol = Color.BLACK;

	@Override
	protected final void initiate() {
		refImg = new BufferedImage(MAX_WIDTH, MAX_HEIGHT, BufferedImage.TYPE_INT_RGB);
		// TODO add some sort of menu below the images
	}

	@Override
	protected final void refresh() {
		loadImage();
		shapes.clear();

		bestApproxImg = new BufferedImage(refImg.getWidth(), refImg.getHeight(), BufferedImage.TYPE_INT_RGB);
		newApproxImg = new BufferedImage(refImg.getWidth(), refImg.getHeight(), BufferedImage.TYPE_INT_RGB);

		redrawImages = true;
		numGenerations = 0;

		drawSolution(bestApproxImg);
		currentFitness = calculateFitness(bestApproxImg);
	}

	@Override
	public final void tick(double secondsPassed) {
		int newShapeIndex = Rand.int_(0, shapes.size());

		shapes.add(newShapeIndex, new Oval());

		drawSolution(newApproxImg);

		int newFitness = calculateFitness(newApproxImg);
		if (newFitness >= currentFitness) {
			// update the current fitness
			currentFitness = newFitness;

			// switch around the images
			BufferedImage temp = bestApproxImg;
			bestApproxImg = newApproxImg;
			newApproxImg = temp;

			// redraw the images so we can see the improvement on screen
			redrawImages = true;
		} else
			// remove the new shape that made our solution worse
			shapes.remove(newShapeIndex);

		numGenerations++;
	}

	@Override
	protected final void render(Graphics2D g) {
		g.setColor(backgroundCol);

		if (redrawImages) {
			g.fillRect(0, 0, Main.canvasWidth, Main.canvasHeight);
			g.drawImage(refImg, xAlign, yAlign, getObserver());
			g.drawImage(bestApproxImg, MAX_WIDTH + xAlign, yAlign, getObserver());

			redrawImages = false;
		}

		g.fillRect(0, 0, 90, 25);
		g.setColor(Color.WHITE);
		g.drawString("Gen: " + numGenerations, 0, 15);
	}

	private final void loadImage() {
		/*
		 * FIRST - load the image
		 */

		// Open a JFileChooser in the current directory
		JFileChooser chooser = new JFileChooser(new File(""));

		// Give the user the option to limit their search to only image files
		FileNameExtensionFilter filter = new FileNameExtensionFilter("Images", "jpg", "jpeg", "png", "gif", "bmp");
		chooser.setFileFilter(filter);

		// if the user chose an image
		int returnVal = chooser.showOpenDialog(null);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
			try {
				// Set the reference image to a user selected file
				refImg = ImageIO.read(chooser.getSelectedFile());
			}
			catch (Exception e) {
				JOptionPane.showMessageDialog(null, "Sorry but that image doesn't work!", "Error", JOptionPane.ERROR_MESSAGE);
				e.printStackTrace();
			}
		}

		/*
		 * SECOND - Make sure it is a suitable size for the window and align the images
		 */
		refImg = Scalr.resize(refImg, 450);

		// Make sure the images will be drawn in the correct place
		xAlign = (int) ((MAX_WIDTH - refImg.getWidth()) / 2.0);
		yAlign = (int) ((MAX_HEIGHT - refImg.getHeight()) / 2.0);
	}

	private final void drawSolution(BufferedImage destination) {
		// Get graphics to draw onto
		Graphics2D g = (Graphics2D) destination.getGraphics();
		// Clear graphics
		g.setColor(Color.BLACK);
		g.fillRect(0, 0, destination.getWidth(), destination.getHeight());

		// Draw shapes onto graphics
		Iterator<Oval> iter = shapes.iterator();
		while (iter.hasNext())
			iter.next().render(g);

		// dispose of graphics
		g.dispose();
	}

	private final int calculateFitness(BufferedImage img) {
		int fitness = 0;

		for (int x = 0; x < img.getWidth(); x++)
			for (int y = 0; y < img.getHeight(); y++) {
				int refRGB = refImg.getRGB(x, y);
				int newRGB = img.getRGB(x, y);

				int deltaRed = (refRGB >> 16) & 0x0ff - (newRGB >> 16) & 0x0ff;
				int deltaGreen = (refRGB >> 8) & 0x0ff - (newRGB >> 8) & 0x0ff;
				int deltaBlue = (refRGB >> 0) & 0x0ff - (newRGB >> 0) & 0x0ff;

				fitness -= (deltaRed * deltaRed) + (deltaGreen * deltaGreen) + (deltaBlue * deltaBlue);
			}

		return fitness;
	}

	private class Oval {
		private final int x, y, width, height;
		private final Color col;

		private Oval() {
			width = Rand.int_(100);
			height = Rand.int_(100);
			x = Rand.int_(bestApproxImg.getWidth() - width);
			y = Rand.int_(bestApproxImg.getHeight() - height);
			col = ColTools.randColour();
		}

		private final void render(Graphics2D g) {
			g.setColor(col);
			g.fillOval(x, y, width, height);
		}
	}
}