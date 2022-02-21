#include "AlgorithmAreaAverageRgb.h"

AlgorithmAreaAverageRgb::AlgorithmAreaAverageRgb(QObject *parent)
    : AlgorithmBase{parent}
{
}

QString AlgorithmAreaAverageRgb::GetName()
{
    return "Area Average";
}

QString AlgorithmAreaAverageRgb::GetDescription()
{
    return "<p>Averages the colour in two zones in the RGB image and compares"
           "the it to the average colour in the same zones in the target "
           "image. Then  pixels are swapped between the zones to minimise RGB "
           "colour error</p>"
           "<p>Error is the sum of the difference between each of the red, "
           "green, & blue colour channels, for both pixels, when compared to "
           "the target image.</p>";
}

void AlgorithmAreaAverageRgb::Iterate()
{
    // Do a block of repeats in one go for efficiency
    const int repeats = 100000;
    for (int i = 0; i < repeats; ++i) {
        QPoint aLoc{ RandomNumber(0, 4096 - 1), RandomNumber(0, 4096 - 1) };
        QPoint bLoc{ RandomNumber(0, 4096 - 1), RandomNumber(0, 4096 - 1) };
        const int areaSize = 3;
        QRgb rgbA = GetAverageColourInArea(allRgb_, aLoc, areaSize);
        QRgb rgbB = GetAverageColourInArea(allRgb_, bLoc, areaSize);
        QRgb targetA = GetAverageColourInArea(target_, aLoc, areaSize);
        QRgb targetB = GetAverageColourInArea(target_, bLoc, areaSize);

        QRgb a = allRgb_.pixel(aLoc);
        QRgb b = allRgb_.pixel(bLoc);
        allRgb_.setPixel(aLoc, b);
        allRgb_.setPixel(bLoc, a);
        QRgb swappedRgbA = GetAverageColourInArea(allRgb_, aLoc, areaSize);
        QRgb swappedRgbB = GetAverageColourInArea(allRgb_, bLoc, areaSize);

        int aDifference = ColourDifference(rgbA, targetA);
        int bDifference = ColourDifference(rgbB, targetB);

        int aSwappedDifference = ColourDifference(swappedRgbA, targetA);
        int bSwappedDifference = ColourDifference(swappedRgbB, targetB);

       if ((aDifference + bDifference) > (aSwappedDifference + bSwappedDifference)) {
           // Already swapped the pixels!
           ++improvements_;
       } else {
           // Swap back if we need to
           allRgb_.setPixel(aLoc, a);
           allRgb_.setPixel(bLoc, b);
       }
    }
    iterations_ += repeats;

    emit onIterationsChanged(iterations_);
    emit onImprovementsChanged(improvements_);
}

QRgb AlgorithmAreaAverageRgb::GetAverageColourInArea(const QImage& image, const QPoint& location, int areaSize) const
{
    int totalRed = 0;
    int totalGreen = 0;
    int totalBlue = 0;

    // Make our area surround the target point
    QPoint topLeft = QPoint(location.x() - (areaSize / 2), location.y() - (areaSize / 2));
    // Make sure our area is contained by our RGB and target images
    topLeft.rx() = std::clamp(topLeft.x(), 0, image.width() - areaSize);
    topLeft.ry() = std::clamp(topLeft.y(), 0, image.height() - areaSize);

    for (int x = topLeft.x(); x < topLeft.x() + areaSize; ++x) {
        for (int y = topLeft.y(); y < topLeft.y() + areaSize; ++y) {
            QRgb pixel = image.pixel(x, y);
            totalRed += qRed(pixel);
            totalGreen += qGreen(pixel);
            totalBlue += qBlue(pixel);
        }
    }

    const int count = std::pow(areaSize, 2);
    return qRgb(totalRed / count, totalGreen / count, totalBlue / count);
}
