#ifndef ALGORITHMBASE_H
#define ALGORITHMBASE_H

#include <QThread>
#include <QTimer>
#include <QImage>

#include <random>

/**
 * This aim of this class is to move the processing of the image onto a
 * different thread, while also making it easy to switch between multiple
 * different algorithms.
 *
 * This class should entirely be interacted with using "Qt::QueuedConnection"s
 */
class AlgorithmBase : public QThread {
    Q_OBJECT
public:
    explicit AlgorithmBase(QObject *parent = nullptr);
    virtual ~AlgorithmBase();

    void SetTargetImage(const QImage& target);
    void SetRgbImage(const QImage& rgb);

    /**
     * @return A user visible string that uniquely identifies this algorithm.
     */
    virtual QString GetName() = 0;
    /**
     * @return A user visible string that gives a brief explanation as to what
     * this algorithm does
     */
    virtual QString GetDescription() = 0;

signals:
    void onIterationsChanged(qulonglong iterations);
    void onImprovementsChanged(qulonglong improvements);
    void onUpdated(QImage allRgb);

public slots:
    void onStartRequested();
    void onStopRequested();
    void onGetUpdateRequested();

protected:
    QImage target_;
    QImage allRgb_;

    qulonglong iterations_;
    qulonglong improvements_;

    int RandomNumber(int min, int max) const;
    int ColourDifference(QRgb a, QRgb b) const;

private:
    inline static std::mt19937 entropy_ = std::mt19937();

    QTimer iterateTimer_;

    /**
     * @brief Iterate Should do some work towards "solving" the allRGB image.
     * iterations and improvements should be updated if necessary, and their
     * signals emitted;
     */
    virtual void Iterate() = 0;
};

#endif // ALGORITHMBASE_H
