allRGB
======

There are exactly 16777216 unique colours RGB colours when using the integers 0-255.

This program makes an image that contains exactly 1 of each colour, then using a hillclimber (which is a genetic algorithm) it attempts to evolve that image to match any image of your choice.

A few points
------------

 - The greater the range of colours your chosen image has, the closer the match can be.

 - The greater the balance between red, green and blue, the closer the match will be, for example, an all red image won't work very well as there will be a lot of left over blue and green pixels that will make the image look grainy.

 - It will be converted into a 4096 by 4096 image, so:
 - any image you chose that is much larger or smaller may suffer from compression or stretching.
 - any image you chose that isn't square will be distorted
