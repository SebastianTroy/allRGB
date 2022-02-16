#include "AllRgbWidget.h"

#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>

#include <cstdlib>
#include <vector>
AllRgbWidget::AllRgbWidget(QWidget *parent)
    : PanAndZoomWidget{parent}
    , iterations_(0)
    , improvements_(0)
{
    CreateDefaultTarget();
    onResetPixelsRequested();

    thread_.setSingleShot(false);
    thread_.setInterval(0);
    connect(&thread_, &QTimer::timeout, this, &AllRgbWidget::ImproveAllRgbImage);
}

void AllRgbWidget::SetTargetImage(const QImage& targetImage)
{
    if (targetImage != target_) {
        target_ = targetImage;
        targetThumbnail_ = target_.scaled(100, 100);

        iterations_ = 0;
        improvements_ = 0;

        update();
    }
}

QImage AllRgbWidget::GetAllRgbImage() const
{
    return allRgb_;
}

void AllRgbWidget::onResetPixelsRequested()
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

void AllRgbWidget::onLoadPixelsRequested()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.xpm *.jpg)");
    QImage newPixels(fileName);

    if (newPixels.width() != allRgbEdge_ || newPixels.height() != allRgbEdge_) {
        QMessageBox::warning(this, "Invalid allRGB image", QString("%1 is not the correct size (4096 * 4096)").arg(fileName));
        return;
    }

    // Allocate a count per pixel
    std::vector<qulonglong> colourCounts(0x00FFFFFF, 0);
    ForEachPixel(newPixels, [&colourCounts](QImage& source, int x, int y) mutable -> void
    {
        colourCounts[0x00FFFFFF & source.pixel(x, y)]++;
    });

    for (qulonglong colourCount : colourCounts) {
        if (colourCount != 1) {
            QString uniqueColourCount = QLocale::system().toString(std::ranges::count(colourCounts, 1));
            QString expectedUniqueColourCount = QLocale::system().toString(std::pow(allRgbEdge_, 2));
            QMessageBox::warning(this, "Invalid allRGB image", QString("%1 only contains %2 out of %3 colours)").arg(fileName, uniqueColourCount, expectedUniqueColourCount));
            return;
        }
    }

    iterations_ = 0;
    improvements_ = 0;

    allRgb_ = newPixels;
}

void AllRgbWidget::onResetViewRequested()
{
    qreal edge = std::min(width(), height());
    qreal scale = edge / allRgbEdge_;
    SetScale(scale);
    ResetTranslation();
    ResetRotation();
}

void AllRgbWidget::onStartRequested()
{
    thread_.start();
}

void AllRgbWidget::onStopRequested()
{
    thread_.stop();
}

void AllRgbWidget::onSaveImageRequested()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Select Image", "", "Images (*.png *.xpm *.jpg)");
    allRgb_.save(fileName);
}

void AllRgbWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);

    p.save();
    // centre the image around the origin
    TransformPainter(p);
    p.drawImage(QRectF(allRgb_.rect()).translated(allRgb_.rect().width() / -2, allRgb_.height() / -2), allRgb_, QRectF(target_.rect()));
    p.restore();

    // draw thumbnail of target
    p.drawImage(targetThumbnail_.rect(), targetThumbnail_, targetThumbnail_.rect());
}

void AllRgbWidget::showEvent(QShowEvent* /*event*/)
{
    onResetViewRequested();
}

void AllRgbWidget::CreateDefaultTarget()
{
    QImage defaultTarget(allRgbEdge_, allRgbEdge_, QImage::Format::Format_RGB32);
    defaultTarget.fill(Qt::black);
    SetTargetImage(defaultTarget);
}

void AllRgbWidget::ForEachPixel(QImage& target, const std::function<void (QImage&, int, int)>& action)
{
    for (int x = 0; x < target.width(); ++x) {
        for (int y = 0; y < target.height(); ++y) {
            action(target, x, y);
        }
    }
}

void AllRgbWidget::ImproveAllRgbImage()
{
    const int repeats = 100000;
    for (int i = 0; i < repeats; ++i) {
        QPoint aLoc{ RandomNumber(0, allRgbEdge_ - 1), RandomNumber(0, allRgbEdge_ - 1) };
        QPoint bLoc{ RandomNumber(0, allRgbEdge_ - 1), RandomNumber(0, allRgbEdge_ - 1) };
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

int AllRgbWidget::RandomNumber(int min, int max) const
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(entropy_);
}

int AllRgbWidget::ColourDifference(QRgb a, QRgb b) const
{
    int bDiff = qBlue(a) - qBlue(b);
    int gDiff = qGreen(a) - qGreen(b);
    int rDiff = qRed(a) - qRed(b);

    /*
     * std::pow ensures each difference is positive, and also places an emphasis
     * on minimising the difference accross all colours.
     *
     * MAYBE A greater emphasis could be placed on green as the human eye
     * percieves the most luminosity in the green channel.
     */
    return std::pow(bDiff, 2) + std::pow(gDiff, 2) + std::pow(rDiff, 2);
}
