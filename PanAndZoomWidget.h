#ifndef PANANDZOOMWIDGET_H
#define PANANDZOOMWIDGET_H

#include <QWidget>

/**
 * @brief Designed to be painted on using a QPainter. Allows the user to click &
 * drag to pan, and use a mouse wheen to scale the graphical content of the
 * widget. It also moves the {0, 0} coordinates to the middle of the widget.
 */
class PanAndZoomWidget : public QWidget {
    Q_OBJECT
public:
    explicit PanAndZoomWidget(QWidget *parent = nullptr);

    void Zoom(int steps);
    void ZoomIn();
    void ZoomOut();
    void SetScale(qreal scale);
    void ResetScale();

    void Translate(const QPointF& translation);
    void SetTranslationX(qreal newX);
    void SetTranslationY(qreal newY);
    void SetTranslation(const QPointF& translation);
    void ResetTranslation();

    void Rotate(qreal radians);
    void SetRotation(qreal rotation);
    void ResetRotation();

    // resets everything
    void ResetView();

signals:
    void onScaleUpdated(qreal scale);
    void onTranslationUpdated(QPointF translation);
    void onRotationUpdated(qreal rotation);

protected:
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

    virtual void resizeEvent(QResizeEvent* event) override;

    void TransformPainter(QPainter& painter) const;

private:
    constexpr static qreal tau_ = std::numbers::pi * qreal{ 2 };

    QPointF translation_;
    qreal scale_;
    qreal rotation_;

    QPointF dragBegin_;
    bool dragging_;
};

#endif // PANANDZOOMWIDGET_H
