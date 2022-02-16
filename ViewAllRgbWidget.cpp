#include "ViewAllRgbWidget.h"

#include <QPainter>
#include <QFileDialog>

#include <cstdlib>

ViewAllRgbWidget::ViewAllRgbWidget(QWidget *parent)
    : PanAndZoomWidget{parent}
    , iterations_(0)
    , improvements_(0)
{
    onResetPixelsRequested();

    thread_.setSingleShot(false);
    thread_.setInterval(0);
    connect(&thread_, &QTimer::timeout, this, &ViewAllRgbWidget::ImproveAllRgbImage);
}

void ViewAllRgbWidget::SetTargetImage(const QImage& targetImage)
{
    target_ = targetImage;
}

QImage ViewAllRgbWidget::GetAllRgbImage()
{
    return allRgb_;
}

void ViewAllRgbWidget::onResetPixelsRequested()
{
    allRgb_ = QImage(allRgbEdge_, allRgbEdge_, QImage::Format::Format_RGB32);

    ForEachPixel(allRgb_, [](QImage& target, int x, int y)
    {
        target.setPixel(x, y, x + (target.width() * y));
    });

    iterations_ = 0;
    improvements_ = 0;

    update();
    emit onIterationsChanged(iterations_);
    emit onImprovementsChanged(improvements_);
}

void ViewAllRgbWidget::onResetViewRequested()
{
    qreal edge = std::min(width(), height());
    qreal scale = edge / allRgbEdge_;
    qDebug() << scale;
    SetScale(scale);
    ResetTranslation();
    ResetRotation();
}

void ViewAllRgbWidget::onStartRequested()
{
    thread_.start();
}

void ViewAllRgbWidget::onStopRequested()
{
    thread_.stop();
}

void ViewAllRgbWidget::onSaveImageRequested()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Select Image", "", "Images (*.png *.xpm *.jpg)");
    allRgb_.save(fileName);
}

void ViewAllRgbWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    TransformPainter(p);

    // centre the image around the origin
    p.drawImage(QRectF(allRgb_.rect()).translated(allRgb_.rect().width() / -2, allRgb_.height() / -2), allRgb_, QRectF(target_.rect()));
}

void ViewAllRgbWidget::showEvent(QShowEvent* /*event*/)
{
    onResetViewRequested();
}

void ViewAllRgbWidget::ForEachPixel(QImage& target, const std::function<void (QImage&, int, int)>& action)
{
    for (int x = 0; x < target.width(); ++x) {
        for (int y = 0; y < target.height(); ++y) {
            action(target, x, y);
        }
    }
}

void ViewAllRgbWidget::ImproveAllRgbImage()
{
    const int repeats = 100000;
    for (int i = 0; i < repeats; ++i) {
        QPoint aLoc{ RandomNumber(0, 4096), RandomNumber(0, 4096) };
        QPoint bLoc{ RandomNumber(0, 4096), RandomNumber(0, 4096) };
        QRgb a = allRgb_.pixel(aLoc);
        QRgb b = allRgb_.pixel(bLoc);

        int aDifference = ColourDifference(a, target_.pixel(aLoc));
        int bDifference = ColourDifference(b, target_.pixel(bLoc));

        int aSwappedDifference = ColourDifference(a, target_.pixel(bLoc));
        int bSwappedDifference = ColourDifference(b, target_.pixel(aLoc));

        if ((aDifference + bDifference) > (aSwappedDifference + bSwappedDifference)) {
            allRgb_.setPixel(aLoc, b);
            allRgb_.setPixel(bLoc, a);
            ++improvements_;
        }
    }
    iterations_ += repeats;

    update();
    emit onIterationsChanged(iterations_);
    emit onImprovementsChanged(improvements_);
}

int ViewAllRgbWidget::RandomNumber(int min, int max) const
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(entropy_);
}

int ViewAllRgbWidget::ColourDifference(QRgb a, QRgb b) const
{
    int bDiff = std::abs(static_cast<int>((a & 0x000000ff) - (b & 0x000000ff)));
    a >>= 8;
    b >>= 8;
    int gDiff = std::abs(static_cast<int>((a & 0x000000ff) - (b & 0x000000ff)));
    a >>= 8;
    b >>= 8;
    int rDiff = std::abs(static_cast<int>((a & 0x000000ff) - (b & 0x000000ff)));
    return bDiff + gDiff + rDiff;
}
