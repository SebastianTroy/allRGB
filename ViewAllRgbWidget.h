#ifndef VIEWALLRGBWIDGET_H
#define VIEWALLRGBWIDGET_H

#include "PanAndZoomWidget.h"

#include <QImage>
#include <QTimer>

#include <functional>
#include <random>

class ViewAllRgbWidget : public PanAndZoomWidget {
    Q_OBJECT
public:
    explicit ViewAllRgbWidget(QWidget *parent = nullptr);

    void SetTargetImage(const QImage& targetImage);
    QImage GetAllRgbImage();

signals:
    void onIterationsChanged(int totalIterations);
    void onImprovementsChanged(int totalImprovements);

public slots:
    void onResetPixelsRequested();
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
    QImage allRgb_;

    int iterations_;
    int improvements_;

    QTimer thread_;

    void ForEachPixel(QImage& target, const std::function<void(QImage& target, int x, int y)>& action);
    void ImproveAllRgbImage();

    int RandomNumber(int min, int max) const;
    int ColourDifference(QRgb a, QRgb b) const;

};

#endif // VIEWALLRGBWIDGET_H
