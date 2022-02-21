#include "AlgorithmSinglePixelSwapHsv.h"

AlgorithmSinglePixelSwapHsv::AlgorithmSinglePixelSwapHsv(QObject* parent)
    : AlgorithmBase(parent)
{
}

QString AlgorithmSinglePixelSwapHsv::GetName()
{
    return "Pixel H, S & V error";
}

QString AlgorithmSinglePixelSwapHsv::GetDescription()
{
    return "<p>Randomly chooses two pixels, and swaps them if the total colour "
           "error would be less.</p>"
           "<p>First, the colour of each pixel is converted to the HSV "
           "colourspace, then the pixels are compared against the target image"
           "to calculate the error, i.e. the difference in HSV values between "
           "the rgb image and the target image.</p>"
           "<p>The error calculation can take advantage of the HSV colour "
           "representation and a quirk of human vision, we are more perceptive "
           "at luminosity (light/dark) differences than hue (colour) "
           "differences. Therefore when calculating error, luminosity error is "
           "considered more important than colour error</p>";
}

void AlgorithmSinglePixelSwapHsv::Iterate()
{
    // Do a block of repeats in one go for efficiency
    const int repeats = 100000;
    for (int i = 0; i < repeats; ++i) {
        QPoint aLoc{ RandomNumber(0, 4096 - 1), RandomNumber(0, 4096 - 1) };
        QPoint bLoc{ RandomNumber(0, 4096 - 1), RandomNumber(0, 4096 - 1) };
        QColor sourceA = allRgb_.pixelColor(aLoc);
        QColor sourceB = allRgb_.pixelColor(bLoc);
        QColor targetA = target_.pixelColor(aLoc);
        QColor targetB = target_.pixelColor(bLoc);

        float aDifference = GetHsvError(sourceA, targetA);
        float bDifference = GetHsvError(sourceB, targetB);

        float aSwappedDifference = GetHsvError(sourceA, targetB);
        float bSwappedDifference = GetHsvError(sourceB, targetA);

        if ((aDifference + bDifference) > (aSwappedDifference + bSwappedDifference)) {
            allRgb_.setPixel(aLoc, sourceB.rgb());
            allRgb_.setPixel(bLoc, sourceA.rgb());
            ++improvements_;
        }
    }
    iterations_ += repeats;

    emit onIterationsChanged(iterations_);
    emit onImprovementsChanged(improvements_);

}

float AlgorithmSinglePixelSwapHsv::GetHsvError(const QColor& a, const QColor& b) const
{
    float ah, as, av;
    float bh, bs, bv;
    a.getHsvF(&ah, &as, &av);
    b.getHsvF(&bh, &bs, &bv);

    float hError = std::abs(ah - bh) * 0.9;
    float sError = std::abs(as - bs) * 1.0;
    float vError = std::abs(av - bv) * 1.1;

    return hError + sError + vError;
}
