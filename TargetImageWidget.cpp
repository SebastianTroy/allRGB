#include "TargetImageWidget.h"

#include <QFileDialog>
#include <QTransform>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>

TargetImageWidget::TargetImageWidget(QWidget* parent)
    : PanAndZoomWidget(parent)
{
    setMinimumSize(600, 600);
}

void TargetImageWidget::LoadImage(QString fileName)
{
    if (image_.load(fileName)) {
        ResetView();
        onResetTranslationRequested();
        onFitToTargetRequested();

        if (image_.size() == QSize(4096, 4096)) {
            int response = QMessageBox::question(this, "Skip modification?", "This image is already 4096 * 4096 pixels, would you like to set this as the target image directly?");
            if (response == QMessageBox::StandardButton::Yes) {
                emit onUseWholeImageRequested();
            }
        }
    }
}

QImage TargetImageWidget::GetImageInViewTargetArea() const
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
    p.translate(-GetTargetRect().topLeft());

    PaintImage(p);

    return allRgbTarget;
}

QImage TargetImageWidget::GetWholeImage() const
{
    return image_;
}

void TargetImageWidget::onLoadImageRequested()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.xpm *.jpg)");
    LoadImage(fileName);
}

void TargetImageWidget::onSaveImageRequested()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Select Image", "", "Images (*.png *.xpm *.jpg)");
    GetImageInViewTargetArea().save(fileName);
}

void TargetImageWidget::onResetTranslationRequested()
{
    ResetTranslation();
}

void TargetImageWidget::onFitToTargetRequested()
{
    qreal minImageEdge = std::min(image_.width(), image_.height());
    qreal minWidgetEdge = std::min(width(), height());
    SetScale(minWidgetEdge / minImageEdge);
}

void TargetImageWidget::onRotateRequested()
{
    Rotate(90);
}

void TargetImageWidget::paintEvent(QPaintEvent* /*event*/)
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

    p.drawRect(GetTargetRect());
}

void TargetImageWidget::PaintImage(QPainter& p) const
{
    p.save();
    // Transform to match the users pan, zoom & rotation
    TransformPainter(p);
    // centre the image around the origin
    p.drawImage(QRectF(image_.rect()).translated(image_.rect().width() / -qreal{ 2 }, image_.height() / -qreal{ 2 }), image_, QRectF(image_.rect()));
    p.restore();
}

QRectF TargetImageWidget::GetTargetRect() const
{
    qreal minWidgetEdge = std::min(width(), height());
    return QRectF((width() - minWidgetEdge) / qreal{ 2 }, (height() - minWidgetEdge) / qreal{ 2 }, minWidgetEdge, minWidgetEdge);
}
