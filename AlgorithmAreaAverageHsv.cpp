#include "AlgorithmAreaAverageHsv.h"

AlgorithmAreaAverageHsv::AlgorithmAreaAverageHsv(QObject *parent)
    : AlgorithmBase{parent}
{
}

QString AlgorithmAreaAverageHsv::GetName()
{
    return "Area Average HSV";
}

QString AlgorithmAreaAverageHsv::GetDescription()
{
    return "<p>Averages the colour in two zones in the RGB image and compares "
           "the the average HSV values in the same zones in the target "
           "image. Then pixels are swapped between the zones to minimise HSV "
           "colour error</p>"
           "<p>Error is the sum of the difference between each of the red, "
           "green, & blue colour channels, for both pixels, when compared to "
           "the target image.</p>";
}

void AlgorithmAreaAverageHsv::Iterate()
{
    // Do a block of repeats in one go for efficiency
    const int repeats = 100000;
    for (int i = 0; i < repeats; ++i) {
        QPoint aLoc{ RandomNumber(0, 4096 - 1), RandomNumber(0, 4096 - 1) };
        QPoint bLoc{ RandomNumber(0, 4096 - 1), RandomNumber(0, 4096 - 1) };
        const int areaSize = 3;
        QRgb hsvA = GetAverageColourInArea(allRgb_, aLoc, areaSize);
        QRgb hsvB = GetAverageColourInArea(allRgb_, bLoc, areaSize);
        QRgb targetA = GetAverageColourInArea(target_, aLoc, areaSize);
        QRgb targetB = GetAverageColourInArea(target_, bLoc, areaSize);

        QRgb a = allRgb_.pixel(aLoc);
        QRgb b = allRgb_.pixel(bLoc);
        allRgb_.setPixel(aLoc, b);
        allRgb_.setPixel(bLoc, a);
        QRgb swappedHsvA = GetAverageColourInArea(allRgb_, aLoc, areaSize);
        QRgb swappedHsvB = GetAverageColourInArea(allRgb_, bLoc, areaSize);

        int aDifference = ColourDifference(hsvA, targetA);
        int bDifference = ColourDifference(hsvB, targetB);

        int aSwappedDifference = ColourDifference(swappedHsvA, targetA);
        int bSwappedDifference = ColourDifference(swappedHsvB, targetB);

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

QRgb AlgorithmAreaAverageHsv::GetAverageColourInArea(const QImage& image, const QPoint& location, int areaSize) const
{
    int totalHue = 0;
    int totalSaturation = 0;
    int totalValue = 0;

    // Make our area surround the target point
    QPoint topLeft = QPoint(location.x() - (areaSize / 2), location.y() - (areaSize / 2));
    // Make sure our area is contained by our HSV and target images
    topLeft.rx() = std::clamp(topLeft.x(), 0, image.width() - areaSize);
    topLeft.ry() = std::clamp(topLeft.y(), 0, image.height() - areaSize);

    for (int x = topLeft.x(); x < topLeft.x() + areaSize; ++x) {
        for (int y = topLeft.y(); y < topLeft.y() + areaSize; ++y) {
            QColor pixel = image.pixelColor(x, y);

            int h, s, v;
            pixel.getHsv(&h, &s, &v);

            totalHue += h;
            totalSaturation += s;
            totalValue += v;
        }
    }

    const int count = std::pow(areaSize, 2);
    return QColor::fromHsv(totalHue / count, totalSaturation / count, totalValue / count).rgb();
}
