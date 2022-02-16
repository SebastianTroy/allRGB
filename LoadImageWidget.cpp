#include "LoadImageWidget.h"

#include <QFileDialog>
#include <QTransform>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QPaintEvent>
#include <QPainter>

LoadImageWidget::LoadImageWidget(QWidget* parent)
    : PanAndZoomWidget(parent)
{
    setMinimumSize(600, 600);
}

void LoadImageWidget::LoadImage(QString fileName)
{
    if (image_.load(fileName)) {
        ResetView();
        onResetTranslationRequested();
        onFitToTargetRequested();
    }
}

QImage LoadImageWidget::GetImageInViewTargetArea() const
{
    const qreal rgbImageSize = 4096;
    QImage allRgbTarget(rgbImageSize, rgbImageSize, QImage::Format_RGB32);

    /*
     *  Our widget isn't 4096*4096, but we've aligned our image to this widget,
     *  so now we need to further scale our painter to account for this
     *  difference.
     */
    QPainter p(&allRgbTarget);
    qreal edge = std::min(width(), height());
    qreal scale = rgbImageSize / edge;
    p.scale(scale, scale);

    PaintImage(p);

    return allRgbTarget;
}

void LoadImageWidget::onLoadImageRequested()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.xpm *.jpg)");
    LoadImage(fileName);
}

void LoadImageWidget::onResetTranslationRequested()
{
    ResetTranslation();
}

void LoadImageWidget::onFitToTargetRequested()
{

    qreal minImageEdge = std::min(image_.width(), image_.height());
    qreal minWidgetEdge = std::min(width(), height());
    SetScale(minWidgetEdge / minImageEdge);
}

void LoadImageWidget::onRotateRequested()
{
    Rotate(90);
}

void LoadImageWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setBackground(QBrush(Qt::white));

    PaintImage(p);

    // Draw bounds that need filling
    p.setCompositionMode(QPainter::CompositionMode_Difference);

    QPen targetAreaPen(Qt::white);
    targetAreaPen.setWidthF(qreal{ 5.0 });
    targetAreaPen.setStyle(Qt::PenStyle::DashLine);
    p.setPen(targetAreaPen);

    qreal minWidgetEdge = std::min(width(), height());
    p.drawRect(QRect(0, 0, minWidgetEdge, minWidgetEdge));
}

void LoadImageWidget::PaintImage(QPainter& p) const
{
    p.save();
    // Transform to match the users pan, zoom & rotation
    TransformPainter(p);
    // centre the image around the origin
    p.drawImage(QRectF(image_.rect()).translated(image_.rect().width() / -2, image_.height() / -2), image_, QRectF(image_.rect()));
    p.restore();
}
