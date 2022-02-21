#ifndef ALGORITHMAREAAVERAGERGB_H
#define ALGORITHMAREAAVERAGERGB_H

#include "AlgorithmBase.h"

class AlgorithmAreaAverageRgb : public AlgorithmBase {
    Q_OBJECT
public:
    explicit AlgorithmAreaAverageRgb(QObject *parent = nullptr);
    virtual ~AlgorithmAreaAverageRgb() {}

    virtual QString GetName() override;
    virtual QString GetDescription() override;

private:
    virtual void Iterate() override;

    QRgb GetAverageColourInArea(const QImage& image, const QPoint& location, int areaSize) const;
};

#endif // ALGORITHMAREAAVERAGERGB_H
