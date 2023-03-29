#include "AlgorithmExhaustiveRgb.h"

#include <iostream>

AlgorithmExhaustiveRgb::AlgorithmExhaustiveRgb(QObject *parent)
    : AlgorithmBase{parent}
{

}

QString AlgorithmExhaustiveRgb::GetName()
{
    return "Exhaustive Pixel R, G & B error";
}

QString AlgorithmExhaustiveRgb::GetDescription()
{
    return "<p>TODO</p>";
}

void AlgorithmExhaustiveRgb::Iterate()
{
    struct OnExit {
        std::function<void()> onExit;
        ~OnExit()
        {
            onExit();
        }
    };

    // Do a block of repeats in one go for efficiency
    const int repeats = 100000;

    auto exigGuard = OnExit{[&]()
    {
        iterations_ += repeats;

        emit onIterationsChanged(iterations_);
        emit onImprovementsChanged(improvements_);
    }};

    int repeat = 0;

    for (; outerX < allRgb_.width(); ++outerX) {
        for (; outerY < allRgb_.height(); ++outerY) {
            for (; innerX < allRgb_.width(); ++innerX) {
                for (; innerY < allRgb_.height(); ++innerY) {
                    QPoint aLoc{ outerX, outerY };
                    QPoint bLoc{ innerX, innerY };
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
                        goto next;
                    }

                    if (++repeat > repeats) {
                        return;
                    }
                }
                innerY = 0;
            }
            innerX = 0;
next:;
        }
        outerY = 0;
    }
    outerX = 0;
}
