#ifndef TARGETIMAGEWIDGET_H
#define TARGETIMAGEWIDGET_H

#include "PanAndZoomWidget.h"

/**
 * This application requires a 4096 by 4096 image to work with, this widget
 * aims to allow a user to load, scale, translate and rotate any image so that
 * it fills a 4096 * 4096 area.
 */
class TargetImageWidget : public PanAndZoomWidget {
    Q_OBJECT
public:
    explicit TargetImageWidget(QWidget* parent = nullptr);

    void LoadImage(QString fileName);
    QImage GetImageInViewTargetArea() const;
    QImage GetWholeImage() const;

signals:
    void onUseWholeImageRequested();

public slots:
    void onLoadImageRequested();
    void onSaveImageRequested();
    void onResetTranslationRequested();
    void onFitToTargetRequested();
    void onRotateRequested();

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    QImage image_;

    void PaintImage(QPainter& p) const;
    QRectF GetTargetRect() const;
};

#endif // TARGETIMAGEWIDGET_H
