#ifndef LOADIMAGEWIDGET_H
#define LOADIMAGEWIDGET_H

#include "PanAndZoomWidget.h"

/**
 * This application requires a 4096 by 4096 image to work with, this widget
 * aims to allow a user to load, scale, translate and rotate any image so that
 * it fills a 4096 * 4096 area.
 */
class LoadImageWidget : public PanAndZoomWidget {
    Q_OBJECT
public:
    explicit LoadImageWidget(QWidget* parent = nullptr);

    void LoadImage(QString fileName);
    QImage GetImageInViewTargetArea() const;

public slots:
    void onLoadImageRequested();
    void onResetTranslationRequested();
    void onFitToTargetRequested();
    void onRotateRequested();

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    QImage image_;

    void PaintImage(QPainter& p) const;
};

#endif // LOADIMAGEWIDGET_H
