#include "AlgorithmBase.h"

AlgorithmBase::AlgorithmBase(QObject *parent)
    : QThread{parent}
    , iterations_(0)
    , improvements_(0)
    , iterateTimer_(this)
{
    iterateTimer_.setSingleShot(false);
    iterateTimer_.setInterval(0);
    connect(&iterateTimer_, &QTimer::timeout, this, &AlgorithmBase::Iterate, Qt::DirectConnection);
}

AlgorithmBase::~AlgorithmBase()
{
    terminate();
}

void AlgorithmBase::SetTargetImage(const QImage& target)
{
    if (target != target_) {
        target_ = target;
        iterations_ = 0;
        improvements_ = 0;
    }
}

void AlgorithmBase::SetRgbImage(const QImage& rgb)
{
    if (allRgb_ != rgb) {
        allRgb_ = rgb;
        iterations_ = 0;
        improvements_ = 0;
    }
}

void AlgorithmBase::onStartRequested()
{
    iterateTimer_.start();
    start();
}

void AlgorithmBase::onStopRequested()
{
    iterateTimer_.stop();
}

void AlgorithmBase::onGetUpdateRequested()
{
    emit onUpdated(allRgb_);
}

int AlgorithmBase::RandomNumber(int min, int max) const
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(entropy_);
}

int AlgorithmBase::ColourDifference(QRgb a, QRgb b) const
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
