#ifndef ALGORITHMSINGLEPIXELSWAPHSV_H
#define ALGORITHMSINGLEPIXELSWAPHSV_H

#include "AlgorithmBase.h"

/**
 * The Algorithm simply selects two pixels and calculates if swapping them would
 * minimise the difference in HSV values between them and the target image. It
 * slightly prefers less error in the S and V channels, allowing for more error
 * in the H channel, which the human eye is slightly less sensitive to.
 */
class AlgorithmSinglePixelSwapHsv : public AlgorithmBase {
    Q_OBJECT
public:
    explicit AlgorithmSinglePixelSwapHsv(QObject* parent = nullptr);
    virtual ~AlgorithmSinglePixelSwapHsv() {}

    virtual QString GetName() override;
    virtual QString GetDescription() override;

private:
    virtual void Iterate() override;

    float GetHsvError(const QColor& a, const QColor& b) const;
};

#endif // ALGORITHMSINGLEPIXELSWAPHSV_H
