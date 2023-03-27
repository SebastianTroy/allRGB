#include "AllRgbWidget.h"

#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>

#include <cstdlib>
#include <vector>
#include <memory>

AllRgbWidget::AllRgbWidget(QWidget *parent)
    : PanAndZoomWidget{parent}
{
    onResetPixelsRequested();

    getUpdateTimer_.setSingleShot(false);
    getUpdateTimer_.setInterval(1000);
}

void AllRgbWidget::SetTargetImage(const QImage& targetImage)
{
    if (targetImage != target_) {
        target_ = targetImage;
        targetThumbnail_ = target_.scaled(100, 100);
        emit onTargetUpdated(target_);
        update();
    }
}

void AllRgbWidget::SetAlgorithm(std::unique_ptr<AlgorithmBase>&& algoritm)
{
    algorithm_ = std::move(algoritm);
    algorithm_->SetTargetImage(target_);
    algorithm_->SetRgbImage(allRgb_);

    connect(&getUpdateTimer_, &QTimer::timeout, algorithm_.get(), &AlgorithmBase::onGetUpdateRequested, Qt::QueuedConnection);

    connect(this, &AllRgbWidget::onTargetUpdated, algorithm_.get(), &AlgorithmBase::SetTargetImage, Qt::QueuedConnection);
    connect(this, &AllRgbWidget::onAllRgbUpdated, algorithm_.get(), &AlgorithmBase::SetRgbImage, Qt::QueuedConnection);

    connect(algorithm_.get(), &AlgorithmBase::onIterationsChanged, this, &AllRgbWidget::onIterationsChanged, Qt::QueuedConnection);
    connect(algorithm_.get(), &AlgorithmBase::onImprovementsChanged, this, &AllRgbWidget::onImprovementsChanged, Qt::QueuedConnection);
    connect(algorithm_.get(), &AlgorithmBase::onUpdated, this, &AllRgbWidget::onUpdateFromAlgorithm, Qt::QueuedConnection);
}

QImage AllRgbWidget::GetAllRgbImage() const
{
    return allRgb_;
}

void AllRgbWidget::onResetPixelsRequested()
{
    allRgb_ = QImage(4096, 4096, QImage::Format::Format_RGB32);

    ForEachPixel(allRgb_, [](QImage& target, int x, int y)
    {
        target.setPixel(x, y, x + (target.width() * y));
    });

    emit onAllRgbUpdated(allRgb_);
}

void AllRgbWidget::onLoadPixelsRequested()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image", "", "Images (*.png *.xpm *.jpg)");
    QImage newPixels(fileName);

    if (newPixels.width() != 4096 || newPixels.height() != 4096) {
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
            QString expectedUniqueColourCount = QLocale::system().toString(4096u * 4096u);
            QMessageBox::warning(this, "Invalid allRGB image", QString("%1 only contains %2 out of %3 colours)").arg(fileName, uniqueColourCount, expectedUniqueColourCount));
            return;
        }
    }

    emit onAllRgbUpdated(newPixels);
}

void AllRgbWidget::onResetViewRequested()
{
    qreal edge = std::min(width(), height());
    qreal scale = edge / 4096;
    SetScale(scale);
    ResetTranslation();
    ResetRotation();
}

void AllRgbWidget::onStartRequested()
{
    if (target_.isNull()) {
        QMessageBox::warning(this, "No Target", "Please click 'Update Target', or go to the other tab to load and set a target image.");
        return;
    }
    algorithm_->onStartRequested();
}

void AllRgbWidget::onStopRequested()
{
    algorithm_->onStopRequested();
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
    p.drawImage(QRectF(allRgb_.rect()).translated(allRgb_.rect().width() / -2, allRgb_.height() / -2), allRgb_, QRectF(allRgb_.rect()));
    p.restore();

    // draw thumbnail of target
    p.drawImage(targetThumbnail_.rect(), targetThumbnail_, targetThumbnail_.rect());
}

void AllRgbWidget::showEvent(QShowEvent* /*event*/)
{
    onResetViewRequested();
    getUpdateTimer_.start();
}

void AllRgbWidget::onUpdateFromAlgorithm(QImage allRgb)
{
    allRgb_ = allRgb.copy();
    update();
}

void AllRgbWidget::ForEachPixel(QImage& target, const std::function<void (QImage&, int, int)>& action)
{
    for (int x = 0; x < target.width(); ++x) {
        for (int y = 0; y < target.height(); ++y) {
            action(target, x, y);
        }
    }
}
