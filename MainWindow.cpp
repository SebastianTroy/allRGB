#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include "TargetImageWidget.h"

#include <QLocale>
#include <QDesktopServices>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/Logo.png"));
    setWindowTitle("All RGB v2.0");

    ///
    /// Load Image Tab
    ///

    // save / load / preview / begin
    connect(ui->targetImageSaveButton, &QPushButton::pressed, ui->allRgbWidget, &TargetImageWidget::onSaveImageRequested);
    connect(ui->targetImageLoadButton, &QPushButton::pressed, ui->allRgbWidget, &TargetImageWidget::onLoadImageRequested);
    connect(ui->targetImagePreviewButton, &QPushButton::pressed, this, &MainWindow::onPreviewTargetImageRequested);
    connect(ui->setTargetImageButton, &QPushButton::pressed, this, &MainWindow::onSetTargetImageFromViewRequested);
    connect(ui->allRgbWidget, &TargetImageWidget::onUseWholeImageRequested, this, &MainWindow::onSetTargetImageFromViewRequested);

    // rotation
    connect(ui->targetImageRotationSlider, &QSlider::sliderMoved, ui->allRgbWidget, &TargetImageWidget::SetRotation);
    connect(ui->targetImageRotate90DegreesButton, &QPushButton::pressed, ui->allRgbWidget, &TargetImageWidget::onRotateRequested);
    connect(ui->allRgbWidget, &TargetImageWidget::onRotationUpdated, ui->targetImageRotationSlider, &QSlider::setValue);

    // scale
    connect(ui->targetImageFitToAreaButton, &QPushButton::pressed, ui->allRgbWidget, &TargetImageWidget::onFitToTargetRequested);
    connect(ui->targetImageScaleSpinBox, &QDoubleSpinBox::valueChanged, ui->allRgbWidget, &TargetImageWidget::SetScale);
    connect(ui->allRgbWidget, &TargetImageWidget::onScaleUpdated, ui->targetImageScaleSpinBox, &QDoubleSpinBox::setValue);

    // transform
    connect(ui->targetImageResetTranslationButton, &QPushButton::pressed, ui->allRgbWidget, &TargetImageWidget::ResetTranslation);
    connect(ui->targetImageXOffsetSpinBox, &QDoubleSpinBox::valueChanged, ui->allRgbWidget, &TargetImageWidget::SetTranslationX);
    connect(ui->targetImageYOffsetSpinBox, &QDoubleSpinBox::valueChanged, ui->allRgbWidget, &TargetImageWidget::SetTranslationY);
    connect(ui->allRgbWidget, &TargetImageWidget::onTranslationUpdated, this, [ui = ui](QPointF translation)
    {
        ui->targetImageXOffsetSpinBox->setValue(translation.rx());
        ui->targetImageYOffsetSpinBox->setValue(translation.ry());
    });

    // Link to website
    connect(ui->moreByTroyDevButton, &QPushButton::pressed, this, []()
    {
        QDesktopServices::openUrl(QUrl("http://www.TroyDev.co.uk"));
    });


    ///
    /// All RGB preview tab
    ///

    // View
    connect(ui->allRgbUpdateTargetButton, &QPushButton::pressed, this, &MainWindow::onSetTargetImageFromViewRequested);
    connect(ui->allRgbResetViewButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &AllRgbWidget::onResetViewRequested);

    // Starting pixels
    connect(ui->allRgbResetPixelsButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &AllRgbWidget::onResetPixelsRequested);
    connect(ui->allRgbLoadButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &AllRgbWidget::onLoadPixelsRequested);

    // All RGB
    connect(ui->allRgbStartButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &AllRgbWidget::onStartRequested);
    connect(ui->allRgbStopButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &AllRgbWidget::onStopRequested);
    connect(ui->allRgbPreviewWidget, &AllRgbWidget::onIterationsChanged, this, [ui = ui](quint64 iterations)
    {
        ui->allRGBIterationsValueLabel->setText(QLocale::system().toString(iterations));
    });
    connect(ui->allRgbPreviewWidget, &AllRgbWidget::onImprovementsChanged, this, [ui = ui](quint64 improvements)
    {
        ui->allRGBImprovementsValueLabel->setText(QLocale::system().toString(improvements));
    });

    // Result
    connect(ui->allRgbPreviewButton, &QPushButton::pressed, this, &MainWindow::onPreviewAllRgbRequested);
    connect(ui->allRgbSaveButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &AllRgbWidget::onSaveImageRequested);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPreviewTargetImageRequested()
{
    OpenImagePreviewWindow(ui->allRgbWidget->GetImageInViewTargetArea());
}

void MainWindow::onPreviewAllRgbRequested()
{
    OpenImagePreviewWindow(ui->allRgbPreviewWidget->GetAllRgbImage());
}

void MainWindow::onSetTargetImageFromViewRequested()
{
    ui->allRgbPreviewWidget->SetTargetImage(ui->allRgbWidget->GetImageInViewTargetArea());
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::onSetTargetImageFromEntireImageRequested()
{
    ui->allRgbPreviewWidget->SetTargetImage(ui->allRgbWidget->GetWholeImage());
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::showEvent(QShowEvent* /*event*/)
{
    ui->allRgbWidget->LoadImage(":DefaultImage.jpg");
}

void MainWindow::OpenImagePreviewWindow(const QImage& toPreview)
{
    QScrollArea* previewArea = new QScrollArea(nullptr);
    QLabel* previewImage = new QLabel();
    previewImage->setPixmap(QPixmap::fromImage(toPreview));
    previewArea->setWidget(previewImage);
    previewArea->showMaximized();
}
