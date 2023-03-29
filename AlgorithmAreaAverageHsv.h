#ifndef ALGORITHMAREAAVERAGEHSV_H
#define ALGORITHMAREAAVERAGEHSV_H

#include "AlgorithmBase.h"

class AlgorithmAreaAverageHsv : public AlgorithmBase {
    Q_OBJECT
public:
    explicit AlgorithmAreaAverageHsv(QObject *parent = nullptr);
    virtual ~AlgorithmAreaAverageHsv() {}

    virtual QString GetName() override;
    virtual QString GetDescription() override;

private:
    virtual void Iterate() override;

    QRgb GetAverageColourInArea(const QImage& image, const QPoint& location, int areaSize) const;
};

#endif // ALGORITHMAREAAVERAGEHSV_H
