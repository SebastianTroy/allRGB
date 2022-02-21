#include "AlgorithmSinglePixelSwapRgb.h"

AlgorithmSinglePixelSwapRgb::AlgorithmSinglePixelSwapRgb(QObject* parent)
    : AlgorithmBase(parent)
{
}

QString AlgorithmSinglePixelSwapRgb::GetName()
{
    return "Pixel R, G & B error";
}

QString AlgorithmSinglePixelSwapRgb::GetDescription()
{
    return "<p>Randomly chooses two pixels, and swaps them if the total colour "
           "error would be less.</p>"
           "<p>Error is the sum of the difference between each of the red, "
           "green, & blue colour channels, for both pixels, when compared to "
           "the target image.</p>";
}

void AlgorithmSinglePixelSwapRgb::Iterate()
{
    // Do a block of repeats in one go for efficiency
    const int repeats = 100000;
    for (int i = 0; i < repeats; ++i) {
        QPoint aLoc{ RandomNumber(0, 4096 - 1), RandomNumber(0, 4096 - 1) };
        QPoint bLoc{ RandomNumber(0, 4096 - 1), RandomNumber(0, 4096 - 1) };
        QRgb a = allRgb_.pixel(aLoc);
        QRgb b = allRgb_.pixel(bLoc);

        int aDifference = ColourDifference(a, target_.pixel(aLoc));
        int bDifference = ColourDifference(b, target_.pixel(bLoc));

        int aSwappedDifference = ColourDifference(a, target_.pixel(bLoc));
        int bSwappedDifference = ColourDifference(b, target_.pixel(aLoc));

        if ((aDifference + bDifference) > (aSwappedDifference + bSwappedDifference)) {
            allRgb_.setPixel(aLoc, b);
            allRgb_.setPixel(bLoc, a);
            ++improvements_;
        }
    }
    iterations_ += repeats;

    emit onIterationsChanged(iterations_);
    emit onImprovementsChanged(improvements_);
}
