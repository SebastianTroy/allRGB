#ifndef ALGORITHMSINGLEPIXELSWAPRGB_H
#define ALGORITHMSINGLEPIXELSWAPRGB_H

#include "AlgorithmBase.h"

/**
 * The Algorithm simply selects two pixels and calculates if swapping them would
 * minimise the difference in RGB values between them and the target image.
 */
class AlgorithmSinglePixelSwapRgb : public AlgorithmBase {
    Q_OBJECT
public:
    explicit AlgorithmSinglePixelSwapRgb(QObject* parent = nullptr);
    virtual ~AlgorithmSinglePixelSwapRgb() {}

    virtual QString GetName() override;
    virtual QString GetDescription() override;

private:
    void Iterate() override;
};

#endif // ALGORITHMSINGLEPIXELHILLSWAPRGB_H
