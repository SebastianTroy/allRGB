#ifndef ALLRGBWIDGET_H
#define ALLRGBWIDGET_H

#include "PanAndZoomWidget.h"

#include <QImage>
#include <QTimer>

#include <functional>
#include <random>

class AllRgbWidget : public PanAndZoomWidget {
    Q_OBJECT
public:
    explicit AllRgbWidget(QWidget *parent = nullptr);

    void SetTargetImage(const QImage& targetImage);
    QImage GetAllRgbImage() const;

signals:
    void onIterationsChanged(quint64 totalIterations);
    void onImprovementsChanged(quint64 totalImprovements);

public slots:
    void onResetPixelsRequested();
    void onLoadPixelsRequested();
    void onResetViewRequested();
    void onStartRequested();
    void onStopRequested();
    void onSaveImageRequested();

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void showEvent(QShowEvent* event) override;

private:
    inline static constexpr int allRgbEdge_ = 4096;
    inline static std::mt19937 entropy_ = std::mt19937();

    QImage target_;
    QImage targetThumbnail_;
    QImage allRgb_;

    quint64 iterations_;
    quint64 improvements_;

    QTimer thread_;

    void CreateDefaultTarget();
    void ForEachPixel(QImage& target, const std::function<void(QImage& target, int x, int y)>& action);
    void ImproveAllRgbImage();

    int RandomNumber(int min, int max) const;
    int ColourDifference(QRgb a, QRgb b) const;

};

#endif // ALLRGBWIDGET_H
