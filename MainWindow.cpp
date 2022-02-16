#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include "LoadImageWidget.h"

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

    // load / preview / begin
    connect(ui->loadImageButton, &QPushButton::pressed, ui->loadImagePreview, &LoadImageWidget::onLoadImageRequested);
    connect(ui->loadImagePreviewButton, &QPushButton::pressed, this, &MainWindow::onLoadImagePreviewRequested);
    connect(ui->cropAndBeginButton, &QPushButton::pressed, this, &MainWindow::onCropAndBeginRequested);

    // rotation
    connect(ui->rotationSlider, &QSlider::sliderMoved, ui->loadImagePreview, &LoadImageWidget::SetRotation);
    connect(ui->rotate90DegreesButton, &QPushButton::pressed, ui->loadImagePreview, &LoadImageWidget::onRotateRequested);
    connect(ui->loadImagePreview, &LoadImageWidget::onRotationUpdated, ui->rotationSlider, &QSlider::setValue);

    // scale
    connect(ui->fitToAreaButton, &QPushButton::pressed, ui->loadImagePreview, &LoadImageWidget::onFitToTargetRequested);
    connect(ui->scaleSpinBox, &QDoubleSpinBox::valueChanged, ui->loadImagePreview, &LoadImageWidget::SetScale);
    connect(ui->loadImagePreview, &LoadImageWidget::onScaleUpdated, ui->scaleSpinBox, &QDoubleSpinBox::setValue);

    // transform
    connect(ui->resetOffsetButton, &QPushButton::pressed, ui->loadImagePreview, &LoadImageWidget::ResetTranslation);
    connect(ui->xOffsetSpinBox, &QDoubleSpinBox::valueChanged, ui->loadImagePreview, &LoadImageWidget::SetTranslationX);
    connect(ui->yOffsetSpinBox, &QDoubleSpinBox::valueChanged, ui->loadImagePreview, &LoadImageWidget::SetTranslationY);
    connect(ui->loadImagePreview, &LoadImageWidget::onTranslationUpdated, this, [ui = ui](QPointF translation)
    {
        ui->xOffsetSpinBox->setValue(translation.rx());
        ui->yOffsetSpinBox->setValue(translation.ry());
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
    connect(ui->allRgbResetPixelsButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &ViewAllRgbWidget::onResetPixelsRequested);
    connect(ui->allRgbResetViewButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &ViewAllRgbWidget::onResetViewRequested);

    // All RGB
    connect(ui->allRgbStartButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &ViewAllRgbWidget::onStartRequested);
    connect(ui->allRgbStopButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &ViewAllRgbWidget::onStopRequested);
    connect(ui->allRgbPreviewWidget, &ViewAllRgbWidget::onIterationsChanged, this, [ui = ui](int iterations)
    {
        ui->iterationsValueLabel->setText(QLocale::system().toString(iterations));
    });
    connect(ui->allRgbPreviewWidget, &ViewAllRgbWidget::onImprovementsChanged, this, [ui = ui](int improvements)
    {
        ui->improvementsValueLabel->setText(QLocale::system().toString(improvements));
    });

    // Result
    connect(ui->allRgbPreviewButton, &QPushButton::pressed, this, &MainWindow::onAllRgbPreviewRequested);
    connect(ui->allRgbSaveButton, &QPushButton::pressed, ui->allRgbPreviewWidget, &ViewAllRgbWidget::onSaveImageRequested);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoadImagePreviewRequested()
{
    OpenImagePreviewWindow(ui->loadImagePreview->GetImageInViewTargetArea());
}

void MainWindow::onAllRgbPreviewRequested()
{
    OpenImagePreviewWindow(ui->allRgbPreviewWidget->GetAllRgbImage());
}

void MainWindow::onCropAndBeginRequested()
{
    ui->allRgbPreviewWidget->onStopRequested();
    ui->allRgbPreviewWidget->SetTargetImage(ui->loadImagePreview->GetImageInViewTargetArea());
    ui->allRgbPreviewWidget->onResetPixelsRequested();
    ui->allRgbPreviewWidget->onResetViewRequested();
    ui->allRgbPreviewWidget->onStartRequested();
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::showEvent(QShowEvent* /*event*/)
{
    ui->loadImagePreview->LoadImage(":DefaultImage.jpg");
}

void MainWindow::OpenImagePreviewWindow(const QImage& toPreview)
{
    QScrollArea* previewArea = new QScrollArea(nullptr);
    QLabel* previewImage = new QLabel();
    previewImage->setPixmap(QPixmap::fromImage(toPreview));
    previewArea->setWidget(previewImage);
    previewArea->showMaximized();
}
