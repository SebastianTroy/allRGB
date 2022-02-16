#include "PanAndZoomWidget.h"

#include <QSizePolicy>
#include <QPainter>
#include <QMouseEvent>
#include <QScrollEvent>

PanAndZoomWidget::PanAndZoomWidget(QWidget *parent)
    : QWidget{parent}
    , translation_(0, 0)
    , scale_(1)
    , rotation_(0)
    , dragBegin_(0, 0)
    , dragging_(false)
{
}

void PanAndZoomWidget::Zoom(int steps)
{
    SetScale(scale_ * (1.0 + (0.01 * steps)));
}

void PanAndZoomWidget::ZoomIn()
{
    Zoom(+1);
}

void PanAndZoomWidget::ZoomOut()
{
    Zoom(-1);
}

void PanAndZoomWidget::SetScale(qreal scale)
{
    scale_ = scale;
    emit(onScaleUpdated(scale_));
    update();
}

void PanAndZoomWidget::ResetScale()
{
    SetScale(1);
}

void PanAndZoomWidget::Translate(const QPointF& translation)
{
    SetTranslation(translation_ + translation);
}

void PanAndZoomWidget::SetTranslationX(qreal newX)
{
    SetTranslation(QPointF(newX, translation_.y()));
}

void PanAndZoomWidget::SetTranslationY(qreal newY)
{
    SetTranslation(QPointF(translation_.x(), newY));
}

void PanAndZoomWidget::SetTranslation(const QPointF& translation)
{
    translation_ = translation;
    emit(onTranslationUpdated(translation_));
    update();
}

void PanAndZoomWidget::ResetTranslation()
{
    Translate(-translation_);
}

void PanAndZoomWidget::Rotate(qreal radians)
{
    SetRotation(rotation_ + radians);
}

void PanAndZoomWidget::SetRotation(qreal rotation)
{
    rotation_ = rotation;
    rotation_ = std::fmod(rotation_, 360);
    emit(onRotationUpdated(rotation_));
    update();
}

void PanAndZoomWidget::ResetRotation()
{
    Rotate(-rotation_);
}

void PanAndZoomWidget::ResetView()
{
    ResetRotation();
    ResetTranslation();
    ResetScale();
}

void PanAndZoomWidget::wheelEvent(QWheelEvent* event)
{
    Zoom(event->angleDelta().y() / 10.0);
}

void PanAndZoomWidget::mouseReleaseEvent(QMouseEvent* /*event*/)
{
    dragging_ = false;
}

void PanAndZoomWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MouseButton::LeftButton) {
        dragging_ = true;
        dragBegin_ = event->globalPosition();
    }
}

void PanAndZoomWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPointF newPosition = event->globalPosition();
    qreal deltaX = (newPosition.x() - dragBegin_.x()) / scale_;
    qreal deltaY = (newPosition.y() - dragBegin_.y()) / scale_;

    // rotation in radians for std functions
    qreal rotation = rotation_ * (std::numbers::pi / 180.0);

    // account for rotation
    qreal rotatedDeltaX = (deltaX * std::cos(rotation)) + (deltaY * std::sin(rotation));
    qreal rotatedDeltaY = -(deltaX * std::sin(rotation)) + (deltaY * std::cos(rotation));

    Translate(QPointF(rotatedDeltaX, rotatedDeltaY));
    dragBegin_ = newPosition;
}

void PanAndZoomWidget::resizeEvent(QResizeEvent* /*event*/)
{
    update();
}

void PanAndZoomWidget::TransformPainter(QPainter& painter) const
{
    painter.translate(width() / 2, height() / 2);
    painter.scale(scale_, scale_);
    painter.rotate(rotation_);
    painter.translate(translation_);
}
