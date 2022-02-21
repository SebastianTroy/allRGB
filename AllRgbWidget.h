#ifndef ALLRGBWIDGET_H
#define ALLRGBWIDGET_H

#include "PanAndZoomWidget.h"
#include "AlgorithmBase.h"

#include <QImage>
#include <QTimer>

#include <functional>

class AllRgbWidget : public PanAndZoomWidget {
    Q_OBJECT
public:
    explicit AllRgbWidget(QWidget *parent = nullptr);

    void SetTargetImage(const QImage& targetImage);
    void SetAlgorithm(std::unique_ptr<AlgorithmBase>&& algoritm);
    QImage GetAllRgbImage() const;

signals:
    void onTargetUpdated(QImage target);
    void onAllRgbUpdated(QImage allRgb);
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

private slots:
    void onUpdateFromAlgorithm(QImage allRgb);

private:
    QImage target_;
    QImage targetThumbnail_;
    QImage allRgb_;

    std::unique_ptr<AlgorithmBase> algorithm_;

    QTimer getUpdateTimer_;

    void ForEachPixel(QImage& target, const std::function<void(QImage& target, int x, int y)>& action);
};

#endif // ALLRGBWIDGET_H
