#ifndef ALGORITHMEXHAUSTIVERGB_H
#define ALGORITHMEXHAUSTIVERGB_H

#include "AlgorithmBase.h"

/**
 * TODO
 */
class AlgorithmExhaustiveRgb : public AlgorithmBase {
    Q_OBJECT
public:
    explicit AlgorithmExhaustiveRgb(QObject* parent = nullptr);
    virtual ~AlgorithmExhaustiveRgb() {}

    virtual QString GetName() override;
    virtual QString GetDescription() override;

private:
    void Iterate() override;

    int outerX = 0;
    int outerY = 0;
    int innerX = 0;
    int innerY = 0;
};

#endif // ALGORITHMEXHAUSTIVERGB_H
